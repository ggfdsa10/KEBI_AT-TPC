#include "ATTPCDriftElectron.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"
#include "ATTPCSetupParameter.hh"

#include "TMath.h"

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

  par = run -> GetParameterContainer();

  fGemVolt = par -> GetParDouble("GemVolt");
  if(par->GetParBool("fixGainP10")==0){
    fPressureRatio = 760./par -> GetParDouble("pressure");
    fElectronNumRef = par -> GetParDouble("ElectronNumRef");
  }
  
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

  fFPNPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("FPNPad", fFPNPadArray, fPersistency);

  fDecoder = new ATTPCDecoderTask();

  return true;
}


void ATTPCDriftElectron::Exec(Option_t*)
{
  gRandom -> SetSeed(0);
  fPadArray -> Delete();
  fFPNPadArray -> Delete();

  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++){
    fTpc -> GetPadPlane(iPlane) -> Clear();
  }
  
  Long64_t nMCtrack = fMCTrackArray -> GetEntries();

  for (Long64_t itrack = 0; itrack < nMCtrack; ++itrack){
    KBMCTrack* track = (KBMCTrack*) fMCTrackArray -> At(itrack);

    Int_t PDG = track -> GetPDG();
    Int_t parentID = track -> GetParentID();
    Double_t KEnergy = (track -> GetKE())*1000000; //[ev]

    KBVector3 posMC(track -> GetVX(), track -> GetVY(), track -> GetVZ());
    
    posMC.SetReferenceAxis(fTpc -> GetEFieldAxis());

    auto plane = fTpc -> GetDriftPlane(posMC.GetXYZ());
    if (plane == nullptr)
      continue;

    Int_t planeID = plane -> GetPlaneID();
    Double_t kPlane = plane -> GetPlaneK();

    Double_t lDrift = std::abs(kPlane - posMC.K());

    // Double_t tDrift = lDrift/fVelocityE;
    // Double_t sigmaLD = fLDiff * TMath::Sqrt(lDrift);
    // Double_t sigmaTD = TransverseDiffusion(lDrift);

    // original diffusion (no corraction) ===================================
    Double_t tDrift = lDrift/fVelocityE;
    Double_t sigmaLD = fLDiff*TMath::Sqrt(lDrift);
    Double_t sigmaTD = fTDiff*TMath::Sqrt(lDrift);
    Double_t sigmaGEMTD = 0.03245/TMath::Sqrt(10.);

    sigmaTD = sqrt(sigmaTD*sigmaTD + 3*pow(sigmaGEMTD, 2));
    // ======================================================================

    Double_t Wvalue = WvalueDistribution();
    Int_t SecondaryNum = KEnergy/Wvalue;
    if(SecondaryNum < 1){ SecondaryNum = 1;}
    
    if(PDG != 11)
      continue;

    for (Int_t iElectron = 0; iElectron < SecondaryNum; iElectron++) {
      Double_t dr    = gRandom -> Gaus(0, sigmaTD);
      Double_t angle = gRandom -> Uniform(2*TMath::Pi());

      Double_t di = dr*TMath::Cos(angle);
      Double_t dj = dr*TMath::Sin(angle);
      Double_t dt = gRandom -> Gaus(0,sigmaLD)/fVelocityE;

      Double_t tDriftTotal = tDrift + dt;
      Double_t tb = tDriftTotal/fTBtime;
      
      if (tb > fNTbs) 
        continue;
      Int_t gain = fGainFunction -> GetRandom();
      Int_t gainRatio = 1;

      if (gain <= 0)
        continue;

      if (fFastCalculate == true){
        if(gain <= 1e+5)
          gainRatio =20;

        else if(gain > 1e+5)
          gainRatio = 500;

        gain = gain/gainRatio;
      }

      for (Int_t iElCluster = 0; iElCluster < gain; iElCluster++) {
	      fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di, posMC.J()+dj, tb, gainRatio, parentID);
      }
    }
  }

  Int_t idx = 0;
  Int_t idxLast = 0;
  
  // for Pad Channel
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
    KBPad *pad;
    TIter itChannel(fTpc -> GetPadPlane(iPlane) -> GetChannelArray());
    while ((pad = (KBPad *) itChannel.Next())) {     
      //if ((fNoise == false) && (pad -> IsActive() == false))
      //  continue;

      KBPad *padSave = new ((*fPadArray)[idx]) KBPad();
      padSave -> SetPad(pad);
      padSave -> CopyPadData(pad);

      Int_t iAsAd = get<0>(fDecoder -> GetBoardID(idx));
      Int_t iAGET = get<1>(fDecoder -> GetBoardID(idx));
      Int_t iChannel = get<2>(fDecoder -> GetBoardID(idx));
      padSave -> SetAsAdID(iAsAd);
      padSave -> SetAGETID(iAGET);
      padSave -> SetChannelID(iChannel);
      idx++;
    }

    // for FPN Channel
    for(int fpnIdx = 0; fpnIdx< 16; fpnIdx++){
      auto FPNpadSave = (KBPad *) fFPNPadArray -> ConstructedAt(fpnIdx);
      Int_t iAsAd = get<0>(fDecoder -> GetBoardFPNID(fpnIdx));
      Int_t iAGET = get<1>(fDecoder -> GetBoardFPNID(fpnIdx));
      Int_t iChannel = get<2>(fDecoder -> GetBoardFPNID(fpnIdx));
      FPNpadSave -> SetAsAdID(iAsAd);
      FPNpadSave -> SetAGETID(iAGET);
      FPNpadSave -> SetChannelID(iChannel);
      FPNpadSave -> SetPadID(fpnIdx);
      FPNpadSave -> SetSortValue(fpnIdx);
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
  Double_t GainMean = TMath::Exp((-16.84+0.07451*fGemVolt)*fPressureRatio);
  Double_t GainParMean = TMath::Exp(-16.84+0.07451*fGemVolt);
  Double_t GainVariance = 1/(pow(TMath::Exp(-20.7+0.07985*fGemVolt)/GainParMean, 2)*fElectronNumRef - fFanoFactor);

  // fGainFunction = new TF1("GainFunction", this, &ATTPCDriftElectron::PolyaFunction, 0, GainMean/0.05, 3);
  // fGainFunction -> SetParameter(0, GainVariance);
  // fGainFunction -> SetParameter(1, GainMean); 
  // fGainFunction -> SetParameter(2, 1);
  // fGainFunction -> SetParameter(2, fGainFunction -> Integral(0,GainMean/0.05)); // normalization

  // test
  fGainFunction = new TF1("GainFunction", this, &ATTPCDriftElectron::PolyaFunction, 0, 15000, 3);
  Double_t a = 3.0369;
  Double_t b = 4503.55;
  Double_t c = 0.00024412;

  if(!par->GetParBool("Physics")){
    b = b*(par->GetParDouble("GemVolt")/400.);
  }
  fGainFunction -> SetParameters(3.0369, 4503.55, 0.00024412);
}

Double_t ATTPCDriftElectron::WvalueDistribution(){
  constexpr double wref = 30.0;
  constexpr double fref = 0.174;

  if (fWvalue <= 0 || fFanoFactor < 0) {
    return 0.;
  } 
  else if (fFanoFactor == 0) {
    return fWvalue;
  }

  const double x = gRandom ->Uniform() * wref * 0.82174;
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

Double_t ATTPCDriftElectron::TransverseDiffusion(Double_t length){

  Double_t CorrSigma1 = 0.0128*TMath::Sqrt(length) +1.61;
  Double_t CorrSigma2 = 0.0316*TMath::Sqrt(length) +1.28;
  Double_t DiffReduceRatio = fTDiff*fTDiff/fBAt0DiffCoef2/fBAt0DiffCoef2;
  Double_t sigmaTD = (CorrSigma1*CorrSigma1)/(CorrSigma2*CorrSigma2)*TMath::Sqrt(fTDiff*fTDiff*length +((CorrSigma1*CorrSigma1)-(CorrSigma2*CorrSigma2))*DiffReduceRatio);
  return sigmaTD;
}