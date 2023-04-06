#ifndef ATTPCDECODERTASK_HH
#define ATTPCDECODERTASK_HH

#include "KBRun.hh"
#include "KBTask.hh"
#include "ATTPC.hh"

#include "TFile.h"
#include "TTree.h"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETBasicFrame.hh"

class ATTPCDecoderTask : public KBTask
{ 
    public:
        ATTPCDecoderTask();
        virtual ~ATTPCDecoderTask() {}

        bool Init();
        void Exec(Option_t*);

        void SetPadPersistency(bool persistence);
        void SaveKEBIData(bool fileToKEBIForm);

        void RunEventChecker(Long64_t currentEvent, GETBasicFrame *frame);

        void LoadData(TString pathToRawData, TString pathToMetaData="");

        void AddData(TString name);
        void LoadMetaData(TString name);
        void SetNumEvents(Long64_t numEvents);
        void ExcuteOnline(Int_t eventIdx, bool skim);
        void PadIDMapping();
        void NewPadIDMapping();
        void BoardIDMapping();

        bool IsFakeEvent(){return fIsFakeEvent;}
        bool IsSparkEvent(){return fIsSparkEvent;}
        bool IsFPNChannel(Int_t chanIdx);
        bool IsEvenChannel(Int_t agetIdx, Int_t chanIdx);
        bool IsDeadChannel(Int_t agetId, Int_t chanId);

        pair<Int_t, Int_t> GetPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanIdx); //[PadID, FPN Id by Pad]
        Int_t GetFPNPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanFPNIdx);
        tuple<Int_t, Int_t, Int_t> GetBoardID(Int_t PadID);
        tuple<Int_t, Int_t, Int_t> GetBoardFPNID(Int_t fpnPadID);

        uint64_t SetPreviousEventTime(uint64_t time){fEventTime = time;}
        uint64_t GetEventTime(){return fEventTime;}
        uint64_t GetEventDiffTime(){return fEventDiffTime;}

        Int_t GetNumEvent(){return fNumEvents;}
        Int_t GetNumberOfFakeEventBefore(){return fNumFakeEvent;}
        Int_t GetNumberOfSparkEventBefore(){return fNumFakeEvent;}

        void GetDate();
        Int_t GetYear(){return fYear;}
        Int_t GEtMonth(){return fMonth;}
        Int_t GetDay(){return fDay;}
        Int_t GetHour(){return fHour;}
        Int_t GetMinute(){return fMinute;}
        Float_t GetSecond(){return fSecond;}

    private:
        TClonesArray* fPadArray;
        TClonesArray* fPadFPNArray;
        TTree* fPadData;

        KBPadPlane *fPadPlane;

        Long64_t fNumEvents = -1;

        GETDecoder *fDecoder;
        GETDecoder *fPreDecoder;

        Long64_t fEventIdx = 0;

        Int_t fNumAsAds = 1;
        Int_t fNumAGETs = 4;
        Int_t fNumChannels = 68;
        Int_t fFPNChanArray[4] = {11, 22, 45, 56};

        Int_t fRowNum = 32;
        Int_t fColumnNum = 8;

        uint64_t fEventTime;
        uint64_t fEventDiffTime;

        Int_t fYear;
        Int_t fMonth;
        Int_t fDay;
        Int_t fHour;
        Int_t fMinute;
        Float_t fSecond;

        Int_t fNumFakeEvent = 0;
        Int_t fNumSparkEvent = 0;

        bool fPersistency = false;
        bool fFileToKEBIForm = true;
        bool fIsFakeEvent = false;
        bool fIsSparkEvent = false;
        bool fIsOnline = false;
        bool fSkimToData = false;

        map<vector<Int_t>, tuple<Int_t, Int_t, Int_t>> fPadIdxArray;
        map<vector<Int_t>, Int_t> fPadFPNIdxArray;

        map<Int_t, tuple<Int_t, Int_t, Int_t>> fBoardIdxArray;
        map<Int_t, tuple<Int_t, Int_t, Int_t>> fBoardFPNIdxArray;

        vector<Int_t> fSkipAAID;

ClassDef(ATTPCDecoderTask, 1)
};

#endif
