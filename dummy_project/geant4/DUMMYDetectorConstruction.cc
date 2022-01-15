#include "DUMMYDetectorConstruction.hh"
#include "KBG4RunManager.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

/**
 * One should use KBG4RunManager::SetSensitiveDetector(G4PVPlacement *) method
 * for data taken in the detector volume to be written to ouput file.
 *
 * It is VERY IMPORTANT that the copyNo of the PVPlacement to be different for
 * sensitive detectors. The branch name of the in the ouput will be "MCStep[copyNo]".
 */

DUMMYDetectorConstruction::DUMMYDetectorConstruction()
: G4VUserDetectorConstruction()
{
}

DUMMYDetectorConstruction::~DUMMYDetectorConstruction()
{
}

G4VPhysicalVolume* DUMMYDetectorConstruction::Construct()
{  
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  G4NistManager* nist = G4NistManager::Instance();

  G4int copyNo = 0;

  G4Material* world_mat = nist -> FindOrBuildMaterial("G4_AIR");
  G4double world_size = 100*mm;
  G4Box* solidWorld = new G4Box("World", 0.5*world_size, 0.5*world_size, 0.5*world_size);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, copyNo=-1, true);

  G4Material* detector_mat = nist -> FindOrBuildMaterial("G4_WATER");
  G4double detector_size = 20*mm;
  G4double detector_offset_z = 30*mm;
  G4Box* solidDetector = new G4Box("Detector", 0.5*detector_size, 0.5*detector_size, 0.5*detector_size);
  G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector, detector_mat, "Detector");
  auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,detector_offset_z), logicDetector, "Detector", logicWorld, false, copyNo=0, true);

  runManager -> SetSensitiveDetector(pvp);

  return physWorld;
}
