// Messenger for GeoModulesParser
// Created by D.S. Leonard on 8/10/25.
//

#ifndef GEOMODULESMESSENGER_CC
#define GEOMODULESMESSENGER_CC
#include "GeoModules/GeoModulesMessenger.hh"
#include <G4ApplicationState.hh>
#include <G4String.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcommand.hh>
#include <G4UIdirectory.hh>
#include <iosfwd>
#include <string>
#include <bits/istream.tcc>
#include "GeoModules/GeoModulesParser.hh"
#include "GeoModules/GeoModulesContext.hh"

namespace DLG4::GeoModules {
    GeoModulesMessenger::GeoModulesMessenger(GeoModulesParser *geo_parser)
        : parser_(geo_parser) {
        auto *detDir = new G4UIdirectory("/detGeometry/");
        detDir->SetGuidance("Control detector geometry options.");

        detGeometrySelectCmd_ = new G4UIcmdWithAString("/detGeometry/select", this);
        detGeometrySelectCmd_->SetGuidance("Select which detector geometry to build.\n"
            "No argument = list available styles.");
        detGeometrySelectCmd_->AvailableForStates(G4State_PreInit);

        detGeometryOptCmd_ = new G4UIcommand("/detGeometry/opt", this);
        detGeometryOptCmd_->SetGuidance("Extra geometry options: \"<option_name> parameters\"");
        detGeometryOptCmd_->AvailableForStates(G4State_PreInit);
        //alternative way, register it as an optional string parameter:
        detGeometryOptCmd_->SetParameter(new G4UIparameter("opt", 's', true));

    }

    void GeoModulesMessenger::SetNewValue(G4UIcommand *command, const G4String newValue) {
        if (command == detGeometrySelectCmd_) {
//TODO, add these checks back. But requires setting up recipes earlier
// likely requires fix to the the make_shared_from_this issue.
            //            if (newValue.empty()) {
//                parser_->ListRecipes();
//            } else if (parser_->HasRecipe(newValue)) {
                parser_->SetWhichGeoRecipe(newValue);
//            } else {
//                G4cerr << "Unknown detector geometry style " << newValue << G4endl;
//            }
        } else if (command == detGeometryOptCmd_) {
            std::stringstream ss(newValue);
            G4String opt, params;
            if (ss >> opt >> std::ws && std::getline(ss, params)) {
                parser_->GetContext()->SetExtraOpt(opt, params);
                G4cout << "/detGeometry/opt " << opt << " " << params << G4endl;
            } else {
                G4cout << "/detGeometry/opt <no option given>" << G4endl;
            }
        }
    }

    G4String GeoModulesMessenger::GetCurrentValue(G4UIcommand *command) {
        if (command == detGeometrySelectCmd_)
            return parser_->GetWhichGeoRecipe();
        return "";
    }
};


#endif //GEOMODULESMESSENGER_CC
