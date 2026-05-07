//
// Created by osboxes on 8/9/25.
//

#ifndef GEOMODULEPARSER_CC
#define GEOMODULEPARSER_CC
#include "GeoModules/GeoModulesParser.hh"
#include "GeoModules/GeoModulesContext.hh"

#include "GeoModules/GeoModulesContext.hh"
#define INCLUDE_GEOMODULE_CLASS_HEADER
#include "autoincludes.inc"
#undef INCLUDE_GEOMODULE_CLASS_HEADER

namespace DLG4::GeoModules {
    void GeoModulesParser::BuildRecipe(const G4String &whichGeoRecipe) const {
        auto recipe = geo_recipes_->find(whichGeoRecipe);
        if (recipe != geo_recipes_->end()) {
            recipe->second(context_);
        } else {
            G4cerr << "ERROR: invalid geometry option" << G4endl << std::flush;
        }
    }

    void GeoModulesParser::ListRecipes() {
        RegisterModules();
        G4cout << "Registered Geometry Recipes (for build selection): ";
        for (const auto &geo : *geo_recipes_) {
            G4cout << "Registered geometry recipe: " << geo.first << G4endl;
        }
        G4cout << G4endl;
    }

    bool GeoModulesParser::HasRecipe(const G4String &whichGeoRecipe) const {
        return geo_recipes_->count(whichGeoRecipe) > 0;
    }

    void GeoModulesParser::AddRecipe(const G4String &key, const GeoRecipe &recipe) {
        geo_recipes_->insert(std::make_pair(key, recipe));
    }

    G4VPhysicalVolume *GeoModulesParser::GetWorldPhys() const {
        return context_->world_phys_;
    }

    void GeoModulesParser::ListModules() {
        RegisterModules();
        context_->ListModules();
    }

    GeoModulesParser::GeoModulesParser() :
        // we own and construct one context and a recipe:
        context_(std::make_shared<GeoModulesContext>()),
        geo_recipes_(std::make_shared<GeoRecipesMap>()) {
        messenger_ = std::make_shared<GeoModulesMessenger>(this);
        // context owns the registeredGeoModules_ map:
    }

    G4VPhysicalVolume *GeoModulesParser::Construct() {
        //Next call COULD go in constructor, but internal shared_from_this use would need to be replaced
        // with a non-deleting shared pointer since it can't be used in ctor:
        RegisterModules();
        BuildSelectedRecipe();
        return GetWorldPhys();
    }

    void GeoModulesParser::RegisterModules() {
        if (modules_registered_) {
            return;
        }
        modules_registered_ = true;
        //First pass register instantiated GeoModule objects with the runner:
#define INCLUDE_GEOMODULE_REGISTRATION
        // @formatter:on
        // noinspection ALL
#include "autoincludes.inc"

#undef INCLUDE_GEOMODULE_REGISTRATION

        //Second pass, register Geometry Recipies with the runner:
#define INCLUDE_GEORECIPES_REGISTRATION
        // noinspection ALL
#include "autoincludes.inc"

#undef INCLUDE_GEORECIPES_REGISTRATION
    }
} // DLG4
#endif
// @formatter:on#include "autoincludes.inc"
