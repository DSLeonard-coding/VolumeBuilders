/*
 * VolumeMaker.cc
 *
 *  Created on: Jun 29, 2024
 *      Author: D. S. Leonard
 */
// Presently includes src for VolumeMaker and GeantMultiPlane classes
// See header for documentation.

#include "BoxBuilder.hh"
#include <string>
#include <VolumeBuilderTypes.hh>
#include <G4Polycone.hh>
#include <G4Polyhedra.hh>
//#include "disableable_shared_from_this.hh"
#include <G4Box.hh>

#include "i_shared_ptr.hh"
#include "StructureBuilder.hpp"
#include "VolumeBuilderConfigs.hh"

class G4String;
using namespace DLG4::Utilities;

//Geant Box constructing methods //////////////////////////////////////////////////
namespace DLG4::VolumeBuilders {
    //Factories
    // Factory methods for creating a BoxBuilder. These have been rewritten to
    // delegate directly to the BoxBuilder's member methods.
    // Yes, AI cranked out the duplication and delegation here after telling it exactly how to!

    BoxBuilderPtr CreateBoxBuilder(const G4String &name) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        return object;
    }

    BoxBuilderPtr CreateBoxBuilder(const G4String &name, const G4double unit) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetDefaultUnit(unit);
        return object;
    }


    BoxBuilderPtr CreateDeltasBoxBuilder(const G4double unit, const G4String &name,
        const G4double edge_x,
        const G4double x_delta,
        const G4double edge_y, const G4double y_delta, const G4double edge_z,
        const G4double z_delta) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXEdgeDelta(unit, edge_x, x_delta)
              ->SetYEdgeDelta(unit, edge_y, y_delta)
              ->SetZEdgeDelta(unit, edge_z, z_delta);
        return object;
    }

    BoxBuilderPtr CreateDeltasBoxBuilder(const G4String &name, const G4double edge_x,
        const G4double x_delta,
        const G4double edge_y,
        const G4double y_delta, const G4double edge_z, const G4double z_delta) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXEdgeDelta(edge_x, x_delta)
              ->SetYEdgeDelta(edge_y, y_delta)
              ->SetZEdgeDelta(edge_z, z_delta);
        return object;
    }

    BoxBuilderPtr CreateZDeltaBoxBuilder(const G4double unit, const G4String &name,
        const G4double x_full_size,
        const G4double y_full_size,
        const G4double edge_z, const G4double z_delta) {
        if (x_full_size <= 0 || y_full_size <= 0) {
            throw std::invalid_argument("Error in CreateZDeltaBoxBuilder(): for volume " + name
                                        + ": Sizes provided without end offsets must be positive.\n");
        }
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXSize(unit, x_full_size)
              ->SetYSize(unit, y_full_size)
              ->SetZEdgeDelta(unit, edge_z, z_delta);
        return object;
    }

    BoxBuilderPtr CreateZDeltaBoxBuilder(const G4String &name, const G4double x_full_size,
        const G4double y_full_size, const G4double edge_z,
        const G4double z_delta) {
        if (x_full_size <= 0 || y_full_size <= 0) {
            throw std::invalid_argument("Error in CreateZDeltaBoxBuilder(): for volume " + name
                                        + ": Sizes provided without end offsets must be positive.\n");
        }
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXSize(x_full_size)
              ->SetYSize(y_full_size)
              ->SetZEdgeDelta(edge_z, z_delta);
        return object;
    }

    BoxBuilderPtr CreateCenteredBoxBuilder(const G4double unit, const G4String &name,
        const G4double x_full_size, const G4double y_full_size,
        const G4double z_full_size) {
        if (x_full_size <= 0 || y_full_size <= 0 || z_full_size <= 0) {
            throw std::invalid_argument("Error in CreateCenteredBoxBuilder(): for volume " + name
                                        + ": Sizes provided must be positive.\n");
        }
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXSize(unit, x_full_size)
              ->SetYSize(unit, y_full_size)
              ->SetZSize(unit, z_full_size);
        return object;
    }

    BoxBuilderPtr CreateCenteredBoxBuilder(const G4String &name, const G4double x_full_size,
        const G4double y_full_size,
        const G4double z_full_size) {
        if (x_full_size <= 0 || y_full_size <= 0 || z_full_size <= 0) {
            throw std::invalid_argument("Error in CreateCenteredBoxBuilder(): for volume " + name
                                        + ": Sizes provided must be positive.\n");
        }
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXSize(x_full_size)
              ->SetYSize(y_full_size)
              ->SetZSize(z_full_size);
        return object;
    }

    BoxBuilderPtr CreateEdgesBoxBuilder(const G4String &name, const G4double x_edge1,
        const G4double x_edge2, const G4double y_edge1,
        const G4double y_edge2, const G4double z_edge1, const G4double z_edge2) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXEdges(x_edge1, x_edge2)
              ->SetYEdges(y_edge1, y_edge2)
              ->SetZEdges(z_edge1, z_edge2);
        return object;
    }

    BoxBuilderPtr CreateEdgesBoxBuilder(const G4double unit, const G4String &name,
        const G4double x_edge1, const G4double x_edge2,
        const G4double y_edge1, const G4double y_edge2, const G4double z_edge1,
        const G4double z_edge2) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXEdges(unit, x_edge1, x_edge2)
              ->SetYEdges(unit, y_edge1, y_edge2)
              ->SetZEdges(unit, z_edge1, z_edge2);
        return object;
    }

    BoxBuilderPtr BoxBuilder::SetXSizeDimensioned(const G4double size) {
        x_size_ = size;
        return shared_from_this();
    }

    BoxBuilderPtr BoxBuilder::SetYSizeDimensioned(const G4double size) {
        y_size_ = size;
        return shared_from_this();
    }

    BoxBuilderPtr BoxBuilder::SetZSizeDimensioned(const G4double size) {
        z_size_ = size;
        return shared_from_this();
    }

    BoxBuilderPtr BoxBuilder::SetInternalOffsetDimensioned(const G4double x, const G4double y,
        const G4double z) {
        this->builder_configs_->internal_offset = G4ThreeVector(x, y, z);
        PropagateTransform();
        return shared_from_this();
    }

    // These methods  use the builder's effective default unit.
    BoxBuilderPtr BoxBuilder::SetXSize(const G4double x_size) {
        return SetXSize(this->GetEffectiveDefaultUnit(), x_size);
    }

    BoxBuilderPtr BoxBuilder::SetYSize(const G4double y_size) {
        return SetYSize(this->GetEffectiveDefaultUnit(), y_size);
    }

    BoxBuilderPtr BoxBuilder::SetZSize(const G4double z_size) {
        return SetZSize(this->GetEffectiveDefaultUnit(), z_size);
    }

    BoxBuilderPtr BoxBuilder::SetXEdges(const G4double x_edge1, const G4double x_edge2) {
        return SetXEdges(this->GetEffectiveDefaultUnit(), x_edge1, x_edge2);
    }

    BoxBuilderPtr BoxBuilder::SetYEdges(const G4double y_edge1, const G4double y_edge2) {
        return SetYEdges(this->GetEffectiveDefaultUnit(), y_edge1, y_edge2);
    }

    BoxBuilderPtr BoxBuilder::SetZEdges(const G4double z_edge1, const G4double z_edge2) {
        return SetZEdges(this->GetEffectiveDefaultUnit(), z_edge1, z_edge2);
    }

    BoxBuilderPtr BoxBuilder::SetXEdgeDelta(const G4double x_edge, const G4double x_delta) {
        return SetXEdgeDelta(this->GetEffectiveDefaultUnit(), x_edge, x_delta);
    }

    BoxBuilderPtr BoxBuilder::SetYEdgeDelta(const G4double y_edge, const G4double y_delta) {
        return SetYEdgeDelta(this->GetEffectiveDefaultUnit(), y_edge, y_delta);
    }

    BoxBuilderPtr BoxBuilder::SetZEdgeDelta(const G4double z_edge, const G4double z_delta) {
        return SetZEdgeDelta(this->GetEffectiveDefaultUnit(), z_edge, z_delta);
    }

    BoxBuilderPtr BoxBuilder::SetInternalOffset(const G4double x, const G4double y,
        const G4double z) {
        return SetInternalOffset(this->GetEffectiveDefaultUnit(), x, y, z);
    }

    // These overloads handle the unit conversion and then delegate to the "Dimensioned" methods.
    BoxBuilderPtr BoxBuilder::SetXSize(const G4double unit, const G4double x_size) {
        return SetXSizeDimensioned(x_size * unit);
    }

    BoxBuilderPtr BoxBuilder::SetYSize(const G4double unit, const G4double y_size) {
        return SetYSizeDimensioned(y_size * unit);
    }

    BoxBuilderPtr BoxBuilder::SetZSize(const G4double unit, const G4double z_size) {
        return SetZSizeDimensioned(z_size * unit);
    }

    // These methods calculate the size and offset, apply the unit, and then delegate to the Dimensioned setters.
    BoxBuilderPtr BoxBuilder::SetXEdges(const G4double unit, const G4double x_edge1,
        const G4double x_edge2) {
        G4double size = std::abs(x_edge2 - x_edge1);
        G4double offset = (x_edge1 + x_edge2) / 2.0;
        SetXSizeDimensioned(size * unit);
        return SetInternalOffsetDimensioned(
            offset * unit,
            this->builder_configs_->internal_offset.y(),
            this->builder_configs_->internal_offset.z()
            );
    }

    BoxBuilderPtr BoxBuilder::SetYEdges(const G4double unit, const G4double y_edge1,
        const G4double y_edge2) {
        G4double size = std::abs(y_edge2 - y_edge1);
        G4double offset = (y_edge1 + y_edge2) / 2.0;
        SetYSizeDimensioned(size * unit);
        return SetInternalOffsetDimensioned(
            this->builder_configs_->internal_offset.x(),
            offset * unit,
            this->builder_configs_->internal_offset.z()
            );
    }

    BoxBuilderPtr BoxBuilder::SetZEdges(const G4double unit, const G4double z_edge1,
        const G4double z_edge2) {
        G4double size = std::abs(z_edge2 - z_edge1);
        G4double offset = (z_edge1 + z_edge2) / 2.0;
        SetZSizeDimensioned(size * unit);
        return SetInternalOffsetDimensioned(
            this->builder_configs_->internal_offset.x(),
            this->builder_configs_->internal_offset.y(),
            offset * unit
            );
    }

    // These methods calculate the size and offset from an edge and delta, apply the unit, and delegate.
    BoxBuilderPtr BoxBuilder::SetXEdgeDelta(const G4double unit, const G4double x_edge,
        const G4double x_delta) {
        G4double size = std::abs(x_delta);
        G4double offset = x_edge + x_delta / 2.0;
        SetXSizeDimensioned(size * unit);
        return SetInternalOffsetDimensioned(
            offset * unit,
            this->builder_configs_->internal_offset.y(),
            this->builder_configs_->internal_offset.z()
            );
    }

    BoxBuilderPtr BoxBuilder::SetYEdgeDelta(const G4double unit, const G4double y_edge,
        const G4double y_delta) {
        G4double size = std::abs(y_delta);
        G4double offset = y_edge + y_delta / 2.0;
        SetYSizeDimensioned(size * unit);
        return SetInternalOffsetDimensioned(
            this->builder_configs_->internal_offset.x(),
            offset * unit,
            this->builder_configs_->internal_offset.z()
            );
    }

    BoxBuilderPtr BoxBuilder::SetZEdgeDelta(const G4double unit, const G4double z_edge,
        const G4double z_delta) {
        G4double size = std::abs(z_delta);
        G4double offset = z_edge + z_delta / 2.0;
        SetZSizeDimensioned(size * unit);
        return SetInternalOffsetDimensioned(
            this->builder_configs_->internal_offset.x(),
            this->builder_configs_->internal_offset.y(),
            offset * unit
            );
    }

    BoxBuilderPtr BoxBuilder::SetInternalOffset(const G4double unit, const G4double x,
        const G4double y, const G4double z) {
        return SetInternalOffsetDimensioned(x * unit, y * unit, z * unit);
    }

    BoxBuilder::BoxBuilder(const G4String &name) {
        this->builder_configs_->name = name;
    }

    //Private default copy ctor.
    //This is used by clone methods of concrete classes
    BoxBuilder::BoxBuilder(const BoxBuilder &other) : VolumeBuilder<BoxBuilder>(other),
                                                      // Call base class copy constructor
                                                      x_size_(other.x_size_),
                                                      y_size_(other.y_size_),
                                                      z_size_(other.z_size_) {
        set_shared_from_this_enabled(false);
        SetName(other.GetBuilderName());
        builder_configs_->internal_offset = other.builder_configs_->internal_offset;
        set_shared_from_this_enabled(true);
    }

    G4VSolid *BoxBuilder::SolidConstructor(const G4String &name) {
        if (this->solid_ptr_.get() != nullptr) {
            std::string error = "Error in BoxBuilder::SolidConstructor: A solid was already built\n"
                "You can copy and rename the builder to reset it and build again.";
            throw std::runtime_error(error);
        }
        G4cout << G4endl;
        G4cout << "New BoxBuilder:" << GetBuilderName() // lie and use the final transformed name
            << "x_size: " << x_size_ << " y_size: " << y_size_ << " z_size: " << z_size_ <<
            G4endl;
        G4cout << "Internal offset: " << this->builder_configs_->internal_offset << G4endl;
        return new G4Box(name, x_size_ / 2.0, y_size_ / 2.0, z_size_ / 2.0);
    }
}
