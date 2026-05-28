//
// Created by D.S. Leonard on 8/9/25.  MIT
//

#ifndef GEOMODULE_HH
#define GEOMODULE_HH
#include "GeoModuleTypes.hh"

namespace DLG4::GeoModules {
    ///@class IGeoModule
    ///@brief GeoModule.hh "IGeoModule.hh"
    ///@brief Simple Interface contract for a Geometry Module class
    ///
    ///@author D.S. Leonard
    ///@date 8/9/25
    ///
    ///@details
    /// These are loaded by the GeoModuleParser class
    /// And are called by GeoModulesContext::ConstructGeoModule()
    /// or through GeoModuleParser::RecipeRunner();
    ///
    ///
    class IGeoModule {
    public:
        IGeoModule() = default;
        virtual void Construct(GeoModulesContextPtr context) = 0;
        virtual void RegisterRecipes(GeoModulesParserPtr parser) = 0;

        void SetName(const G4String &name) {
            module_name_ = name;
        }

        G4String GetName() {
            return module_name_;
        };
        virtual ~IGeoModule() = default;

    private:
        G4String module_name_;
    };
}

#endif  //GEOMODULE_HH
