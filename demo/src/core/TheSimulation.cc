/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/
#include "TheSimulation.hh"
//#include "RootNtuple.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "SimulationMessenger.hh"

// PIMPLEd dtor:
TheSimulation::~TheSimulation() {}

TheSimulation::TheSimulation() {
    // Run manager
    theRunManager.reset(new G4RunManager);

    //The primary default messenger for DLG4::GeoModules is not this
    //It is GeoModulesMessenger.hh/cc
    theSimulationMessenger.reset(new SimulationMessenger());
    theRunManager->SetUserInitialization(theDetectorConstruction);
//    theRunManager->SetUserInitialization(new CupPhysicsList);

    // Example of physics and tracking used in a cupsim implementation:
    // theRunManager->SetUserAction(new CupPrimaryGeneratorAction(nullptr));
    // theRunManager->SetUserAction(new CupRunAction(myRecords.get()));
    // theRunManager->SetUserAction(new CupVEventAction(myRecords.get()));
    // theRunManager->SetUserAction(new CupTrackingAction(myRecords.get()));
    // theRunManager->SetUserAction(new CupSteppingAction(myRecords.get()));

    // For the GeoModules Demo, we'll just stub it out.

    class MinimalPhysicsList: public G4VUserPhysicsList {
    public:
        void ConstructParticle() override {
        }

        void ConstructProcess() override {
        }

        void SetCuts() override {
        }
    };

    theRunManager->SetUserInitialization(new MinimalPhysicsList());

}
