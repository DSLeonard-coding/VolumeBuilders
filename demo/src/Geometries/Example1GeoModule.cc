/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/
//Example1 GeoModule
#include <GeoModules/GeoModules.hh>
#define INCLUDE_GEOMODULE_RECIPES
#include "Example1GeoModule.inc"
#undef INCLUDE_GEOMODULE_RECIPES

#include "DetectorConstruction_includes.hh" // common includes
#include <VolumeBuilders.hh>


namespace VB = DLG4::VolumeBuilders; // Geometry builder helpers.

void Example1GeoModule::Construct(GeoModulesContextPtr context) {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    //@formatter:on

    VB::SetGlobalDefaultUnit(VB::Length::cm); // set a global unit
    G4Color coppertone(0.72, 0.45, .2);
    VB::RZPlane p;
    p.unit = VB::Length::mm; // see prior note.
    G4double some_reference;

    // some shape to union onto our main shape:
    auto another_builder_or_geant_solid = VB::CreatePolyconeBuilder("union")
            //@formatter:off
            ->AddPlane(p.IR = 90       , p.OR = 100*2.0/sqrt(3.) , p.z = -10 )
            ->AddPlane(p.IR            , p.OR                   , p.z -= 15 );

    // just a descriptively named variable for the documented README example:
    G4VPhysicalVolume *another_builder_or_geant_physical_volume = context->GetWorldVolume();

    auto ring_part = VB::CreatePolyhedraBuilder("ring_part", 6)
            // can set configurations in any order mostly, but can be nice to set many things up front before geometry details:
            ->SetMaterial("copper")
            ->SetColor(coppertone) // We can pre-configure the logical-volume!
            ->ForceSolid(true)
            ->AddUnion(another_builder_or_geant_solid, {0, 0, 0}) // with relative z, may not need to provide offset.
            // can predefine this. If a builder, it will auto build if needed!:
            ->SetMother(another_builder_or_geant_physical_volume)
            ->SetDefaultUnit(VB::Length::cm)
            // You can skip things to default them.  Less Noise!!
            // ->SetPhysOffset(something)                           // z referencing defined below may be enough!!
            // ->SetPhysRotation(zero_rot)
    // clang-format-off
            ->AddPlane(p.IR = 50, p.OR = 100, p.z = -10)
            ->AddPlane(p.IR, p.OR, p.z -= 40)
            ->AddPlane(p.IR = 90, p.OR, p.z)
            ->AddPlane(p.IR, p.OR, some_reference = p.z -= 40) //  set a reference for later.
            ->MakePlacement();
    // clang-format-on

    // Logcial volume was auto-named "ring_part_L"
    // Physical volume was auto-maned "ring_part_P" and can get auto copy_no(default) or auto name-numbering.
}

//
