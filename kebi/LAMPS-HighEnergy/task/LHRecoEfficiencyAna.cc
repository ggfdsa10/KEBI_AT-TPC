#include "LHRecoEfficiencyAna.hh"
#include "KBHelixTrack.hh"
#include "KBMCRecoMatching.hh"

#include "KBRun.hh"

#include <vector>
#include <iostream>
using namespace std;

ClassImp(LHRecoEfficiencyAna)

LHRecoEfficiencyAna::LHRecoEfficiencyAna()
:KBTask("LHRecoEfficiencyAna","")
{
}

bool LHRecoEfficiencyAna::Init()
{
  KBRun *run = KBRun::GetRun();

  fMCTrackArray = (TClonesArray *) run -> GetBranch("MCTrack");
  fRecoTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fMCArray = new TClonesArray("KBMCRecoMatching", 100);
  fRecoArray = new TClonesArray("KBMCRecoMatching", 100);

  run -> RegisterBranch("MCMatching",   fMCArray,   fPersistency);
  run -> RegisterBranch("RecoMatching", fRecoArray, fPersistency);

  fMCIDIdx = new Int_t[5000];

  return true;
}

void LHRecoEfficiencyAna::Exec(Option_t*)
{
  fMCArray -> Clear();
  fRecoArray -> Clear();

  auto numMCTracks = fMCTrackArray -> GetEntries();
  for (auto iMCTrack = 0; iMCTrack < numMCTracks; iMCTrack++)
  {
    auto mcTrack = (KBMCTrack *) fMCTrackArray -> At(iMCTrack);
    Int_t mcID = mcTrack -> GetTrackID();

    fMCIDIdx[mcID] = iMCTrack;

    auto matMC = (KBMCRecoMatching *) fMCArray -> ConstructedAt(iMCTrack);
    matMC -> SetMCID(mcID);
    matMC -> SetMCMomentum(mcTrack->Momentum());
    matMC -> SetIsNotFound();
  }

  Int_t numFake = 0;

  auto numRecoTracks = fRecoTrackArray -> GetEntries();
  for (auto iRecoTrack = 0; iRecoTrack < numRecoTracks; iRecoTrack++)
  {
    auto recoTrack = (KBHelixTrack *) fRecoTrackArray -> At(iRecoTrack);

    auto mcID = recoTrack -> GetMCID();
    auto mcTrack = (KBMCTrack *) fMCTrackArray -> At(mcID);

    auto matReco = (KBMCRecoMatching *) fRecoArray -> ConstructedAt(iRecoTrack);
         matReco -> Set(mcID,mcTrack->Momentum(),iRecoTrack,recoTrack->Momentum());
         matReco -> SetIsMatched();

    auto matMC = (KBMCRecoMatching *) fMCArray -> At(fMCIDIdx[mcID]);
    if (matMC -> IsNotFound()) {
      matMC -> SetRecoID(iRecoTrack);
      matMC -> SetRecoMomentum(recoTrack -> Momentum());
      matMC -> SetIsMatched();
    }
    else
      matMC -> AddRecoCand(iRecoTrack,recoTrack->Momentum());
  }

  Int_t countFound = 0;

  //@todo how to set best candidate
  for (auto iMCTrack = 0; iMCTrack < numMCTracks; iMCTrack++)
  {
    auto matMC = (KBMCRecoMatching *) fMCArray -> At(iMCTrack);
    if (matMC -> IsNotFound())
      continue;

    ++countFound;

    Int_t numCands = matMC -> GetNumRecoCand();
    if (numCands == 0)
      continue;

    auto pMC = matMC -> GetMCMomentum();
    auto pReco = matMC -> GetRecoMomentum();

    auto dpBestCand = pMC.Mag()-pReco.Mag();
    auto idxBestCand = -1;
    auto recoIDBestCand = -1;
    auto pBestCand = pReco;

    for (auto iCand = 0; iCand < numCands; ++iCand) {
      auto pcand = matMC -> GetRecoMomentumCand(iCand);
      auto dpCand = pMC.Mag()-pcand.Mag();
      if (dpCand < dpBestCand) {
        dpBestCand = dpCand;
        idxBestCand = iCand;
        recoIDBestCand = matMC -> GetRecoIDCand(iCand);
        pBestCand = pcand;
      }
    }

    if (idxBestCand != -1) {
      matMC -> SetRecoCand(idxBestCand, matMC->GetRecoID(), matMC->GetRecoMomentum());
      matMC -> SetReco(recoIDBestCand, pBestCand);
    }
  }

  kb_info << "MC-Eff: " << countFound << " / " << numMCTracks << " = " << countFound/numMCTracks << endl;
  kb_info << "number of fake reco: " << numFake << endl;

  return;
}
