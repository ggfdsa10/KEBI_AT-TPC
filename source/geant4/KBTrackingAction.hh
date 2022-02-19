#ifndef KBTRACKINGACTION_HH
#define KBTRACKINGACTION_HH

#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class KBTrackingAction : public G4UserTrackingAction
{
  public:
    KBTrackingAction();
    KBTrackingAction(KBG4RunManager *man);
    virtual ~KBTrackingAction() {}

    virtual void PreUserTrackingAction(const G4Track* track);
    virtual void PostUserTrackingAction(const G4Track* track);

  private:
    KBParameterContainer *fProcessTable;

    KBG4RunManager *fRunManager = nullptr;
};

#endif
