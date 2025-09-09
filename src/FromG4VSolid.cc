/*
 * FromG4VSolid.cc
 *
 *  Created on: Jun 29, 2024
 *      Author: D. S. Leonard
 */

// See header for documentation.

#include "FromG4VSolid.hh"
#include "VolumeBuilder.hh"
#include <string>
#include <VolumeBuilderTypes.hh>
//#include "disableable_shared_from_this.hh"
#include "i_shared_ptr.hh"
#include "StructureBuilder.hh"
#include "StructureBuilder.hpp"

class G4String;
using namespace DLG4::Utilities;

//GeantMultiPlane methods //////////////////////////////////////////////////
namespace DLG4::VolumeBuilders {
    //Factories
    FromG4VSolidPtr CreateFromG4VSolid(G4VSolid *solid) {
        auto object = FromG4VSolidPtr();
        object.get()->SetSolid(solid);
        object.get()->SetName(solid->GetName());
        return FromG4VSolidPtr(object);
    }


    //Private default copy ctor.
    //This is used by clone methods of concrete classes
    FromG4VSolid::FromG4VSolid(const FromG4VSolid &other) :
        VolumeBuilder<FromG4VSolid>(other) {
        // Call base class copy constructor
        set_shared_from_this_enabled(false);
        SetName(other.GetBuilderName());
        solid_ptr_.Link(other.solid_ptr_);
        set_shared_from_this_enabled(true);
    }
}
