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
  par -> SetPar("GasWvalue", GasWvalue);
  
  return true;
}

void ATTPCSetupParameter::Exec(Option_t*)
{
}

void ATTPCSetupParameter::SetChannelPersistency(bool persistence) { fPersistency = persistence; }

void ATTPCSetupParameter::GetGasParameters()
{
  TString detMatName = par -> GetParString("detMatName");
  Double_t parEfield = par -> GetParDouble("efield");
  /*
  Double_t bfieldx = par -> GetParDouble("bfieldX");
  Double_t bfieldy = par -> GetParDouble("bfieldY");
  Double_t bfieldz = par -> GetParDouble("bfieldZ");
  */


/* 
     ========== Gas Data with Garfield++ ===================
     Data order [data unit]             --> Unit to convert   

     E-Field [V/cm]                     --> [V/cm]
     B-Field [Tesla]                    --> [Tesla]
     E-B Angle
     E-Velocity [cm/us]                 --> [mm/ns]
     B-Velocity [cm/us]                 --> [mm/ns]
     ExB-Velocity [cm/us]               --> [mm/ns]
     Longitude Diffusion [cm/sqrt(cm)]  --> [mm/sqrt(mm)] 
     Transverse Diffusion [cm/sqrt(cm)] --> [mm/sqrt(mm)}
     Townsend Coefficient [1/cm]        --> [1/mm] 
     Attachment Coefficient [1/cm]      --> [1/mm]
     =======================================================
*/

/*
  fEfield = GasPar[0][index]; // [V/cm]
  fBfield = GasPar[1][index]; // [Tesla]
  fAngle = GasPar[2][index];
  fVelocityE = GasPar[3][index] *0.01; // [mm/ns]
  fVelocityB = GasPar[4][index] *0.01; // [mm/ns]
  fVelocityExB = GasPar[5][index] *0.01; // [mm/ns]
  fLDiff = GasPar[6][index] *10/TMath::Sqrt(10); // [mm/sqrt(mm)]
  fTDiff = GasPar[7][index] *10/TMath::Sqrt(10); // [mm/sqrt(mm)]
  fTownsend = GasPar[8][index] /10; // [1/mm]
  fAttachment = GasPar[9][index] /10; // [1/mm]
  fLorentzAngle = GasPar[10][index];
*/

  TFile * Parameters = new TFile("$KEBIPATH/at-tpc/macros/input/P10Parameters.root","read");
  TF1* VDrift = (TF1*)Parameters -> Get("pol7");
  TF1* TDiff = (TF1*)Parameters -> Get("transfit");
  TF1* LDiff = (TF1*)Parameters -> Get("lengifit");

  fVelocityE = VDrift -> Eval(parEfield) *0.01; //[mm/ns]
  fTDiff = TDiff -> Eval(parEfield) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]
  fLDiff = LDiff -> Eval(parEfield) *10/TMath::Sqrt(10); //[mm/sqrt(mm)]

  cout << fVelocityE << " " << fTDiff << " " << fLDiff << endl;

  
  if(detMatName == "p10") {
    GasWvalue = 26.2; // [ev]
  }
  if(detMatName == "p20") {
    GasWvalue = 26.2; // [ev]
  }
  if(detMatName == "4He") {
    GasWvalue = 41; // [ev]
  } 
}

  
  
  
	  
