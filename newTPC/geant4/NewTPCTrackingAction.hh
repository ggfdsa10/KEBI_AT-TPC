#ifndef NewTPCTRACKINGACTION_HH
#define NewTPCTRACKINGACTION_HH

#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "NewTPCRectnglePad.hh"
#include "NewTPCHoneyCombPad.hh"

class NewTPCTrackingAction : public G4UserTrackingAction
{
  public:
    NewTPCTrackingAction();
    NewTPCTrackingAction(KBG4RunManager *man);
    virtual ~NewTPCTrackingAction() {}

    virtual void PreUserTrackingAction(const G4Track* track);

  private:
    KBParameterContainer *fProcessTable;
    KBG4RunManager *fRunManager = nullptr;
};

#endif
