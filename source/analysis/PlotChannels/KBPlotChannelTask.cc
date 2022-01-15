#include "KBRun.hh"
#include "KBPlotChannelTask.hh"

#include "TROOT.h"
#include "TStyle.h"

#include <iostream>
using namespace std;

ClassImp(KBPlotChannelTask)

KBPlotChannelTask::KBPlotChannelTask()
:KBTask("KBPlotChannelTask","")
{
}

bool KBPlotChannelTask::Init()
{
  KBRun *run = KBRun::GetRun();

  auto par = run -> GetParameterContainer();
  fDetector = run -> GetDetectorSystem() -> GetTpc();
  fNPlanes = fDetector -> GetNumPlanes();
  fTbStart = par -> GetParInt("tbStart");
  fNTbs = par -> GetParInt("nTbs");

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  if (fOutputDir.IsNull())
    fOutputDir = KBRun::GetKEBIPath() + "/data/";

  gROOT -> SetBatch(kTRUE);
  fCvs = new TCanvas("cvs","",600,400);
  fHist = new TH1D("hist","",512,0,512);
  fHist -> SetLineColor(kRed);
  fHist -> SetLineWidth(2);
  fHist -> SetStats(0);
  fHist -> SetMinimum(0);
  fHist -> SetMaximum(4095);
  fHistRaw = new TH1D("histRaw","",512,0,512);
  fHistRaw -> SetStats(0);
  fHistRaw -> SetMinimum(0);
  fHistRaw -> SetMaximum(4095);

  gStyle -> SetTitleFontSize(0.1);

  return true;
}

void KBPlotChannelTask::Exec(Option_t*)
{
  Int_t eventID = KBRun::GetRun() -> GetCurrentEventID();
  Int_t countSave = 0;

  Int_t nPads = fPadArray -> GetEntries();
  for (auto iPad = 0; iPad < nPads; iPad++)
  {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    auto padID = pad -> GetPadID();

    if (fIsSetPadIDRange && (padID < fPadIDLow || padID > fPadIDHigh))
      continue;

    auto out = pad -> GetBufferOut();
    fHist -> Reset();
    for (auto i = 0; i < 512; i++)
      fHist -> SetBinContent(i+1, out[i]);

    TString name = Form("Event%04d_Pad%03d",eventID,padID);
    if (fNameType == 1)
      name = Form("Event%04d_ID%d%d%02d_Amp%.1f",eventID,pad->GetAsAdID(),pad->GetAGETID(),pad->GetChannelID(),pad->GetNoiseAmplitude());
    if (pad -> GetNoiseAmplitude() == 1)
      name = Form("Event%04d_ARef_ID%d%d%02d",eventID,pad->GetAsAdID(),pad->GetAGETID(),pad->GetChannelID());

    fHistRaw -> SetNameTitle(name,name);

    auto raw = pad -> GetBufferRaw();
    fHistRaw -> Reset();
    for (auto i = 0; i < 512; i++)
      fHistRaw -> SetBinContent(i+1, raw[i]);

    fCvs -> cd();
    fHistRaw -> Draw("");
    fHist -> Draw("same");

    fCvs -> SetName(name);
    fCvs -> SaveAs(fOutputDir+fCvs->GetName()+".png");
    countSave++;
  }

  cout << "  [" << this -> GetName() << "] " << countSave << " figures >> " << fOutputDir << endl;
}

void KBPlotChannelTask::SetOutputDirectory(TString dir) { fOutputDir = dir; }
void KBPlotChannelTask::SetPadIDRange(Int_t padIDLow, Int_t padIDHigh)
{
  fIsSetPadIDRange = true;
  fPadIDLow = padIDLow;
  fPadIDHigh = padIDHigh;
}

void KBPlotChannelTask::SetNameType(Int_t type) { fNameType = type; }
