#ifndef LAMPSPROTOTYPEDECODERTASK_HH
#define LAMPSPROTOTYPEDECODERTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "TTree.h"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETCoboFrame.hh"
#include "GETBasicFrame.hh"

class LAPDecoderTask : public KBTask
{ 
  public:
    LAPDecoderTask();
    virtual ~LAPDecoderTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

    void LoadData(TString pathToRawData, TString pathToMetaData="");

    void AddData(TString name);
    void LoadMetaData(TString name);
    void SetNumEvents(Long64_t numEvents);

  private:
    TClonesArray* fPadArray;
    bool fPersistency = false;

    KBPadPlane *fPadPlane;

    Long64_t fNumEvents = -1;

    GETDecoder *fDecoder;
    ifstream fPadMap;

    Int_t fNumAsAds = 4;
    Int_t fNumAGETs = 4;
    Int_t fNumChannelsMax = 68;

    vector<Int_t> fSkipAAID;

  ClassDef(LAPDecoderTask, 1)
};

#endif
