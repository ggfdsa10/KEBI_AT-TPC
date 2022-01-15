#include "NewTPCTrackingAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VProcess.hh"

NewTPCTrackingAction::NewTPCTrackingAction()
: G4UserTrackingAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fProcessTable = fRunManager -> GetProcessTable();
}

NewTPCTrackingAction::NewTPCTrackingAction(KBG4RunManager *man)
: G4UserTrackingAction(), fRunManager(man)
{
  fProcessTable = fRunManager -> GetProcessTable();
}

void NewTPCTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4int parentID = track -> GetParentID();
  G4int trackID = track -> GetTrackID();
  G4int PDG = track -> GetDefinition() -> GetPDGEncoding();
  G4int source = track -> GetTrackID();

  if ((parentID < 4 && PDG == 11) || trackID < 4 ) {      //(PDG == 1000020040) || (PDG == 2212) || (PDG == 13)
    G4ThreeVector momentum = track -> GetMomentum();
    G4ThreeVector position = track -> GetPosition();
    G4double KEnergy = track -> GetKineticEnergy();
    G4int volumeID = track -> GetVolume() -> GetCopyNo();
    
    const G4VProcess *process = track -> GetCreatorProcess();

    G4String processName = "Primary";
    if (process != nullptr)
      processName = process -> GetProcessName();
    G4int processID = fProcessTable -> GetParInt(processName);

    Double_t edep1 =0.;
    Double_t edep2 =0.;
    fRunManager -> AddMCTrack(track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), momentum.x(), momentum.y(), momentum.z(), volumeID, position.x(), position.y(), position.z(), KEnergy, edep1, edep2, processID);
  }
}