#include "KBRun.hh"
#include "LAPDecoderTask.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"

ClassImp(LAPDecoderTask)

LAPDecoderTask::LAPDecoderTask()
:KBTask("LAPDecoderTask","")
{
  fDecoder = new GETDecoder();
} 

bool LAPDecoderTask::Init()
{
  KBRun *run = KBRun::GetRun();

  KBTpc *tpc = (KBTpc *) run -> GetDetector(0);
  fPadPlane = tpc -> GetPadPlane();

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  auto pathToRawData  = fPar -> GetParString("pathToRawData");
  auto pathToMetaData = fPar -> GetParString("pathToMetaData");
  LoadData(pathToRawData, pathToMetaData);

  if (fPar -> CheckPar("expRunSummary"))
  {
    Int_t runID = run -> GetRunID();
    Int_t runID0, numEventsNalval=0, numEventsVME=0;
    std::ifstream summary(fPar -> GetParString("expRunSummary"));
    string line;
    std::getline(summary,line);
    std::getline(summary,line);
    std::getline(summary,line);
    while (1) {
      std::getline(summary,line);
      if (line.empty())
        break;
      istringstream ss(line);
      ss >> runID0;
      if (runID0==runID) {
        ss >> numEventsNalval >> numEventsVME;
        break;
      }
    }
    Int_t numEventsInRun = (numEventsNalval<numEventsVME) ? numEventsNalval : numEventsVME;
    SetNumEvents(numEventsInRun);
  }

  if (fPar -> CheckPar("skipAAID")) {
    auto nAsAds = fPar -> GetParN("skipAAID");
    for (auto i=0; i<nAsAds; ++i)
      fSkipAAID.push_back(fPar->GetParInt("skipAAID",i));
  }
  else {
    fSkipAAID.push_back(200);
  }

  fNumAsAds = fPar -> GetParInt("numAsAds");
  fNumAGETs = fPar -> GetParInt("numAGETs");
  fNumChannelsMax = fPar -> GetParInt("numChannelsMax");

  TString padMapFileName = fPar -> GetParString("padMap");
  kb_info << "pad map: " << padMapFileName.Data() << endl;
  fPadMap.open(padMapFileName.Data());

  if (fDecoder -> GetNumData() == 0) {
    kb_error << "Input file is not set!" << endl;
    return false;
  }

  fDecoder -> SetData(0);

  if (fNumEvents == -1) {
    fDecoder -> GoToEnd();
    fNumEvents = fDecoder -> GetNumFrames();
  }

  run -> SetEntries(fNumEvents);
  
  return true;
}

void LAPDecoderTask::Exec(Option_t*)
{
  fPadArray -> Clear("C");

  Long64_t currentEntry = KBRun::GetRun() -> GetCurrentEventID();
  Int_t countChannels = 0;

  GETCoboFrame *cobo = fDecoder -> GetCoboFrame(currentEntry);

  for (Int_t iAsAd = 0; iAsAd < fNumAsAds; iAsAd++) {
    GETBasicFrame *frame = cobo -> GetFrame(iAsAd);
    Int_t AsAdID = frame -> GetAsadID();
    for (Int_t iAGET = 0; iAGET < fNumAGETs; iAGET++) {
      for (Int_t iChannel = 0; iChannel < fNumChannelsMax; iChannel++) {
        bool skip = false;
        for (auto selAAID : fSkipAAID)
          if (1000*AsAdID+100*iAGET == selAAID)
            skip = true;
        if (skip)
          continue;

        Int_t asad, aget, channel, padID;
        bool foundPad = false;
        fPadMap.clear();
        fPadMap.seekg(0, ios::beg);
        while (fPadMap >> asad >> aget >> channel >> padID) {
          if (asad == AsAdID && aget == iAGET && channel == iChannel) {
            foundPad = true;
            break;
          }
        }
        if (!foundPad)
          continue;

        Int_t *sample = frame -> GetSample(iAGET, iChannel);

        Short_t copy[512] = {0};
        Double_t copy2[512] = {0};
        for (Int_t iTb = 0; iTb < 512; iTb++) {
          Short_t value = sample[iTb];
          copy[iTb] = value;
          copy2[iTb] = (Double_t) value;
        }

        KBPad *pad = fPadPlane -> GetPad(padID);
        if (pad == nullptr)
          continue;

        auto padSave = (KBPad *) fPadArray -> ConstructedAt(countChannels);
        padSave -> SetPad(pad);
        padSave -> SetBufferRaw(copy);
        padSave -> SetBufferOut(copy2);
        padSave -> SetSortValue(padID);
        countChannels++;
      }
    }
  }

  fPadArray -> Sort();

  kb_info << "found " << countChannels << " channels." << endl;
  
  return;
}

void LAPDecoderTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }

void LAPDecoderTask::LoadData(TString pathToRawData, TString pathToMetaData)
{
  if (pathToRawData[pathToRawData.Sizeof()-2] != '/')
    pathToRawData = pathToRawData + "/";

  Int_t runID = KBRun::GetRun() -> GetRunID();

  vector<TString> fileList;

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(pathToRawData.Data())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      TString fileName = ent -> d_name;
      if (fileName.Index(Form("run_%04d.",runID)) == 0)
        fileList.push_back(fileName);
    }
    closedir (dir);
  }
  if(fileList.size() == 0) {
    kb_error << Form("No matching run_%04d",runID) << " in " << pathToRawData << endl;
    return;
  }

  sort(fileList.begin(), fileList.end(), less<TString>());

  for (auto fileName : fileList)
    AddData(pathToRawData+fileName);

  if (pathToMetaData.IsNull())
    pathToMetaData = pathToRawData + Form("run_%04d/metadata/",runID);
  else
    pathToMetaData = pathToMetaData + Form("run_%04d/metadata/",runID);
  if ((dir = opendir (pathToMetaData.Data())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      TString fileName = ent -> d_name;
      if (fileName.Index(Form("run_%04d",runID)) >= 0 && fileName.Index(".root") >= 0) {
        kb_info << "Meta data : " << pathToMetaData+fileName << endl;
        LoadMetaData(pathToMetaData+fileName);
        break;
      }
    }
    closedir (dir);
  }
}

void LAPDecoderTask::LoadMetaData(TString name)
{
  fDecoder -> SetData(0);
  fDecoder -> LoadMetaData(name);
  fNumEvents = fDecoder -> GetNumFrames();
}

void LAPDecoderTask::AddData(TString name) { fDecoder -> AddData(name); }
void LAPDecoderTask::SetNumEvents(Long64_t numEvents) { fNumEvents = numEvents; }
