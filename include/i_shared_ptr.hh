#ifndef ISHAREDother_HH
#define ISHAREDother_HH
/// @file i_shared_ptr.hh
///  @verbatim
/// Created by Douglas S. Leonard on 6/8/25.  All rights Reserved
///
/// MIT license FOR THIS FILE, may not apply to other files.
///
/// @endverbatim

//#include <atomic>    // for std::atomic
#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>
#include <memory>
#include "IVolumeBuilder.hh"


namespace DLG4 {
    /// @class DLG4::i_shared_ptr
    /// A wrapper for std::shared_ptr that allows and facilitates many implicit(i) type conversions.
    /// such as from T to i_shared_ptr<T>.   Very useful for accepting paramters in a CRTP fluent base class
    /// You can accept a type erased class or ctor parameters to it and avoid BOTH the overload (ctor params)
    /// AND the templating (handled in the tpe erasing class in ONE place) at the method definition.
    ///
    ///  WARNING********  DO NOT ADD POINTER DATA TO THIS CLASS.
    ///  DO NOT DELETE THROUGH shared_ptr BASE REFERENCE.
    ///  The base class (shared_ptr) has a non-virtual destructor..  Technically any deletion through
    ///  a base class pointer is undefined behavior according to C++.  According to real compilers,
    ///  It's fine, so long as there is nothing here that requires a delete call or such.
    ///  This COULD be rewritten to hold the shared pointer instead of inheriting from it.  AI even might
    ///  get it right, might.
    ///
    ///
    /// @tparam T The type of the object to be shared.
    template <typename T>
    class i_shared_ptr: public std::shared_ptr<T> {
    public:
        /// Inherit all constructors from std::shared_ptr
        using std::shared_ptr<T>::shared_ptr;

        //Getting all the ctors to work where needed was admitedly a bit of whack-a-mole.
        //and likely is not optimal:
        
        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *> > >
        i_shared_ptr(const std::shared_ptr<U> &ptr) : std::shared_ptr<T>(ptr) {
        }

        /// Template constructor from i_shared_ptr<U> where U can construct T
        template <typename U, typename std::enable_if<!std::is_same<T, U>::value
            //&& std::is_constructible<T, U>::value
            , int>::type = 0>
        i_shared_ptr(const i_shared_ptr<U> &other) : std::shared_ptr<T>(other) {
        }

        // Explicit constructor for std::shared_ptr<const T>
        i_shared_ptr(const std::shared_ptr<std::remove_const_t<T> > &other)
            : std::shared_ptr<T>(other) {
        }

    private:
        template<typename U, typename V = T>
        struct enable_if_abstract : std::enable_if<std::is_abstract<V>::value> {};

    public:
        // Template constructor from i_shared_ptr<U> where T is abstract (move version)
        template <typename U>
        i_shared_ptr(i_shared_ptr<U> &&other,
                     typename enable_if_abstract<U>::type* = nullptr)
            : std::shared_ptr<T>(std::move(other)) {
        }

        // /// Template constructor from i_shared_ptr<U> where U can construct T (move version)
        // template <typename U>
        // i_shared_ptr(i_shared_ptr<U> &&other) : std::shared_ptr<T>(std::move(other)) {
        // }

        /// Explicit constructor from std::shared_ptr<T> (move)
        i_shared_ptr(std::shared_ptr<T> &&other) : std::shared_ptr<T>(std::move(other)) {
        }

    private:
        template <typename X>
        struct is_exact_std_shared_ptr: std::false_type {
        };

        template <typename U>
        struct is_exact_std_shared_ptr<std::shared_ptr<U> >: std::true_type {
        };

        template <typename Y>
        struct is_exact_i_shared_ptr: std::false_type {
        };

        template <typename Z>
        struct is_exact_i_shared_ptr<i_shared_ptr<Z> >: std::true_type {
        };

    public:
        template <typename First, typename... Rest,
            typename = std::enable_if_t<
                !std::is_same_v<std::decay_t<First>, i_shared_ptr<T> >
                && !is_exact_std_shared_ptr<std::decay_t<First> >::value
                //&&  !is_exact_i_shared_ptr<std::decay_t<First>>::value
            > >
        i_shared_ptr(First &&first, Rest &&... rest)
            : std::shared_ptr<T>(new T(std::forward<First>(first), std::forward<Rest>(rest)...)) {
        }


        /// Implicit constructor from raw pointer (danger: takes ownership)
        i_shared_ptr(T *other) : std::shared_ptr<T>(other) {
        }

        template <typename X = T, typename = std::enable_if_t<std::is_base_of_v<
            VolumeBuilders::IVolumeBuilder, X> > >
        operator G4VSolid *() {
            auto other = static_cast<VolumeBuilders::IVolumeBuilder *>(this->get());
            return other->GetFinalSolid();
        }

        /// @ingroup products
        template <typename X = T, typename = std::enable_if_t<std::is_base_of_v<
            VolumeBuilders::IVolumeBuilder, X> > >
        operator G4LogicalVolume *() {
            auto other = static_cast<VolumeBuilders::IVolumeBuilder *>(this->get());
            return other->GetLogicalVolume();
        }

        /// @ingroup products
        template <typename X = T, typename = std::enable_if_t<std::is_base_of_v<
            VolumeBuilders::IVolumeBuilder, X> > >
        operator G4VPhysicalVolume *() {
            auto other = static_cast<VolumeBuilders::IVolumeBuilder *>(this->get());
            return other->GetPlacement();
        }

        /// @ingroup products
        template <typename X = T, typename = std::enable_if_t<std::is_base_of_v<
            VolumeBuilders::IVolumeBuilder, X> > >
        operator G4Transform3D() const {
            static_assert(std::is_base_of_v<VolumeBuilders::IVolumeBuilder, X>,
                    "T must derive from IVolumeBuilder");
            auto other = static_cast<const VolumeBuilders::IVolumeBuilder *>(this->get());
            return other->GetPhysTransform();
        }
    };
    template <typename TargetType, typename SourceType>
    i_shared_ptr<TargetType>
    i_dynamic_pointer_cast(const i_shared_ptr<SourceType>& sourcePtr) noexcept {
        using SharedPtrToTarget = std::shared_ptr<TargetType>;
        if (auto* rawPointer = dynamic_cast<typename SharedPtrToTarget::element_type*>(sourcePtr.get())) {
            return SharedPtrToTarget(sourcePtr, rawPointer);
        }
        return SharedPtrToTarget();
    }
}


#endif // ISHAREDother_HH