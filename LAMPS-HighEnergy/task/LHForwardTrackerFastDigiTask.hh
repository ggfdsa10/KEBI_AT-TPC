#ifndef LHFORWARDTRACKERFASTDIGITIZER_HH
#define LHFORWARDTRACKERFASTDIGITIZER_HH

#include "KBTask.hh"

#include "TH2D.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHForwardTrackerFastDigiTask : public KBTask
{ 
  public:
    LHForwardTrackerFastDigiTask();
    virtual ~LHForwardTrackerFastDigiTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetChannelPersistency(bool persistence);

		void SetDetID(Int_t id){ fDetID = id; }
		void SetEnergyCut(Float_t cut){ eCut = cut; }
		void SetOutputBranchName(TString name) { fOutputBranchName = name; }

  private:
    TClonesArray* fMCStepArray;
		TClonesArray* fHitArray;

    bool fPersistency = true;

		Int_t fDetID = 40;
		TString fOutputBranchName = "ForwardTrackerHit";

		Int_t ftLayerN;
		Float_t ftLayerD;
		Float_t ftLayerZ;
		Float_t ftLayerT;

		Float_t eCut = 1e-5; //MeV

		TH2D *h2d_pos[10];

  ClassDef(LHForwardTrackerFastDigiTask, 1)
};

#endif
