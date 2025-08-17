/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/
#include "TheSimulation.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "DetectorMessenger.hh"

TheSimulation::TheSimulation() {
    // Run manager

    // UserInitialization classes
    theDetectorConstruction.reset(new DetectorConstruction);
    theMessenger.reset(new DetectorMessenger(theDetectorConstruction.get()));
    theRunManager.reset(new G4RunManager);

    theRunManager->SetUserInitialization(theDetectorConstruction.get());
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

