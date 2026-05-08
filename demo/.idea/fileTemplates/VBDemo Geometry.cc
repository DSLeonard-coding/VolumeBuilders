// pre-declared clion template variables.  You can delete these in the generated files.
// ${MODULE_BASE_NAME}
// ${GEORECIPE_NAME}
// ${MATERIAL}

// ReSharper disable CppExpressionWithoutSideEffects
//${MODULE_BASE_NAME} GeoModule
#[[#include]]# <GeoModules/GeoModules.hh>
#[[#define]]# INCLUDE_GEOMODULE_SHARED_OBJECTS
#[[#include]]# "${MODULE_BASE_NAME}GeoModule.inc"
#[[#undef]]# INCLUDE_GEOMODULE_SHARED_OBJECTS

#[[#include]]# "DetectorConstruction_includes.hh" // common includes
#[[#include]]# <VolumeBuilders.hh>

using namespace CLHEP;
namespace VB = DLG4::VolumeBuilders; // Geometry builder helpers.

void ${MODULE_BASE_NAME}GeoModule::Construct(GeoModulesContextPtr context) {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;
    G4cout << "now building ${MODULE_BASE_NAME}GeoModule()  ()\n" << G4endl;

    //make a dedicated sample_material for vertex generation.  Can adjust density with optional 3rd parameter
    G4Material *sample_mat = VBHelpers::CopyMaterial("${MATERIAL}", "sample_mat");
    VB::SetGlobalDefaultUnit(CLHEP::mm); // set a global unit

    auto box_part = VB::CreateZDeltaBoxBuilder(
                        "${GEORECIPE_NAME}_box",     // name
                        10,             // x total size
                        10,             // y total size
                        -5,             //  z start
                        10)             //  z change
                    ->SetColor(1, 0, 0) // red
                    ->SetPhysOffset(context->sample_center))      //Use a context variable to position!
                    ->SetMother(context->GetWorldPhys())
                    ->SetMaterial(sample_mat)
                    ->ForceSolid(true)
                    ->MakePlacement();
}
