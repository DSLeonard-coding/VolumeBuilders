/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/
//Assembly GeoModule
#include <GeoModules/GeoModules.hh>
#define INCLUDE_GEOMODULE_RECIPES
#include "AssemblyGeoModule.inc"
#undef INCLUDE_GEOMODULE_RECIPES

#include "DetectorConstruction_includes.hh" // common includes
#include <VolumeBuilders.hh>


namespace VB = DLG4::VolumeBuilders; // Geometry builder helpers.

void AssemblyGeoModule::Construct(GeoModulesContextPtr context) {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    //@formatter:on

    DLG4::VolumeBuilders::SetGlobalDefaultUnit(VB::Length::mm); // set a global unit
    G4Color coppertone(0.72, 0.45, .2);
    VB::RZPlane p;
    p.unit = VB::Length::mm; // see prior note.
    G4double some_reference;

    auto shape = VB::CreatePolyhedraBuilder("part", 3)
            //@formatter:off
            ->AddPlane(p.IR = 40       , p.OR = 50 , p.z = 0 )
            ->AddPlane(p.IR            , p.OR                   , p.z -= 100 );
    //@formatter:on

    auto assembly = VB::CreateAssembly("example_assembly");
    auto temp = shape;
    for (int i = 0; i < 3; i++) {
        temp->ForkAndReset("part_" + std::to_string(i))
            ->SetPhysOffset({0, 250. * (i), 0.})
            ->AddTo(assembly);
    }

    assembly->SetMother(context->GetWorldVolume())
            ->SetMaterial("copper")
            ->SetColor(0, 1, 0) // We can pre-configure the logical-volume!
            ->ForceSolid(true)
            ->SetPhysOffset({0, 0, -200})
            ->PlaceAndFork()
            ->SetColor(1, 0, 0) // but the fork still shares logical volume so they are now ALL red.
            ->StackPhysRotation(G4RotationMatrix().rotateY(-90.0 * CLHEP::deg))
            ->MakePlacement()
            // but we can clone only the Final solid, and rebuild LV with new color:
            ->ForkForLogicalVolume("blue")
            ->SetColor(0, 0, 1)
            ->StackPhysRotation(G4RotationMatrix().rotateY(-90.0 * CLHEP::deg))
            ->MakePlacement();
}
