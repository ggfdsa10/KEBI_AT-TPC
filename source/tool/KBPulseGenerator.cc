#include "KBGlobal.hh"
#include "KBRun.hh"
#include "KBPulseGenerator.hh"
#include "TSystem.h"
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

ClassImp(KBPulseGenerator);

KBPulseGenerator* KBPulseGenerator::fInstance = nullptr;

KBPulseGenerator* KBPulseGenerator::GetPulseGenerator(KBParameterContainer *par) {
  if (fInstance == nullptr) {
    TString fileName;
    if (par -> CheckPar("pulserData"))
      fileName = par -> GetParString("pulserData");
    fInstance = new KBPulseGenerator(fileName);
  }
  return fInstance;
}

KBPulseGenerator* KBPulseGenerator::GetPulseGenerator(TString fileName) {
  if (fInstance == nullptr)
    fInstance = new KBPulseGenerator(fileName);
  return fInstance;
}

KBPulseGenerator::KBPulseGenerator(TString fileName)
{
  Initialize(fileName);
}

bool KBPulseGenerator::Initialize(TString fileName)
{
  if (fileName=="deltaFunction") {
    fIsDeltaFunction = true;
    return true;
  }

  if (fileName.IsNull())
    fileName = "pulser_464ns.dat";
  TString fileNameConfigured = KBRun::ConfigureDataPath(fileName,true,"$(KEBIPATH)/input/",false);
  if (fileNameConfigured.IsNull()) {
    KBLog("KBPulseGenerator","Initialize",0,4) << "Input pulser file: " << fileName << " is not found!" << endl; 
    return false;
  }

  KBLog("KBPulseGenerator","Initialize",0,2) << "Using pulser file: " << fileNameConfigured << endl;
  ifstream file(fileNameConfigured);
  string line;

  while (getline(file, line) && line.find("#") == 0) {}
  istringstream ss(line);
  ss >> fShapingTime >> fNumDataPoints >> fStepSize >> fNumAscending >> fNDFTbs;

  if (fNumDataPoints < 20 || fStepSize > 1) {
    KBLog("KBPulseGenerator","Initialize",0,4) << "Number of data points (" << fNumDataPoints << ") should be >= 20, fStepSize (" << fStepSize << " should be < 1." << endl;
    KBLog("KBPulseGenerator","Initialize",0,4) << "Check file: " << fileName << endl;
    return false;
  }

  fPulseData = new KBSamplePoint[fNumDataPoints];

  Double_t max = 0;
  for (Int_t iData = 0; iData < fNumDataPoints; iData++)
  {
    getline(file, line);
    if (line.find("#") == 0) {
      iData--;
      continue;
    }

    fPulseData[iData].Init(line);
    Double_t value = fPulseData[iData].fValue;
    if (iData == 0)
      fThresholdRatio = value;

    if (value > max) {
      max = value;
      fTbAtMax = iData * fStepSize;
    }
  }

  Double_t c = 1./max;
  Double_t valuePre = 0, valueCur = 0;
  fTbAtThreshold = 0;
  fTbAtTail = 0;

  for (Int_t iData = 0; iData < fNumDataPoints; iData++)
  {
    fPulseData[iData].fValue = c * fPulseData[iData].fValue;

    valuePre = valueCur;
    valueCur = fPulseData[iData].fValue;

    if (fTbAtThreshold == 0 && valueCur > fThresholdRatio)
    {
      fTbAtThreshold = iData * fStepSize;
      Int_t next = iData + 1/fStepSize;
      fThresholdTbStep = fPulseData[next].fValue - fPulseData[iData].fValue;
    }

    if (fTbAtTail == 0 && valueCur < valuePre && valueCur < 0.1)
      fTbAtTail = iData * fStepSize;
  }

  file.close();

  return true;
}

Double_t 
KBPulseGenerator::Pulse(Double_t x, Double_t amp, Double_t tb0)
{
  if (fIsDeltaFunction)
  {
    if (x>=tb0-1 && x<tb0) {
      auto val = amp * (x - tb0 + 1);
      //return val;
      return amp;
    }
    else if (x>=tb0 && x<tb0+1) {
      auto val = - amp * (x - tb0 - 1);
      //return val;
      return amp;
    }
    else
      return 0;
  }

  Double_t tb = x - tb0;
  if (tb < 0) 
    return 0;

  Int_t tbInStep = tb / fStepSize;
  if (tbInStep > fNumDataPoints - 2) 
    return 0;

  Double_t r = (tb / fStepSize) - tbInStep;
  Double_t val = r * fPulseData[tbInStep + 1].fValue + (1 - r) * fPulseData[tbInStep].fValue;

  return amp * val;
}

Double_t 
KBPulseGenerator::PulseF1(Double_t *x, Double_t *par)
{
  if (fIsDeltaFunction)
  {
    if (x[0]>=par[1]-1 && x[0]<par[1]) {
      auto val = par[0] * (x[0] - par[1] + 1);
      //return val;
      return par[0];
    }
    else if (x[0]>=par[1] && x[0]<par[1]+1) {
      auto val = - par[0] * (x[0] - par[1] - 1);
      //return val;
      return par[0];
    }
    else
      return 0;
  }

  Double_t tb = x[0] - par[1];
  if (tb < 0) 
    return 0;

  Int_t tbInStep = tb / fStepSize;
  if (tbInStep > fNumDataPoints - 2)
    return 0;

  Double_t r = (tb / fStepSize) - tbInStep;
  Double_t val = r * fPulseData[tbInStep + 1].fValue + (1 - r) * fPulseData[tbInStep].fValue;

  return par[0] * val;
}

TF1*
KBPulseGenerator::GetPulseFunction(TString name)
{
  if (name.IsNull()) 
    name = Form("STPulse_%d", fNumF1++);

  if (fIsDeltaFunction)
  {
    //TF1* f1 = new TF1(name, "(x>=[1]-1 && x<[1])*[0]*(x-[1]+1) + (x>=[1] && x<[1]+1)*(-[0])*(x-[1]-1)", 0, 512);
    TF1* f1 = new TF1(name, "(x>=[1]-1 && x<[1])*[0] + (x>=[1] && x<[1]+1)*(-[0])", 0, 512);
    f1 -> SetNpx(512);
    return f1;
  }

  TF1* f1 = new TF1(name, this, &KBPulseGenerator::PulseF1, 0, 512, 2, "KBPulseGenerator", "PulseF1");
  return f1;
}

   Int_t  KBPulseGenerator::GetShapingTime()     { return fShapingTime;     }
Double_t  KBPulseGenerator::GetTbAtThreshold()   { return fTbAtThreshold;   }
Double_t  KBPulseGenerator::GetTbAtTail()        { return fTbAtTail;        }
Double_t  KBPulseGenerator::GetTbAtMax()         { return fTbAtMax;         }
   Int_t  KBPulseGenerator::GetNumAscending()    { return fNumAscending;    }
Double_t  KBPulseGenerator::GetThresholdTbStep() { return fThresholdTbStep; }
   Int_t  KBPulseGenerator::GetNumDataPoints()   { return fNumDataPoints;   }
Double_t  KBPulseGenerator::GetStepSize()        { return fStepSize;        }
   Int_t  KBPulseGenerator::GetNDFTbs()          { return fNDFTbs;          }

KBSamplePoint **KBPulseGenerator::GetPulseData()  { return &fPulseData; }

void
KBPulseGenerator::Print()
{
  if (fIsDeltaFunction) {
    KBLog("KBPulseGenerator","Print",0,2) << "[KBPulseGenerator INFO] DeltaFunction" << endl;
    return;
  }

  KBLog("KBPulseGenerator","Print",0,2) << "[KBPulseGenerator INFO]" << endl;
  KBLog("KBPulseGenerator","Print",0,2) << " == Shaping time : " << fShapingTime << " ns" << endl;
  KBLog("KBPulseGenerator","Print",0,2) << " == Number of data points : " << fNumDataPoints << endl;
  KBLog("KBPulseGenerator","Print",0,2) << " == Step size between data points : " << fStepSize << endl;
  KBLog("KBPulseGenerator","Print",0,2) << " == Threshold for one timebucket step : " << fThresholdTbStep << endl; 
  KBLog("KBPulseGenerator","Print",0,2) << " == Number of timebucket while rising : " << fNumAscending << endl;
  KBLog("KBPulseGenerator","Print",0,2) << " == Timebucket at threshold (" << setw(3) << fThresholdRatio 
       << " of peak) : " << fTbAtThreshold << endl; 
  KBLog("KBPulseGenerator","Print",0,2) << " == Timebucket at peak : " << fTbAtMax << endl; 
  KBLog("KBPulseGenerator","Print",0,2) << " == Timebucket difference from threshold to peak : " 
       << fTbAtMax - fTbAtThreshold << endl; 
  KBLog("KBPulseGenerator","Print",0,2) << " == Number of degree of freedom : " << fNDFTbs << endl;
}

void
KBPulseGenerator::SavePulseData(TString name, Bool_t smoothTail)
{
  Double_t max = 0;
  for (Int_t iData = 0; iData < fNumDataPoints; iData++) {
    if (fPulseData[iData].fValue > max)
      max = fPulseData[iData].fValue;
  }

  Double_t c = 1/max;
  if (max != 1) {
    for (Int_t iData = 0; iData < fNumDataPoints; iData++)
      fPulseData[iData].fValue = c * fPulseData[iData].fValue;
  }

  if (smoothTail) {
    if (fTailFunction == nullptr)
      fTailFunction = new TF1("tail", "x*landau(0)", fTbAtTail - 1, fNumDataPoints - 1);

    fTailFunction -> SetParameters(1, 4, 1);

    if (fTailGraph == nullptr)
      fTailGraph = new TGraph();

    fTailGraph -> Clear();
    fTailGraph -> Set(0);

    Int_t iTailStart = (Int_t)(fTbAtTail/fStepSize);
    for (Int_t iData = iTailStart; iData < fNumDataPoints; iData++)
      fTailGraph -> SetPoint(fTailGraph -> GetN(), iData * fStepSize, fPulseData[iData].fValue);

    fTailGraph -> Fit(fTailFunction, "R");

    for (Int_t iData = iTailStart; iData < fNumDataPoints; iData++)
      fPulseData[iData].fValue = fTailFunction -> Eval(iData * fStepSize);
  }

  ofstream file(name.Data());
  file << "#(shaping time) (number of data points) (step size) (rising tb threshold number) (default ndf)" << endl;
  file << fShapingTime << " " << fNumDataPoints << " " << fStepSize << " " << fNumAscending << " " << fNDFTbs << endl;

  file << "#(value) (rms) (total weight)" << endl;
  for (Int_t iData = 0; iData < fNumDataPoints; iData++) {
    KBSamplePoint sample = fPulseData[iData];
    file << sample.GetSummary() << endl;
  }

  file.close();
}
