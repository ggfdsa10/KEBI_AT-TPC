#ifndef LAMPSPROTOTYPEDECODERFROMROOTTASK_HH
#define LAMPSPROTOTYPEDECODERFROMROOTTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "TTree.h"

#include "TClonesArray.h"

class LAPDecoderFromRootTask : public KBTask
{ 
  public:
    LAPDecoderFromRootTask();
    virtual ~LAPDecoderFromRootTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);
    void SetDataPath(TString pathToData);

  private:
    TClonesArray* fPadArray;
    bool fPersistency = false;

    TString fInputFileName;
    TTree *fInputTree = nullptr;
    TClonesArray *fChannelArray[6];

    KBPadPlane *fPadPlane;

    Long64_t fNumEvents = -1;

    ifstream fPadMap;

    Int_t fNumAsAds = 6;
    Int_t fNumAGETs = 4;
    Int_t fNumChannelsMax = 68;

    vector<Int_t> fSkipAAID;

  ClassDef(LAPDecoderFromRootTask, 1)
};

class TPCData : public TObject
{
  public:
    TPCData() { ResetAll(); }
    ~TPCData() {};
    void ResetAll() {
      AGETIdx = -1;
      CHANIdx = -1;
      Channel = -1;
      posX    = 0;
      posY    = 0;
      memset( ADC, 0, sizeof( ADC ));
    }
    Int_t COBOIdx;
    Int_t ASADIdx;
    Int_t AGETIdx;
    Int_t CHANIdx;
    Double_t posX;
    Double_t posY;
    Int_t Channel;
    UShort_t ADC[512];

    //ClassDef( TPCData, 1 )
}; 

#endif
