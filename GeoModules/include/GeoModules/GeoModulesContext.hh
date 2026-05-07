//
// Created by D.S. Leonard on 8/9/25.
//

#ifndef GEOMODULESCONTEXT_HH
#define GEOMODULESCONTEXT_HH
#include "GeoModuleTypes.hh"

#include <G4String.hh>
#include <G4Material.hh>
#include <G4Element.hh>
#include <G4VPhysicalVolume.hh>
#include <memory>
#include <CLHEP/Units/SystemOfUnits.h>
//including these from here is a dirty hack
//It's for use in XYZGeoModule.cc that include GeoModulesContext.hh
//But should include more.  Doing it here risks header loops
//Probably need a master include for those.
#include "IGeoModule.hh"
#include "GeoModulesParser.hh"
#define GEOMDULES_CONTEXT_INCLUDES
#include "GeoModulesContextVariables.inc"
#undef GEOMDULES_CONTEXT_INCLUDES

namespace DLG4::GeoModules {
    ///@class GeoModulesContext GeoModulesContext.hh "GeoModulesContext.hh"
    ///@brief Context, including shared geometry data for GeoModules
    ///@details This is the main place to add shared variables for interconnections betweend Geometry Modules
    ///
    ///@author D.S. Leonard
    ///@date 8/9/25
    ///

    class GeoModulesContext: public std::enable_shared_from_this<GeoModulesContext> {
    private:
        OptsMapPtr extraOpts_;

    public:
        GeoModulesContext();

        //Must be made by user.
#if __has_include("GeoModulesContextVariables.inc")
#include "GeoModulesContextVariables.inc"
#endif


        std::stringstream GetExtraOpt(const G4String &opt) const {
            if ((*extraOpts_)[opt] != "") {
                return std::stringstream((*extraOpts_)[opt]);
            } else {
                return std::stringstream();
            }
        }

        void SetExtraOpt(const G4String &opt, const G4String &value) {
            (*extraOpts_)[opt] = value;
        }

        /// string keyed map of all discovered GeoModules.
        /// Runs a GeoModule by name
        void ConstructGeoModule(const G4String &moduleName);
        GeoModulePtr GetGeoModule(const G4String &moduleName) const;
        G4Material* GetMaterial(const G4String &materialName);
        G4Element* GetElement(const G4String &elementName);
        void ListModules() const;

        void SetWorldPhys(G4VPhysicalVolume *world_phys) {
            world_phys_ = world_phys;
        };

    private:
        friend class GeoModulesParser;
        G4VPhysicalVolume *world_phys_{nullptr};
        GeoModulesMapPtr registeredGeoModules_;
    };
}

#endif  //GEOMODULESCONTEXT_HH
