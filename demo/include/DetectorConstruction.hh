/**@file

 @author D. S. Leonard
 This once evolved from versions through G.H. smith and others,
 but the 2025 version guts literally everything and starts over.
 There's almost nothing left here anyway.
 */

#ifndef DetectorConstruction_HH
#define DetectorConstruction_HH 1
#include "../../GeoModules/include/GeoModules/GeoModulesDetectorConstruction.hh"

using namespace DLG4::GeoModules;
//  Make a first pass on pluggable includes to pickup #defines

///@class DetectorConstruction DetectorConstruction.hh "DetectorConstruction.hh"
///The main detector setup and construction class
///  This is just a shim/wrapper. See GeoModulesDetectorConstruction for details.
///  You could as well just use that directly if not adding anything here:
class DetectorConstruction: public GeoModulesDetectorConstruction {
public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default; // destructor
};

#endif
