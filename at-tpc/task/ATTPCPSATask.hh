#ifndef ATTPCPSATASK_HH
#define ATTPCPSATASK_HH

#include "KBRun.hh"
#include "KBTask.hh"
#include "ATTPCPSA.hh"
#include "KBPadPlane.hh"
#include "KBTpc.hh"

#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"


class ATTPCPSATask : public KBTask
{ 
  public:
    ATTPCPSATask();
    virtual ~ATTPCPSATask();

    bool Init();
    void Exec(Option_t*);

    void SetPSA(ATTPCPSA *psa);

		void SetInputBranchName(TString name) { fInputBranchName = name; }
		void SetOutputBranchName(TString name) { fOutputBranchName = name; }

    void SetHitPersistency(bool persistence);

  private:
    KBRun *run;

    TClonesArray* fPadArray;
    TClonesArray* fHitArray;

		TString fInputBranchName = "Pad";
		TString fOutputBranchName = "Hit";

    bool fPersistency = true;

    KBTpc *fTpc;

    Int_t fNPlanes;
    Int_t fTbStart;
    Int_t fNTbs;
    Double_t fDriftVelocity;
    Double_t fTbTime;
    Double_t fADCThreshold;

    KBPadPlane *fPadPlane[2];

    ATTPCPSA *fPSA;

    Int_t eventIndex =0;
    Int_t padNum = 0;
    
    vector<vector<vector<Double_t>>> mcArray;
    vector<vector<vector<Double_t>>> reconArray;

  ClassDef(ATTPCPSATask, 1)
};

#endif