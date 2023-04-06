#ifndef ATTPCEVENTACTION_HH
#define ATTPCEVENTACTION_HH

#include "TVector3.h"
#include "KBG4RunManager.hh"
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"

class ATTPCEventAction : public G4UserEventAction
{
  public:
    ATTPCEventAction();
    ATTPCEventAction(KBG4RunManager *man);
    virtual ~ATTPCEventAction() {}

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    Int_t GetEventID(){return fEventNumber;}
    Double_t GetInteractionEnergy(){return fInteractionEnergy;}
    bool IsInteractionEvent();
    void SetInteractionPoint(TVector3 pos, TVector3 mom);

  private:
    KBG4RunManager* fRunManager = nullptr;
    KBParameterContainer* fPar = nullptr;

    Int_t fEventNumber = 0;
    int fStepNum = 0;
    Double_t fInteractionEnergy = 0.;
    TVector3 fInteractionPos;
    TVector3 fInteractionMom;
    
};

#endif