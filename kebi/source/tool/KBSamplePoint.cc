#include "KBSamplePoint.hh"

#include <iostream>

ClassImp(KBSamplePoint);

KBSamplePoint::KBSamplePoint()
{
  Init(-999, 0, 1);
}

KBSamplePoint::KBSamplePoint(KBSamplePoint &sample)
{
  Init(sample.fValue, sample.fRMS, sample.fWeightSum);
}

KBSamplePoint::KBSamplePoint(Double_t v, Double_t rms, Double_t w)
{
  Init(v, rms, w);
}

KBSamplePoint::~KBSamplePoint()
{
}

void 
KBSamplePoint::Print()
{
  cout << " KBSamplePoint " << fValue << " | " << fRMS << " | " << fWeightSum << endl;
}

void 
KBSamplePoint::Init(Double_t v, Double_t rms, Double_t w)
{
  fValue = v;
  fRMS = rms;
  fWeightSum = w;
}

void 
KBSamplePoint::Init(string line)
{
  istringstream ss(line);
  ss >> fValue >> fRMS >> fWeightSum;
}

void 
KBSamplePoint::Update(Double_t v, Double_t w)
{
  fValue = (fWeightSum * fValue + w * v) / (fWeightSum + w);
  fRMS = (fWeightSum * fRMS) / (fWeightSum + w)
         + w * (fValue - v) * (fValue - v) / fWeightSum;

  fWeightSum += w;
}

TString
KBSamplePoint::GetSummary()
{
  return Form("%.6g %.6g %.6g", fValue, fRMS, fWeightSum);
}
