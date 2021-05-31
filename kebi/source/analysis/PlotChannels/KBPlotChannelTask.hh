#ifndef KBPLOTCHANNELTASK_HH
#define KBPLOTCHANNELTASK_HH

#include "KBTask.hh"
#include "KBPadPlane.hh"
#include "KBDetector.hh"
#include "KBDetectorPlane.hh"
#include "KBPadPlane.hh"

#include "TClonesArray.h"

class KBPlotChannelTask : public KBTask
{ 
  public:
    KBPlotChannelTask();
    virtual ~KBPlotChannelTask() {};

    bool Init();
    void Exec(Option_t*);

    void SetOutputDirectory(TString dir);
    void SetPadIDRange(Int_t padIDLow, Int_t padIDHigh);
    void SetNameType(Int_t type);

  private:
    TClonesArray* fPadArray;

    Int_t fNPlanes;
    Int_t fTbStart;
    Int_t fNTbs;

    bool fIsSetPadIDRange = false;
    Int_t fPadIDLow;
    Int_t fPadIDHigh;

    KBDetector *fDetector;
    TString fOutputDir = "";
    TCanvas *fCvs;
    TH1D *fHist;
    TH1D *fHistRaw;

    Int_t fNameType = 0;

  ClassDef(KBPlotChannelTask, 1)
};

#endif
