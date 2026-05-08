// pre-declared clion template variables.  You can delete these in the generated files.
// BoxTest
// BoxTest
// G4_CONCRETE

// ReSharper disable CppExpressionWithoutSideEffects
//BoxExample GeoModule
#include <GeoModules/GeoModules.hh>
#define INCLUDE_GEOMODULE_RECIPES
#include "BoxTestGeoModule.inc"
#undef INCLUDE_GEOMODULE_RECIPES

#include "DetectorConstruction_includes.hh" // common includes
#include <VolumeBuilders.hh>

using namespace CLHEP;
namespace VB = DLG4::VolumeBuilders; // Geometry builder helpers.

void BoxTestGeoModule::Construct(GeoModulesContextPtr context) {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    G4cout << "now building BoxTestGeoModule()  ()\n" << G4endl;

    //make a dedicated sample_material for vertex generation.  Can adjust density with optional 3rd parameter
    G4Material *sample_mat = VBHelpers::CopyMaterial("G4_CONCRETE", "sample_mat");
    VB::SetGlobalDefaultUnit(CLHEP::mm); // set a global unit

    auto box_part = VB::CreateZDeltaBoxBuilder(
                    "BoxTest_box", // name
                    10, // x total size
                    10, // y total size
                    -5, //  z start
                    10) //  z change
            ->SetColor(1, 0, 0) // red
            ->SetPhysOffset(context->sample_center)   // use a context variable to position it!!
            ->SetMother(context->GetWorldVolume())
            ->SetMaterial(sample_mat)
            ->ForceSolid(true)
            //->SetPhysOffset({mm, 0, 0, 0})
            ->MakePlacement();
}
