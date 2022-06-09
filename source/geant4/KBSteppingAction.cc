#include "KBSteppingAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

KBSteppingAction::KBSteppingAction()
: G4UserSteppingAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
}

KBSteppingAction::KBSteppingAction(KBG4RunManager *man)
: G4UserSteppingAction(), fRunManager(man)
{
}

void KBSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();

  G4ThreeVector pos = step -> GetTrack() -> GetPosition();
  G4ThreeVector mom = step -> GetTrack() -> GetMomentum();
  G4int preNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	// if ( preNo<=-1 ) return;
    G4double edep = step -> GetTotalEnergyDeposit(); 

    G4int parentID = step -> GetTrack() -> GetParentID(); 
    G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
    //G4ThreeVector stepPos = .5 * (step -> GetPreStepPoint() -> GetPosition() + step -> GetPostStepPoint() -> GetPosition());
	G4ThreeVector stepPos = step -> GetPreStepPoint() -> GetPosition();
//   if(parentID ==0){
    fRunManager -> AddMCStep(preNo, stepPos.x(), stepPos.y(), stepPos.z(), time, edep);
//   }

  if (stat == fWorldBoundary) {
    fRunManager -> AddTrackVertex(mom.x(),mom.y(),mom.z(),preNo,pos.x(),pos.y(),pos.z());
    return;
  }


  G4int postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
  if (preNo != postNo || step -> GetNumberOfSecondariesInCurrentStep() > 0)
    fRunManager -> AddTrackVertex(mom.x(),mom.y(),mom.z(),preNo,pos.x(),pos.y(),pos.z());





}
