//
// Created by osboxes on 8/9/25.
//

#ifndef GEOMODULESCONTEXT_CC
#define GEOMODULESCONTEXT_CC
#include <G4NistManager.hh>

#include "GeoModules/GeoModulesContext.hh"
#include "GeoModules/GeoModuleTypes.hh"
#include <G4String.hh>
#include "GeoModules/IGeoModule.hh"

namespace DLG4::GeoModules {
    GeoModulesContext::GeoModulesContext() :
        registeredGeoModules_(std::make_shared<GeoModulesMap>()),
        extraOpts_(std::make_shared<OptsMap>()) {
    }

    void GeoModulesContext::ConstructGeoModule(const G4String &moduleName) {
        if (auto module = registeredGeoModules_->find(moduleName);
            module != registeredGeoModules_->end()) {
            module->second->Construct(this->shared_from_this());
        } else {
            throw std::runtime_error(
                "ERROR: invalid module name "+moduleName+" provided to GeoModulesContext::ConstructGeoModule");
        }
    }

    GeoModulePtr GeoModulesContext::GetGeoModule(const G4String &moduleName) const {
        auto module = registeredGeoModules_->find(moduleName);
        if (module != registeredGeoModules_->end()) {
            return module->second;
        } else {
            throw std::runtime_error(
                "ERROR: invalid module name provided to GeoModulesContext::GetGeoModule\n");
        }
    }

    void GeoModulesContext::ListModules() const {
        G4cout << "Detector geometry modules: ";
        for (const auto &module : *registeredGeoModules_) {
            G4cout << "Registered module: " << module.first << G4endl;
        }
        G4cout << G4endl;
    };

}

#endif  //GEOMODULESCONTEXT_CC
