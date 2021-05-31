#include "KBRun.hh"
#include "LAPBeamTrackingTask.hh"
#include "KBLinearTrack.hh"
#include "KBTpcHit.hh"
#include "KBHitArray.hh"

#include <iostream>
using namespace std;

ClassImp(LAPBeamTrackingTask)

LAPBeamTrackingTask::LAPBeamTrackingTask()
:KBTask("LAPBeamTrackingTask","")
{
} 

bool LAPBeamTrackingTask::Init()
{
  auto run = KBRun::GetRun();

  fHitArray = (TClonesArray *) run -> GetBranch("Hit");

  fHitClusterArray = new TClonesArray("KBTpcHit");
  fBeamTrackArray = new TClonesArray("KBLinearTrack",1);

  run -> RegisterBranch("HitCluster", fHitClusterArray, fHitClusterPersistency);
  run -> RegisterBranch("Beam", fBeamTrackArray, fLinearTrackPersistency);

  fBeamTbRange1 = fPar -> GetParDouble("beamTbRange",0);
  fBeamTbRange2 = fPar -> GetParDouble("beamTbRange",1);
  fHitChargeHL = fPar -> GetParDouble("beamHitChargeHL");
  fNumSections = fPar -> GetParInt("numSections");
  for (auto section=0; section<fNumSections; ++section)
    fNumLayers[section] = fPar -> GetParInt("numLayers",section);

  return true;
}

void LAPBeamTrackingTask::Exec(Option_t*)
{
  fBeamTrackArray -> Clear("C");
  fHitClusterArray -> Clear("C");

  Int_t nHits = fHitArray -> GetEntries();
  if (nHits < 4) {
    kb_info << "No beam track found. Least number of hits should be 4. You have " << nHits << endl;
    return;
  }


  // apply cut
  for (auto iHit = 0; iHit < nHits; iHit++) {
    auto hit = (KBTpcHit *) fHitArray -> At(iHit);
    auto tb = hit -> GetTb();
    if (tb < fBeamTbRange1 || tb > fBeamTbRange2 || hit -> GetCharge() > fHitChargeHL)
      hit -> SetTrackID(-1);
    else
      hit -> SetTrackID(0);
  }


  // make clusters for all layers
  auto numLayersMax = fNumLayers[0];
  for (auto section=0; section<fNumSections; ++section)
    if (numLayersMax > fNumLayers[section])
      numLayersMax = fNumLayers[section];

  for (auto iSection = 0; iSection < fNumSections; ++iSection) {
    for (auto iLayer = 0; iLayer < fNumLayers[iSection]; ++iLayer) {
      auto cluster = (KBTpcHit *) fHitClusterArray -> ConstructedAt(iSection*numLayersMax+iLayer);
      cluster -> SetSection(iSection);
      cluster -> SetLayer(iLayer);
    }
  }


  // add hits to clusters
  auto nextHit = TIter(fHitArray);
  while (auto hit = (KBTpcHit *) nextHit()) {
    if (hit->GetTrackID()==0) {
      auto idxCluster = (hit -> GetSection() * numLayersMax) + hit -> GetLayer();
      auto cluster = (KBTpcHit *) fHitClusterArray -> At(idxCluster);
      cluster -> AddHit(hit);
    }
  }
  fHitClusterArray -> Compress();


  // remove empty clusters
  Int_t numClusters = fHitClusterArray -> GetEntriesFast();
  for (auto iCluster = 0; iCluster < numClusters; ++iCluster) {
    auto cluster = (KBTpcHit *) fHitClusterArray -> At(iCluster);
    if (cluster -> GetCharge() <= 0)
      fHitClusterArray -> Remove(cluster);
  }
  fHitClusterArray -> Compress();

  auto numClustersFinal = fHitClusterArray -> GetEntries();
  if (numClustersFinal < 4) {
    kb_info << "No beam track found. Least number of clusters should be 4. You have " << numClustersFinal << endl;
    return;
  }


  // add clusters and fit beam track
  auto beamTrack = (KBLinearTrack *) fBeamTrackArray -> ConstructedAt(0);
  auto nextHitCluster = TIter(fHitClusterArray);
  while (auto cluster = (KBTpcHit *) nextHitCluster())
    beamTrack -> AddHit(cluster);
  beamTrack -> Fit();



  //
  for (auto iCluster = 0; iCluster < numClustersFinal; ++iCluster) {
    auto cluster = (KBTpcHit *) fHitClusterArray -> At(iCluster);
    auto pos = cluster -> GetPosition();
    TVector3 poca = beamTrack -> ClosestPointOnLine(pos);
    cluster -> SetDPosition((pos-poca));
  }


  kb_info << "Beam track found from " << numClustersFinal << " clustsers with rms: " << beamTrack -> GetRMS() << endl;
}
