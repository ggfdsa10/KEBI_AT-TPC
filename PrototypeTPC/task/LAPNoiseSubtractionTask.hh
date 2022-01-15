#ifndef LAMPSPROTOTYPENOISESUBTRACTIONTASK_HH
#define LAMPSPROTOTYPENOISESUBTRACTIONTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "TTree.h"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETCoboFrame.hh"
#include "GETBasicFrame.hh"

class LAPNoiseSubtractionTask : public KBTask
{ 
  public:
    LAPNoiseSubtractionTask();
    virtual ~LAPNoiseSubtractionTask() {}

    bool Init();
    void Exec(Option_t*);

    void SkipNoiseSubtraction(bool val) { fSkipNoiseSubtraction = val; }

  private:
    void FindReferencePad();
    void CopyRaw(Short_t *in, Double_t *out);
    void Set0(Double_t *out);
    Double_t BaseLineCorrection(Double_t *out, Int_t tbi, Int_t tbf);
    Double_t NoiseAmplitudeCorrection(Double_t *out, Double_t *ref, Int_t tbi, Int_t tbf);
    void SaturationCorrection(Double_t *out, Short_t *raw, Double_t baseLine);

  private:
    TClonesArray* fPadArray;

    bool fSkipNoiseSubtraction = false;

    int fMethodFindRef = 0;

    vector<int> fSkipAsadIDs;
    vector<int> fSkipAGETIDs;

    Int_t fIdxPadRef = -1;
    Int_t fTbSamplingNoiseStart = 0;
    Int_t fTbSamplingNoiseEnd = 180;

  ClassDef(LAPNoiseSubtractionTask, 1)
};

#endif
