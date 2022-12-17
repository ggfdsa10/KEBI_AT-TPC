#include "NewTPCEventAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "NewTPCSteppingAction.hh"
#include "NewTPCRandomPrimaryGenerate.hh"

NewTPCEventAction::NewTPCEventAction()
: G4UserEventAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
}

NewTPCEventAction::NewTPCEventAction(KBG4RunManager *man)
: G4UserEventAction(), fRunManager(man)
{
    fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
    fPar = fRunManager -> GetParameterContainer();
}

void NewTPCEventAction::BeginOfEventAction(const G4Event* event)
{
    fEventNumber = event -> GetEventID();
    fStepNum = 0;
    if(fPar -> GetParBool("Physics")){
        NewTPCRandomPrimaryGenerate* PrimaryGen = (NewTPCRandomPrimaryGenerate*)fRunManager -> GetUserPrimaryGeneratorAction();
        fInteractionEnergy = PrimaryGen -> GetInteractionEnergy();
    }
}

void NewTPCEventAction::EndOfEventAction(const G4Event* event)
{
    if(fPar -> GetParBool("Physics")){
        if(fEventNumber%2 == 0){
            NewTPCRandomPrimaryGenerate* PrimaryGen = (NewTPCRandomPrimaryGenerate*)fRunManager -> GetUserPrimaryGeneratorAction();
            PrimaryGen -> SetInteractionPoint(fInteractionPos, fInteractionMom);
        }
        if(fEventNumber%2 ==1){fRunManager -> NextEvent();}
    }
    else{fRunManager -> NextEvent();}
}
 
bool NewTPCEventAction::IsInteractionEvent()
{
    if(fEventNumber%2 == 0){
        fStepNum += 1;
        if(fStepNum == 1){return true;}
        else{return false;}
    }
    else{return false;}
}

void NewTPCEventAction::SetInteractionPoint(TVector3 pos, TVector3 mom)
{
    fInteractionPos = pos;
    fInteractionMom = mom;
}