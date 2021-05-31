#include "ATTPCSetupParameter.hh"

#include "TMath.h"
#include <iostream>
#include <fstream>
#include <algorithm>

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
  Double_t parEfield = par -> GetParDouble("efield");
  /*
  Double_t bfieldx = par -> GetParDouble("bfieldX");
  Double_t bfieldy = par -> GetParDouble("bfieldY");
  Double_t bfieldz = par -> GetParDouble("bfieldZ");
  */
  TString detMatName = par -> GetParString("detMatName");
  TString GasData = par -> GetParString("gasdata");

  // Gas Data Preprocessing

  fstream GasFile;
  GasFile.open(GasData, ios::in);

  Double_t pEfield = 0;
  Double_t pBfield = 0;
  Double_t pAngle = 0;
  Double_t pVelocityE = 0;
  Double_t pVelocityB = 0;
  Double_t pVelocityExB = 0;
  Double_t pLDiff = 0;
  Double_t pTDiff = 0;
  Double_t pTownsend = 0;
  Double_t pAttachment = 0;
  Double_t pLorentzAngle = 0;
  
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
     Lorentz Angle
     =======================================================
  */

  bool empty = GasFile.peek() == EOF;
  if (empty == false){
    while (!GasFile.eof()){
      GasFile >> pEfield >> pBfield >> pAngle >> pVelocityE >> pVelocityB >> pVelocityExB >> pLDiff >> pTDiff >> pTownsend >> pAttachment >> pLorentzAngle;
      
      GasPar[0].push_back(pEfield);
      GasPar[1].push_back(pBfield);
      GasPar[2].push_back(pAngle);
      GasPar[3].push_back(pVelocityE);
      GasPar[4].push_back(pVelocityB);
      GasPar[5].push_back(pVelocityExB);
      GasPar[6].push_back(pLDiff);
      GasPar[7].push_back(pTDiff);
      GasPar[8].push_back(pTownsend);
      GasPar[9].push_back(pAttachment);
      GasPar[10].push_back(pLorentzAngle);
    }
  }
  
  // Bfield to be updated 
  vector<double> diff;
  for (int i = 0; i < GasPar[0].size(); i++){
    double a = abs(GasPar[0][i] - parEfield);
    diff.push_back(a);
  }
  
  int index = min_element(diff.begin(), diff.end()) - diff.begin();
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

  
  
  
	  
