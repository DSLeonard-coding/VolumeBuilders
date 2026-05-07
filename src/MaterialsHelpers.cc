//GeoModulesCopyMaterials.  D.S. Leonard 2026 (code is much older, ~ 2017
#include "MaterialsHelpers.hh"
#include "G4NistManager.hh"
using namespace CLHEP;

namespace DLG4::VolumeBuilders::Helpers {


    G4Material *GetMaterial(const G4String &materialName) {
        // Check local material store (materials already created/defined)
        // 2nd parameter is warn if missing.
        G4Material *mat = G4Material::GetMaterial(materialName, false);

        if (mat) {
            return mat;
        }

        // Fallback to NIST manager
        G4NistManager *nist = G4NistManager::Instance();
        mat = nist->FindOrBuildMaterial(materialName);

        if (!mat) {
            G4Exception("GeoModulesContext::GetMaterial", "MatNotFound",
                    FatalException, ("Could not find material: " + materialName).c_str());
        }

        return mat;
    }

    G4Element *GetElement(const G4String &elementName) {
        // Check the GIlobal Element Table first
        G4Element *el = G4Element::GetElement(elementName, false);

        if (el) {
            return el;
        }

        // Fallback to NIST manager for standard elements
        G4NistManager *nist = G4NistManager::Instance();
        el = nist->FindOrBuildElement(elementName);

        if (!el) {
            G4Exception("GeoModulesContext::GetElement", "ElementNotFound",
                    FatalException, ("Could not find element: " + elementName).c_str());
        }

        return el;
    }

    G4Material *CopyMaterial(
            G4MaterialPtrOrString source, const G4String &name, const G4double dens, const G4double unit) {
        const double dens_with_unit = dens * unit;
        constexpr int n_components = 1;
        constexpr G4double fraction_mass = 100 * perCent;
        const auto dest = new G4Material(name, dens_with_unit, n_components);
        dest->AddMaterial(source, fraction_mass);
        return dest;
    }
    // G4Material *CopyMaterial(
    //         const G4String &source, const G4String &name, const G4double dens, const G4double unit) {
    //     return CopyMaterial(GetMaterial(source), name, dens, unit);
    // }

    G4Material *CopyMaterial(G4MaterialPtrOrString source, const G4String &name) {
        const G4double dens = source->GetDensity();
        const auto dest = CopyMaterial(source, name, dens, 1);
        return dest;
    }

}
