
#include "ATTPCPSATask.hh"
#include "KBChannelHit.hh"
#include "KBTpcHit.hh"

#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

ClassImp(ATTPCPSATask)

ATTPCPSATask::ATTPCPSATask()
:KBTask("ATTPCPSATask","")
{
}

ATTPCPSATask::~ATTPCPSATask()
{
}

bool ATTPCPSATask::Init()
{
  run = KBRun::GetRun();
  auto par = run -> GetParameterContainer();
  fTpc = run -> GetDetectorSystem() -> GetTpc();

  fNPlanes = fTpc -> GetNumPlanes();
  fDriftVelocity = par -> GetParDouble("VelocityE");
  fTbTime = par -> GetParDouble("TBtime");

  fPadArray = (TClonesArray *) run -> GetBranch(fInputBranchName);

  for (auto iPlane = 0; iPlane < fNPlanes; iPlane++)
    fPadPlane[iPlane] = fTpc -> GetPadPlane(iPlane);


  fHitArray = new TClonesArray("KBTpcHit", 5000);
	run -> RegisterBranch(fOutputBranchName, fHitArray, fPersistency);

  if (fPSA == nullptr)
    fPSA = new ATTPCPSA();
  fPSA -> SetParameters(par);

  return true;
}

void ATTPCPSATask::Exec(Option_t*)
{
  fHitArray -> Clear("C");

  Int_t nPads = fPadArray -> GetEntriesFast();

  Int_t idx = 0;
    mcArray.clear();
    reconArray.clear();

    eventIndex = run->GetCurrentEventID();

  for (auto iPad = 0; iPad < nPads; iPad++) {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    Double_t kPlane = fPadPlane[pad->GetPlaneID()] -> GetPlaneK();

    auto bufferOut = pad -> GetBufferOut();

    vector<KBChannelHit> hitArray;

    fPSA -> AnalyzeChannel(bufferOut, &hitArray);
    padNum = pad -> GetPadID();

    int index=0;
    for (auto channelHit : hitArray) {
      ///@todo build pad plane dependent code
      Double_t k = (channelHit.GetTDC()-0.5)*fTbTime*fDriftVelocity;
        
      KBVector3 pos(fTpc->GetEFieldAxis(),pad->GetI(),pad->GetJ(),k);

      auto hit = (KBTpcHit *) fHitArray -> ConstructedAt(idx);

      hit -> SetHitID(idx);
      hit -> SetPadID(pad -> GetPadID());
      hit -> SetX(pos.X());
      hit -> SetY(pos.Y());
      hit -> SetZ(pos.K());
      hit -> SetTb(channelHit.GetTDC());
      hit -> SetCharge(channelHit.GetADC());
      hit -> SetSection(pad -> GetSection());
      hit -> SetRow(pad -> GetRow());
      hit -> SetLayer(pad -> GetLayer());


      if(fPar->CheckPar("isExpData")==false){
        auto idArray = pad -> GetMCIDArray();
        auto tbArray = pad -> GetMCTbArray();

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

        if (dist < 100){ //XXX
          hit -> SetMCTag(idArray -> at(atMC), dist*fTbTime*fDriftVelocity, 1);
          hit -> SetTrackID(idArray -> at(atMC));
        }
      }
      idx++;
    }
  }

  kb_info << "Number of found hits: " << idx << endl;
}

void ATTPCPSATask::SetPSA(ATTPCPSA *psa)
{
  fPSA = psa;
}

void ATTPCPSATask::SetHitPersistency(bool persistence) { fPersistency = persistence; }
