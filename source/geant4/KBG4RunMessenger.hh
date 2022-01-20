#ifndef KBG4RUNMESSENGER_HH
#define KBG4RUNMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "KBG4RunManager.hh"

class KBG4RunMessenger : public G4UImessenger
{
  public:
    KBG4RunMessenger(KBG4RunManager *);
    virtual ~KBG4RunMessenger();

   void SetNewValue(G4UIcommand * command,G4String newValues);

  private:
    KBG4RunManager *fRunManager;

    G4UIcmdWithoutParameter *fBeamOnAll;
    G4UIcmdWithABool *fSuppressMessage;
};

#endif
