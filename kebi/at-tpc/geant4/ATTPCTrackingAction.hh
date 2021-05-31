#ifndef ATTPCTRACKINGACTION_HH
#define ATTPCTRACKINGACTION_HH

#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class ATTPCTrackingAction : public G4UserTrackingAction
{
  public:
    ATTPCTrackingAction();
    ATTPCTrackingAction(KBG4RunManager *man);
    virtual ~ATTPCTrackingAction() {}

    virtual void PreUserTrackingAction(const G4Track* track);

  private:
    KBParameterContainer *fProcessTable;

    KBG4RunManager *fRunManager = nullptr;
};

#endif
