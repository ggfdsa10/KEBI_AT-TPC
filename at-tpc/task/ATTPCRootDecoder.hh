#ifndef ATTPCRootDecoder_HH
#define ATTPCRootDecoder_HH

#include "KBRun.hh"
#include "KBTask.hh"
#include "ATTPC.hh"

#include "TFile.h"
#include "TTree.h"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETBasicFrame.hh"

class ATTPCRootDecoder : public KBTask
{ 
    public:
        ATTPCRootDecoder();
        virtual ~ATTPCRootDecoder() {}

        bool Init();
        void Exec(Option_t*);

        void SetPadPersistency(bool persistence);
        void SaveKEBIData(bool fileToKEBIForm);

        void LoadRootFile(TString pathToRawData);

        void SetNumEvents(Int_t numEvents);

        void PadIDMapping();
        void NewPadIDMapping();
        void BoardIDMapping();

        bool IsFPNChannel(Int_t chanIdx);
        bool IsEvenChannel(Int_t agetIdx, Int_t chanIdx);
        bool IsDeadChannel(Int_t agetId, Int_t chanId);

        pair<Int_t, Int_t> GetPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanIdx); //[PadID, FPN Id by Pad]
        Int_t GetFPNPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanFPNIdx);
        tuple<Int_t, Int_t, Int_t> GetBoardID(Int_t PadID);
        tuple<Int_t, Int_t, Int_t> GetBoardFPNID(Int_t fpnPadID);

        Int_t GetNumRun(){return fNumRun;}
        Int_t GetNumEvent(){return fNumEvents;}
        Double_t GetEventTime(){return fEventTime;}
        Double_t GetEventDiffTime(){return fEventDiffTime;}


    private:
        TClonesArray* fPadArray;
        TClonesArray* fPadFPNArray;
        TTree* fPadData;

        KBPadPlane *fPadPlane;

        TFile* rootFile;
        TTree* rootTree;

        Int_t fNumRun;
        Int_t fNumEvents = -1;
        Int_t fEventIdx = 0;
        Double_t fEventTime;
        Double_t fEventDiffTime;
        Int_t fADC[4][4][68][512];
        Bool_t fIsBroken;

        Int_t fNumAsAds = 4;
        Int_t fNumAGETs = 4;
        Int_t fNumChannels = 68;
        Int_t fFPNChanArray[4] = {11, 22, 45, 56};

        Int_t fRowNum = 32;
        Int_t fColumnNum = 8;

        bool fPersistency = false;
        bool fFileToKEBIForm = true;

        map<vector<Int_t>, tuple<Int_t, Int_t, Int_t>> fPadIdxArray;
        map<vector<Int_t>, Int_t> fPadFPNIdxArray;

        map<Int_t, tuple<Int_t, Int_t, Int_t>> fBoardIdxArray;
        map<Int_t, tuple<Int_t, Int_t, Int_t>> fBoardFPNIdxArray;

        vector<Int_t> fSkipAAID;

ClassDef(ATTPCRootDecoder, 1)
};

#endif
