//
// Created by osboxes on 8/10/25.
//

#ifndef GEOMODULESMESSENGER_HH
#define GEOMODULESMESSENGER_HH
#include "GeoModuleTypes.hh"
#include "G4UImessenger.hh"

#include <G4UIcmdWithAString.hh>

namespace DLG4::GeoModules {
    class GeoModulesMessenger final: public G4UImessenger {
    public:
        GeoModulesMessenger() = default;
        explicit GeoModulesMessenger(GeoModulesParser *geo_parser);
        ~GeoModulesMessenger() override = default;

        void SetNewValue(G4UIcommand *command, G4String newValue) override;
        G4String GetCurrentValue(G4UIcommand *command) override;

    private:
        GeoModulesParser *parser_;
        G4UIcmdWithAString *detGeometrySelectCmd_;
        G4UIcommand *detGeometryOptCmd_;
    };
}

#endif //GEOMODULESMESSENGER_HH
