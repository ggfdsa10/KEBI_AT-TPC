#include "KBRun.hh"
#include "LAPDecoderFromRootTask.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"

ClassImp(LAPDecoderFromRootTask)

LAPDecoderFromRootTask::LAPDecoderFromRootTask()
:KBTask("LAPDecoderFromRootTask","")
{
} 

bool LAPDecoderFromRootTask::Init()
{
  KBRun *run = KBRun::GetRun();

  KBTpc *tpc = (KBTpc *) run -> GetDetector(0);
  fPadPlane = tpc -> GetPadPlane();

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  fNumAsAds = fPar -> GetParInt("numAsAds");
  fNumAGETs = fPar -> GetParInt("numAGETs");
  fNumChannelsMax = fPar -> GetParInt("numChannelsMax");

  TString padMapFileName = fPar -> GetParString("padMap");
  kb_info << "pad map: " << padMapFileName.Data() << endl;
  fPadMap.open(padMapFileName.Data());


  if (fInputFileName.IsNull()) {
    if (fPar -> CheckPar("pathToData")) {
      auto pathToData = fPar -> GetParString("pathToData");
      SetDataPath(pathToData);
    }
    else {
      kb_error << "Input file is Empty! Set parameter, pathToData or use SetDataPath()" << endl;
      return false;
    }
  }

  auto file = new TFile(fInputFileName);
  if (file->IsZombie())
    return false;
  fInputTree = (TTree *) file -> Get("TPCTree");

  for (Int_t iAsAd = 0; iAsAd < fNumAsAds; iAsAd++) {
    fChannelArray[iAsAd] = nullptr;
    fInputTree -> SetBranchAddress(Form("Arr_%d",iAsAd), &fChannelArray[iAsAd]);
  }

  fNumEvents = run -> GetNumEvents(); 
  Long64_t numEventsInTree = fInputTree -> GetEntries();
  if (fNumEvents > numEventsInTree) {
    kb_warning << "Number of event is changed from " << fNumEvents << " to " << numEventsInTree << endl;
    fNumEvents = numEventsInTree;
  }
  else if (fNumEvents<=0) {
    fNumEvents = numEventsInTree;
    kb_warning << "Number of event is set to " << fNumEvents << endl;
  }
  run -> SetEntries(fNumEvents);
  
  return true;
}

void LAPDecoderFromRootTask::Exec(Option_t*)
{
  fPadArray -> Clear("C");

  fInputTree -> GetEntry(KBRun::GetRun()->GetCurrentEventID());

  int countChannels = 0;
  for (Int_t iAsAd = 0; iAsAd < fNumAsAds; iAsAd++)
  {
    auto numChannels = fChannelArray[iAsAd] -> GetEntries();

    for (Int_t iChannel=0; iChannel<numChannels; ++iChannel)
    {
      auto channel = (TPCData *) fChannelArray[iAsAd] -> At(iChannel);
      auto CoboID = channel -> COBOIdx;
      auto AsAdID = channel -> ASADIdx;
      auto iAGET  = channel -> AGETIdx;
      auto channelID = channel -> CHANIdx;

      bool skip = false;
      for (auto selAAID : fSkipAAID)
        if (1000*AsAdID+100*iAGET == selAAID)
          skip = true;
      if (skip)
        continue;

      fPadMap.clear();
      fPadMap.seekg(0, ios::beg);
      bool foundPad = false;
      int channelID0, asad, aget, padID;
      while (fPadMap >> asad >> aget >> channelID0 >> padID) {
        if (asad == AsAdID && aget == iAGET && channelID0 == channelID) {
          foundPad = true;
          break;
        }
      }
      if (!foundPad)
        continue;

      KBPad *pad = fPadPlane -> GetPad(padID);
      if (pad == nullptr)
        continue;

      Short_t copy[512] = {0};
      Double_t copy2[512] = {0};
      for (Int_t iTb = 0; iTb < 512; iTb++) {
        Short_t value = channel -> ADC[iTb];
        copy[iTb] = value;
        copy2[iTb] = (Double_t) value;
      }

      auto padSave = (KBPad *) fPadArray -> ConstructedAt(countChannels);
      padSave -> SetPad(pad);
      padSave -> SetBufferRaw(copy);
      padSave -> SetBufferOut(copy2);
      padSave -> SetSortValue(padID);

      countChannels++;
    }
  }

  fPadArray -> Sort();

  kb_info << "found " << countChannels << " channels." << endl;
  
  return;
}

void LAPDecoderFromRootTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }

void LAPDecoderFromRootTask::SetDataPath(TString pathToRawData)
{
  if (!pathToRawData.EndsWith("/"))
    pathToRawData = pathToRawData + "/";

  fInputFileName = pathToRawData + Form("run_%04d.root",KBRun::GetRun() -> GetRunID());
  cout << "Input file is set : " << fInputFileName << endl;
}
