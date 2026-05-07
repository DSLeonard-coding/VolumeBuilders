//
// Created by D.S. Leonard on 8/9/25.
//

#ifndef GEOMODULEPARSER_HH
#define GEOMODULEPARSER_HH
#include "GeoModulesContext.hh"
#include "GeoModuleTypes.hh"
#include "GeoModulesMessenger.hh"

// Include the geometry module classes:
// First pass to pick up any un-sectioned defines.
//#include "autoincludes.inc"

namespace DLG4::GeoModules {
    ///@class GeoModulesParser GeoModulesParser.hh "GeoModuleParser.hh"
    ///@brief Parses and registers GeoModules and GeoModuleRecipe objects from .inc files.
    ///
    ///@author D.S. Leonard
    ///@date 8/9/25
    ///
    class GeoModulesParser: public std::enable_shared_from_this<GeoModulesParser> {
    public:
        GeoModulesParser();
        ~GeoModulesParser() = default;
        G4VPhysicalVolume *Construct();
        void RegisterModules();
        void ListModules();

        void BuildSelectedRecipe() const {
            BuildRecipe(whichGeoRecipe_);
        };
        void BuildRecipe(const G4String &whichGeoRecipe) const;
        void ListRecipes();
        bool HasRecipe(const G4String &whichGeoRecipe) const;
        void AddRecipe(const G4String &key, const GeoRecipe &recipe);

        void SetContext(const GeoModulesContextPtr &context) {
            context_ = context;
        };
        ///Get the selected geometry recipe
        G4String GetWhichGeoRecipe(void) const {
            return whichGeoRecipe_;
        }

        ///Set the geometry recipe to be built
        void SetWhichGeoRecipe(const G4String &w) {
            whichGeoRecipe_ = w;
        }

        GeoModulesContextPtr GetContext() const {
            return context_;
        }

        G4VPhysicalVolume *GetWorldPhys() const;

    private:
        // This initializes the command interface which selects the geometry recipe to build:
        // Use a vis.mac or emmit commands in main.
        GeoModulesMessengerPtr messenger_;
        /// string keyed map of all discovered GeoRecipeMap, each taking a GeoModulesContext as a parameter.
        GeoRecipeMapPtr geo_recipes_;
        GeoModulesContextPtr context_;
        G4String whichGeoRecipe_ = "";
        bool modules_registered_{false};
    };
};

#endif  //GEOMODULEPARSER_HH
