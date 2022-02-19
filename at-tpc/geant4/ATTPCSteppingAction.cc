#include "ATTPCSteppingAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

ATTPCSteppingAction::ATTPCSteppingAction()
: G4UserSteppingAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
}

ATTPCSteppingAction::ATTPCSteppingAction(KBG4RunManager *man)
: G4UserSteppingAction(), fRunManager(man)
{
}

void ATTPCSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();

  G4ThreeVector pos = step -> GetTrack() -> GetPosition();
  G4ThreeVector mom = step -> GetTrack() -> GetMomentum();
  G4int preNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

  if (stat == fWorldBoundary){
    fRunManager -> AddTrackVertex(mom.x(),mom.y(),mom.z(),preNo,pos.x(),pos.y(),pos.z());
    return;
  }

  G4int postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

  if (preNo != postNo || step -> GetNumberOfSecondariesInCurrentStep() > 0){
    fRunManager -> AddTrackVertex(mom.x(),mom.y(),mom.z(),preNo,pos.x(),pos.y(),pos.z());
  }

  G4int parentID = step -> GetTrack() -> GetParentID(); 
  if(parentID ==0){
    G4double edep = step -> GetTotalEnergyDeposit(); 
    G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
    G4ThreeVector stepPos = step -> GetPreStepPoint() -> GetPosition();
    fRunManager -> AddMCStep(preNo, stepPos.x(), stepPos.y(), stepPos.z(), time, edep);
  }

  if(parentID == 0 && postNo ==1){
      ATTPCTrackingAction *trackAction = (ATTPCTrackingAction *) G4EventManager::GetEventManager() -> GetUserTrackingAction();

      G4double length = step -> GetTrack() -> GetTrackLength();
      G4double deltaEdep = step -> GetDeltaEnergy(); 
      
      trackAction -> SetPrimaryEdep(length, deltaEdep);
  }

}