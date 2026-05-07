//GeoModulesCopyMaterials.  D.S. Leonard 2026 (code is much older, ~ 2017
#include <G4Material.hh>
#include <G4String.hh>
#include <G4Element.hh>
#ifndef GEOMODULE_MATERIALS_HELPERS_HH
#define GEOMODULE_MATERIALS_HELPERS_HH
using namespace CLHEP;

namespace DLG4::GeoModules {
        G4Material *CopyMaterial(
                        G4Material *source, const G4String &name, const G4double dens, const G4double unit);

        G4Material *CopyMaterial(
                        const G4String &source, const G4String &name, const G4double dens, const G4double unit);

        G4Material *CopyMaterial(G4Material *source, const G4String &name);

        G4Material *CopyMaterial(const G4String &source, const G4String &name);

        G4Material *GetMaterial(const G4String &materialName);

        G4Element *GetElement(const G4String &elementName);
#endif
}
