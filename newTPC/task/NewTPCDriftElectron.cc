#include "NewTPCDriftElectron.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"
#include "NewTPCSetupParameter.hh"

#include "TCanvas.h"
#include "TMath.h"

ClassImp(NewTPCDriftElectron)

NewTPCDriftElectron::NewTPCDriftElectron()
:KBTask("NewTPCDriftElectron","")
{
}

bool NewTPCDriftElectron::Init()
{
  fRandom = new TRandom3(time(0));
  KBRun *run = KBRun::GetRun();
  fTpc = (NewTPC *) run -> GetDetectorSystem() -> GetTpc();
  fMCTrackArray = (TClonesArray *) run -> GetBranch("MCTrack");
  fNPlanes = fTpc -> GetNumPlanes();

  par = run -> GetParameterContainer();

  fGemVolt = par -> GetParDouble("GemVolt");
  fVelocityE =  par -> GetParDouble("VelocityE");
  fLDiff = par -> GetParDouble("LDiff");
  fTDiff = par -> GetParDouble("TDiff");
  fBAt0DiffCoef2 = par -> GetParDouble("BAt0DiffCoef2");
  fWvalue = par -> GetParDouble("GasWvalue");
  fFanoFactor = par -> GetParDouble("FanoFactor");
  fNTbs = par -> GetParInt("NTbs");
  fTBtime = par -> GetParDouble("TBtime");
  fNoise = par -> GetParBool("NoiseOn");
  fFastCalculate = par -> GetParBool("FastCalculate");

  GainDistribution();

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  return true;
}


void NewTPCDriftElectron::Exec(Option_t*)
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
    Double_t sigmaTD = TransverseDiffusion(lDrift);

    Double_t Wvalue = WvalueDistribution();
    Int_t SecondaryNum = KEnergy/Wvalue;
    
    if(SecondaryNum < 1){ SecondaryNum = 1;}
    
    for (Int_t iElectron = 0; iElectron < SecondaryNum; iElectron++) {
      Double_t dr    = fRandom -> Gaus(0, sigmaTD);
      Double_t angle = fRandom -> Uniform(2*TMath::Pi());

      Double_t di = dr*TMath::Cos(angle);
      Double_t dj = dr*TMath::Sin(angle);
      Double_t dt = fRandom -> Gaus(0,sigmaLD)/fVelocityE;

      Double_t tDriftTotal = tDrift + dt;
      Double_t tb = tDriftTotal/fTBtime;

      if (tb > fNTbs) 
        continue;
      Int_t gain = fGainFunction -> GetRandom();
      Int_t gainRatio = 1;

      if (gain <= 0)
        continue;

      if (fFastCalculate == true){
        if(fGemVolt <= 300)
          gainRatio =20;

        else if(fGemVolt > 300)
          gainRatio = 100;

        gain = gain/gainRatio;
      }
      
      for (Int_t iElCluster = 0; iElCluster < gain; iElCluster++) {
	      fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di, posMC.J()+dj, tb, gainRatio, trackID);
      }
    }
  }

  Int_t idx = 0;
  Int_t idxLast = 0;
  
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
    KBPad *pad;
    TIter itChannel(fTpc -> GetPadPlane(iPlane) -> GetChannelArray());
    while ((pad = (KBPad *) itChannel.Next())) {     
      //if ((fNoise == false) && (pad -> IsActive() == false))
      //  continue;

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

Double_t NewTPCDriftElectron::PolyaFunction(Double_t *x, Double_t *Par){
  Double_t Polya_val = (1/Par[2])*(1/TMath::Gamma(Par[0]))*TMath::Power(Par[0],Par[0])*TMath::Power(x[0]/Par[1],Par[0]-1)*TMath::Exp(-1*x[0]*Par[0]/Par[1]);
  return Polya_val;
}

void NewTPCDriftElectron::GainDistribution(){
  Double_t Electron = 219.1;
  Double_t GainMean = TMath::Exp(-16.84+0.07451*fGemVolt);
  Double_t GainVariance = 1/(pow(TMath::Exp(-20.7+0.07985*fGemVolt) / GainMean, 2)*Electron - fFanoFactor);

  fGainFunction = new TF1("GainFunction", this, &NewTPCDriftElectron::PolyaFunction, 0, GainMean/0.05, 3);
  fGainFunction -> SetParameter(0, GainVariance);
  fGainFunction -> SetParameter(1, GainMean); 
  fGainFunction -> SetParameter(2, 1);
  fGainFunction -> SetParameter(2, fGainFunction -> Integral(0,GainMean/0.05)); // normalization
}

Double_t NewTPCDriftElectron::WvalueDistribution(){
  constexpr double wref = 30.0;
  constexpr double fref = 0.174;

  if (fWvalue <= 0 || fFanoFactor < 0) {
    return 0.;
  } 
  else if (fFanoFactor == 0) {
    return fWvalue;
  }

  const double x = fRandom ->Uniform() * wref * 0.82174;
  double e;

  if (x < 0) {
    e = 0.5 * wref;
  } 
  else if (x < 0.5 * wref) {
    e = 0.5 * wref + x;
  } 
  else if (x < wref * 0.82174) {
    constexpr double w4 = wref * wref * wref * wref;
    e = std::cbrt(2 * w4 / (5 * wref - 6 * x));
  } 
  else {
    e = 3.064 * wref;
  }

  const double sqf = sqrt(fFanoFactor / fref);
  return (fWvalue / wref) * sqf * e + fWvalue * (1. - sqf);
}

Double_t NewTPCDriftElectron::TransverseDiffusion(Double_t length){

  Double_t CorrSigma1 = 0.0128*TMath::Sqrt(length) +1.61;
  Double_t CorrSigma2 = 0.0316*TMath::Sqrt(length) +1.28;
  Double_t DiffReduceRatio = fTDiff/fBAt0DiffCoef2;
  Double_t sigmaTD = (CorrSigma1*CorrSigma1)/(CorrSigma2*CorrSigma2)*TMath::Sqrt(fTDiff*fTDiff*length +((CorrSigma1*CorrSigma1)-(CorrSigma2*CorrSigma2))*DiffReduceRatio);

  return sigmaTD;
}
