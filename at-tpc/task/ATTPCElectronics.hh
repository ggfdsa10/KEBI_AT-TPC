#ifndef ATTPCELECTRONICS_HH
#define ATTPCELECTRONICS_HH

#include <time.h>
#include <tuple>
#include "KBTask.hh"
#include "ATTPC.hh"
#include "KBPulseGenerator.hh"
#include "TF1.h"
#include "TClonesArray.h"
#include "TRandom3.h"
#include "ATTPCStripPad.hh"

class ATTPCElectronics : public KBTask
{ 
  public:
    ATTPCElectronics(Bool_t usePointChargeMC = true);
    virtual ~ATTPCElectronics() {}

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fPadArray;
    ATTPCStripPad* fStripPad;
    Int_t fNPlanes;
    Int_t fNTbs;
    Double_t fEChargeToADC;
    Double_t fDynamicRange;
    Double_t fElectronCharge = 1.6021773349e-19; // [C]
    Double_t fADCMaxAmp = 4095; // Dynamic range maximum ratio

    TF1 *fPulseFunction;

    Bool_t fUsePointChargeMC = true;
    Bool_t fNoiseOn;
    Bool_t fStripView = false;

  ClassDef(ATTPCElectronics, 1)
};

#endif
