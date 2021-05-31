#ifndef ATTPCELECTRONICS_HH
#define ATTPCELECTRONICS_HH

#include "KBTask.hh"
#include "LHTpc.hh"
#include "KBPulseGenerator.hh"
#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"
#include <time.h>

class ATTPCElectronics : public KBTask
{ 
  public:
    ATTPCElectronics(Bool_t usePointChargeMC = false);
    virtual ~ATTPCElectronics() {}

    bool Init();
    void Exec(Option_t*);

  private:
    void Noise();
  
    TClonesArray* fPadArray;

    Int_t fNPlanes;
    Int_t fNTbs;
    Double_t fEChargeToADC;
    Double_t fDynamicRange;
    Double_t fElectronCharge = 1.6021773349e-19; // [C]
    Double_t fADCMaxAmp = 4096; // Dynamic range maximum ratio

    TF1 *fPulseFunction;

    Bool_t fUsePointChargeMC = false;
    Bool_t fNoiseOn;

  ClassDef(ATTPCElectronics, 1)
};

#endif
