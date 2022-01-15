#include "KBRun.hh"
#include "KBPSATask.hh"
#include "KBChannelHit.hh"
#include "KBTpcHit.hh"

#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

ClassImp(KBPSATask)

KBPSATask::KBPSATask()
:KBTask("KBPSATask","")
{
}

bool KBPSATask::Init()
{
  KBRun *run = KBRun::GetRun();

  fTpc = run -> GetDetectorSystem() -> GetTpc();
  fNPlanes = fTpc -> GetNumPlanes();

  fDriftVelocity = fPar -> GetParDouble("gasDriftVelocity");
  fTbTime = fPar -> GetParDouble("tbTime");
  if (fPar -> CheckPar("tbLengthOffset"))
    fTbLengthOffset = fPar -> GetParDouble("tbLengthOffset");

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  for (auto iPlane = 0; iPlane < fNPlanes; iPlane++)
    fPadPlane[iPlane] = fTpc -> GetPadPlane(iPlane);

  fHitArray = new TClonesArray("KBTpcHit", 5000);
  run -> RegisterBranch("Hit", fHitArray, fPersistency);

  if (fPSA == nullptr)
    fPSA = new KBPSA();

  this -> Add(fPSA);
  fPSA -> Init();

  return true;
}

void KBPSATask::Exec(Option_t*)
{
  fHitArray -> Clear("C");

  Int_t nPads = fPadArray -> GetEntriesFast();

  Int_t idx = 0;

  for (auto iPad = 0; iPad < nPads; iPad++) {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    Double_t kPlane = fPadPlane[pad->GetPlaneID()] -> GetPlaneK();

    auto bufferOut = pad -> GetBufferOut();
 
    vector<KBChannelHit> hitArray;
    fPSA -> AnalyzeChannel(bufferOut, &hitArray);
  
    auto idArray = pad -> GetMCIDArray();
    auto tbArray = pad -> GetMCTbArray();

    //if (hitArray.size() == 1 && pad -> GetNumMCIDs() == 1) //XXX
    //if (pad -> GetNumMCIDs() == 1) //XXX
    for (auto channelHit : hitArray) {

      ///@todo build pad plane dependent code
      Double_t k = kPlane - (channelHit.GetTDC()+0.5)*fTbTime*fDriftVelocity + fTbLengthOffset;

      KBVector3 pos(fTpc->GetEFieldAxis(),pad->GetI(),pad->GetJ(),k);

      auto hit = (KBTpcHit *) fHitArray -> ConstructedAt(idx);
      hit -> SetHitID(idx);
      hit -> SetPadID(pad -> GetPadID());
      hit -> SetX(pos.X());
      hit -> SetY(pos.Y());
      hit -> SetZ(pos.Z());
      hit -> SetTb(channelHit.GetTDC());
      hit -> SetCharge(channelHit.GetADC());
      hit -> SetSection(pad -> GetSection());
      hit -> SetRow(pad -> GetRow());
      hit -> SetLayer(pad -> GetLayer());

      Int_t atMC = 0;
      Double_t dist = 100.;
      Int_t numMCs = idArray -> size();
      for (Int_t iMC = 0; iMC < numMCs; ++iMC) {
        Double_t dtb = hit -> GetTb() - tbArray -> at(iMC);
        if (abs(dtb) < abs(dist)) {
          dist = dtb;
          atMC = iMC;
        }
      }
      if (dist < 100) //XXX
        hit -> SetMCTag(idArray -> at(atMC), dist*fTbTime*fDriftVelocity, 1);

      idx++;
    }
  }
  kb_info << "Number of found hits: " << idx << endl;
}

void KBPSATask::SetPSA(KBPSA *psa)
{
  fPSA = psa;
}

void KBPSATask::SetHitPersistency(bool persistence) { fPersistency = persistence; }
