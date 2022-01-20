#ifndef ATTPCTRACKINGACTION_HH
#define ATTPCTRACKINGACTION_HH

#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "ATTPCRandomPrimaryGenerate.hh"
<<<<<<< HEAD
#include "ATTPCRectanglePad.hh"
=======
#include "ATTPCRectnglePad.hh"
>>>>>>> 2079b1e984042cd756ba71efe7768120e39f4343
#include "ATTPCHoneyCombPad.hh"

class ATTPCTrackingAction : public G4UserTrackingAction
{
  public:
    ATTPCTrackingAction();
    ATTPCTrackingAction(KBG4RunManager *man);
    virtual ~ATTPCTrackingAction() {}

    virtual void PreUserTrackingAction(const G4Track* track);
    virtual void PostUserTrackingAction(const G4Track* track);
    void SetPrimaryEdep(Double_t length, Double_t edep);

  private:
    KBParameterContainer *fProcessTable;
    KBG4RunManager *fRunManager = nullptr;
    ATTPCRandomPrimaryGenerate *fPrimaryGerateAction = nullptr;

    Double_t fTrackStartPoint = 10.; // [mm]
    Double_t fTrackEndEPoint = 0.;
    Double_t fEdep = 0.;

    vector<double> fTrackLength;
    vector<double> fTrackEdep;
    vector<double> fDiffLengthBuffer1;
    vector<double> fDiffLengthBuffer2;

    G4ThreeVector fMomentum;
    G4ThreeVector fPosition;
    G4double fKEnergy;
    G4int fVolumeID;
    G4int fProcessID;
    G4int fNumberOfPrimary;
};

#endif
