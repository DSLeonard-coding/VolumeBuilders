/**
* @file
* Created by @author Douglas S. Leonard on @date 6/29/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

#include "VolumeBuildersTypes.hh"
#include "ISolidBuilder.hh"
#include "VolumeBuilder.hh"
#include "VolumeBuilderReference.hh"
#include "StructureBuilderReference.hh"
#include "StructureBuilder.hpp"

namespace DLG4::VolumeBuilders::_internals_ {
    G4VSolid *VolumeBuilderReference::SolidConstructor(const G4String &name) {
        if (!this->builder_configs_->istructure_ptr) {
            throw std::runtime_error(
                "Error in VolumeBuilderReference::VolumeBuilderReference::SolidConstructor(const G4String &name) \n"
                "In VolumeBuilderReference::VolumeBuilderReference::SolidConstructor(const G4String &name): No solid configuration to construct on"
                + this->builder_configs_->name);
        }
        return this->builder_configs_->istructure_ptr->SolidConstructor(name);
    }


    VolumeBuilderReference::VolumeBuilderReference(G4VSolid *solid) { // NOLINT(*-explicit-constructor)
        // disable shared from this so we can use fluent calls... but NO CHAINING!!
        set_shared_from_this_enabled(false);
        this->SetSolid_impl(solid);
        this->SetName(solid->GetName());
        set_shared_from_this_enabled(true);
    }

    VolumeBuilderReference::VolumeBuilderReference(G4LogicalVolume *volume) { // NOLINT(*-explicit-constructor)
        set_shared_from_this_enabled(false);
        this->SetLogicalVolume_impl(volume);
        this->SetName(volume->GetName());
        set_shared_from_this_enabled(true);
    }

    VolumeBuilderReference::VolumeBuilderReference(G4VPhysicalVolume *volume) {
        // NOLINT(*-explicit-constructor)
        set_shared_from_this_enabled(false);
        if (volume) {
            // Extract the logical volume from the physical volume
            G4LogicalVolume *logical = volume->GetLogicalVolume();
            if (logical) {
                logicvol_ptr_.LinkToRaw(logical); // error here
            }
            // Store the physical volume reference
            placement_.LinkToRaw(volume);
            this->SetName(volume->GetName());
        }
        set_shared_from_this_enabled(true);
    }

    SharedPtr<IStructureBuilder> VolumeBuilderReference::clone_impl() const {
        // have the erased base clone itself:
        auto copy = builder_configs_->istructure_ptr->clone_impl();
        // Then have it create a builderPtr from itself:
        auto builder_view = copy->ToBuilderView();
        builder_view->StoreIStructurePtr(copy); // this is the owning copy.
        builder_view->StoreBuilderView(builder_view);

        //return static_cast<std::shared_ptr<ISolidBuilder>>(builder_view) ;  // and return the builder
        return builder_view; // and return the builder
    }
}

//
