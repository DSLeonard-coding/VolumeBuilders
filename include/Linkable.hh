#pragma once
/**
* @class DLG4::Linkable
* @verbatim
* Created by Douglas S. Leonard on 6/10/25.  All rights Reserved
* See related files for license, if any is provided.
*
* A transitive view of another  Linkable<T> or, as a root link, an owning T copy or non-owning T* view.  Data access is value semantics, immutability control (of the links).
* Every link can be set only once, but once linked the view transitively controls the linked Linkable, including its link command, so it's possible
* to call Link more than once from a single Linkable but not to relink or to acquire more than one concrete resource.

* Links can be thought of as a view linking toward a single root (downlink) source. The root may be in an unlinked state,  or may
* have a non-owning or owning shared pointer to data.   Links to other links are always effectively non-owning.  While the data itself is internally linked via shared
* via shared pointer, the link stack itself becomes invalid if another link is deleted, making it unsafe to operate on at that point. It's better
* to think of links like raw pointers in this regard.  Multiple links can be made from other linkables to a single source or source link.  Every individual link can only only be set once.

* If the root itself is not linked, all objects on the tree will behave as not linked, because
* there is still no source, and any object in the tree can call link to link the root to another link
* or to a source.  Once the root is linked to a source, all objects in the tree are linked to a source and cannot be
* be relinked.  Other objects can be still be linked to linkables in the tree, and then having a source, will also be fully linked.

*
* Copy and assignment work on the contents, not the internal pointer.
* It provides value-like semantics even for heap/pointer objects, and
* with const Linkable, you can only Link  once, but it doesn't have to be in an initializer!
*
* Makes relinking more deliberate, even in ctor initializers.
*
* Const refers to pointer.  It does NOT protect the contents of the object.
* Use ex: Linkable<const int> to make CONTENTS const.
* This is consistent with shared pointer behavior, but note operator= here acts on contents,
*  and CAN modify them unless T itself is a const type.
*
* Gaurantees:
* -Will NEVER modify the shared_ptr itself without an explicit Link(..) call or SET_LINK parameter
* -Will never CREATE a shared_ptr that ties to an exisiting shared_ptr without the same.
*   because the copy ctor creates a new ptr from a deep copy.
* -No concern for accidentally re-initializing to something that can be changed elsewhere (bugs!).
* -copy ctor and assignment semantics both follow value semantics, behaving like a T
*   more than like a shred pointer.
*   Just use it like a T, EXCEPT when you explicitly link.
*
* Heap Object (pointers) Safety:
* For heap objects O you usually want a Linkable<O> NOT a Linkable<O*>
* The Linkable O is internally an O* and can be initialized from an O* but
* because it acts like an O, you cannot modify the stored pointer.
* Unless you re-link (impossible with const) you will always access the memory.
*
*   @endverbatim
*/

// Wrap an object in a linkable pointer.
// Link to other reference wrappers.
#ifndef LINKABLE_HH
#define LINKABLE_HH

#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <vector>
#define SET_LINK_TYPE std::true_type
#define SET_LINK std::true_type{}

// Geant things need to not be deleted:
namespace PersistentObjectRegistry {
    // For Geant, we need things left persistent.
    // Yes, in header this is multiple "registries" but this is a black hole anyway.
    inline std::vector<std::shared_ptr<void>> black_hole;
    inline std::mutex s_registry_mutex;
}

using namespace PersistentObjectRegistry;

namespace DLG4 {
    // enable a mocked version that helps code analysis, now incomplete though.
    //#define DEBUG
#ifdef DEBUG
    // a mock version to help with IDE.
    template <typename T>
    class Linkable {
    private:
        T value_; // Store a plain object for simplicity

    public:
        Linkable() : value_() {
            std::cout << "Mock Linkable default ctor\n";
        }

        Linkable(const Linkable<T> &other) : value_(other.value_) {
            std::cout << "Mock Linkable copy ctor (deep copy)\n";
        }

        Linkable(const Linkable<T> &other, SET_LINK_TYPE) : value_(other.value_) {
            std::cout << "Mock Linkable linking ctor\n";
        }

        void reset() {
            std::cout << "Mock Linkable reset()\n";
        }

        T* get(){ return &T; };
        explicit Linkable(const T &other) : value_(other) {
            std::cout << "Mock Linkable value ctor\n";
        }

        explicit operator bool() const noexcept {
            return true;
        }

        T *operator->() {
            return &value_;
        }

        T &operator*() {
            return value_;
        }

        Linkable<T> &operator=(const Linkable<T> &other) {
            std::cout << "Mock Linkable assignment\n";
            value_ = other.value_;
            return *this;
        }

        void Link(const Linkable<T> &other) {
            std::cout << "Mock Linkable Link()\n";
            value_ = other.value_;
        }
    };
#else

    template <typename T>
    class Linkable {
    private:
        std::shared_ptr<T> ref_ = nullptr;                       // points to the source if any
        mutable std::vector<Linkable<T> *> backlinks_; // points to things that linked to us.
        mutable Linkable<T> *downlink_ = nullptr;      // points to things we linked to.
        mutable Linkable<T> *rootlink_ = nullptr;
        static inline std::recursive_mutex s_link_mutex;

    public:
        /// Default constructor
        Linkable()
            : ref_() {
        }

        //destructor
        ~Linkable() {
            std::lock_guard<std::recursive_mutex> lock(s_link_mutex);
            // remove ourselves from the backlinks of the downlink
            // this prevents corrupt backlink propagation after we are deleted.
            if (downlink_) {
                auto& parent_backlinks = downlink_->backlinks_;
                auto it = std::remove(parent_backlinks.begin(), parent_backlinks.end(), this);
                if (it != parent_backlinks.end()) {
                    parent_backlinks.erase(it, parent_backlinks.end());
                }
            }
            // remove downlinks from our backlinks
            // Does this orphan chains?:
            //  If we have a ref, link to will error anyway
            //  If we don't have a ref, we just become the root.
            //  No harm.  Nothing linked, nothing lost.
            // But backlinks become detatched from each other, have different views.
            // So relink to the first backlink, making it the root.
            Linkable<T> *first_link = nullptr;
            for (auto* child : backlinks_) {
                if (child->downlink_ == this) {
                    if (bool is_first_link = false; !is_first_link) {
                        first_link = child;
                        child->downlink_ = nullptr;
                    } else {
                        child->downlink_ = first_link;
                    }
                }
            }
            backlinks_.clear(); //if this matters, something is already very wrong.  But it can help unmask that.
        }

        /// Copy constructor does deep copy, not link
        /// This is safest and default
        Linkable(const Linkable<T> &other):
            ref_(std::make_shared<T>(*other.ref_)) {
        }

        ///The EXPLICIT linking copy ctor
        Linkable(Linkable<T> &other, SET_LINK_TYPE) {
            ref_=nullptr;
            LinkTreeTo(other);
        }

        /// Copy constructor does deep copy, not link
        /// This is safest and default
        explicit Linkable(const T &other)
            : ref_(std::make_shared<T>(other)) {
        }

        //
        // const Forwarding methods:

        /// -> foward For non-pointer T
        template <typename U = T>
        std::enable_if_t<!std::is_pointer_v<U>, U *>
        operator->() const {
            return ref_.get();
        }

        /// operator-> forward For pointer T
        template <typename U = T>
        std::enable_if_t<std::is_pointer_v<U>, std::remove_pointer_t<U> *>
        operator->() const {
            return *ref_;
        }

        /// operator* for non-pointer T: returns reference to T
        template <typename U = T>
        std::enable_if_t<!std::is_pointer_v<U>, U &>
        operator*() const {
            return *ref_;
        }

        /// operator* for pointer T: returns reference to the pointed-to object
        template <typename U = T>
        std::enable_if_t<std::is_pointer_v<U>, std::remove_pointer_t<U> &>
        operator*() const {
            return **ref_;
        }

        /// Allow CONTENT update (not reference) through assignment
        /// This is allowed even for const wrapper.
        const Linkable<T> &operator=(const Linkable<T> &other) const {
            *ref_ = *(other.ref_);
            return *this; // return const ref to self.
        }

        Linkable &operator=(const T &other) const {
            *ref_ = other;
            return *this;
        }

        /// Link to an existing T object via pointer (non-owning)
        void LinkToRaw(T *ptr) {
            auto data = std::shared_ptr<T>(ptr, [](T *) {
                /* do nothing */
            });
            LinkTreeTo(data);
        }

        // Non const link reseters:

        /// Reset Link to another Linakble:
        void Link(Linkable<T> &other) {
            LinkTreeTo(other);
        }

        template <typename X>
        struct false_template: std::false_type {
        };

        //Accept shared ownership.
        void ShareLink(std::shared_ptr<T> other) {
            if (!other) {
                throw std::invalid_argument("\n Error in Linkable::Link: null pointer passed.\n");
            }
            auto data = other;
            LinkTreeTo(data);
        }

        /**
         * Link and own a T and link to it.
         */
        template <typename... Args,
            std::enable_if_t<
                !(std::conjunction_v<std::is_same<std::decay_t<Args>, Linkable<T>>...>),
                int>  = 0>
        void ConstructAndLink(Args &&... args) {
            auto data = std::make_shared<T>(std::forward<Args>(args)...);
            LinkTreeTo(data);
        }

        explicit operator bool() const noexcept {
            return static_cast<bool>(ref_);
        }

        T *get_mutable() const {
            return ref_.get();
        };

        const T *get() const {
            return ref_.get();
        };

        void make_persistent() {
            std::lock_guard<std::mutex> lock(s_registry_mutex);
            black_hole.push_back(ref_);
        }

        // operator T() const {
        //     if (!ref_) {
        //         throw std::runtime_error("Attempt to convert empty Linkable to T");
        //     }
        //     return *ref_;  // Return a copy of the contained object
        // }

        operator T *() const {
            if (!ref_) {
                throw std::runtime_error("Attempt to convert empty Linkable to T*");
            }
            return ref_.get(); // Return a copy of the contained object
        }

    private
    :
        // private methods used to update the links network.

        // called on a linkable to recrusive update all back-links to its data.
        void updateTreeRefs(const Linkable<T> &target_ref) {
            for (auto backlink : backlinks_) {
                backlink->ref_ = this->ref_;
                if (backlink == &target_ref) {
                    throw std::logic_error("Fatal error: A loop occurred when linking Linkables.");
                }
                backlink->updateTreeRefs(target_ref);
            }
        }

        void LinkTreeTo(Linkable<T> &other) {
            std::lock_guard<std::recursive_mutex> lock(s_link_mutex);
            if (!ref_) {
                // if ref is set, root is already linked to a source
                if (!downlink_) {
                    // we are the root, update links, not just data ref:
                    downlink_ = &other;
                    other.backlinks_.emplace_back(this);
                    if (other.ref_) {
                        other.updateTreeRefs(other);
                    }
                } else {
                    // recurse toward root to apply the link there:
                    downlink_->LinkTreeTo(other);
                }
            } else {
                throw std::logic_error(
                    "Linkable::Link() called on object already linked to data.\n");
            }
        }

        void LinkTreeTo(std::shared_ptr<T> &other) {
            std::lock_guard<std::recursive_mutex> lock(s_link_mutex);
            if (!other.get()) {
                throw std::logic_error("Attempting to link a Linkable<T> to null data.\n");
            }
            if (!ref_) {
                if (!downlink_) {
                    // we are the root, update OUR data:
                    ref_ = other;
                    rootlink_ = this;
                    if (rootlink_->ref_) {
                        rootlink_->updateTreeRefs(*rootlink_);
                    }
                } else {
                    // recurse toward root to apply the link there:
                    if (std::find(downlink_->backlinks_.begin(), downlink_->backlinks_.end(), this) == downlink_->backlinks_.end()) {
                        downlink_->backlinks_.emplace_back(this);
                    }
                    downlink_->LinkTreeTo(other);
                }
            } else {
                throw std::logic_error(
                    "Linkable::Link() called on object already linked to data.\n");
            }
        }
    };


#endif //DEBUG
} // DLG4


#endif //LINKABLE_HPP