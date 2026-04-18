#ifndef DLG4_STRUCTUREBUILDER_HPP
#define DLG4_STRUCTUREBUILDER_HPP
/**
* @file
* Created by @author Douglas S. Leonard on @date 6/29/25.  All rights Reserved
* See related files for license, if any is provided.
*
* @brief
*
*/
#include "Linkable.hh"
#include "VolumeBuildersTypes.hh"
#include "StructureViewCore.hh"
#include "StructureBuilder.hh"
#include "BuilderViewCore.hh"
#include <memory>


namespace DLG4::VolumeBuilders::_internals_ {
#define BASE StructureBuilder<U>
#define DERIVED typename BASE::DerivedPtr


    // template <typename U>
    // template <typename T>
    // BASE::StructureBuilder(i_shared_ptr<T> other,
    //         std::enable_if_t<std::is_base_of_v<VolumeBuilder<T>, T>, int>  =
    //                 0) : StructureBuilder(other, SET_LINK) {
    // }

    template <typename U>
    BASE::StructureBuilder(const StructureBuilder &other) :
        builder_configs_(other.builder_configs_),
        boolean_configs_(other.boolean_configs_),
        lv_configs_(other.lv_configs_),
        placement_configs_(
            other.placement_configs_) {
        // avoid masked bugs from stale view:
        builder_configs_->builder_view = nullptr;
        builder_configs_->istructure_ptr = nullptr;
    }


    /**
     * Main constructor to link to another builder
     * This is a super cheat.  By linking CRTP base data, we don't need forwarders
     * For each and every CRTP method here!!  Just for the (much fewer) derived class overrides!
     * We get FULL CRTP polymorphic tye erasure at a tiny fraction of the price!
     * @param other The builder to reference
     */
    template <typename U>
    template <typename T, typename std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>, int>>
    BASE::StructureBuilder(const SharedPtr<T> &other, SET_LINK_TYPE)
        : builder_configs_(other->builder_configs_,SET_LINK),
          boolean_configs_(other->boolean_configs_,SET_LINK),
          lv_configs_(other->lv_configs_, SET_LINK),
          placement_configs_(other->placement_configs_,SET_LINK),
          final_solid_ptr_(other->final_solid_ptr_,SET_LINK),
          solid_ptr_(other->solid_ptr_,SET_LINK),
          logicvol_ptr_(other->logicvol_ptr_,SET_LINK),
          placement_(other->placement_,SET_LINK) {
        builder_configs_->istructure_ptr = IStructurePtr(other);
        // provide a builder view on builders:
        if (other->placement_configs_->is_builder) {
            BuilderView builder_view = other->ToBuilderView();
            //We're storing the view in the builder by writing to the builder through that same view!
            builder_view->StoreBuilderView(builder_view);
        }
    }

    template <typename U>
    BASE::~StructureBuilder() {
        // optionally release resources (default)
        // ReSharper disable once CppIfCanBeReplacedByConstexprIf
        if (!has_ownership_ && placement_configs_->is_builder) {
            // This is SO wrong.  This should be done at CREATION of members,
            // not deletion of class.
            // But it works for now and we're super restrictive on creation.
            placement_.make_persistent();
        }
    }

    template <typename U>
    DERIVED BASE::MakePlacement() {
        if (was_placed_) {
            throw std::runtime_error("Cannot make placement - structure was already placed");
        }
        was_placed_ = true;
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                auto name = this->builder_configs_->builder_view->GetPlacementBaseName();
                child->placement_configs_->parent_name = name;
                //recurse until concrete, below:
                child->MakePlacement();
            }
        } else {
            // concrete builder. Break recursion and call builder placement.
            this->builder_configs_->builder_view->MakePlacement();
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::
    PlaceAndFork() {
        this->MakePlacement();
        auto clone = this->ForkForPlacement(); // default/preset options
        return clone;
    }

    template <typename U>
    DERIVED BASE::
    SetName(const G4String &name) {
        this->builder_configs_->builder_view->SetName(name);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetMaterial(
        G4Material *material) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetMaterial(material);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetMaterial(material);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetColor(double r, double g,
        double b, double alpha) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetColor(r, g, b, alpha);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetColor(r, g, b, alpha);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetColor(
        const G4Colour &color) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetColor(color);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetColor(color);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetAlpha(G4double alpha) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetAlpha(alpha);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetAlpha(alpha);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::ForceSolid(bool x) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->ForceSolid(x);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->ForceSolid(x);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::
    SetVisibility(bool is_visible) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetVisibility(is_visible);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetVisibility(is_visible);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetPhysRotation(
        const G4RotationMatrix &rot) {
        // Transforms are kept hierarchical and essentially are combined when placed:
        // This way order of setting different hierarchy levels doesn't matter.
        // This removes some creative uses, but also a lot of confusion.
        this->builder_configs_->builder_view->SetPhysRotation(rot);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::StackPhysRotation(
        const G4RotationMatrix &stacked_rot) {
        this->builder_configs_->builder_view->StackPhysRotation(stacked_rot);
        auto retval = this->shared_from_this();
        return retval;
    }

    template <typename U>
    DERIVED BASE::SetPhysOffset(
        const Unit3Vec &offset) {
        this->builder_configs_->builder_view->SetPhysOffset(offset);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::StackPhysOffset(
        const Unit3Vec &offset) {
        this->builder_configs_->builder_view->StackPhysOffset(offset);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetPhysTransform(
        const UnitlessG4Transform3D &new_transform) {
        this->builder_configs_->builder_view->SetPhysTransform(new_transform);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::StackPhysTransform(
        const UnitlessG4Transform3D &stacked_transform) {
        this->builder_configs_->builder_view->StackPhysTransform(stacked_transform);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::OverridePlacementName(
        const G4String &pName) {
        this->builder_configs_->builder_view->OverridePlacementName(pName);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetCopyNo(G4int pCopyNo) {
        //TODO FIX THIS
        std::cout << "fixme" << std::endl;
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetSurfaceCheck(
        G4bool pSurfChk) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetSurfaceCheck(pSurfChk);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetSurfaceCheck(pSurfChk);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetMother(
        const BuilderView &mother) {
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetMother(mother);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetMother(mother);
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetAutoPlacementNaming(
        bool set) {
        //FIXME check overriding of explicit_name_set
        // naming is hierarchical.  So just set ours.
        this->builder_configs_->builder_view->SetAutoPlacementNaming(set);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::
    SetAutoCopyNo(bool set) {
        // Copy No is not hierarchical, so enable individual auto set.
        if (!placement_configs_->is_builder) {
            // assembly
            for (auto &child : placement_configs_->children) {
                child->SetAutoCopyNo(set);
            }
        } else {
            // concrete builder... operate with concrete view:
            this->builder_configs_->builder_view->SetAutoCopyNo(set);
        }
        return this->shared_from_this();
    }


    template <typename U>
    DERIVED BASE::ForkForPlacement(
        // TODO review what do with copy_no
        std::optional<int> copy_no, const G4String &name_override) {
        // Polymorphic clone through builder view method.
        auto c1 = this->builder_configs_->builder_view->ForkForPlacement();
        auto c2 = c1->builder_configs_->istructure_ptr;
        auto copy = i_dynamic_pointer_cast<U>(c2);
        if (!placement_configs_->is_builder) {
            // assembly
            copy->placement_configs_->children.clear();
            for (auto &child : placement_configs_->children) {
                auto builderview_clone = child->builder_configs_->builder_view->ForkForPlacement(
                    std::nullopt, "", true);
                auto clone_istructure_view = builderview_clone->builder_configs_->istructure_ptr->
                                                                ToStructureView();
                copy->placement_configs_->children.emplace_back(clone_istructure_view);
            }
        }
        return DERIVED(copy);
    }

    template <typename U>
    DERIVED BASE::ForkForLogicalVolume(const G4String &new_name) {
        // Polymorphic clone through builder view method.
        auto c1 = this->builder_configs_->builder_view->ForkForLogicalVolume(new_name);
        auto c2 = c1->builder_configs_->istructure_ptr;
        auto copy = i_dynamic_pointer_cast<U>(c2);
        if (!placement_configs_->is_builder) {
            // assembly
            copy->placement_configs_->children.clear();
            for (auto &child : placement_configs_->children) {
                auto builderview_clone = child->builder_configs_->builder_view->ForkForLogicalVolume(
                    new_name);
                auto clone_istructure_view = builderview_clone->builder_configs_->istructure_ptr->
                                                                ToStructureView();
                copy->placement_configs_->children.emplace_back(clone_istructure_view);
            }
        }
        return DERIVED(copy);
    }

    template <typename U>
    DERIVED BASE::CopyPlacementConfigsFrom(
        const BuilderView &other) {
        // we just copy THIS structure's configs.. no recursion.
        this->builder_configs_->builder_view->CopyPlacementConfigsFrom(other);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetDefaultUnit(
        G4double unit) {
        // Transforms are all stored INTERNALLY with units... so we can safely
        // set the interface unit on each hierachy.
        this->builder_configs_->builder_view->SetDefaultUnit(unit);
        return this->shared_from_this();
    }


    template <typename U>
    G4double BASE::GetEffectiveDefaultUnit() const {
        return this->builder_configs_->builder_view->GetEffectiveDefaultUnit();
    }

    template <typename U>
    G4String BASE::GetBuilderName() const {
        return this->builder_configs_->builder_view->GetBuilderName();
    }

    template <typename U>
    BASE::StructureBuilder() {
        lv_configs_->vis_att = G4VisAttributes(true);
        placement_configs_->is_builder = false;
    }


    template <typename U>
    void BASE::ValidateForPVBuild(std::string const &site) {
        builder_configs_->builder_view->ValidateForPVBuild(site);
    }

    template <typename U>
    void BASE::ValidatePlacementNotBuilt(
        const std::string &operation) const {
        builder_configs_->builder_view->ValidatePlacementNotBuilt(operation);
    }

    template <typename U>
    StructureView BASE::ToStructureView() const {
        return builder_configs_->istructure_ptr->ToStructureView();
    }

    template <typename U>
    BuilderView BASE::ToBuilderView() const {
        // calls the BuilderView copy/convert ctor::
        // presently the i_shared converter only works with l-value.
        std::shared_ptr<U> builder_std_ptr =
            std::const_pointer_cast<U>(this->shared_from_this());
        auto x = DerivedPtr(builder_std_ptr);
        return BuilderView(x);
    }


    template <typename U>
    SharedPtr<IStructureBuilder> BASE::clone_impl() const {
        // consider moving this to Assembly derived class since nothing else uses the base implementation.
        const U &derived_ref = static_cast<const U &>(*this); // downcast
        auto retval = new U(derived_ref);                     // copy
        auto shared_ptr = i_shared_ptr<U>(retval);
        BuilderView builder_view = shared_ptr->ToBuilderView();
        builder_view->SetName(this->GetBuilderName());
        //We're storing the view in the builder by writing to the builder through that same view!
        builder_view->StoreBuilderView(builder_view);

        return shared_mutable_this(retval); // wrap and return.
    }

    template <typename U>
    G4String BASE::GetLogicVolName() const {
        return this->builder_configs_->builder_view->GetLogicVolName();
    }

#undef BASE
#undef DERIVED
}

//reveiw explict name parameter,  copy algorithm, name propogation...
#endif
