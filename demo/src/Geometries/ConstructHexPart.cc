#include "DetectorConstruction_includes.hh" // common includes
#include "VolumeBuilders.hh"
using namespace CLHEP;
using namespace DLG4::VolumeBuilders; // Geometry builder helpers.

void DetectorConstruction::ConstructHexPart() {
    static bool firstcall = true;
    if (!firstcall) {
        // only run once
        return;
    }
    firstcall = false;

    //make a dedicated sample_material for vertex generation:
    // Density in materials is 1.14, but internet :) says 1.18 to 1.19 for cast acrylic.  Should weigh it really:
    G4Material *sample_mat = DetectorConstruction::CopyMaterial(_copper, "sample_mat");
    G4cout << "now ConstructArrayHexPart()\n" << G4endl;

    G4double sample_thickness_mm = 50;
    if (ExtraOpts.find("hex_insert_mm") != ExtraOpts.end()) {
        //if option is set
        std::stringstream ss(ExtraOpts["hex_insert_mm"]);
        ss >> sample_thickness_mm; //read it
    } else {
        //else set default but complain
        // Echo command instructions:
        sample_thickness_mm = 50.;
        G4cout << "Use:" << G4endl;
        G4cout << "    /detGeometry/opt \"hex_insert_mm\" " << G4endl;
        G4cout << "    to set thickness of hex shield insert" << G4endl;
        G4cout << "Assuming default value of:" << 50 << false << G4endl;
    }

    //  Copper  hex:  //////////////////////
    RZPlane p; // just for clarity and temps, since we use the component setting method here:
    auto hex_filler = CreatePolyhedraBuilder(
                          "hex_filler",
                          6,
                          30.) // start at 30 deg
                      ->SetVisibility(true)
                      ->SetMaterial(sample_mat)
                      ->SetColor(0.8, 0.45, 0.45, 1)
                      ->ForceSolid(true)
                      ->SetDefaultUnit(CLHEP::mm)
            //@formatter:off
              ->AddPlane(p.IR = 0,  p.OR = 110.0 / 2.0, p.z = 0)
              ->AddPlane(p.IR,      p.OR,               p.z += sample_thickness_mm);

    //*******************WARNING  This may not be exactly the same as top of surround, derived in different ways.
    //*******************samples that sit on both should be reviewed  for overlap ex:  Cu2019 sample
    auto filler_id = CreatePolyconeBuilder("filler_cu_id") // will be unioned to hex_filler.
                ->AddPlane(p.IR = 0,    p.OR = 85 / 2.0,    p.z = -0.1)
                ->AddPlane(p.IR,        p.OR,               p.z += sample_thickness_mm+0.1);
    //@formatter:on

    hex_filler->AddSubtraction(filler_id)
              ->SetBooleanName("hex_insert");


    hex_filler
        ->SetMother(world_phys)
        ->PlaceAndFork();
}
