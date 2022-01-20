#include "KBG4RunMessenger.hh"

KBG4RunMessenger::KBG4RunMessenger(KBG4RunManager *runManager)
: fRunManager(runManager)
{
  fBeamOnAll = new G4UIcmdWithoutParameter("/run/beamOnAll",this);
  fSuppressMessage = new G4UIcmdWithABool("/run/suppressPP",this);
}

KBG4RunMessenger::~KBG4RunMessenger()
{
}

void KBG4RunMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
  if (command==fBeamOnAll)
    fRunManager -> BeamOnAll();
  else if (command==fSuppressMessage)
    fRunManager -> SetSuppressInitMessage(fSuppressMessage->GetNewBoolValue(newValue));

}
