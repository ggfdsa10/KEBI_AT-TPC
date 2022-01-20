#ifndef KBSTEPPINGACTION_HH
#define KBSTEPPINGACTION_HH

#include "KBG4RunManager.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "globals.hh"

class KBSteppingAction : public G4UserSteppingAction
{
  public:
    KBSteppingAction();
    KBSteppingAction(KBG4RunManager *man);
    virtual ~KBSteppingAction() {}

    virtual void UserSteppingAction(const G4Step*);

  private:
    KBG4RunManager *fRunManager = nullptr;
};

#endif
