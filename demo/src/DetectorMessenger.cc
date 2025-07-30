//
// DetectorMessenger.cc by Glenn Horton-Smith, Dec 1999
////////////////////////////////////////////////////////////////
// DetectorMessenger
////////////////////////////////////////////////////////////////

//#include "CupSim/local_g4compat.hh"

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "globals.hh"

#include <stdlib.h>   // for strtol
#include "fstream"    // for file streams
#include <sstream>  // string streams.

DetectorMessenger::DetectorMessenger(DetectorConstruction *Detector)
:
        Detector(Detector) {
    // the Detector directory
    G4UIdirectory *DetectorDir = new G4UIdirectory("/detGeometry/");
    DetectorDir->SetGuidance("Control the detector geometry options.");

    // the select command
    DetGeometrySelectCmd = new G4UIcommand("/detGeometry/select", this);
    DetGeometrySelectCmd->SetGuidance
    ("Select which detector you want to build");
    DetGeometrySelectCmd->SetGuidance
    ("Use with no parameters to get list of available detector styles.");
    DetGeometrySelectCmd->AvailableForStates(G4State_PreInit);
    DetGeometrySelectCmd->SetParameter(new G4UIparameter("which", 's', true));

    //DL adds a generic option reader allowing options to be looked up by name as needed elsewhere.
    DetGeometryOptCmd = new G4UIcommand("/detGeometry/opt", this);
    DetGeometryOptCmd->SetGuidance
    ("Input additional geometry options");
    DetGeometryOptCmd->SetGuidance
    ("Format is \"<option_name> parameters\" with the quotes.");
    DetGeometryOptCmd->AvailableForStates(G4State_PreInit);
    //Dl isn't sure if next line is useful :
    DetGeometryOptCmd->SetParameter(new G4UIparameter("opt", 's', true));
}

DetectorMessenger::~DetectorMessenger() {
}

void DetectorMessenger::
SetNewValue(G4UIcommand *command, G4String newValues) {
    // GeometrySelectCmd
    if (command == DetGeometrySelectCmd) {
        if (newValues.length() == 0) {
//D.L simplifies (many things) with map based solutions. Simplifies code elsewhere.
            Detector->ListGeometries();
        } else {
            G4cout << "detGeometry/select " << newValues.c_str() << G4endl;
            if (Detector->Geometries.find(newValues) != Detector->Geometries.end()) {
                Detector->SetWhichDetGeometry(newValues);
                return;
            } else {
                G4cerr << "Unknown detector geometry style " << newValues << G4endl;
            }
        }
    } else if (command == DetGeometryOptCmd) { // read extra options.
        std::stringstream ss(newValues);
        G4String opt;
        G4String optparams;
        if (ss >> opt >> std::ws && std::getline(ss, optparams, '\0')) {
            Detector->ExtraOpts[opt] = optparams;
            G4cout << "read /detGeometry/opt " << opt << " " << Detector->ExtraOpts[opt] << G4endl;
        } else {
// Don't treat this an error, we have unused option slots in the macros, just log it to stdout:
            G4cout << "/detGeometry/opt " << "Opt:" << opt << "optparams: " << optparams << G4endl << std::flush;
            G4cout << "    ... No option given" << G4endl << std::flush;
        }
    } else {
        G4cerr << "invalid detector \"set\" command: " << command->GetCommandPath() << command->GetCommandName() << "\n"
                << std::flush;
    } // end newvalues conditional
}

G4String DetectorMessenger::
GetCurrentValue(G4UIcommand *command) {
    // GeometrySelectCmd
    if (command == DetGeometrySelectCmd) {
//D.L map version:
        return Detector->GetWhichDetGeometry();
    }

    // invalid command
    else {
        return G4String("invalid DetectorMessenger \"get\" command");
    }
}
