//CC1_basegeom GeoModule
//file and template author D.S. Leonard, refactored 2025

//*********EDIT TO DEFINE A UNIQUE MODULE NAME HERE*****************
//***The class name, for use in the .cc, will be MODULE_BASEGeoModule*****
#define MODULE_BASE Materials

#include "GeoModules/GeoModule_Inline1.inl"
#ifdef INCLUDE_GEOMODULE_RECIPES
/////////////Edit the Recipe name and the recipe  ////////////////////////
// Materials module is associated with no geometry recipe.
void CLASSNAME::RegisterRecipes(GeoModulesParserPtr parser) {}


#endif
#include "GeoModules/GeoModule_Inline2.inl"