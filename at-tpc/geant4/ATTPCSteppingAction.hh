#ifndef ATTPCSTEPPINGACTION_HH
#define ATTPCSTEPPINGACTION_HH

#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "ATTPCTrackingAction.hh"
<<<<<<< HEAD
#include "ATTPCRectanglePad.hh"
=======
#include "ATTPCRectnglePad.hh"
>>>>>>> 2079b1e984042cd756ba71efe7768120e39f4343
#include "ATTPCHoneyCombPad.hh"

class ATTPCSteppingAction : public G4UserSteppingAction
{
  public:
    ATTPCSteppingAction();
    ATTPCSteppingAction(KBG4RunManager *man);
    virtual ~ATTPCSteppingAction() {}

    virtual void UserSteppingAction(const G4Step* step);

  private:
    KBG4RunManager *fRunManager = nullptr;
};

#endif