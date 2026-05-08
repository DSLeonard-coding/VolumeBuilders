//Example1 GeoModule
//file and template author D.S. Leonard, refactored 2025
//This is an X-header, included in parts

//*********EDIT TO DEFINE A UNIQUE MODULE NAME HERE*****************
//***The class name, for use in the .cc, will be MODULE_BASEGeoModule*****
#define MODULE_BASE Example1

#include "GeoModules/GeoModule_Inline1.inl"
#ifdef INCLUDE_GEOMODULE_RECIPES
/////////////Edit the Recipe name and the recipe  ////////////////////////
void CLASSNAME::RegisterRecipes(GeoModulesParserPtr parser) {
    parser->AddRecipe("Example1",
        [](GeoModulesContextPtr context) -> void {
            context->ConstructGeoModule("Materials");
// Geometries have dependancies and call each other if needed
//  Implicit dependancies are only set where absolutely required anyway.
            context->ConstructGeoModule("World");
            context->ConstructGeoModule("Example1");
    });
}

#endif
#include "GeoModules/GeoModule_Inline2.inl"
