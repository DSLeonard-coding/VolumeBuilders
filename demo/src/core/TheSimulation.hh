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



class TheSimulation {
    // D.S.L.'s memory managment, no more delete:
public:
    TheSimulation();

    ~TheSimulation(); //defaulted in cc for PIMPL

    void ListGeoRecipes() const {
        theDetectorConstruction->GetGeoModulesParser()->ListRecipes();
    }

    void ListGeoModules() const {
        theDetectorConstruction->GetGeoModulesParser()->ListModules();
    }

private:
    //The messenger is no handled by and in the GeoModulesParser
    std::unique_ptr<G4RunManager> theRunManager;
    //Eclude ntuples for demo:
    //std::unique_ptr<RootNtuple> myRecords;
    std::unique_ptr<G4UImessenger> theSimulationMessenger;
    // seems that theRunManager->SetUserInitialization takes ownership/cleans-up (undocumented "feature")
    // So we just keep a raw pointer:
    DetectorConstruction * const theDetectorConstruction = new DetectorConstruction();

};


#endif //THESIMULATION_HH
