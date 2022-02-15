#include "ATTPCSetupParameter.hh"

#include "TMath.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "TFile.h"
#include "TF1.h"

using namespace std;
using namespace TMath;

ClassImp(ATTPCSetupParameter)

ATTPCSetupParameter::ATTPCSetupParameter()
:KBTask("ATTPCSetupParameter","")
{
}

bool ATTPCSetupParameter::Init()
{
  GetGasParameters();
  par -> SetPar("VelocityE", fVelocityE);
  par -> SetPar("VelocityExB", fVelocityExB);
  par -> SetPar("LDiff", fLDiff);
  par -> SetPar("TDiff", fTDiff);
  par -> SetPar("BAt0DiffCoef2", fBAt0Coef2);
  par -> SetPar("GasWvalue", fGasWvalue);
  par -> SetPar("FanoFactor",fFanoFactor);
  par -> SetPar("ElectronNumRef", fElectronNumRef);
  
  return true;
}

void ATTPCSetupParameter::Exec(Option_t*)
{
}

void ATTPCSetupParameter::SetChannelPersistency(bool persistence) { fPersistency = persistence; }

void ATTPCSetupParameter::GetGasParameters()
{
  TString detMatName = par -> GetParString("detMatName");
  fiC4H10Ratio = par -> GetParDouble("iC4H10Ratio");
  fpressure = par -> GetParDouble("pressure")/760.; // [atm]
  bfieldx = par -> GetParDouble("bfieldX");
  bfieldy = par -> GetParDouble("bfieldY");
  bfieldz = par -> GetParDouble("bfieldZ");
  Double_t parEfield = par -> GetParDouble("efield");


  // ========== Gas Data with Garfield++ ===================
  // Data order [data unit]             --> Unit to convert   

  // E-Field [V/cm]                     --> [V/cm]
  // B-Field [Tesla]                    --> [Tesla]
  // E-B Angle
  // E-Velocity [cm/us]                 --> [mm/ns]
  // B-Velocity [cm/us]                 --> [mm/ns]
  // ExB-Velocity [cm/us]               --> [mm/ns]
  // Longitude Diffusion [cm/sqrt(cm)]  --> [mm/sqrt(mm)] 
  // Transverse Diffusion [cm/sqrt(cm)] --> [mm/sqrt(mm)]
  // =======================================================


  if(detMatName == "p10") {
    fGasWvalue = 26.4*0.9 + 27.3*0.1; // [ev]
    fFanoFactor = 0.17*0.9 + 0.26*0.1;
    fElectronNumRef = 219.1;

    TFile * Parameters = new TFile("$KEBIPATH/at-tpc/macros/input/P10Parameters.root","read");
    TF1* VDrift = (TF1*)Parameters -> Get("pol7");
    TF1* LDiff = (TF1*)Parameters -> Get("lengifit");

    fVelocityE = VDrift -> Eval(parEfield) *0.01; //[mm/ns]

    Double_t fBfieldCoef1 = 0.0000453*TMath::Power(bfieldz,2) -0.000283*bfieldz +0.00031*TMath::Sqrt(bfieldz) +0.00000174;
    Double_t fBfieldCoef2 = 0.0555*TMath::Exp(bfieldz*(-4.25)) +0.00289;
    fBAt0Coef2 = (0.00000174*parEfield +0.0555 +0.00289)*10/TMath::Sqrt(10); //[mm/sqrt(mm)]   Transvers diffsuion coefficient at B field =0 for correction

    fTDiff = (fBfieldCoef1*parEfield +fBfieldCoef2) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]
    fLDiff = LDiff -> Eval(parEfield) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  }

  if(detMatName == "4He") {
    GetHe4_iC4H10Parameters();

    fGasWvalue = 41.3; // [ev]
    fFanoFactor = 0.17; 

    Double_t fENumPar0 = 143.005;
    Double_t fENumPar1 = 2.26307e-05-2.00916e-05/fpressure;
    Double_t fENumPar2 = -3.53165e-08+3.2839e-08/fpressure+3.24292e-07/(fpressure*fpressure);

    fElectronNumRef = fENumPar0+fENumPar1*parEfield+fENumPar2*parEfield*parEfield;

    fVelocityE = fVEarray[0][fIndexPressure][0]+fVEarray[0][fIndexPressure][1]*sqrt(parEfield)+fVEarray[0][fIndexPressure][2]*parEfield+fVEarray[0][fIndexPressure][3]*pow(parEfield,2)+fVEarray[0][fIndexPressure][4]*pow(parEfield,3)+fVEarray[0][fIndexPressure][5]*pow(parEfield,4)+fVEarray[0][fIndexPressure][6]*pow(parEfield,5);
    fVelocityE = fVelocityE*0.01; //[mm/ns]

    fLDiff = fLDarray[0][fIndexPressure][0]+fLDarray[0][fIndexPressure][1]/parEfield+fLDarray[0][fIndexPressure][2]/parEfield/parEfield+fLDarray[0][fIndexPressure][3]/sqrt(parEfield)+fLDarray[0][fIndexPressure][4]*sqrt(parEfield)+fLDarray[0][fIndexPressure][5]*parEfield*parEfield*exp(fLDarray[0][fIndexPressure][6]*parEfield+fLDarray[0][fIndexPressure][7]);
    fLDiff = fLDiff*10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    fTDiff = fTDarray[0][fIndexPressure][fIndexMagnet][0]+fTDarray[0][fIndexPressure][fIndexMagnet][1]/parEfield+fTDarray[0][fIndexPressure][fIndexMagnet][2]/parEfield/parEfield+fTDarray[0][fIndexPressure][fIndexMagnet][3]/sqrt(parEfield)+fTDarray[0][fIndexPressure][fIndexMagnet][4]*sqrt(parEfield)+fTDarray[0][fIndexPressure][fIndexMagnet][5]*parEfield*parEfield*exp(fTDarray[0][fIndexPressure][fIndexMagnet][6]*parEfield+fTDarray[0][fIndexPressure][fIndexMagnet][7]);
    fTDiff = fTDiff*10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    fBAt0Coef2 = fTDarray[0][fIndexPressure][0][0]+fTDarray[0][fIndexPressure][0][1]/parEfield+fTDarray[0][fIndexPressure][0][2]/parEfield/parEfield+fTDarray[0][fIndexPressure][0][3]/sqrt(parEfield)+fTDarray[0][fIndexPressure][0][4]*sqrt(parEfield)+fTDarray[0][fIndexPressure][0][5]*parEfield*parEfield*exp(fTDarray[0][fIndexPressure][0][6]*parEfield+fTDarray[0][fIndexPressure][0][7]);
    fBAt0Coef2 = fBAt0Coef2*10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  } 

  if(detMatName == "4He_iC4H10") {
    GetHe4_iC4H10Parameters();

    fGasWvalue = 41.3*(1. -0.1*fiC4H10Ratio) + 23.4*(0.1*fiC4H10Ratio); // [ev]
    fFanoFactor = 0.17*(1. -0.1*fiC4H10Ratio) + 0.26*(0.1*fiC4H10Ratio);

    Double_t fENumPar00 = 143.172+0.676134*fiC4H10Ratio;
    Double_t fENumPar10 = 1.55422e-4-1.63818e-4*sqrt(fiC4H10Ratio)+3.42238e-05*fiC4H10Ratio-3.42506e-07*fiC4H10Ratio*fiC4H10Ratio;
    Double_t fENumPar11 = -8.38428e-05+1.28356e-4*sqrt(fiC4H10Ratio)-2.99891e-05*fiC4H10Ratio+3.52035e-07*fiC4H10Ratio*fiC4H10Ratio;
    Double_t fENumPar20 = 1.70499e-08 +3.33743e-08/fiC4H10Ratio -4.06082e-10*fiC4H10Ratio -3.55058e-11*fiC4H10Ratio*fiC4H10Ratio;
    Double_t fENumPar21 = -2.16738e-08 -5.67422e-08/fiC4H10Ratio +9.58559e-10*fiC4H10Ratio +6.04049e-11*fiC4H10Ratio*fiC4H10Ratio;
    Double_t fENumPar22 = exp(-14.9758-0.286417*fiC4H10Ratio);

    Double_t fENumPar0 = fENumPar00;
    Double_t fENumPar1 = fENumPar10+fENumPar11/fpressure;
    Double_t fENumPar2 = fENumPar20+fENumPar21/fpressure+fENumPar22/(fpressure*fpressure);

    fElectronNumRef = fENumPar0+fENumPar1*parEfield+fENumPar2*parEfield*parEfield;

    fVelocityE = fVEarray[fIndexRatio][fIndexPressure][0]+fVEarray[fIndexRatio][fIndexPressure][1]*sqrt(parEfield)+fVEarray[fIndexRatio][fIndexPressure][2]*parEfield+fVEarray[fIndexRatio][fIndexPressure][3]*pow(parEfield,2)+fVEarray[fIndexRatio][fIndexPressure][4]*pow(parEfield,3)+fVEarray[fIndexRatio][fIndexPressure][5]*pow(parEfield,4)+fVEarray[fIndexRatio][fIndexPressure][6]*pow(parEfield,5);
    fVelocityE = fVelocityE*0.01; //[mm/ns]

    fLDiff = fLDarray[fIndexRatio][fIndexPressure][0]+fLDarray[fIndexRatio][fIndexPressure][1]/parEfield+fLDarray[fIndexRatio][fIndexPressure][2]/parEfield/parEfield+fLDarray[fIndexRatio][fIndexPressure][3]/sqrt(parEfield)+fLDarray[fIndexRatio][fIndexPressure][4]*sqrt(parEfield)+fLDarray[fIndexRatio][fIndexPressure][5]*parEfield*parEfield*exp(fLDarray[fIndexRatio][fIndexPressure][6]*parEfield+fLDarray[fIndexRatio][fIndexPressure][7]);
    fLDiff = fLDiff*10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    fTDiff = fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][0]+fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][1]/parEfield+fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][2]/parEfield/parEfield+fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][3]/sqrt(parEfield)+fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][4]*sqrt(parEfield)+fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][5]*parEfield*parEfield*exp(fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][6]*parEfield+fTDarray[fIndexRatio][fIndexPressure][fIndexMagnet][7]);
    fTDiff = fTDiff*10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    fBAt0Coef2 = fTDarray[fIndexRatio][fIndexPressure][0][0]+fTDarray[fIndexRatio][fIndexPressure][0][1]/parEfield+fTDarray[fIndexRatio][fIndexPressure][0][2]/parEfield/parEfield+fTDarray[fIndexRatio][fIndexPressure][0][3]/sqrt(parEfield)+fTDarray[fIndexRatio][fIndexPressure][0][4]*sqrt(parEfield)+fTDarray[fIndexRatio][fIndexPressure][0][5]*parEfield*parEfield*exp(fTDarray[fIndexRatio][fIndexPressure][0][6]*parEfield+fTDarray[fIndexRatio][fIndexPressure][0][7]);
    fBAt0Coef2 = fBAt0Coef2*10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  }

  if(detMatName == "iC4H10") {
    GetHe4_iC4H10Parameters();

    fGasWvalue = 23.4; // [ev]
    fFanoFactor = 0.26;

    Double_t fENumPar0 = 252.235-0.00220129/fpressure;
    Double_t fENumPar1 = -2.24944e-05+1.17926e-05/fpressure;
    Double_t fENumPar2 = 8.3272e-10/(fpressure*fpressure)+3.02103e-09/fpressure-3.86401e-09;

    fElectronNumRef = fENumPar0+fENumPar1*parEfield+fENumPar2*parEfield*parEfield;

    fVelocityE = fVEarray[7][fIndexPressure][0]+fVEarray[7][fIndexPressure][1]*sqrt(parEfield)+fVEarray[7][fIndexPressure][2]*parEfield+fVEarray[7][fIndexPressure][3]*pow(parEfield,2)+fVEarray[7][fIndexPressure][4]*pow(parEfield,3)+fVEarray[7][fIndexPressure][5]*pow(parEfield,4)+fVEarray[7][fIndexPressure][6]*pow(parEfield,5);
    fVelocityE = fVelocityE*0.01; //[mm/ns]

    fLDiff = fLDarray[7][fIndexPressure][0]+fLDarray[7][fIndexPressure][1]/parEfield+fLDarray[7][fIndexPressure][2]/parEfield/parEfield+fLDarray[7][fIndexPressure][3]/sqrt(parEfield)+fLDarray[7][fIndexPressure][4]*sqrt(parEfield)+fLDarray[7][fIndexPressure][5]*parEfield*parEfield*exp(fLDarray[7][fIndexPressure][6]*parEfield+fLDarray[7][fIndexPressure][7]);
    fLDiff = fLDiff*10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    fTDiff = fTDarray[7][fIndexPressure][fIndexMagnet][0]+fTDarray[7][fIndexPressure][fIndexMagnet][1]/parEfield+fTDarray[7][fIndexPressure][fIndexMagnet][2]/parEfield/parEfield+fTDarray[7][fIndexPressure][fIndexMagnet][3]/sqrt(parEfield)+fTDarray[7][fIndexPressure][fIndexMagnet][4]*sqrt(parEfield)+fTDarray[7][fIndexPressure][fIndexMagnet][5]*parEfield*parEfield*exp(fTDarray[7][fIndexPressure][fIndexMagnet][6]*parEfield+fTDarray[7][fIndexPressure][fIndexMagnet][7]);
    fTDiff = fTDiff*10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    fBAt0Coef2 = fTDarray[7][fIndexPressure][0][0]+fTDarray[7][fIndexPressure][0][1]/parEfield+fTDarray[7][fIndexPressure][0][2]/parEfield/parEfield+fTDarray[7][fIndexPressure][0][3]/sqrt(parEfield)+fTDarray[7][fIndexPressure][0][4]*sqrt(parEfield)+fTDarray[7][fIndexPressure][0][5]*parEfield*parEfield*exp(fTDarray[7][fIndexPressure][0][6]*parEfield+fTDarray[7][fIndexPressure][0][7]);
    fBAt0Coef2 = fBAt0Coef2*10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  }
}

void ATTPCSetupParameter::GetHe4_iC4H10Parameters(){
  TFile *Parameters = new TFile("$KEBIPATH/at-tpc/macros/input/He4+iC4H10_Parameters.root","read");
  auto treePar = (TTree*)Parameters -> Get("GasParArray");
  treePar -> SetBranchAddress("VEarray", &fVEarray);
  treePar -> SetBranchAddress("TDarray", &fTDarray);
  treePar -> SetBranchAddress("LDarray", &fLDarray);
  treePar->GetEntry(0);

  float arrayGasRatio[7] = {0., 0.5, 1., 5., 10., 15., 20.};
  float arrayPressure[7] = {0.05, 0.1, 0.2, 0.4, 0.6, 0.8, 1.};
  float arraymagnet[6] = {0., 0.3, 0.6, 0.9, 1.2, 1.5};

  float tmp = 0.f;
  float min = 100.f;
  float near = 0.f;
  int index = 0; 
  for(int i=0; i<7; i++){
    tmp = arrayGasRatio[i] - fiC4H10Ratio;
    if (abs(min) > abs(tmp)){
      min = tmp;
      near = arrayGasRatio[i];
      index = i;
    }
  }
  fIndexRatio = index;

  tmp = 0.f;
  min = 100.f;
  near = 0.f;
  index = 0; 
  for(int i=0; i<7; i++){
    tmp = arrayPressure[i] - fpressure;
    if (abs(min) > abs(tmp)){
      min = tmp;
      near = arrayPressure[i];
      index = i;
    }
  }
  fIndexPressure = index;

  tmp = 0.f;
  min = 100.f;
  near = 0.f;
  index = 0; 
  for(int i=0; i<6; i++){
    tmp = arraymagnet[i] - bfieldz;
    if (abs(min) > abs(tmp)){
      min = tmp;
      near = arraymagnet[i];
      index = i;
    }
  }
  fIndexMagnet = index;
}
  
	  
