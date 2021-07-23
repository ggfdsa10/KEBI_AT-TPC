#include "ATTPCDriftElectron.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"
#include "ATTPCSetupParameter.hh"

#include "TCanvas.h"
#include "TMath.h"

ClassImp(ATTPCDriftElectron)

ATTPCDriftElectron::ATTPCDriftElectron()
:KBTask("ATTPCDriftElectron","")
{
}

bool ATTPCDriftElectron::Init()
{
  fRandom = new TRandom3(time(0));
  KBRun *run = KBRun::GetRun();
  fTpc = (ATTPC *) run -> GetDetectorSystem() -> GetTpc();
  fMCTrackArray = (TClonesArray *) run -> GetBranch("MCTrack");
  fNPlanes = fTpc -> GetNumPlanes();

  par = run -> GetParameterContainer();

  fGemVolt = par -> GetParDouble("GemVolt");
  fVelocityE =  par -> GetParDouble("VelocityE");
  fLDiff = par -> GetParDouble("LDiff");
  fTDiff = par -> GetParDouble("TDiff");
  fWvalue = par -> GetParDouble("GasWvalue");
  fNTbs = par -> GetParInt("NTbs");
  fTBtime = par -> GetParDouble("TBtime");

  GainDistribution();

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  return true;
}


void ATTPCDriftElectron::Exec(Option_t*)
{
  
  fPadArray -> Delete();
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++){
    fTpc -> GetPadPlane(iPlane) -> Clear();
  }
  
  Long64_t nMCtrack = fMCTrackArray -> GetEntries();

  for (Long64_t itrack = 0; itrack < nMCtrack; ++itrack){
    KBMCTrack* track = (KBMCTrack*) fMCTrackArray -> At(itrack);

    Int_t trackID = track -> GetParentID();
    Int_t SourceCut = track -> GetTrackID();
    Double_t KEnergy = (track -> GetKE())*1000000; //[ev]

    if(SourceCut == 1)
      continue;

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
    if(SecondaryNum < 1){ SecondaryNum = 1;}

    for (Int_t iElectron = 0; iElectron < SecondaryNum; iElectron++) {
      Double_t dr    = fRandom -> Gaus(0, sigmaTD);
      Double_t angle = fRandom -> Uniform(2*TMath::Pi());

      Double_t di = dr*TMath::Cos(angle);
      Double_t dj = dr*TMath::Sin(angle);
      Double_t dt = fRandom -> Gaus(0,sigmaLD)/fVelocityE;

      Double_t tDriftTotal = tDrift + dt;
      Int_t tb = (Int_t)(tDriftTotal/fTBtime);
      
      if (tb > fNTbs) 
        continue;

      Int_t gain = fGainFunction -> GetRandom();
      if (gain <= 0)
        continue;

      Double_t GEMTD = fTDiff * sqrt(fNumGEMLayer*fGEMLayerTerm);
      Double_t iDiffGEM, jDiffGEM;

      for (Int_t iElCluster = 0; iElCluster < gain; iElCluster++) {
        Double_t angle1 = fRandom -> Uniform(2*TMath::Pi());
        Double_t GEMdr = fRandom -> Gaus(0, GEMTD);
        iDiffGEM = GEMdr*TMath::Cos(angle1);
        jDiffGEM = GEMdr*TMath::Sin(angle1);

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

      if (pad -> IsActive() == true)
        idx++;
    }

    kb_info << "Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
    idxLast = idx;
  }
  
  return;
}

Double_t ATTPCDriftElectron::PolyaFunction(Double_t *x, Double_t *Par){
  Double_t Polya_val = (1/Par[2])*(1/TMath::Gamma(Par[0]))*TMath::Power(Par[0],Par[0])*TMath::Power(x[0]/Par[1],Par[0]-1)*TMath::Exp(-1*x[0]*Par[0]/Par[1]);
  return Polya_val;
}

void ATTPCDriftElectron::GainDistribution(){
  fGEMLayerTerm = 0.2; //[cm]
  Double_t Electron = 219.1;
  Double_t Fano = 0.2; // about 0.2 in Argon based gas mixture
  Double_t GainMean = TMath::Exp(-16.84+0.07451*fGemVolt);
  Double_t GainVariance = 1/(pow(TMath::Exp(-20.7+0.07985*fGemVolt) / GainMean, 2)*Electron - Fano);

  fGainFunction = new TF1("GainFunction", this, &ATTPCDriftElectron::PolyaFunction, 0, GainMean/0.05, 3);
  fGainFunction -> SetParameter(0, GainVariance);
  fGainFunction -> SetParameter(1, GainMean); 
  fGainFunction -> SetParameter(2, 1);
  fGainFunction -> SetParameter(2, fGainFunction -> Integral(0,GainMean/0.05)); // normalization
}
