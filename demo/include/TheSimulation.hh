#pragma once
/**
* @file
* Created by @author Douglas S. Leonard on @date 7/12/25.  All rights Reserved
* See related files for license, if any is provided.
* 
* @brief
*
*/

//

#ifndef THESIMULATION_HH
#define THESIMULATION_HH
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "DetectorMessenger.hh"


class TheSimulation {
    // D.S.L.'s memory managment, no more delete:
public:
    TheSimulation();

    ~TheSimulation() {
        // Because CERN memory management is pointer party like it's 1999 --DSL:
        theDetectorConstruction.release();
    }

    void ListGeometries() const {
        theDetectorConstruction->ListGeometries();
    }

private:
    std::unique_ptr<G4RunManager> theRunManager;
    std::unique_ptr<DetectorConstruction> theDetectorConstruction;
    std::unique_ptr<G4UImessenger> theMessenger;
    // probably not used:
    std::unique_ptr<G4UImessenger> theDebugMessenger;
};


#endif //THESIMULATION_HH