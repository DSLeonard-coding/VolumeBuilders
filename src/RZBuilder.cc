/*
 * VolumeMaker.cc
 *
 *  Created on: Jun 29, 2024
 *      Author: D. S. Leonard
 */
// Presently includes src for VolumeMaker and GeantMultiPlane classes
// See header for documentation.

#include "RZBuilder.hh"
#include <string>
#include <VolumeBuildersTypes.hh>
#include <G4Polycone.hh>
#include <G4Polyhedra.hh>
//#include "disableable_shared_from_this.hh"
#include "i_shared_ptr.hh"
#include "StructureBuilder.hh"
#include "StructureBuilder.hpp"

class G4String;
using namespace DLG4::Utilities;

//GeantMultiPlane methods //////////////////////////////////////////////////
using namespace DLG4::VolumeBuilders::_internals_;

namespace DLG4::VolumeBuilders {
    //Factories
    RZBuilderPtr CreatePolyconeBuilder(const G4String &name, G4double phi_start, G4double phi_tot) {
        // We could actually construct these as RZBuilderPtr without using new, vis i_shared_ptr constructor.
        // But maybe we can unfriend i_shared_ptr in the future if we use new here, and avoid exposing
        // a backdoor to the general ctor, which doesn't select a MakeSolidFunctionPtr.
        // ReSharper disable once CppDFAMemoryLeak
        auto object = RZBuilderPtr(new RZBuilder(name, phi_start, phi_tot, 0));
        object->MakeSolidFunctionPtr_ = &RZBuilder::MakePolycone;
        return RZBuilderPtr(object);
    }

    RZBuilderPtr CreatePolyhedraBuilder(const G4String &name, int sides, G4double phi_start,
                                        G4double phi_tot) {
        // ReSharper disable once CppDFAMemoryLeak
        auto object = new RZBuilder(name, phi_start, phi_tot, sides);
        object->MakeSolidFunctionPtr_ = &RZBuilder::MakePolyhedra;
        return RZBuilderPtr(object);
    }

    RZBuilderPtr CreateCylinderBuilder(
        G4double unit, const G4String &name, G4double endz, G4double h, G4double OR, G4double IR) {
        // ReSharper disable once CppDFAMemoryLeak
        auto object = RZBuilderPtr(new RZBuilder(name));
        object->MakeSolidFunctionPtr_ = &RZBuilder::MakePolycone;
        RZPlane plane;
        plane.unit = unit;
        plane.z = endz;
        plane.IR = IR;
        plane.OR = OR;
        object->AddPlane(plane);
        plane.z += h;
        object->AddPlane(plane);
        return RZBuilderPtr(object);
    }

    RZBuilderPtr CreateCylinderBuilder(const G4String &name, G4double endz, G4double h, G4double OR,
        G4double IR) {
        return CreateCylinderBuilder(BuilderConfigs::global_default_unit, name, endz, h, OR, IR);
    }
}

namespace DLG4::VolumeBuilders::_internals_ {
    RZBuilder::RZBuilder(const G4String &name, G4double init_phi_start, G4double init_phi_tot,
        int init_sides) :
        VolumeBuilder<RZBuilder>(), sides_(init_sides), phi_start_deg_(init_phi_start),
        phi_tot_deg(init_phi_tot) {
        set_shared_from_this_enabled(false);
        SetName(name);
        set_shared_from_this_enabled(true);
    }

    //Private default copy ctor.
    //This is used by clone methods of concrete classes
    RZBuilder::RZBuilder(const RZBuilder &other) :
        VolumeBuilder<RZBuilder>(other), // Call base class copy constructor
        sides_(other.sides_), phi_start_deg_(other.phi_start_deg_), phi_tot_deg(other.phi_tot_deg),
        num_planes_(other.num_planes_), z_(other.z_), IR_(other.IR_), OR_(other.OR_),
        MakeSolidFunctionPtr_(other.MakeSolidFunctionPtr_) {
        set_shared_from_this_enabled(false);
        SetName(other.GetBuilderName());
        set_shared_from_this_enabled(true);
    }


    RZBuilderPtr RZBuilder::FillSolidConfig() {
        if (solid_ptr_) {
            throw std::runtime_error("Error in RZBuilder::FillSolidConfig\"\n"
                "Cannot fill a solid builder CONFIGURATION, when the solid is already built from it."
                "Use ForkAndReset(\"new_name\") first to get an unbuilt copy.\n\n");
        }

        // set all ID's to 0.
        for (int i = 0; i < num_planes_; i++) {
            this->IR_[i] = 0;
        }
        return this->shared_from_this();
    }


    RZBuilderPtr RZBuilder::ReflectZSolidConfig() {
        if (solid_ptr_) {
            throw std::runtime_error("Error in RZBuilder::ReflectZSolidConfig\"\n"
                "Cannot flip a solid builder CONFIGURATION, when the solid is already built from it.\n"
                "Use ReflectZFinalSolidCopy(\"new_name\")  instead to copy and flip the solid.\n\n");
        }
        // Mirror z values
        for (int i = 0; i < num_planes_; i++) {
            this->z_[i] = -this->z_[i];
        }
        return this->shared_from_this();
    }

    RZBuilderPtr RZBuilder::SetNumSides(G4double n) {
        if (MakeSolidFunctionPtr_ == &RZBuilder::MakePolyhedra) {
            sides_ = n;
        } else {
            G4cout <<
                "Warning in RZBuilder: Calling SetNumSides() on a builder that makes round things"
                << " might be as clever as giving a balloon to a hedgehog. \n Ignoring call." <<
                G4endl;
        }
        return shared_from_this();
    }

    // Overloads are handled a lot more implicitly in VolumeBuilder, but it would all have to be redone
    // here since the vector parameter names need to be different.
    // overload to take direct values
    RZBuilderPtr RZBuilder::AddPlane(const RZPlane &plane) {
        if (plane.IR < 0 || plane.OR < 0) {
            throw std::runtime_error("Error in RZBuilder::AddPlane()"
                                     " for builder " + GetBuilderName() + ".\n"
                                     "IR and OR must be non-negative.");
        }
        IR_.push_back(plane.IR * plane.unit);
        OR_.push_back(plane.OR * plane.unit);
        z_.push_back(plane.z * plane.unit);
        num_planes_++;
        return shared_from_this();
    }

    RZBuilderPtr RZBuilder::AddPlane(G4double unit, G4double IR, G4double OR, G4double z) {
        auto plane = RZPlane{unit, IR, OR, z};
        return AddPlane(plane);
    }

    // overload to take direct values with default unit
    RZBuilderPtr RZBuilder::AddPlane(G4double IR, G4double OR, G4double z) {
        auto plane = RZPlane{GetEffectiveDefaultUnit(), IR, OR, z};
        return AddPlane(plane);
    }

    //Overloads for adding multiple planes at once:
    // 1. Vector of full RZPlane objects (each with their own units)
    RZBuilderPtr RZBuilder::AddPlanes(const std::vector<RZPlane> &planes) {
        for (const auto &plane : planes) {
            AddPlane(plane);
        }
        return shared_from_this();
    }

    // 2. Unitless planes using builder's preset default unit
    RZBuilderPtr RZBuilder::AddPlanes(const std::vector<RZPlaneUnitless> &planes) {
        for (const auto &plane : planes) {
            G4double unit = this->GetEffectiveDefaultUnit();
            auto rzplane = RZPlane{unit, plane.IR, plane.OR, plane.z};
            AddPlane(rzplane);
        }
        return shared_from_this();
    }

    // 3. Most compact - one unit for all planes
    RZBuilderPtr RZBuilder::AddPlanes(G4double unit, const std::vector<RZPlaneUnitless> &planes) {
        for (const auto &plane : planes) {
            auto rzplane = RZPlane{unit, plane.IR, plane.OR, plane.z};
            AddPlane(rzplane);
        }
        return shared_from_this();
    }


    G4VSolid *RZBuilder::MakePolycone(const G4String &name) {
        if (this->solid_ptr_.get() != nullptr) {
            std::string error = "Error in MakePolycone: A solid was already built\n"
                "You can copy and rename the builder to reset it and build again.";
            throw std::runtime_error(error);
        }
        G4cout << G4endl;
        G4cout << "New Polycone:" << GetBuilderName() << " phi_start: " << phi_start_deg_ <<
            " degrees, phi_tot: " << phi_tot_deg
            << " degrees" << G4endl;
        for (int i = 0; i < z_.size(); i++) {
            G4cout << "z: " << z_[i] << " IR: " << IR_[i] << "  OR: " << OR_[i] << G4endl;
        }
        auto retval = new G4Polycone(
            name, phi_start_deg_ * CLHEP::deg, phi_tot_deg * CLHEP::deg, num_planes_, z_.data(),
            IR_.data(),
            OR_.data());
        return retval;
    }

    G4VSolid *RZBuilder::MakePolyhedra(const G4String &name) {
        if (this->solid_ptr_.get() != nullptr) {
            std::string error = "Error in MakePolyhedra: A solid was already built\n"
                "You can copy and rename the builder to reset it and build again.";
            throw std::runtime_error(error);
        }
        G4cout << G4endl;
        G4cout << "New Polyhedra:" << GetBuilderName() << " with " << sides_ << " sides, "
            << " phi_start: " << phi_start_deg_ << " degrees, phi_tot: " << phi_tot_deg <<
            " degrees" << G4endl;
        for (int i = 0; i < z_.size(); i++) {
            G4cout << "z: " << z_[i] << " IR: " << IR_[i] << "  OR: " << OR_[i] << G4endl;
        }
        auto retval = new G4Polyhedra(
            name, phi_start_deg_ * CLHEP::deg, phi_tot_deg * CLHEP::deg, sides_, num_planes_,
            z_.data(),
            IR_.data(), OR_.data());
        return retval;
    }


    //  Could use a displaced box macro too...
    //  Can use something like G4VSolid *displacedBox = new G4DisplacedSolid("displacedBox",aBox,rotvet,G4ThreeVector(x,y,z));
    //  But I had issues making that work in the past.
    //  Probably should just have a separate class, no need for z planes.

    // Development notes:
    // Presently, no deletion is done.  It's assumed this is used in a typical sloppy environment where the geometry generator has no
    // instance lifetime so cannot manage lifetime of its generated things.
    // However, we could add an optional "take_ownership"  flag that signals the builder should delete all its owned Geant things.
    // We'd need a move constructor for the return by value in the methods, or the internal copy would delete the resources.
    // This avoids the user messing with unique_ptrs and doesn't require a change in user code.
    // We'd need to track all volumes used in unions, and decide if we own materials or not (probably not).
    //  Could copy them, but then I think they need new names in geant.
    //  Anyway, this can all be done without breaking anything.
}
