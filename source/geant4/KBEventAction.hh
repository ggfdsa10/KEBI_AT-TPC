#ifndef KBEVENTACTION_HH
#define KBEVENTACTION_HH

#include "KBG4RunManager.hh"
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"

class KBEventAction : public G4UserEventAction
{
  public:
    KBEventAction();
    KBEventAction(KBG4RunManager *man);
    virtual ~KBEventAction() {}

    virtual void EndOfEventAction(const G4Event* event);

  private:
    KBG4RunManager *fRunManager = nullptr;
};

#endif
