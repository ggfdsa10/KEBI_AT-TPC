#include "NewTPCSteppingAction.hh"

#include "TVector3.h"
#include "G4Event.hh"
#include "G4RunManager.hh"

NewTPCSteppingAction::NewTPCSteppingAction()
: G4UserSteppingAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
}

NewTPCSteppingAction::NewTPCSteppingAction(KBG4RunManager *man)
: G4UserSteppingAction(), fRunManager(man)
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
}

void NewTPCSteppingAction::UserSteppingAction(const G4Step* step)
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
  G4int trackID = step -> GetTrack() -> GetTrackID();

  if(parentID ==0){
    G4double edep = step -> GetTotalEnergyDeposit(); 
    G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
    G4ThreeVector stepPos = step -> GetPreStepPoint() -> GetPosition();
    fRunManager -> AddMCStep(preNo, stepPos.x(), stepPos.y(), stepPos.z(), time, edep);
  }

  // for Physics interaction
  if(fPar->GetParBool("Physics") && parentID == 0 && trackID == 1){
    NewTPCEventAction* EventAction = (NewTPCEventAction *) G4EventManager::GetEventManager() -> GetUserEventAction();

    if(EventAction -> GetInteractionEnergy() > step -> GetPreStepPoint() -> GetKineticEnergy() &&  EventAction->IsInteractionEvent()){
      TVector3 position;
      position.SetXYZ(pos.x(), pos.y(), pos.z());
      TVector3 momentum;
      momentum.SetXYZ(mom.x(), mom.y(), mom.z());
      EventAction -> SetInteractionPoint(position , momentum);
    }
  }


}