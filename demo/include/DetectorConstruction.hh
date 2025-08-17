/**@file
 DetectorConstruction main class for GenericLAND simulation

 Original by Glenn Horton-Smith, December 2004.
 (Based on earlier work first written Dec 1999.)
 Some more materials and their classification by Dario Motta, Jan 2005.

 This file is part of the GenericLAND software library.

 @author Glenn Horton-Smith
 @author Dario Motta
 Complete overhaul by D. Leonard, most recently 2021.
 */

#ifndef DetectorConstruction_HH
#define DetectorConstruction_HH 1


//  Make a first pass on pluggable includes to pickup #defines
#include "include/autoincludes.inc"
#define HPGE_INCLUDES_INITIALIZED
// Make a second pass to produce conditional #defines
#include <G4RunManager.hh>
#include "G4VUserDetectorConstruction.hh"

#include "include/autoincludes.inc"

class DetectorConstruction final: public G4VUserDetectorConstruction {
public:
    // returns a copy of a material
    static G4Material *CopyMaterial(G4Material *source, G4String name);

    static G4Material *CopyMaterial(G4Material *source, G4String name, G4double dens);

    DetectorConstruction();           //constructor
    ~DetectorConstruction() override; // destructor

    G4VPhysicalVolume *Construct() override; // make the volumes, return ptr to world

    static G4String GetWhichDetGeometry() {
        return whichDetGeometry;
    }

    static G4String GetDetGeometryType() {
        return DetGeometryType;
    }

    virtual void SetWhichDetGeometry(G4String w) {
        whichDetGeometry = w;
    }

    void ListGeometries();

protected:
    // a map containing any values provided for extra commands.
    static G4String whichDetGeometry;
    static G4String DetGeometryType;
    class DetectorMessenger *Messenger;
    // now declared through a pluggable include:
    //        void ConstructMaterials();  // make all needed materials

    // Get globals from pluggable includes:
#           define INCLUDE_SIM_GLOBALS
#           include "include/autoincludes.inc"

#           undef INCLUDE_SIM_GLOBALS


    /* D.L stores named geometry build sequences */
    /* These still get executed later */
    /* But having it here means the named list becomes available for the Messenger
     without maintenance scattered in difference places */

public:
    std::map<G4String, G4String> ExtraOpts;
    // basically an associative array of function pointers with string keys.
    // a map connecting options to the "detGeometry/select " to procedure:
    std::map<G4String, void (*)(DetectorConstruction *c)> Geometries;

    G4VPhysicalVolume *world_phys;

    void InitializeGeos() {
        // Get geometry definitions from pluggable includes:
#           define INCLUDE_SIM_GEOMETRIES
        typedef DetectorConstruction object;
#           include "include/autoincludes.inc"
#include "../src/Geometries/ConstructHexPart.inc"

#           undef INCLUDE_SIM_GEOMETRIES
    }
};


#endif