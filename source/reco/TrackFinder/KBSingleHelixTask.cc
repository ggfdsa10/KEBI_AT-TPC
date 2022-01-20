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
  fReferenceAxis = run -> GetParameterContainer() -> GetParAxis("tpcBFieldAxis");

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

  return true;
}

void KBSingleHelixTask::SetTrackPersistency(bool val) { fPersistency = val; }
