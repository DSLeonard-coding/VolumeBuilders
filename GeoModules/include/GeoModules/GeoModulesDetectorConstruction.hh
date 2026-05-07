/**@file

 @author D. S. Leonard
 This once evolved from versions through G.H. smith and others,
 but the 2025 version guts literally everything and starts over.
 There's almost nothing left here anyway.
 */

#ifndef GeoModulesDetectorConstruction_HH
#define GeoModulesDetectorConstruction_HH 1
#include <G4VUserDetectorConstruction.hh>
#include "GeoModules/IGeoModule.hh"
#include "GeoModules/GeoModulesParser.hh"

using namespace DLG4::GeoModules;
//  Make a first pass on pluggable includes to pickup #defines

///@class DetectorConstruction DetectorConstruction.hh "DetectorConstruction.hh"
///The main detector setup and construction class
///Ultimately responsible for building the world volume.
///But everything has been delegated out to GeoModulesParser now.
///GeoModulesParser pulls geometry modules, each a GeoModule subclass,
///and recipes of modules to run.  It must
/// handle recipe selection through macros, and it owns the messenger too.
///Leaving this as non-final, but if you want inheritance, consider inherritting
///from GeoModulesParser directly.  The most transparent though is to just make a new
/// class composed like this one and add what you need.
class GeoModulesDetectorConstruction: public G4VUserDetectorConstruction {
public:
    GeoModulesDetectorConstruction() = default;
    ~GeoModulesDetectorConstruction() override = default; // destructor

    // These should not be here
    // returns a copy of a material
    static G4Material *CopyMaterial(G4Material *source, const G4String &name);
    static G4Material *CopyMaterial(G4Material *source, const G4String &name, G4double dens, G4double unit);

    // make the volumes; return ptr to world
    G4VPhysicalVolume *Construct() override { return geoModulesParser_->Construct(); };
    virtual GeoModulesParserPtr GetGeoModulesParser() const { return geoModulesParser_; }

protected:
    GeoModulesParserPtr geoModulesParser_ = std::make_shared<GeoModulesParser>();
};

#endif
