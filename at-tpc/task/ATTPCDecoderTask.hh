#ifndef ATTPCDECODERTASK_HH
#define ATTPCDECODERTASK_HH

#include "KBTask.hh"
#include "ATTPC.hh"
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

        void LoadData(TString pathToRawData, TString pathToMetaData="");

        void AddData(TString name);
        void LoadMetaData(TString name);
        void SetNumEvents(Long64_t numEvents);

        bool IsFPNChannel(Int_t chanIdx);
        void PadMapping(Int_t &agetIdx, Int_t &chanIdx, Int_t &colIdx, Int_t &rowIdx);
        Int_t GetPadID(Int_t colIdx, Int_t rowIdx);

        double GetEventTime();
        void GetDate();
        Int_t GetYear(){return fYear;}
        Int_t GEtMonth(){return fMonth;}
        Int_t GetDay(){return fDay;}
        Int_t GetHour(){return fHour;}
        Int_t GetMinute(){return fMinute;}
        Float_t GetSecond(){return fSecond;}

    private:
        TClonesArray* fPadArray;
        TTree* fPadData;
        
        bool fPersistency = false;
        bool fFileToKEBIForm = true;

        KBPadPlane *fPadPlane;

        Long64_t fNumEvents = -1;

        GETDecoder *fDecoder;

        Int_t fNumAsAds = 1;
        Int_t fNumAGETs = 4;
        Int_t fNumChannelsMax = 68;

        Double_t fEventTime;
        Int_t fYear;
        Int_t fMonth;
        Int_t fDay;
        Int_t fHour;
        Int_t fMinute;
        Float_t fSecond;

        vector<Int_t> fSkipAAID;

ClassDef(ATTPCDecoderTask, 1)
};

#endif
