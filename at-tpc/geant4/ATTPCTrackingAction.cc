#include <algorithm>
#include "ATTPCTrackingAction.hh"

#include "G4Event.hh"
#include "KBEventAction.hh"
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
  fTrackLength.clear();
  fTrackEdep.clear();
  fDiffLengthBuffer1.clear();
  fDiffLengthBuffer2.clear();
  fEdep = 0.;

  G4int parentID = track -> GetParentID();
  G4int PDG = track -> GetDefinition() -> GetPDGEncoding();

  fPrimaryGerateAction = (ATTPCRandomPrimaryGenerate*)KBG4RunManager::GetRunManager() -> GetUserPrimaryGeneratorAction();
  fNumberOfPrimary = fPrimaryGerateAction -> GetNumberOfPrimary();

  if (parentID == 0 || (parentID <= fNumberOfPrimary && PDG == 11)) {
    fMomentum = track -> GetMomentum();
    fPosition = track -> GetPosition();
    fKEnergy = track -> GetKineticEnergy();
    fVolumeID = track -> GetVolume() -> GetCopyNo();

    const G4VProcess *process = track -> GetCreatorProcess();
    Double_t edep1 = 0.;
    Double_t edep2 = 0.;

    G4String processName = "Primary";
    if (process != nullptr)
      processName = process -> GetProcessName();
    fProcessID = fProcessTable -> GetParInt(processName);
    fRunManager -> AddMCTrack(0, track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), fMomentum.x(), fMomentum.y(), fMomentum.z(), fVolumeID, fPosition.x(), fPosition.y(), fPosition.z(), fKEnergy, edep1, edep2, fProcessID);


  }
}

void ATTPCTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  G4int parentID = track -> GetParentID();
  G4int PDG = track -> GetDefinition() -> GetPDGEncoding();

  if (parentID == 0  || (parentID <= fNumberOfPrimary && PDG == 11)) {
    Double_t edep1 = 0.;
    Double_t edep2 = 0.;

    if(PDG==2212){

      Double_t corrLength = fTrackLength.front();
      Double_t TotalLength = fTrackLength.back() - corrLength;
      fTrackEndEPoint = TotalLength - fTrackStartPoint;

      for(int i=0; i<fTrackLength.size(); i++){
        fDiffLengthBuffer2.push_back(abs(fTrackLength[i] - fTrackEndEPoint));
      }

      int indexEPoint1 = min_element(fDiffLengthBuffer1.begin(), fDiffLengthBuffer1.end()) -fDiffLengthBuffer1.begin();
      int indexEPoint2 = min_element(fDiffLengthBuffer2.begin(), fDiffLengthBuffer2.end()) -fDiffLengthBuffer2.begin();

      edep1 = fTrackEdep[indexEPoint1];
      edep2 = fTrackEdep[indexEPoint2];
    }
    // fRunManager -> AddMCTrack(track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), fMomentum.x(), fMomentum.y(), fMomentum.z(), fVolumeID, fPosition.x(), fPosition.y(), fPosition.z(), fKEnergy, edep1, edep2, fProcessID);
  }

}

void ATTPCTrackingAction::SetPrimaryEdep(Double_t length, Double_t edep)
{
  fEdep += abs(edep);
  fTrackLength.push_back(length);
  fTrackEdep.push_back(fEdep);
  fDiffLengthBuffer1.push_back(abs(length -fTrackStartPoint));
}