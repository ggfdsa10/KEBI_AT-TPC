#include "KBSingleHelixTask.hh"
#include "KBRun.hh"
#include "KBTpcHit.hh"
#include <iostream>
using namespace std;

ClassImp(KBSingleHelixTask)

KBSingleHelixTask::KBSingleHelixTask()
:KBTask("KBSingleHelixTask","")
{
}

bool KBSingleHelixTask::Init()
{
  KBRun *run = KBRun::GetRun();

  fHitArray = (TClonesArray *) run -> GetBranch("Hit");
  fReferenceAxis = run -> GetParameterContainer() -> GetParAxis("LHTF_refAxis");

  fTrackArray = new TClonesArray("KBHelixTrack");
  run -> RegisterBranch("Tracklet", fTrackArray, fPersistency);

  {
    run -> GetOutputFile() -> cd();
    fTree = new TTree("track","");
    fTree -> Branch("l",&fLength);
    fTree -> Branch("n",&fNumHits);
    fTree -> Branch("rms",&fRMS);
    fTree -> Branch("rmst",&fRMST);
    fTree -> Branch("rmsr",&fRMSR);


    fTreeByHits = new TTree("trackByHits","");
    fTreeByHits -> Branch("rmsW1", &RMSW1);
    fTreeByHits -> Branch("rmsW2", &RMSW2);
    fTreeByHits -> Branch("rmsH", &RMSH);
  }

  return true;
}

void KBSingleHelixTask::Exec(Option_t*)
{
  fTrackArray -> Clear("C");

  KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> ConstructedAt(0);
  track -> SetTrackID(0);
  track -> SetReferenceAxis(fReferenceAxis);

  Int_t numHits = fHitArray -> GetEntries();
  for (auto iHit=0; iHit<numHits; ++iHit) {
    auto hit = (KBTpcHit *) fHitArray -> At(iHit);
    track -> AddHit(hit);
  }
  track -> Fit();
  track -> FinalizeHits();

  // sort by tpc beam point.(track head is nearest beam point than track tail)
  if(track->PositionAtHead().y() > track->PositionAtTail().y()){
    kb_info << "Head and Tail of the track are reversed!! " << endl;
    kb_info << " Transformation head to tail. " << endl;
    Double_t fHeadA = track -> GetH();
    Double_t fTailA = track -> GetT();

    track -> SetH(fTailA);
    track -> SetT(fHeadA);
  }

  if(track->PositionAtHead().y() > track->PositionAtTail().y()){
    kb_error << " not transfer head and tail !!! " << endl;
  }

  for(auto iHit=0; iHit<numHits; ++iHit) {
    auto hit = (KBTpcHit *) fHitArray -> At(iHit);
    auto hitPos = hit->GetPosition();
    TVector3 hitm = track -> Map(hitPos);
    RMSW1 = hitm.x();
    RMSW2 = hitm.y();
    RMSH = hitm.z();
    fTreeByHits -> Fill();
  }


  fNumHits = track -> GetNumHits();
  fLength = track -> TrackLength();
  fRMS = track -> GetRMS();
  fRMSR = track -> GetRMSR();
  fRMST = track -> GetRMST();
  fTree -> Fill();

  track -> Print("s");

  return;
}

bool KBSingleHelixTask::EndOfRun()
{
  KBRun::GetRun() -> GetOutputFile() -> cd();
  fTree -> Write();
  fTreeByHits -> Write();

  return true;
}

void KBSingleHelixTask::SetTrackPersistency(bool val) { fPersistency = val; }
