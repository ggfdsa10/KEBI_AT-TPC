#include "KBRun.hh"
#include "LAPNoiseSubtractionTask.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"

ClassImp(LAPNoiseSubtractionTask)

LAPNoiseSubtractionTask::LAPNoiseSubtractionTask()
:KBTask("LAPNoiseSubtractionTask","")
{
} 

bool LAPNoiseSubtractionTask::Init()
{
  fPadArray = (TClonesArray *) KBRun::GetRun() -> GetBranch("Pad");

  if (fPar -> CheckPar("skipNoiseSubtraction"))
    fSkipNoiseSubtraction = fPar -> GetParBool("skipNoiseSubtraction");
  fTbSamplingNoiseStart = fPar -> GetParInt("tbSamplingNoiseRange",0);
  fTbSamplingNoiseEnd = fPar -> GetParInt("tbSamplingNoiseRange",1);

  if (fPar -> CheckPar("skip_AsAd_IDs"))
  {
    int numIDs = fPar -> GetParN("skip_AsAd_IDs");
    for (auto iSkip=0; iSkip<numIDs; ++iSkip)
    {
      fSkipAsadIDs.push_back(fPar->GetParInt("skip_AsAd_IDs",iSkip));
      fSkipAGETIDs.push_back(fPar->GetParInt("skip_AGET_IDs",iSkip));
    }
  }

  if (fPar -> CheckPar("NSFindRefMethod"))
  {
    TString method = fPar -> GetParString("NSFindRefMethod");
    if (method=="minimize_stddev") fMethodFindRef = 2;
    if (method=="minimum_chare_diff") fMethodFindRef = 1;
  }


  return true;
}

void LAPNoiseSubtractionTask::Exec(Option_t*)
{
  Int_t nPads = fPadArray -> GetEntries();

  if (fSkipNoiseSubtraction)
  {
    for (Int_t iPad = 0; iPad < nPads; iPad++) {
      KBPad *pad = (KBPad *) fPadArray -> At(iPad);
      Short_t *raw = pad -> GetBufferRaw();
      Double_t out[512] = {0};

      CopyRaw(raw, out);
      pad -> SetBufferOut(out);
    }

    kb_info << "skiped" << endl;
    return;
  }

  FindReferencePad();

  if (fIdxPadRef == -1) {
    kb_warning << "Cannot find reference pad." << endl;
    return;
  }

  KBPad *padRef = (KBPad *) fPadArray -> At(fIdxPadRef);
  Short_t *rawRef = padRef -> GetBufferRaw();

  Double_t outRef[512] = {0};
  padRef -> SetBufferOut(outRef);

  CopyRaw(rawRef, outRef);
  Double_t baseLineRef = BaseLineCorrection(outRef, fTbSamplingNoiseStart, fTbSamplingNoiseEnd);

  padRef -> SetBaseLine(baseLineRef);
  padRef -> SetNoiseAmplitude(1);



  for (Int_t iPad = 0; iPad < nPads; iPad++) {
    if (iPad == fIdxPadRef)
      continue;

    KBPad *pad = (KBPad *) fPadArray -> At(iPad);

    auto asadID = pad -> GetAsAdID();
    auto agetID = pad -> GetAGETID();

    Short_t *raw = pad -> GetBufferRaw();
    Double_t out[512] = {0};

    Double_t baseLine = 0;
    Double_t amp = 0;

    bool skipPad = false;
    for (auto iSkip=0; iSkip<int(fSkipAsadIDs.size()); ++iSkip) {
      if (asadID==fSkipAsadIDs[iSkip] && agetID==fSkipAGETIDs[iSkip]) {
        skipPad = true;
        break;
      }
    }

    if (skipPad) {
      Set0(out);
    }
    else {
      CopyRaw(raw, out);
      baseLine = BaseLineCorrection(out, fTbSamplingNoiseStart, fTbSamplingNoiseEnd);
      amp = NoiseAmplitudeCorrection(out, outRef, fTbSamplingNoiseStart, fTbSamplingNoiseEnd);
      SaturationCorrection(out, raw, baseLine);
    }

    pad -> SetBufferOut(out);
    pad -> SetBaseLine(baseLine);
    pad -> SetNoiseAmplitude(amp);
  }

  kb_info << "Noise subtracted with ref-padID: " << fIdxPadRef << endl;
}

void LAPNoiseSubtractionTask::FindReferencePad()
{
  Int_t nPads = fPadArray -> GetEntries();

  if (fMethodFindRef==2)
  {
    Double_t stdDevMin = 100000.;

    for (Int_t iPad = 0; iPad < nPads; iPad++) {
      KBPad *pad = (KBPad *) fPadArray -> At(iPad);

      auto asadID = pad -> GetAsAdID();
      auto agetID = pad -> GetAGETID();

      bool skipPad = false;
      for (auto iSkip=0; iSkip<int(fSkipAsadIDs.size()); ++iSkip) {
        if (asadID==fSkipAsadIDs[iSkip] && agetID==fSkipAGETIDs[iSkip]) {
          skipPad = true;
          break;
        }
      }
      if (skipPad)
        continue;

      Short_t *raw = pad -> GetBufferRaw();

      Double_t mean = 0;
      for (Int_t tb = 1; tb < 510; tb++) {
        Double_t val = Double_t(raw[tb]);
        mean += val;
      }
      mean = mean/510;

      int count0 = 0;
      for (Int_t tb = 1; tb < 510; tb++) {
        Double_t val = Double_t(raw[tb]);
        if (val==0)
          count0++;
      }
      if (count0>100)
        continue;

      Double_t stdDev = 0;
      for (Int_t tb = 1; tb < 510; tb++) {
        Double_t val = Double_t(raw[tb]);
        stdDev += (mean-val)*(mean-val);
      }
      stdDev = sqrt(stdDev/509);

      if (stdDev<stdDevMin) {
        stdDevMin = stdDev;
        fIdxPadRef = iPad;
      }
    }
    return;
  }

  Double_t yDiffMax = 0.;
  Double_t yDiffMin = DBL_MAX;

  for (Int_t iPad = 0; iPad < nPads; iPad++) {
    KBPad *pad = (KBPad *) fPadArray -> At(iPad);

    auto asadID = pad -> GetAsAdID();
    auto agetID = pad -> GetAGETID();

    bool skipPad = false;
    for (auto iSkip=0; iSkip<int(fSkipAsadIDs.size()); ++iSkip) {
      if (asadID==fSkipAsadIDs[iSkip] && agetID==fSkipAGETIDs[iSkip]) {
        skipPad = true;
        break;
      }
    }
    if (skipPad)
      continue;

    Short_t *raw = pad -> GetBufferRaw();

    int count0 = 0;
    for (Int_t tb = 1; tb < 510; tb++) {
      Double_t val = Double_t(raw[tb]);
      if (val==0)
        count0++;
    }
    if (count0>100)
      continue;

    Double_t yMax = 0.;
    Double_t yMin = DBL_MAX;
    for (Int_t tb = 1; tb < 510; tb++) {
      Double_t val = Double_t(raw[tb]);
      if (val > yMax) yMax = val;
      if (val < yMin) yMin = val;
    }

    Double_t yDiff = yMax - yMin;

    if (fMethodFindRef==1) {
      if (yDiff < yDiffMin) {
        yDiffMin = yDiff;
        fIdxPadRef = iPad;
      }
    }
    else {
      if (TMath::Abs(pad -> GetRow()) < 8)
        continue;

      if (yMin != 0 && yMax < 2000 && yDiff > yDiffMax) {
        yDiffMax = yDiff;
        fIdxPadRef = iPad;
      }
      else if (yDiffMax == 0 && yDiff < yDiffMin) {
        yDiffMin = yDiff;
        fIdxPadRef = iPad;
      }
    }
  }
}

void LAPNoiseSubtractionTask::CopyRaw(Short_t *in, Double_t *out)
{
  for (Int_t tb = 0; tb < 512; tb++)
    out[tb] = Double_t(in[tb]);
}

void LAPNoiseSubtractionTask::Set0(Double_t *out)
{
  for (Int_t tb = 0; tb < 512; tb++)
    out[tb] = 0;
}

Double_t LAPNoiseSubtractionTask::BaseLineCorrection(Double_t *out, Int_t tbi, Int_t tbf)
{
  Double_t baseLine = 0.;
  for (Int_t tb = tbi; tb < tbf; tb++)
    baseLine +=  out[tb];

  baseLine = baseLine/(tbf - tbi + 1);
  for (Int_t tb = 0; tb < 512; tb++)
    out[tb] = out[tb] - baseLine;

  return baseLine;
}

Double_t LAPNoiseSubtractionTask::NoiseAmplitudeCorrection(Double_t *out, Double_t *ref, Int_t tbi, Int_t tbf)
{
  Double_t sum1 = 0.;
  Double_t sum2 = 0.;

  for (Int_t tb = tbi; tb < tbf; tb++) {
    Double_t valRef = ref[tb];
    Double_t val = out[tb];
    if (val == 0)
      continue;
    sum1 += valRef * val;
    sum2 += valRef * valRef;
  }

  Double_t amp = sum1/sum2;

  for (Int_t tb = 0; tb < 512; tb++)
    out[tb] = out[tb] - ref[tb]*amp;

  return amp;
}

void LAPNoiseSubtractionTask::SaturationCorrection(Double_t *out, Short_t *raw, Double_t baseLine)
{
  Double_t saturationHigh = 4095 - baseLine;
  for (Int_t tb = 0; tb < 512; tb++) {
    if (raw[tb] == 0)
      out[tb] = 0;
    else if (raw[tb] == 4095)
      out[tb] = saturationHigh;
    if (out[tb] > saturationHigh)
      out[tb] = saturationHigh;
  }
}
