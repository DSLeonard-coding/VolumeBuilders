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

    G4Material* GeoModulesContext::GetMaterial(const G4String &materialName) {
        // Check local material store (materials already created/defined)
        // 2nd parameter is warn if missing.
        G4Material* mat = G4Material::GetMaterial(materialName, false);

        if (mat) {
            return mat;
        }

        // Fallback to NIST manager
        G4NistManager* nist = G4NistManager::Instance();
        mat = nist->FindOrBuildMaterial(materialName);

        if (!mat) {
            G4Exception("GeoModulesContext::GetMaterial", "MatNotFound",
                        FatalException, ("Could not find material: " + materialName).c_str());
        }

        return mat;
    }

    G4Element* GeoModulesContext::GetElement(const G4String &elementName) {
        // Check the GIlobal Element Table first
        G4Element* el = G4Element::GetElement(elementName, false);

        if (el) {
            return el;
        }

        // Fallback to NIST manager for standard elements
        G4NistManager* nist = G4NistManager::Instance();
        el = nist->FindOrBuildElement(elementName);

        if (!el) {
            G4Exception("GeoModulesContext::GetElement", "ElementNotFound",
                        FatalException, ("Could not find element: " + elementName).c_str());
        }

        return el;
    }
}

#endif  //GEOMODULESCONTEXT_CC
