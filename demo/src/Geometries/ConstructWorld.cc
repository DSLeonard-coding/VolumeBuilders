#include "DetectorConstruction_includes.hh" // common includes
#include "VolumeBuilders.hh"
using namespace CLHEP;
namespace VB = DLG4::VolumeBuilders; // Geometry builder helpers.

void DetectorConstruction::ConstructWorld() {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;

    //    original version of this code snippet for comparisson:
    //    auto *zero_rot = new G4RotationMatrix();
    //    zero_rot->rotateY(0. * deg);   // null rotation, probably redundant.
    //    auto zero_shift = G4ThreeVector(0, 0, 0); // no pointer, because geant copies them.
    //
    //    G4double bounding_size = 3. * meter / 2.0;
    //    auto boxHall = new G4Box("worldbox", bounding_size, bounding_size, bounding_size);
    //    auto logiHall = new G4LogicalVolume(boxHall, _air, "logiHall");
    //    auto logiHallVis = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8, 0.1));
    //    logiHall->SetVisAttributes(logiHallVis);
    //    logiHall->SetVisAttributes(G4VisAttributes::Invisible);
    //    G4VPhysicalVolume *physHall =
    //        new G4PVPlacement(0, zero_shift, logiHall, "world", NULL, false, 0);
    //    world_phys = physHall;

    // now just becomes:
    VB::SetGlobalDefaultUnit(CLHEP::mm);
    world_phys = VB::CreateCenteredBoxBuilder("hallbox", 3000, 3000, 3000)
                 ->SetMaterial(_air)
                 ->SetColor(0.8, 0.8, 0.8, 0.1)
                 ->SetVisibility(false)
                 ->GetPlacement();
}
