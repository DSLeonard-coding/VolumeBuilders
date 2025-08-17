/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

// ReSharper disable CppExpressionWithoutSideEffects
#include "DetectorConstruction_includes.hh" // common includes
#include <VolumeBuilderIncludes.hh>

using namespace CLHEP;
using namespace DLG4::VolumeBuilders; // Geometry builder helpers.

void DetectorConstruction::ConstructBoxExample() {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    //@formatter:on

    DLG4::VolumeBuilders::SetGlobalDefaultUnit(CLHEP::mm); // set a global unit
    BuilderViewList builder_list;
    // a small box to mark the world center
    auto box_part = CreateZDeltaBoxBuilder(
                        "box_part",     // name
                        10,             // x total size
                        10,             // y total size
                        -5,             //  z start
                        10)             //  z change
                    ->SetColor(1, 0, 0) // red
                    ->SetPhysOffset({0, 0, 0})
                    ->AddTo(builder_list);

    // multiple boxes arranged in y with a z-edged referenced to 0:
    CreateZDeltaBoxBuilder("box_part2", 100, 100, 0, 200)->SetColor(0, 1, 0) // green
                                                         ->AddTo(builder_list);
    CreateZDeltaBoxBuilder("box_part3", 100, 100, 0, 100)->SetColor(0, .5, .5) // blue-green
                                                         ->AddTo(builder_list);
    CreateZDeltaBoxBuilder("box_part4", 100, 100, 0, 50)->SetColor(0.5, 0.7, 1) // blue
                                                        ->AddTo(builder_list);
    CreateZDeltaBoxBuilder("box_part4", 100, 100, 50, 100)
        ->SetColor(150. / 255, 0, 175. / 255) // purple
        ->AddTo(builder_list);
    // a box rotated around y with its own origin still at the z=0 edge, ie rotated off-center:
    CreateZDeltaBoxBuilder("box_part4", 100, 100, 0, 200)
        // can set configurations in any order mostly, but can be nice to set many things up front before geometry details:
        ->SetColor(255. / 255, 165. / 255, 0) // orange
        ->SetPhysRotation(G4RotationMatrix().rotateY(-90.0 * deg))
        ->AddTo(builder_list);
    // arrange all boxes in y and set common properties:
    double y = 0;
    for (auto &builder : builder_list) {
        builder->SetMother(world_phys)
               ->SetMaterial(_copper)
               ->ForceSolid(true)
               ->SetPhysOffset({mm, 0, y, 0}) // distribute in y
               ->MakePlacement();
        y += 100;
    }
}

//
