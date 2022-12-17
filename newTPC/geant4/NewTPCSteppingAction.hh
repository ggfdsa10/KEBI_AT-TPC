#ifndef NewTPCSTEPPINGACTION_HH
#define NewTPCSTEPPINGACTION_HH

#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "NewTPCEventAction.hh"

class NewTPCSteppingAction : public G4UserSteppingAction
{
  public:
    NewTPCSteppingAction();
    NewTPCSteppingAction(KBG4RunManager *man);
    virtual ~NewTPCSteppingAction() {}

    virtual void UserSteppingAction(const G4Step* step);

  private:
    KBG4RunManager* fRunManager = nullptr;
    KBParameterContainer* fPar = nullptr;
};

#endif