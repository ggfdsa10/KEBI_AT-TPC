#ifndef ATTPCHELIXTRACKFINDINGTASK_HH
#define ATTPCHELIXTRACKFINDINGTASK_HH

#include "TClonesArray.h"
#include "TGraphErrors.h"

#include "KBTask.hh"
#include "KBHelixTrack.hh"
#include "KBTpcHit.hh"
#include "KBHitArray.hh"

#include "ATTPC.hh"
#include "KBPadPlane.hh"

#include <vector>
using namespace std;

class ATTPCHelixTrackFindingTask : public KBTask
{
  public:
    ATTPCHelixTrackFindingTask() : KBTask("ATTPCHelixTrackFindingTask","ATTPCHelixTrackFindingTask") {}
    virtual ~ATTPCHelixTrackFindingTask() {}

    virtual bool Init();
    virtual void Exec(Option_t*);

    void SetTrackPersistency(bool val) { fPersistency = val; }

    enum StepNo : int {
      kStepInitArray,
      kStepNewTrack,
      kStepRemoveTrack,
      kStepInitTrack,
      kStepInitTrackAddHit,
      kStepContinuum,
      kStepContinuumAddHit,
      kStepExtrapolation,
      kStepExtrapolationAddHit,
      kStepConfirmation,
      kStepFinalizeTrack,
      kStepNextPhase,
      kStepEndEvent,
      kStepEndOfEvent,
    };

    bool ExecStep();

    bool ExecStepUptoTrackNum(Int_t numTracks);

    KBHelixTrack *GetCurrentTrack() const { return fCurrentTrack; }

    void SetHitBranchName(TString name) { fBranchNameHit = name; }
    void SetTrackletBranchName(TString name) { fBranchNameTracklet = name; }

  private:
    int StepInitArray();
    int StepNewTrack();
    int StepRemoveTrack();
    int StepInitTrack();
    int StepInitTrackAddHit();
    int StepContinuum();
    int StepContinuumAddHit();
    int StepExtrapolation();
    int StepExtrapolationAddHit();
    int StepConfirmation();
    int StepFinalizeTrack();
    int StepNextPhase();
    int StepEndEvent();

    void ReturnBadHitsToPadPlane();

    double CorrelateHitWithTrackCandidate(KBHelixTrack *track, KBTpcHit *hit);
    double CorrelateHitWithTrack(KBHelixTrack *track, KBTpcHit *hit, Double_t scale=1);

    int CheckParentTrackID(KBTpcHit *hit);
    bool CheckTrackQuality(KBHelixTrack *track);
    double CheckTrackContinuity(KBHelixTrack *track);
    bool CheckHitDistInAlphaIsLargerThanQuarterPi(KBHelixTrack *track, Double_t dLength);

    bool BuildAndConfirmTrack(KBHelixTrack *track, bool &tailToHead);
    bool AutoBuildByExtrapolation(KBHelixTrack *track, bool &buildHead, Double_t &extrapolationLength);
    bool AutoBuildAtPosition(KBHelixTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t scale=1);

  private:
    ATTPC *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    TClonesArray *fHitArray = nullptr;
    TClonesArray *fTrackArray = nullptr;

    TString fBranchNameHit = "Hit";
    TString fBranchNameTracklet = "Tracklet";

    bool fPersistency = true;

    KBHitArray *fTrackHits = nullptr;
    KBHitArray *fCandHits = nullptr;
    KBHitArray *fGoodHits = nullptr;
    KBHitArray *fBadHits  = nullptr;

    Double_t fDefaultScale;
    Double_t fTrackWCutLL;  ///< Track width cut low limit
    Double_t fTrackWCutHL;  ///< Track width cut high limit
    Double_t fTrackHCutLL;  ///< Track height cut low limit
    Double_t fTrackHCutHL;  ///< Track height cut high limit

    KBVector3::Axis fReferenceAxis;

    Int_t fPhaseIndex = 0;

    Int_t fMinHitsToFitInitTrack = 6;        ///< try track fit if track has more than this number of hits in track  // === finding from the distribution of hitted pad number. ===
    Int_t fCutMinNumHitsInitTrack = 10;      ///  // === finding from the distribution of hitted pad number. more minimum hitted pad number then the fMinHitsToFitInitTrack parameter. ===
    Int_t fCutMaxNumHitsInitTrack = 45;      ///< return hits if track has more than this number of hits within initialization stage // === if select the gas type and the geometry, finding the mean hitted pad number +sigma with 1 track by event. 
    Int_t fCutMinNumHitsFinalTrack = 10;     ///< remove track if track has smaller than this number of hits within initialization stage
    Double_t fCutMinHelixRadius = 350.;      ///< helix radius cut for initialization stage       // === the minimum radius of the incident beam in TPC find to caculation at mixmum magnet. [mm]===
    Double_t fTrackLengthCutScale = 2.5;     ///< track length cut for initialization stage is [this_var] * track -> GetRMSR() // === fix value that reference in SPIRIT TPC. ===
    Double_t fCutdkInExpectedTrackPath = 1.; // the correlation distance cut through helix axis during the track path between two hits [mm] // === use the CorrelateHitWithTrackCandidate() ===

    KBHelixTrack *fCurrentTrack = nullptr;

    Int_t fNextStep = StepNo::kStepInitArray;
    Int_t fNumCandHits;
    Int_t fNumGoodHits;
    Int_t fNumBadHits;

    TCanvas *fCvsCurrentTrack = nullptr;
    TGraphErrors *fGraphCurrentTrackPoint = nullptr;

  ClassDef(ATTPCHelixTrackFindingTask, 1)
};

#endif