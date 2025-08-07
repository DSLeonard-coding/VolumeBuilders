// pre-declared clion template variables.  You can delete these in the generated files.
// ${METHOD_NAME}
// ${GEOMETRY_NAME}
// ${MATERIAL}

// ReSharper disable CppExpressionWithoutSideEffects
#[[#include]]# "DetectorConstruction_includes.hh" // common includes
#[[#include]]# <VolumeBuilderIncludes.hh>
/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

using namespace CLHEP;
using namespace DLG4::VolumeBuilders; // Geometry builder helpers.

void DetectorConstruction::Construct${METHOD_NAME}() {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    
    //make a dedicated sample_material for vertex generation.  Can adjust density with optional 3rd parameter
    G4Material *sample_mat = DetectorConstruction::CopyMaterial(${MATERIAL}, "sample_mat");
    G4cout << "now ConstructArrayHexPart()\n" << G4endl;
    auto box_part = CreateZDeltaBoxBuilder(
                        "${GEOMETRY_NAME}_box",     // name
                        10,             // x total size
                        10,             // y total size
                        -5,             //  z start
                        10)             //  z change
                    ->SetColor(1, 0, 0) // red
                    ->SetPhysOffset({0, 0, 0})
                    ->SetMother(world_phys)
                    ->SetMaterial(sample_mat)
                    ->ForceSolid(true)
                    //->SetPhysOffset({mm, 0, 0, 0}) 
                    ->MakePlacement();
}

