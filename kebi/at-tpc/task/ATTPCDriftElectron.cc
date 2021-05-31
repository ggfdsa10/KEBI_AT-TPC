#include "ATTPCDriftElectron.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"
#include "ATTPCSetupParameter.hh"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"

#include <iostream>

ClassImp(ATTPCDriftElectron)

ATTPCDriftElectron::ATTPCDriftElectron()
:KBTask("ATTPCDriftElectron","")
{
}


bool ATTPCDriftElectron::Init()
{
  KBRun *run = KBRun::GetRun();
  fTpc = (ATTPC *) run -> GetDetectorSystem() -> GetTpc();
  fMCTrackArray = (TClonesArray *) run -> GetBranch("MCTrack");
  fNPlanes = fTpc -> GetNumPlanes();

  KBParameterContainer *par = run -> GetParameterContainer();

  fVelocityE =  par -> GetParDouble("VelocityE");
  fLDiff = (par -> GetParDouble("LDiff"));
  fTDiff = (par -> GetParDouble("TDiff"));
  fWvalue = par -> GetParDouble("GasWvalue");
  fNTbs = par -> GetParInt("NTbs");
  fTBtime = par -> GetParDouble("TBtime");
  
  TString gemDataFile;
  TFile *gemFile = new TFile("$KEBIPATH/input/tripleGEM.root", "read");
  fGainFunction = (TF1*) gemFile -> Get("gainFit");
  fGainZeroRatio = (((TObjString *) ((TList *) gemFile -> GetListOfKeys()) -> At(2)) -> GetString()).Atof();
  
  fDiffusionFunction = (TH2D*) ((TCanvas*) gemFile -> Get("diffusion")) -> FindObject("distHist");

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);
  
  
  
  return true;
}


void ATTPCDriftElectron::Exec(Option_t*)
{
 
  gRandom -> SetSeed(0);
  
  fPadArray -> Delete();
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++){
    fTpc -> GetPadPlane(iPlane) -> Clear();
  }
  
  Long64_t nMCtrack = fMCTrackArray -> GetEntries();
  for ( Long64_t itrack = 0; itrack < nMCtrack; ++itrack){
    KBMCTrack* track = (KBMCTrack*) fMCTrackArray -> At(itrack);

    Int_t trackID = track -> GetParentID();
    Double_t KEnergy = (track -> GetKE())*1000000; //[ev]
    
    KBVector3 posMC(track -> GetVX(), track -> GetVY(), track -> GetVZ());
    posMC.SetReferenceAxis(fTpc -> GetEFieldAxis());

    auto plane = fTpc -> GetDriftPlane(posMC.GetXYZ());
    if (plane == nullptr)
      continue;

    Int_t planeID = plane -> GetPlaneID();
    Double_t kPlane = plane -> GetPlaneK();

    Double_t lDrift = std::abs(kPlane - posMC.K());
    Double_t tDrift = lDrift/fVelocityE;
    

    Double_t sigmaLD = fLDiff * sqrt(lDrift);
    Double_t sigmaTD = fTDiff * sqrt(lDrift);

    Int_t SecondaryNum = KEnergy/fWvalue;

    for (Int_t iElectron = 0; iElectron < SecondaryNum; iElectron++) {
      Double_t dr    = gRandom -> Gaus(0, sigmaTD);
      Double_t angle = gRandom -> Uniform(2*TMath::Pi());

      Double_t di = dr*TMath::Cos(angle);
      Double_t dj = dr*TMath::Sin(angle);
      Double_t dt = gRandom -> Gaus(0,sigmaLD)/fVelocityE;

      Double_t tDriftTotal = tDrift + std::abs(dt);
      Int_t tb = (Int_t)(tDriftTotal/fTBtime);

      if (tb > fNTbs) 
        continue;

      Int_t gain = fGainFunction -> GetRandom() * (1 - fGainZeroRatio);
      if (gain <= 0)
        continue;

      Double_t iDiffGEM, jDiffGEM;

      for (Int_t iElCluster = 0; iElCluster < gain; iElCluster++) {
	fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);

	fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di+iDiffGEM, posMC.J()+dj+jDiffGEM, tb, 1, trackID);
      }
    }
  }

  Int_t idx = 0;
  Int_t idxLast = 0;
  
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
    KBPad *pad;
    TIter itChannel(fTpc -> GetPadPlane(iPlane) -> GetChannelArray());
    while ((pad = (KBPad *) itChannel.Next())) {     

      KBPad *padSave = new ((*fPadArray)[idx]) KBPad();
      padSave -> SetPad(pad);
      padSave -> CopyPadData(pad);
      idx++;
    }

    kb_info << "Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
    idxLast = idx;
  }
  
  return;
}



void ATTPCDriftElectron::SetPadPersistency(bool persistence) { fPersistency = persistence; }
