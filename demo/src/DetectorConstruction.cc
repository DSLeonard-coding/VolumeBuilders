#include "DetectorConstruction_includes.hh" // common includes
#include "DetectorMessenger.hh"

using namespace CLHEP;

//int DetectorConstruction::whichDetGeometry = 99999;

G4String DetectorConstruction::whichDetGeometry = "";
G4String DetectorConstruction::DetGeometryType = "";


DetectorConstruction::DetectorConstruction()
{
    InitializeGeos();
//    whichDetector = kDetector_Detector;
    Messenger = new DetectorMessenger(this);
    ListGeometries();
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

    // delete the old detector if we are constructing a new one
    if (world_phys) {
        delete world_phys;
        world_phys = NULL;
    }
//D.L now does this with associative lookup of the procedure.
// removes code maintenance from here.
    if (Geometries.find(whichDetGeometry) != Geometries.end()) {
        Geometries[whichDetGeometry](this);
    } else {
        G4cerr << "ERROR: invalid geometry option"
                << G4endl << std::flush;
    }

    return world_phys;
}

void DetectorConstruction::ListGeometries() const {
    G4cout << "Detector geometries: ";
    for (const auto &geo : Geometries) {
        G4cout << "Registred geometry: " << geo.first << G4endl;
    }
    G4cout << G4endl;
}


//takes <source> material and returns a pointer to ac opy named <name>
// G4Material copy constructor is private, so leverage Addmaterial at 100%
//  DL 2020.
G4Material* DetectorConstruction::CopyMaterial(G4Material *source, G4String name, G4double dens) {

    int ncomponents = 1;
    G4double fractionmass = 100 * perCent;
    G4Material *dest = new G4Material(name, dens, ncomponents);
    dest->AddMaterial(source, fractionmass);
    return dest;
}

G4Material* DetectorConstruction::CopyMaterial(G4Material *source, G4String name) {
    const G4double dens = source->GetDensity();
    G4Material *dest = CopyMaterial(source, name, dens);
    return dest;
}

