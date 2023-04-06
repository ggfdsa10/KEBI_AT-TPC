#ifndef ATTPCNOISESUBTRACTTASK_HH
#define ATTPCNOISESUBTRACTTASK_HH

#include <algorithm>
#include <tuple>

#include "KBRun.hh"
#include "KBTask.hh"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETBasicFrame.hh"

#include "ATTPC.hh"
#include "ATTPCDecoderTask.hh"

#include "TFile.h"
#include "TTree.h"

class ATTPCNoiseSubtractTask : public KBTask
{ 
    public:
        ATTPCNoiseSubtractTask();
        virtual ~ATTPCNoiseSubtractTask() {}

        bool Init();
        void Exec(Option_t*);

        void SetPadPersistency(bool persistence);

    private:
        KBPadPlane *fPadPlane;
        TClonesArray *fPadArray;
        TClonesArray *fPadFPNArray;

        ATTPCDecoderTask *fDecoder;

        Int_t fNumTB = 0;
        Int_t fNumAsAds = 1;
        Int_t fNumAGETs = 4;
        Int_t fNumChannels = 68;

        Double_t fTBStart = 1.;
        Double_t fTBEnd = 500.;
        Int_t fNoiseFindParNum = 4;

        bool fPersistency = false;


ClassDef(ATTPCNoiseSubtractTask, 1)
};

static bool compareThirdByDescending(const tuple<int, int, double> &a, const tuple<int, int, double> &b) {
    return (get<2>(a) > get<2>(b));
}

#endif
