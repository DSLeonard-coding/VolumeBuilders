// This is NOT a header file.  It is code gen for the GeoModule .inc files
// It is included in the user .inc file to define the class from a macro name.

#ifndef GEO_STRING_HELPERS
#define GEO_STRING_HELPERS
#define CONCAT(a,b) a##b
#define EXPAND_AND_CONCAT(a,b) CONCAT(a,b)  // forces expansion first

#define GEOM_STRINGIFY(x) #x
#define GEOM_TOSTRING(x) GEOM_STRINGIFY(x)
#endif

#define CLASSNAME EXPAND_AND_CONCAT(MODULE_BASE, GeoModule)
#define MODULE_STRING GEOM_TOSTRING(MODULE_BASE)

#if (defined INCLUDE_GEOMODULE_CLASS_HEADER) || ( defined INCLUDE_GEOMODULE_SHARED_OBJECTS)
//#define GUARD_NAME EXPAND_AND_CONCAT(CLASSNAME, GUARD)
//#ifndef GUARD_NAME
//#define GUARD_NAME
using namespace DLG4::GeoModules;
class CLASSNAME : public IGeoModule
{
public:
    CLASSNAME() {
        SetName(MODULE_STRING);
    }
    void Construct(GeoModulesContextPtr context) override;
    void RegisterRecipes(GeoModulesParserPtr parser) override;
};
//#endif  GUARD_NAME
#endif

//inline-included executable code to actually do the registrations.
// for both the module and the recipes.
#ifdef INCLUDE_GEOMODULE_REGISTRATION
// this is included/executed from GeoModulesParser::RegisterModules()
//TODO This is a a little inconsistent with recipe registration.
// Better: module.RegisterMoudule(shared_from_this)
// And let the module register itself using parser.AddModule().
if (context_->registeredGeoModules_->find(MODULE_STRING) == context_->registeredGeoModules_->end()) {
    auto mod = std::make_shared<CLASSNAME>(); // keep a strong handle
    context_->registeredGeoModules_->insert({MODULE_STRING, mod});
} else {
    G4cerr << "Warning: duplicate registration of GeoModule attempted" << MODULE_STRING << G4endl;
    G4cerr << "This is not harmful but could indicate something unexpected happening"  << G4endl;
}
#endif

#ifdef INCLUDE_GEORECIPES_REGISTRATION
// this is included/executed from GeoModulesParser::RegisterModules()
// so "this" here is a pointer to parser.
// Just run the recipe registration function on the module.
this->context_->GetGeoModule(MODULE_STRING)->RegisterRecipes(this->shared_from_this());
#endif
