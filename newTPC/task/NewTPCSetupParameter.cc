#include "NewTPCSetupParameter.hh"

#include "TMath.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "TFile.h"
#include "TF1.h"

using namespace std;
using namespace TMath;

ClassImp(NewTPCSetupParameter)

NewTPCSetupParameter::NewTPCSetupParameter()
:KBTask("NewTPCSetupParameter","")
{
}

bool NewTPCSetupParameter::Init()
{
  GetGasParameters();
  par -> SetPar("VelocityE", fVelocityE);
  par -> SetPar("VelocityExB", fVelocityExB);
  par -> SetPar("LDiff", fLDiff);
  par -> SetPar("TDiff", fTDiff);
  par -> SetPar("BAt0DiffCoef2", fBAt0Coef2);
  par -> SetPar("GasWvalue", fGasWvalue);
  par -> SetPar("FanoFactor",fFanoFactor);
  
  return true;
}

void NewTPCSetupParameter::Exec(Option_t*)
{
}

void NewTPCSetupParameter::SetChannelPersistency(bool persistence) { fPersistency = persistence; }

void NewTPCSetupParameter::GetGasParameters()
{
 TString detMatName = par -> GetParString("detMatName");
  Double_t parEfield = par -> GetParDouble("efield");
  Double_t bfieldx = par -> GetParDouble("bfieldX");
  Double_t bfieldy = par -> GetParDouble("bfieldY");
  Double_t bfieldz = par -> GetParDouble("bfieldZ");

  // ========== Gas Data with Garfield++ ===================
  // Data order [data unit]             --> Unit to convert   

  // E-Field [V/cm]                     --> [V/cm]
  // B-Field [Tesla]                    --> [Tesla]
  // E-B Angle
  // E-Velocity [cm/us]                 --> [mm/ns]
  // B-Velocity [cm/us]                 --> [mm/ns]
  // ExB-Velocity [cm/us]               --> [mm/ns]
  // Longitude Diffusion [cm/sqrt(cm)]  --> [mm/sqrt(mm)] 
  // Transverse Diffusion [cm/sqrt(cm)] --> [mm/sqrt(mm)}
  // =======================================================

  if(detMatName == "p10") {
    fGasWvalue = 26.2; // [ev]
    fFanoFactor = 0.2;

    TFile * Parameters = new TFile("$KEBIPATH/newTPC/macros/input/P10Parameters.root","read");
    TF1* VDrift = (TF1*)Parameters -> Get("pol7");
    TF1* LDiff = (TF1*)Parameters -> Get("lengifit");

    fVelocityE = VDrift -> Eval(parEfield) *0.01; //[mm/ns]

    Double_t fBfieldCoef1 = 0.0000453*TMath::Power(bfieldz,2) -0.000283*bfieldz +0.00031*TMath::Sqrt(bfieldz) +0.00000174;
    Double_t fBfieldCoef2 = 0.0555*TMath::Exp(bfieldz*(-4.25)) +0.00289;
    fBAt0Coef2 = (0.00000174*parEfield +0.0555 +0.00289)*10/TMath::Sqrt(10); //[mm/sqrt(mm)]   Transvers diffsuion coefficient at B field =0 for correction

    fTDiff = (fBfieldCoef1*parEfield +fBfieldCoef2) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]
    fLDiff = LDiff -> Eval(parEfield) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  }
  if(detMatName == "p20") {
    fGasWvalue = 26.2; // [ev]
    fFanoFactor = 0.2;
  }
  if(detMatName == "4He") {
    fGasWvalue = 41; // [ev]
    fFanoFactor =0.2; 

    fVelocityE = (1.97207e-1 +1.39349e-2*parEfield -9.34245e-5*TMath::Power(parEfield,2) +4.35889e-7*TMath::Power(parEfield,3) -6.33769e-10*TMath::Power(parEfield,4)) *0.01; // [mm/ns]
    fLDiff = (1.332e-1 -5.15e-4*parEfield +7.2156e-6*TMath::Power(parEfield,2) -1.7274e-8*TMath::Power(parEfield,3)) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]

    Double_t fBfieldCoef1 = 0.170081*TMath::Exp(-1.88638*bfieldz) + 0.0131659;
    Double_t fBfieldCoef2 = 8.15e-7 + 0.00443761*bfieldz -0.00450315*TMath::Power(bfieldz,2) + 0.00131847*TMath::Power(bfieldz,3);
    Double_t fBfieldCoef3 = -2.2441e-05*bfieldz +2.53282e-05*TMath::Power(bfieldz,2) -8.02352e-06*TMath::Power(bfieldz,3);
    Double_t fBfieldCoef4 =  3.67471e-08*bfieldz -4.47213e-08*TMath::Power(bfieldz,2) +1.49915e-08*TMath::Power(bfieldz,3);

    fBAt0Coef2 = (0.170081 +0.0131659 +8.15e-7*parEfield)*10/TMath::Sqrt(10); //[mm/sqrt(mm)]
    fTDiff = (fBfieldCoef1 + fBfieldCoef2*parEfield + fBfieldCoef3*TMath::Power(parEfield,2) + fBfieldCoef4*TMath::Power(parEfield,3))*10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  } 
}
  
  
  
	  
