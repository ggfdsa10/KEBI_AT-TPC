
#include "NewTPCPSATask.hh"
#include "KBChannelHit.hh"
#include "KBTpcHit.hh"

#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

ClassImp(NewTPCPSATask)

NewTPCPSATask::NewTPCPSATask()
:KBTask("NewTPCPSATask","")
{
    // root file init
    rootfile = new TFile("$KEBIPATH/at-tpc/codeTest/lowData_reco.root", "recreate");
    ArrayTree = new TTree("tree","tree");
    
    ArrayTree->Branch("eventIndex",&eventIndex, "eventIndex/I");
    ArrayTree->Branch("mcArray",&mcArray);
    ArrayTree->Branch("reconArray",&reconArray);
}

NewTPCPSATask::~NewTPCPSATask(){
}

bool NewTPCPSATask::Init()
{
  run = KBRun::GetRun();
    cout << " done..." << endl;
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
    fPSA = new NewTPCPSA();
  fPSA -> SetParameters(par);

  return true;
}

void NewTPCPSATask::Exec(Option_t*)
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
    vector<vector<pair<double, double>>> TBArray;
    fPSA -> AnalyzeChannel(bufferOut, &hitArray, &TBArray);

    auto idArray = pad -> GetMCIDArray();
    auto tbArray = pad -> GetMCTbArray();

    padNum = pad -> GetPadID();
    vector<vector<double>> mcArrayBuffer;
    vector<vector<double>> reconArrayBuffer;

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


        vector<double> mcArrayADC;
        vector<double> reconArrayADC;

        // // root file macors
        for(int i=0; i<TBArray[index].size(); i++){
            double mcADC = TBArray[index][i].first;
            double reconADC = TBArray[index][i].second;

            mcArrayADC.push_back(mcADC);
            reconArrayADC.push_back(reconADC);
        }
        mcArrayBuffer.push_back(mcArrayADC);
        reconArrayBuffer.push_back(reconArrayADC);
      idx++;
      index++;
    }

    mcArray.push_back(mcArrayBuffer);
    reconArray.push_back(reconArrayBuffer);
  }

    ArrayTree->Fill();
  kb_info << "Number of found hits: " << idx << endl;

  
  if(run->GetCurrentEventID()==run->GetEndEventID()){
    rootfile->cd();
    ArrayTree->Write();
    rootfile->Close();
  }
}

void NewTPCPSATask::SetPSA(NewTPCPSA *psa)
{
  fPSA = psa;
}

void NewTPCPSATask::SetHitPersistency(bool persistence) { fPersistency = persistence; }
