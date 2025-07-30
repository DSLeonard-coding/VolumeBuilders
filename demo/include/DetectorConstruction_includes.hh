#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

#include "globals.hh"

#include "DetectorConstruction.hh"  // the DetectorConstruction class header

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SDManager.hh"

#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"

#include "G4OpticalSurface.hh"

#include "G4EllipticalTube.hh"
#include "G4EllipticalCone.hh"

#include "G4Material.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4TransportationManager.hh"
#include "G4UnionSolid.hh"
#include "G4SmartVoxelHeader.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"

#include "G4Types.hh"
#include <sstream>

#include "G4NistManager.hh"
