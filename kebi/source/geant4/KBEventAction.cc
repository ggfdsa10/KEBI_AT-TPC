#include "KBEventAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

KBEventAction::KBEventAction()
: G4UserEventAction()
{
  fRunManager = (KBG4RunManager *) KBG4RunManager::GetRunManager();
}

KBEventAction::KBEventAction(KBG4RunManager *man)
: G4UserEventAction(), fRunManager(man)
{
}
void KBEventAction::EndOfEventAction(const G4Event*)
{
  fRunManager -> NextEvent();
}
