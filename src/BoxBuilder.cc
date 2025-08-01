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
#include "StructureBuilder.hh"
#include "StructureBuilder.hpp"
#include "VolumeBuilderConfigs.hh"

class G4String;
using namespace DLG4::Utilities;

//Geant Box constructing methods //////////////////////////////////////////////////
namespace DLG4::VolumeBuilders {
    //Factories
    BoxBuilderPtr CreateBoxBuilder(G4double unit, const G4String &name, G4double end_x, G4double x_size,
            G4double end_y, G4double y_size, G4double end_z, G4double z_size) {
        auto object = BoxBuilderPtr(new BoxBuilder(name));
        object->SetXSize(abs(x_size * unit));
        object->SetYSize(abs(y_size * unit));
        object->SetZSize(abs(z_size * unit));
        object->SetInternalOffset(
                (end_x + x_size / 2.0) * unit,
                (end_y + y_size / 2.0) * unit,
                (end_z + z_size / 2.0) * unit);
        return object;
    }

    BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double end_x, G4double x_size, G4double end_y,
            G4double y_size, G4double end_z, G4double z_size) {
        return CreateBoxBuilder(BuilderConfigs::global_default_unit, name, end_x, x_size, end_y, y_size,
                end_z, z_size);
    }

    BoxBuilderPtr CreateBoxBuilder(G4double unit, const G4String &name, G4double x_size, G4double y_size,
            G4double end_z, G4double z_size) {
        if (x_size <= 0 || y_size <= 0) {
            throw std::invalid_argument("Error in CreateBoxBuilder(): for volume " + name
                                        + ": Sizes provided without end offsets must be positive.\n");
        }
        return CreateBoxBuilder(unit, name, -(x_size / 2.0), x_size, -(y_size / 2.0), y_size, end_z,
                z_size);
    }

    BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double x_size, G4double y_size, G4double end_z,
            G4double z_size) {
        return CreateBoxBuilder(BuilderConfigs::global_default_unit, name, x_size, y_size, end_z, z_size);
    }

    BoxBuilderPtr CreateBoxBuilder(G4double unit, const G4String &name, G4double x_size, G4double y_size,
            G4double z_size) {
        if (x_size <= 0 || y_size <= 0 || z_size <= 0) {
            throw std::invalid_argument("Error in CreateBoxBuilder(): for volume " + name
                                        + ": Sizes provided without end offsets must be positive.\n");
        }
        return CreateBoxBuilder(unit, name, -(x_size / 2.0), x_size, -(y_size / 2.0), y_size,
                -(z_size / 2.0), z_size);
    }

    BoxBuilderPtr CreateBoxBuilder(const G4String &name, G4double x_size, G4double y_size,
            G4double z_size) {
        return CreateBoxBuilder(BuilderConfigs::global_default_unit, name, x_size, y_size, z_size);
    }

    BoxBuilderPtr BoxBuilder::SetXSize(G4double size) {
        x_size_ = size;
        return shared_from_this();
    }

    BoxBuilderPtr BoxBuilder::SetYSize(G4double size) {
        y_size_ = size;
        return shared_from_this();
    }

    BoxBuilderPtr BoxBuilder::SetZSize(G4double size) {
        z_size_ = size;
        return shared_from_this();
    }

    BoxBuilderPtr BoxBuilder::SetInternalOffset(G4double x, G4double y, G4double z) {
        this->builder_configs_->internal_offset = G4ThreeVector(x, y, z);
        PropagateTransform();
        return shared_from_this();
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

    G4VSolid* BoxBuilder::SolidConstructor(const G4String &name) {
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