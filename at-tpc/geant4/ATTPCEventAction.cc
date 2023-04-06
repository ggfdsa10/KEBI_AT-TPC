#include "ATTPCEventAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "ATTPCSteppingAction.hh"
#include "ATTPCRandomPrimaryGenerate.hh"

ATTPCEventAction::ATTPCEventAction()
: G4UserEventAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
}

ATTPCEventAction::ATTPCEventAction(KBG4RunManager *man)
: G4UserEventAction(), fRunManager(man)
{
    fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
    fPar = fRunManager -> GetParameterContainer();
}

void ATTPCEventAction::BeginOfEventAction(const G4Event* event)
{
    fEventNumber = event -> GetEventID();
    fStepNum = 0;
    if(fPar -> GetParBool("Physics")){
        ATTPCRandomPrimaryGenerate* PrimaryGen = (ATTPCRandomPrimaryGenerate*)fRunManager -> GetUserPrimaryGeneratorAction();
        fInteractionEnergy = PrimaryGen -> GetInteractionEnergy();
    }
}

void ATTPCEventAction::EndOfEventAction(const G4Event* event)
{
    if(fPar -> GetParBool("Physics")){
        if(fEventNumber%2 == 0){
            ATTPCRandomPrimaryGenerate* PrimaryGen = (ATTPCRandomPrimaryGenerate*)fRunManager -> GetUserPrimaryGeneratorAction();
            PrimaryGen -> SetInteractionPoint(fInteractionPos, fInteractionMom);
        }
        if(fEventNumber%2 ==1){fRunManager -> NextEvent();}
    }
    else{fRunManager -> NextEvent();}
}
 
bool ATTPCEventAction::IsInteractionEvent()
{
    if(fEventNumber%2 == 0){
        fStepNum += 1;
        if(fStepNum == 1){return true;}
        else{return false;}
    }
    else{return false;}
}

void ATTPCEventAction::SetInteractionPoint(TVector3 pos, TVector3 mom)
{
    fInteractionPos = pos;
    fInteractionMom = mom;
}