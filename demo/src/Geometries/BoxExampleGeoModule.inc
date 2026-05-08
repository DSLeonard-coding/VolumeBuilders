//BoxExample GeoModule
//file and template author D.S. Leonard, refactored 2025
//This is an X-header, included in parts

//*********EDIT TO DEFINE A UNIQUE MODULE NAME HERE*****************
//***The class name, for use in the .cc, will be MODULE_BASEGeoModule*****
#define MODULE_BASE BoxExample

#include "GeoModules/GeoModule_Inline1.inl"
#ifdef INCLUDE_GEOMODULE_SHARED_OBJECTS
/////////////Edit the Recipe name and the recipe  ////////////////////////
void CLASSNAME::RegisterRecipes(GeoModulesParserPtr parser) {
    parser->AddRecipe("BoxExample",
        [](GeoModulesContextPtr context) -> void {
            context->ConstructGeoModule("Materials");
            context->ConstructGeoModule("World");
            context->ConstructGeoModule("BoxExample");
    });
}

#endif
#include "GeoModules/GeoModule_Inline2.inl"
