#pragma once
//
// Created by Douglas S. Leonard on 6/18/25.  All rights Reserved
// See related files for license, if any is provided.
//
// Types for VolumeBuilder classes

#ifndef VOLUMEBUILDERCONFIGS_HH
#define VOLUMEBUILDERCONFIGS_HH
#include <G4Colour.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4VisAttributes.hh>
#include <memory>
#include <optional>
#include <vector>
#include <CLHEP/Units/SystemOfUnits.h>

#include "disableable_shared_from_this.hh"
#include "i_shared_ptr.hh"

#define ONCE_MUTABLE mutable  // for linkable objects

namespace DLG4::VolumeBuilders::_internals_ {
    class VolumeBuilderCore;
    template <typename U>
    class VolumeBuilderBase;
    using UnitlessG4Transform3D = G4Transform3D;

    // Configurations for VolumeBuilder
    struct BooleanSolid {
        VolumeBuilder vol_ref;
        bool is_subtraction;
        bool is_intersection;
        G4ThreeVector offset{};
        G4RotationMatrix *rotation{};
    };

    struct BuilderConfigs {
        G4String name{}; // initial solid name
        std::optional<G4double> default_unit;
        bool reflect_base_solid_z;
        // actually global, but it needs to live somewhere untemplated.
        static G4double global_default_unit;
        // Interface pointer for type erased class access (Copied in linking ctor only)
        // Linkable<ISolidBuilder> isolid_ptr;
        // Linkable <VolumeBuetilderReference> builder_view;
        IStructurePtr istructure_ptr;
        VolumeBuilder builder_view;
        // an internal offset for non-centered solids:
        G4ThreeVector internal_offset{};
    };

    struct BooleanConfigs {
        std::vector<BooleanSolid> booleans;
        G4String boolean_name{}; //derived from name if not explicitly provided.
        bool reflect_z{false};
        BooleanConfigs() = default;
        ~BooleanConfigs() = default;
    };

    struct VolumeConfigsValueTypes {
        // everything defeault copyable, no ctor maintenace needed:
        G4Colour color{0.5, 0.5, 0.5, 1};
        bool force_solid{false};
        bool is_visible{true};
        G4Material *material{}; //G4 takes ownership
        G4VisAttributes vis_att{};
    };

    struct VolumeConfigs: public VolumeConfigsValueTypes {
        // the stuff with custom/deep copy
        //            std::shared_ptr<G4VisAttributes> vis_att{}; //G4 copies (when passed by value)
        // although some items in it are pointers.

        VolumeConfigs() = default;
        ~VolumeConfigs() = default;
        /// Copy constructor
        VolumeConfigs(const VolumeConfigs &other);
        /// Operator =
        VolumeConfigs &operator=(const VolumeConfigs &other);
        // base for copy and =
        void copyFrom(const VolumeConfigs &other);
    };

    // Set the default value for the global default unit.
    //  (The default for the default for the default).
    inline G4double BuilderConfigs::global_default_unit = CLHEP::mm;

    // Placement parameters
    struct PlacementConfigs {
        // the hierarchical assembly elements, if any
        std::vector<StructureBuilder> children;
        bool is_builder = true;
        G4RotationMatrix rotation{}; //G4 takes ownership
        G4ThreeVector translation = G4ThreeVector(0, 0, 0);
        G4Transform3D transform{};
        // for hierarchies
        G4RotationMatrix parent_rotation{};
        G4ThreeVector parent_translation = G4ThreeVector(0, 0, 0);
        // total summed transformation in hierarchy
        G4RotationMatrix total_rotation{}; //G4 takes ownership
        G4ThreeVector total_translation = G4ThreeVector(0, 0, 0);
        // component of name added from parent in hierarchy
        G4String parent_name = "";
        //G4 copies
        VolumeBuilder mother = nullptr; // Alternative mother source if raw is null
        G4int copy_no = 0;            // formal G4 copy_no_
        G4int copy_count = 0;         // used in placement naming if auto_copy_name == true;
        G4bool surface_check = false;
        G4bool auto_copyno = true;
        G4bool auto_copy_name = false;

        ~PlacementConfigs() = default;
    };
}
namespace DLG4::VolumeBuilders {
    /**
     * @brief Set the default unit for all VolumeBuilder methods.
     * @ingroup Units
     * */
    inline void SetGlobalDefaultUnit(G4double unit) {
        _internals_::BuilderConfigs::global_default_unit = unit;
    }
}
#endif //VOLUMEBUIDERTYPES_HH
//TODO Implement auto parent name incrementing?
