// GeoModule
// Do not directly include the .incl file.   GeoModulesParser will do it correctly.
#include "GeoModules/GeoModulesParser.hh"
#include "DetectorConstruction_includes.hh" // common includes
// author D.S. Leonard, major rewrite in 2025.
using namespace CLHEP;

//TODO  these have no reason to be here and are slated to be moved to VolumeBuilders probably.
//takes <source> material and returns a pointer to ac opy named <name>
// G4Material copy constructor is private, so leverage Addmaterial at 100%
//  DL 2020.
G4Material *GeoModulesDetectorConstruction::CopyMaterial(
    G4Material *source, const G4String &name, const G4double dens, const G4double unit) {
    const double dens_with_unit=dens*unit;
    constexpr int n_components = 1;
    constexpr G4double fraction_mass = 100 * perCent;
    const auto dest = new G4Material(name, dens_with_unit, n_components);
    dest->AddMaterial(source, fraction_mass);
    return dest;
}


G4Material *GeoModulesDetectorConstruction::CopyMaterial(G4Material *source, const G4String &name) {
    const G4double dens = source->GetDensity();
    const auto dest = CopyMaterial(source, name, dens, 1);
    return dest;
}
