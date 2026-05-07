//GeoModulesCopyMaterials.  D.S. Leonard 2026 (code is much older, ~ 2017
#pragma once
#include "VolumeBuildersTypes.hh"
#include <G4Material.hh>
#include <G4String.hh>
#include <G4Element.hh>
#ifndef GEOMODULE_MATERIALS_HELPERS_HH
#define GEOMODULE_MATERIALS_HELPERS_HH
using namespace CLHEP;

namespace DLG4::VolumeBuilders::Helpers {
    /// Copy a G4Material to one with a new name, and change its density
    G4Material *CopyMaterial(
            G4MaterialPtrOrString source, const G4String &name, G4double dens, G4double unit);

    /// Copy a G4Material to one with a new name.
    G4Material *CopyMaterial(G4MaterialPtrOrString source, const G4String &name);

    /**
     * Get material by local or NIST name.
     * @param materialName
     * @return
     */
    G4Material *GetMaterial(const G4String &materialName);
    G4Element *GetElement(const G4String &elementName);

    /// @class G4MaterialPtrOrString Auto-converter for materials parameters.
    /// Takes a G4Material*  or a String name of a local or NIST material
    /// or whatever GetMaterials presently supports.
    class G4MaterialPtrOrString {
    public:
        G4MaterialPtrOrString()
            : m_ptr(nullptr) {
        }

        // Add this constructor to handle "literal strings"
        G4MaterialPtrOrString(const char *name)
            : m_ptr(VBHelpers::GetMaterial(G4String(name))) {
        }

        // Converting constructor from G4Material*
        G4MaterialPtrOrString(G4Material *mat)
            : m_ptr(mat) {
        }

        // Converting constructor from string
        G4MaterialPtrOrString(const G4String &name)
            : m_ptr(VBHelpers::GetMaterial(name)) {
        }

        // Conversion operator to G4Material*
        operator G4Material *() const { return m_ptr; }
        // Arrow operator to use it like a pointer directly
        G4Material *operator->() const { return m_ptr; }
        // Explicit bool operator prevents accidental math/logic errors
        // but allows use in 'if' statements.
        explicit operator bool() const {
            return m_ptr != nullptr;
        }

    private:
        G4Material *m_ptr;
    };
}
#endif
