#ifndef LHNDFASTDIGITIZER_HH
#define LHNDFASTDIGITIZER_HH

#include "KBTask.hh"

#include "TH2D.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHNDFastDigiTask : public KBTask
{ 
  public:
    LHNDFastDigiTask();
    virtual ~LHNDFastDigiTask() {}

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

		Int_t fDetID = 50;
		Int_t ndLayerN = 5;
		Int_t ndSubLayerN = 2;
		Int_t ndSlatN = 20;
		TString fOutputBranchName = "NDHit";

		Float_t ndOfsX;
		Float_t ndOfsZ;
		Float_t ndTheta;

		Float_t eCut = 1e-5; //MeV

		TH2D *h2d_time_pos[200];
		TProfile *hprof_fixed_pos[2];

  ClassDef(LHNDFastDigiTask, 1)
};

#endif
