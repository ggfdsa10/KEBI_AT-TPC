#include "LHMCRecoMatchingTask.hh"
#include "KBHelixTrack.hh"
#include "KBMCRecoMatching.hh"

#include "KBRun.hh"

#include <vector>
#include <iostream>
using namespace std;

ClassImp(LHMCRecoMatchingTask)

LHMCRecoMatchingTask::LHMCRecoMatchingTask()
:KBTask("LHMCRecoMatchingTask","")
{
}

bool LHMCRecoMatchingTask::Init()
{
  KBRun *run = KBRun::GetRun();

  fMCTrackArray = (TClonesArray *) run -> GetBranch("MCTrack");
  fRecoTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fMCArray = new TClonesArray("KBMCRecoMatching", 100);
  fRecoArray = new TClonesArray("KBMCRecoMatching", 100);
  fMatchingArray = new TClonesArray("KBMCRecoMatching",100);
  run -> RegisterBranch("MCReco", fMatchingArray, fPersistency);

  return true;
}

void LHMCRecoMatchingTask::Exec(Option_t*)
{
  fMCArray -> Clear();
  fRecoArray -> Clear();
  fMatchingArray -> Clear();

  auto numMCTracks = fMCTrackArray -> GetEntries();
  for (auto iTrack = 0; iTrack < numMCTracks; iTrack++)
  {
    auto mcTrack = (KBMCTrack *) fMCTrackArray -> At(iTrack);
    if (mcTrack -> GetPDG() == 2112)
      continue;

    auto momMC = mcTrack -> GetMomentum();

    auto mc = (KBMCRecoMatching *) fMCArray -> ConstructedAt(fMCArray -> GetEntries());
    mc -> SetMCID(iTrack);
    mc -> SetMCMomentum(momMC);
    mc -> SetIsNotFound();
  }

  auto numRecoTracks = fRecoTrackArray -> GetEntries();
  for (auto iTrack = 0; iTrack < numRecoTracks; iTrack++)
  {
    auto recoTrack = (KBHelixTrack *) fRecoTrackArray -> At(iTrack);
    if (recoTrack -> GetParentID() != 0)
      continue;

    auto momReco = recoTrack -> Momentum();

    auto reco = (KBMCRecoMatching *) fRecoArray -> ConstructedAt(fRecoArray -> GetEntries());
    reco -> SetRecoID(iTrack);
    reco -> SetRecoMomentum(momReco);
    reco -> SetIsFake();
  }

  auto numMC = fMCArray -> GetEntries();

  Int_t countMatch = 0;
  Double_t cutArray[] = {.001, .002, .003, .004, .006, .008, .01, .015, .02, .03, .046, .08, .1};
  for (auto cut : cutArray)
  {
    for (auto iMC = 0; iMC < numMC; iMC++)
    {
      auto mc = (KBMCRecoMatching *) fMCArray -> At(iMC);
      if (mc -> IsMatched())
        continue;

      auto momMC = mc -> GetMCMomentum();

      KBMCRecoMatching *bestMatching = nullptr;
      Int_t bestMatchingIdx = -1;
      Double_t bestMatchingV = 0;
      TVector3 bestMatchingMom;
      auto numReco = fRecoArray -> GetEntries();
      for (auto iReco = 0; iReco < numReco; iReco++)
      {
        auto reco = (KBMCRecoMatching *) fRecoArray -> At(iReco);
        if (reco -> IsMatched())
          continue;

        auto momReco = reco -> GetRecoMomentum();

        auto momReco2 = momReco;
        momReco2 = momReco2.Unit();

        auto momMC2 = momMC;
        momMC2 = momMC2.Unit();
        auto v = (momMC2.Dot(momReco2));

        if (v > 1 - cut && bestMatchingV < v) {
          bestMatchingV = v;
          bestMatching = reco;
          bestMatchingIdx = iReco;
          bestMatchingMom = momReco;
        }
      }


      if (bestMatching != nullptr) {
        bestMatching -> SetIsMatched();
        mc -> SetIsMatched();

        auto truth = (KBMCRecoMatching *) fMatchingArray -> ConstructedAt(fMatchingArray -> GetEntries());
        truth -> SetIsMatched();
        truth -> Set(iMC, momMC, bestMatchingIdx, bestMatchingMom);
        countMatch++;
      }
    }
  }

  for (auto iMC = 0; iMC < numMC; iMC++)
  {
    auto mc = (KBMCRecoMatching *) fMCArray -> At(iMC);
    if (mc -> IsMatched())
      continue;

    auto truth = (KBMCRecoMatching *) fMatchingArray -> ConstructedAt(fMatchingArray -> GetEntries());
    truth -> SetIsNotFound();
    truth -> Set(iMC, mc -> GetMCMomentum(), -1, TVector3(0,0,0));
  }

  auto numReco = fRecoArray -> GetEntries();
  for (auto iReco = 0; iReco < numReco; iReco++)
  {
    auto reco = (KBMCRecoMatching *) fRecoArray -> At(iReco);
    if (reco -> IsMatched())
      continue;

    auto truth = (KBMCRecoMatching *) fMatchingArray -> ConstructedAt(fMatchingArray -> GetEntries());
    truth -> SetIsFake();
    truth -> Set(-1, TVector3(0,0,0), iReco, reco -> GetRecoMomentum());
  }

  kb_info << "MC Matching: " << countMatch << " / " << fMCArray -> GetEntries() << endl;

  return;
}
