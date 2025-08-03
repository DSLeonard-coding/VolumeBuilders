/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

#include "DetectorConstruction_includes.hh" // common includes
#include <VolumeBuilderIncludes.hh>

using namespace CLHEP;
using namespace DLG4::VolumeBuilders; // Geometry builder helpers.

void DetectorConstruction::ConstructAssembly() {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    //clang-format on

    DLG4::VolumeBuilders::SetGlobalDefaultUnit(CLHEP::mm); // set a global unit
    G4Color coppertone(0.72, 0.45, .2);
    RZPlane p;
    p.unit = mm; // see prior note.
    G4double some_reference;

    auto cylinder = CreatePolyhedraBuilder("part", 3)
            //clang-format off
            ->AddPlane(p.IR = 40       , p.OR = 50 , p.z = 0 )
            ->AddPlane(p.IR            , p.OR                   , p.z -= 100 );
    //clang-format on

    auto assembly = CreateAssembly("example_assembly");
    auto temp = cylinder;
    for (int i = 0; i < 3; i++) {
        for (int i = 0; i < 3; i++) {
            temp->ForkAndReset("part_" + std::to_string(i))
                    ->SetPhysOffset({0, 250. * (i), 0.})
                    ->AddTo(assembly);
        }
    }

    assembly->SetMother(world_phys)
            ->SetMaterial(_copper)
            ->SetColor(0, 1, 0) // We can pre-configure the logical-volume!
            ->ForceSolid(true)
            ->SetPhysOffset({0, 0, -200})
            ->PlaceAndFork()
            ->SetColor(1, 0, 0) // but the fork still shares logical volume so they are now ALL red.
            ->StackPhysRotation(G4RotationMatrix().rotateY(-90.0 * deg))
            ->MakePlacement()
            // but we can clone only the Final solid, and rebuild LV with new color:
            ->ForkLogicalVolume("blue")
            ->SetColor(0, 0, 1)
            ->StackPhysRotation(G4RotationMatrix().rotateY(-90.0 * deg))
            ->MakePlacement();
}

