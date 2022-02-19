#include "KBTrackingAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VProcess.hh"

KBTrackingAction::KBTrackingAction()
: G4UserTrackingAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fProcessTable = fRunManager -> GetProcessTable();
}

KBTrackingAction::KBTrackingAction(KBG4RunManager *man)
: G4UserTrackingAction(), fRunManager(man)
{
  fProcessTable = fRunManager -> GetProcessTable();
}

void KBTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4ThreeVector momentum = track -> GetMomentum();
  G4ThreeVector position = track -> GetPosition();
  G4int volumeID = track -> GetVolume() -> GetCopyNo();

  const G4VProcess *process = track -> GetCreatorProcess();
  G4String processName = "Primary";
  if (process != nullptr)
    processName = process -> GetProcessName();
  G4int processID = fProcessTable -> GetParInt(processName);

  fRunManager -> AddMCTrack(0, track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), momentum.x(), momentum.y(), momentum.z(), volumeID, position.x(), position.y(), position.z(),0, 0, 0, processID);
}

void KBTrackingAction::PostUserTrackingAction(const G4Track* track)
{
	G4double energy = track -> GetKineticEnergy();
	if ( energy<1e-10 ) return;

  G4ThreeVector momentum = track -> GetMomentum();
  G4ThreeVector position = track -> GetPosition();
  G4int volumeID = track -> GetVolume() -> GetCopyNo();

  const G4VProcess *process = track -> GetCreatorProcess();
  G4String processName = "Primary";
  if (process != nullptr)
    processName = process -> GetProcessName();
  G4int processID = fProcessTable -> GetParInt(processName);

  fRunManager -> AddMCTrack(1, track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), momentum.x(), momentum.y(), momentum.z(), volumeID, position.x(), position.y(), position.z(), 0, 0, 0, processID);
}
