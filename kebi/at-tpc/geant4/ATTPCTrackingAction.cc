#include "ATTPCTrackingAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VProcess.hh"

ATTPCTrackingAction::ATTPCTrackingAction()
: G4UserTrackingAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fProcessTable = fRunManager -> GetProcessTable();
}

ATTPCTrackingAction::ATTPCTrackingAction(KBG4RunManager *man)
: G4UserTrackingAction(), fRunManager(man)
{
  fProcessTable = fRunManager -> GetProcessTable();
}

void ATTPCTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4int parentID = track -> GetParentID();
  G4int PDG = track -> GetDefinition() -> GetPDGEncoding();

  if (parentID == 1 && PDG == 11) {
    G4ThreeVector momentum = track -> GetMomentum();
    G4ThreeVector position = track -> GetPosition();
    G4double KEnergy = track -> GetKineticEnergy();
    G4int volumeID = track -> GetVolume() -> GetCopyNo();
    
    const G4VProcess *process = track -> GetCreatorProcess();
    G4String processName = "Primary";
    if (process != nullptr)
      processName = process -> GetProcessName();
    G4int processID = fProcessTable -> GetParInt(processName);
    
    fRunManager -> AddMCTrack(track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), momentum.x(), momentum.y(), momentum.z(), volumeID, position.x(), position.y(), position.z(), KEnergy, processID);
  }
}
