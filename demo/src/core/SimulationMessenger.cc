/// SimulationMessenger composed by Douglas S. Leonard 2025,
///
/// A place for top-level messenger commands
///
/// The Geometry Recipe selection Messenger is in GeoModulesMessenger.hh/cc
///
///  Initially at least as selected commands from CupDebugMessenger
///  but intended for customization.
///
/// The top level messenger.  All commands should go in a messenger that is specific
/// to the class in use, or in a top level messenger.  Reason: all requirements to use
/// unrelated classes (like geometry and physics list) should be determined at the top level only.
/// This messenger was created to fix this, and particularly to remove the CupDetectorConstructor
/// argument/requirement from the same messenger that was needed to handle physics options,
/// while not modifiable in the top-level repo.
///


#include "SimulationMessenger.hh"

#include "G4Timer.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4ios.hh"

#if __has_include("CupSim/CupParam.hh")
//#include("CupSim/CupParam.hh")
#define CUP_PARAM
#endif

#include "G4RunManager.hh"

//#include "CupSim/CupParam.hh"

#include <fstream>  // for file streams
#include <iomanip>  // for setw(), etc..
#include <sstream>  // for string streams
#include <stdlib.h> // for strtol

using namespace std;

SimulationMessenger::SimulationMessenger() {
    // the simulation directory
    DebugDir = new G4UIdirectory("/simulation/");
    DebugDir->SetGuidance("User-added debugging, tests, and diagnostics.");


    // the database override commmand
    dovercmd = new G4UIcommand("/simulation/cupparam", this);
    dovercmd->SetGuidance("Inspect or modify the options and values database");
    dovercmd->SetParameter(new G4UIparameter("identifier", 's', false));
    dovercmd->SetParameter(new G4UIparameter("value", 'd', true));

    // the database read commmand
    dreadcmd = new G4UIcommand("/simulation/cupparam_read", this);
    dreadcmd->SetGuidance("Read name/value pairs from a file into option/value db");
    dreadcmd->SetParameter(new G4UIparameter("filename", 's', false));

    // the database dump commmand
    ddumpcmd = new G4UIcommand("/simulation/cupparam_dump", this);
    ddumpcmd->SetGuidance("Dump the option/value database (to terminal or a file)");
    ddumpcmd->SetParameter(new G4UIparameter("filename", 's', true));


    // the setseed command
    seedcmd = new G4UIcommand("/simulation/setseed", this);
    seedcmd->SetGuidance("Change random number generator state using setSeed(seed, luxury).");
    seedcmd->SetGuidance("The \"luxury\" parameter may be omitted; the default random number\n"
        "generator doesn't use it anyway.\n"
        "There is no getseed command, for Very Good Reasons, so use Geant4's\n"
        "/run/storeRandomNumberStatus and restoreRandomNumberStatus commands\n"
        "for saving/restoring the generator status in general.");
    seedcmd->SetParameter(new G4UIparameter("seed", 'd', false));
    seedcmd->SetParameter(new G4UIparameter("luxury_level", 'd', true));

    // the SetRunIDCounter command
    runIDcmd = new G4UIcommand("/simulation/SetRunIDCounter", this);
    runIDcmd->SetGuidance("Set Geant4 run number for next run");
    runIDcmd->SetParameter(new G4UIparameter("number", 'i', false));

#ifdef G4DEBUG
    // illuminationMap
    illucmd = new G4UIcommand("/simulation/dump_illumination_map", this);
    illucmd->SetGuidance("Dump a pretty picture of particle hits, for debugging");
#endif
}

SimulationMessenger::~SimulationMessenger() {
    delete DebugDir;
    delete dovercmd;
    delete dreadcmd;
    delete ddumpcmd;
    delete seedcmd;
    delete runIDcmd;
#ifdef G4DEBUG
    delete illucmd;
#endif
}


void SimulationMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
    if (command->GetCommandName() == "cupparam") {
#ifdef CUP_PARAM
        CupParam &db(CupParam::GetDB());
        std::istringstream iss(newValues.c_str());
        G4String parameterName;
        G4double new_value;
        iss >> parameterName;
        if (iss.fail()) {
            G4cerr << "Could not parse parameter name from command args\n";
            G4cerr.flush();
            return;
        }
        iss >> new_value;
        // set new value, if value was provided
        if (!(iss.fail()))
            db[parameterName] = new_value;
        // print out current value
        switch (db.count(parameterName)) {
            case 0:
                G4cout << parameterName << " undefined" << G4endl;
                break;
            default:
                G4cout << parameterName << " is multiply defined! " << db.count(parameterName)
                    << G4endl;
            case 1:
                G4cout << parameterName << "\t" << db[parameterName] << G4endl;
                break;
        }
    } else if (command->GetCommandName() == "cupparam_read") {
        CupParam &db(CupParam::GetDB());
        db.ReadFile(newValues.c_str());
    } else if (command->GetCommandName() == "cupparam_dump") {
        CupParam &db(CupParam::GetDB());
        if (newValues.length() > 0) {
            std::ofstream ofstr(newValues.c_str());
            if (!ofstr.good()) {
                G4cerr << "Could not open output file " << newValues << G4endl;
                return;
            }
            db.WriteFile(ofstr);
            ofstr.close();
        } else {
            db.WriteFile(G4cout);
            G4cout.flush();
        }
#else
        G4cout << "Warning :  CupParam.hh not included, but cupparam command passed.  \n" <<
        "It will be ignored\n" << G4endl;
#endif
    } else if (command->GetCommandName() == "setseed") {
        std::istringstream iss(newValues.c_str());
        long seed = 1;
        int luxury_level = 0;
        iss >> seed >> luxury_level;
        // HepRandom::getTheEngine()->setSeed(seed, luxury_level);
        G4Random::getTheEngine()->setSeed(seed, luxury_level);
    } else if (command->GetCommandName() == "SetRunIDCounter") {
        int i = atoi(newValues);
        G4RunManager::GetRunManager()->SetRunIDCounter(i);
        G4cout << "Set RunIDCounter to " << i << endl;
    }

#ifdef G4DEBUG
    else if (command->GetCommandName() == "dump_illumination_map") {
        extern int CupSteppingAction_dump_IlluminationMap(void);
        CupSteppingAction_dump_IlluminationMap();
    }
#endif

    // invalid command
    else {
        G4cerr << "invalid Cup \"set\" command\n" << std::flush;
    }
}

G4String SimulationMessenger::GetCurrentValue(G4UIcommand * /*command*/) {
    return G4String("invalid SimulationMessenger \"get\" command");
}
