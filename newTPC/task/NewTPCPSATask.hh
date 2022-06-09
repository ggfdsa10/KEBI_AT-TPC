#ifndef NEWTPCPSATASK_HH
#define NEWTPCPSATASK_HH

#include "KBRun.hh"
#include "KBTask.hh"
#include "NewTPCPSA.hh"
#include "KBPadPlane.hh"
#include "KBTpc.hh"

#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"


class NewTPCPSATask : public KBTask
{ 
  public:
    NewTPCPSATask();
    virtual ~NewTPCPSATask();

    bool Init();
    void Exec(Option_t*);

    void SetPSA(NewTPCPSA *psa);

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

    NewTPCPSA *fPSA;

    //root file define
    TFile *rootfile;
    TTree *ArrayTree;

    Int_t eventIndex =0;
    Int_t padNum = 0;
    
    vector<vector<vector<Double_t>>> mcArray;
    vector<vector<vector<Double_t>>> reconArray;

  ClassDef(NewTPCPSATask, 1)
};

#endif