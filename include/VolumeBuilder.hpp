#pragma once
#ifndef VOLUMEMAKER_HPP  // for code analysis
#define VOLUMEMAKER_HPP
// Header implementation for VolumeMaker.hh
// DO NOT INCLUDE THIS FILE DIRECTLY
// INCLUDE VolumeBuilder.hh
//
// Created by Douglas S. Leonard on 6/8/25.
// Implicit or explicit misrepresentation authorship is not protected under any license.
// See surrounding files for licesne
//
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SubtractionSolid.hh>
#include "Linkable.hh"
#include "VolumeBuilderReference.hh"
#include "VolumeBuilderTypes.hh"
#include "StructureBuilderReference.hh"
#include "Assembly.hh"
#include <memory>

// this will never be netered because we are ALWAYS entered FROM here:
// but it helps code analysis
#include <G4IntersectionSolid.hh>
#include <G4ReflectedSolid.hh>

#include "VolumeBuilder.hh"

namespace DLG4::VolumeBuilders {
    // used in CRTP implementation, should be improved probably.
#define BASE VolumeBuilder<U>
#define DERIVED typename BASE::DerivedPtr

    //First: non-templated implementations

    // These became trivial, but leaving them for future use.
    inline void VolumeConfigs::copyFrom(const VolumeConfigs &other) {
        //TODO:  What's this about?
    }

    inline
    VolumeConfigs::VolumeConfigs(const VolumeConfigs &other) : VolumeConfigsValueTypes(other) {
        copyFrom(other);
    }

    inline VolumeConfigs &VolumeConfigs::operator=(const VolumeConfigs &other) {
        copyFrom(other);
        return *this;
    }


    // Now templates

    // VolumeMaker methods //////////////////////////////////////////////////
    template <typename U>
    BASE::VolumeBuilder() : solid_ptr_(),
                            logicvol_ptr_(),
                            placement_() {
        lv_configs_->material = G4Material::GetMaterial("VolumeBuilderDefaultGas");

        if (!lv_configs_->material) {
            // If lv_configs_->material is still nullptr (meaning it wasn't found),
            // then create the new material and assign it.
            lv_configs_->material = new G4Material(
                // This is Geant. Of course we won't delete it :)
                "VolumeBuilderDefaultGas", 2., 4. * CLHEP::g / CLHEP::mole,
                0. * CLHEP::mg / CLHEP::cm3, kStateGas,
                4.3 * CLHEP::kelvin, 1.e-8 * CLHEP::bar
                );
        }
        // lv_configs_->material = new G4Material( // This is Geant.  Of course we won't delete it :)
        //         "VolumeBuilderDefaultGas", 2., 4. * CLHEP::g / CLHEP::mole,
        //         0. * CLHEP::mg / CLHEP::cm3, kStateGas,
        //         4.3 * CLHEP::kelvin, 1.e-8 * CLHEP::bar);
        //        lv_configs_->vis_att = std::make_shared<G4VisAttributes>(true);
        lv_configs_->vis_att = G4VisAttributes(true);
    }

    // copy ctor, called by derived copy ctor
    template <typename U>
    BASE::VolumeBuilder(const BASE &other): builder_configs_(other.builder_configs_),
                                            boolean_configs_(other.boolean_configs_),
                                            lv_configs_(other.lv_configs_),
                                            placement_configs_(other.placement_configs_) {
        // avoid masked bugs from stale view:
        StoreBuilderView(nullptr);
        StoreIStructurePtr(nullptr);
        // don't copy products since they can only be linked once.
        // Copy methods will do that.
    }


    //ctor to link to another builder
    //  If we put all our data in a common base class, we wouldn't need to
    // template this.  Is that better?  Enh... Whatever.
    template <typename U>
    template <typename T, std::enable_if_t<std::is_base_of_v<IStructureBuilder, T>, int>>
    DLG4::VolumeBuilders::VolumeBuilder<U>::VolumeBuilder(
        const SharedPtr<T> &other,
        SET_LINK_TYPE) : builder_configs_(other->builder_configs_,SET_LINK),
                         boolean_configs_(other->boolean_configs_,SET_LINK),
                         lv_configs_(other->lv_configs_, SET_LINK),
                         placement_configs_(other->placement_configs_,SET_LINK),
                         solid_ptr_(other->solid_ptr_,SET_LINK),
                         logicvol_ptr_(other->logicvol_ptr_,SET_LINK),
                         placement_(other->placement_,SET_LINK) {
        builder_configs_->istructure_ptr = IStructurePtr(other);
    }


    template <typename U>
    BASE::~VolumeBuilder() {
        // optionally release resources (default)
        // ReSharper disable once CppIfCanBeReplacedByConstexprIf
        if (!has_ownership_) {
            // This is SO wrong.  This should be done at CREATION of members,
            // not deletion of class.
            // But it works for now and we're super restrictive on creation.
            final_solid_ptr_.make_persistent();
            solid_ptr_.make_persistent();
            logicvol_ptr_.make_persistent();
            placement_.make_persistent();
        }
    }

    template <typename U>
    DERIVED BASE::MakeLogicalVolume(G4Material *material, G4String name) {
        ValidateForVolumeBuild(STRINGIFY(BASE) "MakeLogicalVolume");
        //if no volume was previously built, should be safe to reset material.
        if (material) {
            lv_configs_->material = material;
        }
        if (name.empty()) {
            name = this->GetLogicVolName() + "_L";
        }
        // Implicit construction through
        // ctor args..
        logicvol_ptr_.ConstructAndLink(final_solid_ptr_,
            lv_configs_->material,
            name);

        ApplyAttributes_();
        return this->shared_from_this();
    }

    template <typename U>
    G4String BASE::GetLogicVolName() const {
        if (!boolean_configs_->boolean_name.empty()) {
            return boolean_configs_->boolean_name;
        } else {
            boolean_configs_->boolean_name = builder_configs_->name;
        }
        return boolean_configs_->boolean_name;
    }

    template <typename U>
    DERIVED BASE::SetBooleanName(const G4String &name) {
        this->boolean_configs_->boolean_name = name;
        return this->shared_from_this();
    }

    template <typename U>
    G4VSolid *BASE::GetBaseSolid() {
        if (!this->solid_ptr_) {
            MakeSolid();
        }
        return solid_ptr_.get_mutable();
    }

    template <typename U>
    G4VPhysicalVolume *BASE::GetPlacement() {
        if (!this->placement_) {
            BASE::MakePlacement();
        }
        return this->placement_.get_mutable();
    }

    template <typename U>
    G4LogicalVolume *BASE::GetLogicalVolume() {
        if (!logicvol_ptr_) {
            BASE::MakeLogicalVolume();
        }
        return logicvol_ptr_.get_mutable();
    }

    //The separation here was made to allow
    //the non-fluent _impl calls to be used in ctors, but the new disable_shared_from_this
    // solves that anyway. These can probably be consolidated now.
    template <typename U>
    DERIVED BASE::SetLogicalVolume(G4LogicalVolume *logical_volume) {
        SetLogicalVolume_impl(logical_volume);
        return this->shared_from_this();
    }

    //non-fluent implementation
    template <typename U>
    void BASE::SetLogicalVolume_impl(G4LogicalVolume *logical_volume) {
        try {
            this->logicvol_ptr_.LinkToRaw(logical_volume);
            this->logicvol_ptr_.make_persistent();
        } catch (const std::logic_error &) {
            std::string error =
                "Error in SetLogicalVolume()"
                "for builder named: \"" + builder_configs_->name + "\""
                "  A Logical Volume has already been set or built\n"
                "Start with a new builder or copy a configured one with "
                "builder->SetPlacementBuilder(\"newname\")";
            throw std::runtime_error(error);
        }
    }

    //generic setsolid version
    //FIXME: remove impl
    template <typename U>
    DERIVED BASE::SetSolid(G4VSolid *solid) {
        SetSolid_impl(solid);
        return this->shared_from_this();
    }

    // non-fluent implementation:
    template <typename U>
    void BASE::SetSolid_impl(G4VSolid *solid) {
        try {
            this->solid_ptr_.LinkToRaw(solid);
            this->solid_ptr_.make_persistent();
        } catch (const std::logic_error &) {
            std::string error =
                "Error in SetLogicalVolume();"
                "for builder named: \"" + builder_configs_->name + "\"\n"
                "  A Logical Volume has already been set or built\n"
                "Start with a new builder or copy a configured one with "
                "builder->SetPlacementBuilder(\"newname\")";
            throw std::runtime_error(error);
        }
    }


    // End of Setsolid versions

    template <typename U>
    DERIVED BASE::AddUnion(const BuilderView &other, const Unit3Vec &new_offset,
        G4RotationMatrix *rotation) {
        bool is_subtraction = false;
        bool is_intersection = false;
        AddBoolean(other, is_subtraction, is_intersection, new_offset, rotation);
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::AddSubtraction(const BuilderView &other, const Unit3Vec &new_offset,
        G4RotationMatrix *rotation) {
        if (other) {
            bool is_subtraction = true;
            bool is_intersection = false;
            AddBoolean(other, is_subtraction, is_intersection, new_offset, rotation);
        } else {
            throw std::runtime_error("Error in AddBoolean(): "
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     "volume is invalid (null)");
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::AddIntersection(const BuilderView &other, const Unit3Vec &new_offset,
        G4RotationMatrix *rotation) {
        if (other) {
            bool is_subtraction = false;
            bool is_intersection = true;
            AddBoolean(other, is_subtraction, is_intersection, new_offset, rotation);
        } else {
            throw std::runtime_error("Error in AddBoolean() "
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     " volume is invalid (null)");
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::AddBoolean(const BuilderView &other, bool is_subtraction,
        bool is_intersection, const Unit3Vec &new_offset, G4RotationMatrix *rotation) {
        auto offset = ProvisionUnits(new_offset);

        if (other) {
            boolean_configs_->booleans.emplace_back(
                BooleanSolid{other, is_subtraction, is_intersection, offset, rotation});
        } else {
            throw std::runtime_error("Error in AddBoolean() "
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     " volume is invalid (null)");
        }
        return this->shared_from_this();
    }

    template <typename U>
    G4VSolid * BASE::GetFinalSolid() {
        if (!final_solid_ptr_ && placement_configs_->is_builder) {
            MakeFinalSolid(std::string("")); // checks are done in there.
        }
        // should probably have a check here, although it's a bug anyway if not built now.
        return this->final_solid_ptr_;
    }


    template <typename U>
    DERIVED BASE::MakeFinalSolid(G4String boolean_name) {
        this->ValidateForBooleanBuild(STRINGIFY(BASE) "MakeBooleans");
        if (boolean_name.empty()) {
            // may rename unimplemented
            boolean_name = builder_configs_->name;
        }
        SetBooleanName(boolean_name);
        G4String name_temp;
        G4String final_name;
        //  if no booleans, we forward the original to the end result.
        G4VSolid *temp_ptr = solid_ptr_.get_mutable();
        if (boolean_configs_->boolean_name.empty()) {
            final_name = builder_configs_->name + "_B";
        } else {
            final_name = boolean_configs_->boolean_name;
        }
        for (size_t i = 0; i < boolean_configs_->booleans.size(); i++) {
            auto boolean = boolean_configs_->booleans[i];
            const bool is_last = (i == (boolean_configs_->booleans.size() - 1)) && !boolean_configs_
                                 ->reflect_z;
            const size_t count = i + 1;
            if (boolean_configs_->boolean_name.empty()) {
                name_temp = builder_configs_->name + "_B" + std::to_string(count);
            }
            if (is_last) {
                name_temp = final_name;
            }
            auto count_str = std::to_string(count); //union number string
            if (!boolean.vol_ref->solid_ptr_) {
                // lazy trigger solid construction:
                boolean.vol_ref->MakeSolid();
            }
            if (boolean.is_subtraction) {
                temp_ptr = new G4SubtractionSolid(
                    name_temp,
                    temp_ptr,
                    boolean.vol_ref->solid_ptr_.get_mutable(),
                    boolean.rotation,
                    boolean.offset
                    + ((boolean.rotation != nullptr)
                           ? *boolean.rotation * boolean.vol_ref->builder_configs_->
                                                         internal_offset
                           : 0 * boolean.offset)
                    - this->builder_configs_->internal_offset
                    );
            } else if (boolean.is_intersection) {
                temp_ptr = new G4IntersectionSolid(
                    name_temp,
                    temp_ptr,
                    boolean.vol_ref->solid_ptr_.get_mutable(),
                    boolean.rotation,
                    boolean.offset
                    + ((boolean.rotation != nullptr)
                           ? *boolean.rotation * boolean.vol_ref->builder_configs_->
                                                         internal_offset
                           : 0 * boolean.offset)
                    - this->builder_configs_->internal_offset
                    );
            } else {
                temp_ptr = new G4UnionSolid(
                    name_temp,
                    temp_ptr,
                    boolean.vol_ref->solid_ptr_.get_mutable(),
                    boolean.rotation,
                    boolean.offset
                    + ((boolean.rotation != nullptr)
                           ? *boolean.rotation * boolean.vol_ref->builder_configs_->
                                                         internal_offset
                           : 0 * boolean.offset)
                    - this->builder_configs_->internal_offset
                    );
            }
        }
        if (boolean_configs_->reflect_z) {
            temp_ptr = new G4ReflectedSolid(final_name,
                temp_ptr,
                G4ReflectZ3D(0));
            auto io = this->builder_configs_->internal_offset;
            // flip internal offset
            this->builder_configs_->internal_offset = {io.getX(), io.getY(), -io.getZ()};
        }

        // we get one shot to link the final solid.
        if (temp_ptr) {
            final_solid_ptr_.LinkToRaw(temp_ptr);
        } else {
            throw std::runtime_error("Unknown Error in MakeBooleans "
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     "Failed to construct final solid.");
        }

        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetName(const G4String &name) {
        this->builder_configs_->name = name;
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetMaterial(G4Material *material) {
        lv_configs_->material = material;
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetColor(const double r, const double g, const double b, const double alpha) {
        lv_configs_->color = G4Colour(r, g, b, alpha);
        ApplyAttributes_();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetAlpha(double alpha) {
        lv_configs_->color.SetAlpha(alpha);
        ApplyAttributes_();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetColor(const G4Colour &color) {
        lv_configs_->color = color;
        ApplyAttributes_();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::ForceSolid(bool x) {
        lv_configs_->force_solid = x;
        ApplyAttributes_();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetVisibility(bool x) {
        lv_configs_->is_visible = x;
        ApplyAttributes_();
        return this->shared_from_this();
    }


    template <typename U>
    // ReSharper disable once CppMemberFunctionMayBeConst
    void BASE::ApplyAttributes_() {
        if (logicvol_ptr_) {
            lv_configs_->vis_att.SetVisibility(lv_configs_->is_visible);
            lv_configs_->vis_att.SetColor(lv_configs_->color); // blue
            lv_configs_->vis_att.SetForceSolid(lv_configs_->force_solid);
            //            logicvol_ptr_->SetVisAttributes(lv_configs_->vis_att.get());
            logicvol_ptr_->SetVisAttributes(lv_configs_->vis_att);
        } // will be applied when a logical volume is created.
    }

    template <typename U>
    DERIVED BASE::MakePlacement() {
        // the copy part comes from validate below, if we're already built:
        ValidateForPVBuild(STRINGIFY(BASE) "MakePlacement");
        // 1. Get the logical volume for *this* builder
        auto *currentLogical = this->GetLogicalVolume();
        if (!(explicit_physical_copy_name_set_
              || explicit_copyno_set_
              || placement_configs_->auto_copy_name
              || placement_configs_->auto_copyno)
        ) {
            G4cout << "Warning: Placement Builder for " + builder_configs_->name +
                " copied without setting name or copy number and auto naming and numbering have been disabled \n"
                "Copy number auto-incrementing will be re-enabled \n"
                "  Use SetAutoCopyNo(true), SetAuCopyName(true), OverridePlacementName(\"name\"),  SetCopyNo(num)  \n"
                " to pass a name or number to MakePlacement()" << G4endl;
            placement_configs_->copy_no = PlacementNameRegistry::GetNameCount(
                GetPlacementBaseName());
        }

        // 2. Resolve the Mother Logical Volume
        G4LogicalVolume *effectiveMotherLogical = nullptr;

        G4String base_name = this->GetPlacementBaseName();
        G4String final_name;
        if (placement_configs_->auto_copy_name) {
            // auto naming incrementing
            final_name = base_name + "_P" +
                         std::to_string(
                             PlacementNameRegistry::GetNameCount(GetPlacementBaseName()));
        } else {
            final_name = GetPlacementBaseName() + "_P";
        }
        // unless an override was set, then take it verbatim, user's blame :):
        if (!placement_name_override_.empty()) {
            final_name = placement_name_override_;
        }

        if (placement_configs_->mother == nullptr || !placement_configs_->mother->
            GetLogicalVolume()) {
            // will lazay trigger mother build if needed
            G4cout << "WARNING in MakePlacement of " + final_name +
                ". No mother volume was set or constructable.\n"
                "Defaulting to world volume" << G4endl;
        }

        if (placement_configs_->mother) {
            effectiveMotherLogical = placement_configs_->mother->GetLogicalVolume();
        } else {
            effectiveMotherLogical = nullptr; // world placement
        }

        auto transform = G4Transform3D(placement_configs_->total_rotation,
            placement_configs_->total_translation);
        // 4. Create the G4PVPlacement
        // placement is <G4VPhysicalVolume> and can construct itself:
        placement_.LinkToRaw(new G4PVPlacement(
            transform, // G4Transform3D transform
            currentLogical, // pCurrentLogical
            final_name, // pName
            effectiveMotherLogical, // pMotherLogical
            false, // many option, only false allowed (unless you remember Geant3 :))
            placement_configs_->copy_no, // pCopyNo
            placement_configs_->surface_check // pSurfChk
            ));
        if (!placement_) {
            throw std::runtime_error(
                "MakeLogicalPlacement failed "
                "for builder named: \"" + builder_configs_->name + "\"\n"
                " pointer is null after calling G4PVPlacement().");
        }
        return this->shared_from_this();
    }

    //  SetRotation
    template <typename U>
    DERIVED BASE::SetPhysRotation(const G4RotationMatrix &rot) {
        G4RotationMatrix nullrot;
        this->placement_configs_->rotation = rot;
        PropagateTransform();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::StackPhysRotation(const G4RotationMatrix &rot) {
        // some aliases
        const auto &old_rot = placement_configs_->rotation;
        const auto &old_trans = placement_configs_->translation;
        // sanitize input
        auto new_rot = rot * old_rot; // order matters.
        auto new_trans = rot * old_trans;
        this->placement_configs_->rotation = new_rot;
        this->placement_configs_->translation = new_trans;
        PropagateTransform();
        return this->shared_from_this();
    }

    // SetTranslation
    template <typename U>
    DERIVED BASE::SetPhysOffset(const Unit3Vec &new_offset) {
        const auto &offset = ProvisionUnits(new_offset);
        placement_configs_->translation = offset;
        PropagateTransform();
        return this->shared_from_this();
    }

    // SetTranslation
    template <typename U>
    DERIVED BASE::StackPhysOffset(const Unit3Vec &stacked_offset) {
        const auto &offset = ProvisionUnits(stacked_offset);
        placement_configs_->translation += offset; // add translation
        PropagateTransform();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetPhysTransform(const G4Transform3D &new_transform) {
        SetPhysRotation(new_transform.getRotation()); // gets a copy on stack
        SetPhysOffset({GetEffectiveDefaultUnit(), new_transform.getTranslation()});
        PropagateTransform();
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::StackPhysTransform(const G4Transform3D &new_transform) {
        //order matters when stacking.  Not when "setting"
        StackPhysRotation(new_transform.getRotation()); // gets a copy on stack
        StackPhysOffset({GetEffectiveDefaultUnit(), new_transform.getTranslation()});
        PropagateTransform();
        return this->shared_from_this();
    }

    template <typename U>
    void BASE::PropagateTransform() {
        // Apply transform of parent to relative transform of us, to get our total transform
        G4RotationMatrix total_rotation = this->placement_configs_->parent_rotation
                                          * this->placement_configs_->rotation;
        G4ThreeVector total_translation = this->placement_configs_->parent_translation
                                          + this->placement_configs_->parent_rotation
                                          * (
                                              this->placement_configs_->translation
                                              + this->placement_configs_->rotation
                                              * this->builder_configs_->internal_offset
                                          );
        if (!placement_configs_->is_builder) {
            // we're an assembly, apply total transform as parent transform to our children
            for (auto &child : placement_configs_->children) {
                child->placement_configs_->parent_rotation = total_rotation;
                child->placement_configs_->parent_translation = total_translation;
                child->builder_configs_->builder_view->PropagateTransform();
            }
        } else {
            // we're a concrete builder, so set our total transform
            placement_configs_->total_rotation = total_rotation;
            placement_configs_->total_translation = total_translation;
        }
    }

    // OverridePlacementName
    template <typename U>
    DERIVED BASE::OverridePlacementName(const G4String &pName) {
        explicit_physical_copy_name_set_ = static_cast<bool>(pName);
        this->placement_name_override_ = pName;
        return this->shared_from_this();
    }

    // SetCopyNo
    template <typename U>
    DERIVED BASE::SetCopyNo(G4int pCopyNo) {
        explicit_copyno_set_ = static_cast<bool>(pCopyNo);
        this->placement_configs_->copy_no = pCopyNo;
        return this->shared_from_this();
    }

    // SetSurfaceCheck
    template <typename U>
    DERIVED BASE::SetSurfaceCheck(G4bool pSurfChk) {
        this->placement_configs_->surface_check = pSurfChk;
        return this->shared_from_this();
    }

    // SetMother (builder pointer)
    template <typename U>
    DERIVED BASE::SetMother(const BuilderView &mother) {
        if (!mother) {
            throw std::runtime_error("Error in VolumeBuilder::SetMother,"
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     " no valid mother physical volume provided");
            // We cannot actually fully check this yet because we allow this to be a forward association
            // The logical volume is not required to be constructable before we make a placement.
        }
        this->placement_configs_->mother = mother;
        return this->shared_from_this();
    }


    template <typename U>
    DERIVED BASE::ForkAndReset(const G4String &new_name) const {
        NoNameCheck(new_name, "CopySolidBuilder");
        DerivedPtr copy = this->Clone();
        copy->SetName(new_name); // Set base name for derived class
        copy->SetBooleanName("");
        return copy;
    }

    template <typename U>
    DERIVED BASE::ForkForFinalSolid(const G4String &new_name) {
        NoNameCheck(new_name, "ForkForFinalSolid");
        if (!solid_ptr_) {
            MakeSolid();
        }
        DerivedPtr copy = this->Clone();
        copy->SetName(new_name); // Set base name for derived class
        copy->SetBooleanName("");
        // all products are cleared by default, (re)build up to Solid
        copy->solid_ptr_.Link(this->solid_ptr_);
        return copy;
    }

    template <typename U>
    DERIVED BASE::ForkForLogicalVolume(const G4String &new_name) {
        NoNameCheck(new_name, "ForkForLogicalVolume");
        if (!final_solid_ptr_ && placement_configs_->is_builder) {
            [[maybe_unused]] auto discard = GetFinalSolid();
        }
        DerivedPtr copy = this->Clone();
        copy->SetName(new_name); // Set base name for derived class
        copy->SetBooleanName("");
        // all products are cleared by default, (re)build up to Solid
        if (solid_ptr_ && placement_configs_->is_builder) {
            copy->solid_ptr_.Link(this->solid_ptr_);
        }
        if (final_solid_ptr_ && placement_configs_->is_builder) {
            copy->final_solid_ptr_.Link(this->final_solid_ptr_);
        }
        return copy;
    }

    template <typename U>
    DERIVED BASE::ForkForPlacement(std::optional<int> copy_no,
        const G4String &name_override, bool parent_name_was_set) {
        // really a clone of logical volume:
        if (!logicvol_ptr_ && placement_configs_->is_builder) {
            MakeLogicalVolume();
        }
        DerivedPtr copy = this->Clone();
        copy->explicit_copyno_set_ = static_cast<bool>(copy_no);
        copy->explicit_physical_copy_name_set_ = !name_override.empty() || parent_name_was_set;
        copy->placement_name_override_ = name_override;
        // register name and get provisional copy_no:
        copy->placement_configs_->copy_no = PlacementNameRegistry::IncrementNameCount(
            GetPlacementBaseName());
        //Override if copy_no was passed:
        copy->placement_configs_->copy_no = copy_no.value_or(copy->placement_configs_->copy_no);

        // Restore logical_volume pointer and below
        copy->solid_ptr_.Link(this->solid_ptr_);
        copy->final_solid_ptr_.Link(this->final_solid_ptr_);
        copy->logicvol_ptr_.Link(this->logicvol_ptr_);

        return copy;
    }


    template <typename U>
    DERIVED BASE::SetAutoPlacementNaming(const bool set) {
        placement_configs_->auto_copy_name = set;
        if (set) {
            placement_configs_->auto_copyno = false;
        }
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::SetAutoCopyNo(const bool set) {
        placement_configs_->auto_copyno = set;
        if (set) {
            placement_configs_->auto_copy_name = false;
        }
        return this->shared_from_this();
    }


    // 3. Determine the Placement Name
    template <typename U>
    G4String BASE::GetPlacementBaseName() const {
        // top priority is actually boolean name.  override name is not supposed to get
        // extended, but we'll fallback to it if it's all there is and if auto name incrementing ends up used.
        G4String base_name = boolean_configs_->boolean_name;
        if (base_name.empty()) {
            if (builder_configs_->name.empty()) {
                if (placement_name_override_.empty()) {
                    throw std::runtime_error("Err in DLG4::VolumeBuilders::"
                                             "for builder named: \"" + builder_configs_->name +
                                             "\"\n"
                                             "GetPlacementBaseName\n"
                                             "No names are defined.  Cannot build");
                } else {
                    base_name = placement_name_override_;
                }
            } else {
                base_name = builder_configs_->name;
            }
        }
        // prepend parent name, if any, for hierarchies
        if (!placement_configs_->parent_name.empty()) {
            base_name = placement_configs_->parent_name + ":" + base_name;
        }
        return base_name;
    }


    template <typename U>
    DERIVED BASE::CopyPlacementConfigsFrom(const BuilderView &other) {
        this->placement_configs_ = other->placement_configs_;
        // receiver is responsible for unique naming/numbering.
        // we can't clobber auto-name/number incrementing from another config:
        // Does not copy the solid/boolean, just to get configs ONLY
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::CopyVolumeConfigsFrom(const BuilderView &other) {
        // Booleans are NOT considered as part of Volume Configs.
        // We keep our booleans.
        auto name = this->builder_configs_->name; // backup our name
        auto booleans = this->boolean_configs_->booleans;
        this->lv_configs_ = other->lv_configs_;
        //   restor name.  This is easier than keeping name separately. :P
        this->builder_configs_->name = name;
        this->boolean_configs_->booleans = booleans; // restore ours.
        return this->shared_from_this();
    }


    template <typename U>
    void BASE::make_persistent(const std::shared_ptr<void> &obj) {
        std::lock_guard<std::mutex> lock(s_registry_mutex);
        black_hole.push_back(obj);
    }

    template <typename U>
    void BASE::ValidateSolidNotBuilt(const std::string &operation) const {
        if (solid_ptr_) {
            throw std::runtime_error("Cannot " + operation + " - solid already built "
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     "!");
        }
    }

    template <typename U>
    void BASE::ValidateBooleanNotBuilt(const std::string &operation) const {
        if (final_solid_ptr_) {
            throw std::runtime_error("Cannot " + operation + " - boolean already built!"
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                );
        }
    }

    template <typename U>
    void BASE::ValidateLogicalNotBuilt(const std::string &operation) const {
        if (logicvol_ptr_) {
            throw std::runtime_error("Cannot " + operation + " - logical volume already built!"
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                );
        }
    }

    template <typename U>
    void BASE::ValidatePlacementNotBuilt(const std::string &operation) const {
        if (placement_) {
            throw std::runtime_error("Cannot " + operation + " - placement already built!"
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                );
        }
    }

    /// Build chain validators:
    /// /
    //Validator for physical volume build:
    template <typename U>
    void BASE::ValidateForPVBuild(std::string const &site) {
        if (placement_) {
            throw std::runtime_error(">>> Error in " + site + " Physical Volume was already built\n"
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     "Use ForkForPlacement to copy and rebuild.");
        }
        // ReSharper disable once CppRedundantBooleanExpressionArgument
        if (!logicvol_ptr_ && enable_full_lazy_builds && placement_configs_->is_builder) {
            MakeLogicalVolume(); // will trigger ValidateForVolumeBuilder below
        }
        if (!logicvol_ptr_ && placement_configs_->is_builder) {
            throw std::runtime_error(
                "Error in " + site + ": LogicVolume is null after MakeLogicalVolume()."
                "for builder named: \"" + builder_configs_->name + "\"\n"
                );
        }
    }

    template <typename U>
    void BASE::ValidateForVolumeBuild(std::string const &site) {
        if (logicvol_ptr_) {
            std::string error = "Error in " + site + " Booleans were already built\n"
                                "for builder named: \"" + builder_configs_->name + "\"\n"
                                "You can copy and rename the builder to reset it and build again.\n";
            throw std::runtime_error(error);
        }
        // ReSharper disable once CppRedundantBooleanExpressionArgument
        if (!final_solid_ptr_ && enable_full_lazy_builds && placement_configs_->is_builder) {
            MakeFinalSolid(); // Will trigger ValidateForBooleanBuild below
        }
        if (!final_solid_ptr_ && placement_configs_->is_builder) {
            // MakeBooleans always set it.
            throw std::runtime_error(
                "Error in ValidateForVolumeBuild from " + site +
                "for builder named: \"" + builder_configs_->name + "\"\n"
                ": It's not possible to produce this error.");
        }
    }

    template <typename U>
    void BASE::ValidateForBooleanBuild(std::string const &site) {
        if (final_solid_ptr_) {
            std::string error = "Error in " + site + " A solid was already built\n"
                                "for builder named: \"" + builder_configs_->name + "\"\n"
                                "You can copy and rename the builder to reset it and build again.";
            throw std::runtime_error(error);
        }
        // ReSharper disable once CppRedundantBooleanExpressionArgument
        if (!solid_ptr_ && enable_full_lazy_builds && placement_configs_->is_builder) {
            MakeSolid(); // Trigger solid creation if not already done
        }
        if (!solid_ptr_ && placement_configs_->is_builder) {
            throw std::runtime_error("Error in " + site + ": "
                                     "for builder named: \"" + builder_configs_->name + "\"\n"
                                     "Solid is null after MakeSolid().");
        }
    }

    template <typename U>
    DERIVED BASE::SetDefaultUnit(G4double unit) {
        this->builder_configs_->default_unit = unit;
        return this->shared_from_this();
    }

    template <typename U>
    G4double BASE::GetEffectiveDefaultUnit() const {
        auto temp = builder_configs_.get();
        auto local = temp->default_unit;
        //        auto local = builder_configs_->default_unit;
        auto global = BuilderConfigs::global_default_unit;
        G4double default_unit = local.value_or(global);
        return default_unit;
    }

    template <typename U>
    DERIVED BASE::ReflectZFinalSolid() {
        if (final_solid_ptr_) {
            throw std::runtime_error("Error VolumeBuilder::ReflectZFinalSolid,  \n"
                "The final solid is already built.  \n");
        }
        boolean_configs_->reflect_z = true;
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::ReflectZBaseSolid() {
        if (final_solid_ptr_) {
            throw std::runtime_error("Error VolumeBuilder::ReflectZBaseSolid,  \n"
                "The base solid is already built.  \n");
        }
        builder_configs_->reflect_base_solid_z = true;
        return this->shared_from_this();
    }

    template <typename U>
    DERIVED BASE::MakeSolid() {
        G4VSolid *solid;
        G4String final_name = GetBuilderName();
        if (builder_configs_->reflect_base_solid_z) {
            solid = SolidConstructor(final_name + "_proto_solid");
            solid = new G4ReflectedSolid(final_name, solid, G4ReflectZ3D(0));
        } else {
            solid = SolidConstructor(final_name);
        }
        solid_ptr_.LinkToRaw(solid);
        return this->shared_from_this();
    }


    // apply the active default unit to a provided vector
    // unless the vector alerady has units.
    template <typename U>
    G4ThreeVector BASE::ProvisionUnits(const Unit3Vec &vec) const {
        // will apply default or internal unit.
        return vec.apply_units(GetEffectiveDefaultUnit());
    }

    template <typename U>
    void BASE::StoreIStructurePtr(const IStructurePtr &istructure_ptr) {
        // can only set this ONCE!!
        builder_configs_->istructure_ptr = IStructurePtr(istructure_ptr);
    }

    template <typename U>
    void BASE::StoreBuilderView(const BuilderView &builder_view) {
        // can only set this ONCE!!
        builder_configs_->builder_view = builder_view;
    }

    template <typename U>
    G4String BASE::GetBuilderName() const {
        return this->builder_configs_->name;
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
    StructureView BASE::ToStructureView() const {
        // calls the structure view copy/convert ctor:
        std::shared_ptr<U> builder_std_ptr =
            std::const_pointer_cast<U>(this->shared_from_this());
        auto x = DerivedPtr(builder_std_ptr);
        return StructureView(x);
    }

    template <typename U>
    SharedPtr<IStructureBuilder> BASE::clone_impl() const {
        const U &derived_ref = static_cast<const U &>(*this); // downcast
        auto retval = new U(derived_ref);                     // copy
        auto shared_ptr = i_shared_ptr<U>(retval);
        return shared_mutable_this(retval); // wrap and return.
    }

    template <typename U>
    DERIVED BASE::AddTo(BuilderViewList &list) const {
        list.emplace_back(this->ToBuilderView());
        auto retval = shared_mutable_this(this);
        return retval;
    }

    template <typename U>
    DERIVED BASE::AddTo(StructureViewList &list) const {
        list.emplace_back(this->ToStructureView());
        auto retval = shared_mutable_this(this);
        return retval;
    }

    template <typename U>
    DERIVED BASE::AddTo(AssemblyPtr &assembly) const {
        assembly->AddStructure(this->ToStructureView());
        auto retval = shared_mutable_this(this);
        return retval;
    }

#undef DERIVED
#undef BASE
} // namespace DLG4::VolumeBuilders

#endif  //VOLUMEMAKER_HPP
//TODO Check release build for -g omission.
