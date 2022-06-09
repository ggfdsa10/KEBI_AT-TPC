#include "KBRun.hh"
#include "ATTPCDecoderTask.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"

ClassImp(ATTPCDecoderTask)

ATTPCDecoderTask::ATTPCDecoderTask()
:KBTask("ATTPCDecoderTask","")
{
    fDecoder = new GETDecoder();
} 

bool ATTPCDecoderTask::Init()
{
    KBRun *run = KBRun::GetRun();
    ATTPC *tpc = (ATTPC *) run -> GetDetector(0);
    fPadPlane = tpc -> GetPadPlane();

    if(fFileToKEBIForm){
        fPadArray = new TClonesArray("KBPad");
        run -> RegisterBranch("Pad", fPadArray, fPersistency);
    } 

    TString pathToRawData  = fPar -> GetParString("pathToRawData");
    TString pathToMetaData = "";

    if(fPar -> CheckPar("pathToMetaData")){
        auto pathToMetaData = fPar -> GetParString("pathToMetaData");
    }
    LoadData(pathToRawData, pathToMetaData);
    

    fNumAsAds = fPar -> GetParInt("numAsAds");
    fNumAGETs = fPar -> GetParInt("numAGETs");
    fNumChannelsMax = fPar -> GetParInt("numChannelsMax");

    if (fDecoder -> GetNumData() == 0){
        kb_error << "Input file is not set!" << endl;
        return false;
    }

    fDecoder -> SetData(0);

    if (fNumEvents == -1){
        fDecoder -> GoToEnd();
        fNumEvents = fDecoder -> GetNumFrames();
    }

    run -> SetEntries(fNumEvents);
    
    return true;
}

void ATTPCDecoderTask::Exec(Option_t*)
{
    if(fFileToKEBIForm){fPadArray -> Clear("C");}

    Long64_t currentEntry = KBRun::GetRun() -> GetCurrentEventID();
    Int_t countChannels = 0;

    GETBasicFrame *frame = fDecoder -> GetBasicFrame(currentEntry);

    fEventTime = (double)frame->GetEventTime()/ 1.E8;

    for (Int_t iAsAd = 0; iAsAd < fNumAsAds; iAsAd++) {
        Int_t AsAdID = frame -> GetAsadID();
        for (Int_t iAGET = 0; iAGET < fNumAGETs; iAGET++) {
            for (Int_t iChannel = 0; iChannel < fNumChannelsMax; iChannel++) {
                if (IsFPNChannel(iChannel)) continue;

                Int_t colIdx, rowIdx = 0;
                PadMapping(iAGET, iChannel, colIdx, rowIdx);
                Int_t padID = GetPadID(colIdx, rowIdx);

                Int_t *sample = frame -> GetSample(iAGET, iChannel);

                Short_t copy[512] = {0};
                Double_t copy2[512] = {0};
                for (Int_t iTb = 0; iTb < 512; iTb++) {
                    Short_t value = sample[iTb];
                    copy[iTb] = value;
                    copy2[iTb] = (Double_t) value;
                }

                if(fFileToKEBIForm){
                    KBPad *pad = fPadPlane -> GetPad(padID);
                    if (pad == nullptr)
                    continue;

                    auto padSave = (KBPad *) fPadArray -> ConstructedAt(countChannels);
                    padSave -> SetPad(pad);
                    padSave -> SetAsAdID(iAsAd);
                    padSave -> SetAGETID(iAGET);
                    padSave -> SetChannelID(iChannel);
                    padSave -> SetPadID(padID);
                    padSave -> SetBufferRaw(copy);
                    padSave -> SetBufferOut(copy2);
                    padSave -> SetSortValue(padID);
                    countChannels++;
                }
            }
        }
    }

    if(fFileToKEBIForm){fPadArray -> Sort();}
    kb_info << "found " << countChannels << " channels." << endl;
    return;
}

void ATTPCDecoderTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }
void ATTPCDecoderTask::SaveKEBIData(bool fileToKEBIForm) {fFileToKEBIForm = fileToKEBIForm;}

void ATTPCDecoderTask::LoadData(TString pathToRawData, TString pathToMetaData)
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
            if (fileName.Index("CoBo_") == 0)
                fileList.push_back(fileName);
                if(fileName.Length() > 10){
                    std::string filename = fileName.Data();
                    fYear = std::stoi(filename.substr(filename.length() - 34, 4));
                    fMonth = std::stoi(filename.substr(filename.length() - 29, 2));
                    fDay = std::stoi(filename.substr(filename.length() - 26, 2));
                    fHour = std::stoi(filename.substr(filename.length() - 23, 2));
                    fMinute = std::stoi(filename.substr(filename.length() - 20, 2));
                    fSecond = std::stof(filename.substr(filename.length() - 17, 6));
                }
            }
        closedir (dir);
    }
    if(fileList.size() == 0) {
        kb_error << "CoBo_.graw" << " in " << pathToRawData << endl;
        return;
    }

    sort(fileList.begin(), fileList.end(), less<TString>());

    for (auto fileName : fileList){
        cout << "file list! " << pathToRawData+fileName << endl;
        AddData(pathToRawData+fileName);
    }

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

void ATTPCDecoderTask::LoadMetaData(TString name)
{
  fDecoder -> SetData(0);
  fDecoder -> LoadMetaData(name);
  fNumEvents = fDecoder -> GetNumFrames();
}

void ATTPCDecoderTask::AddData(TString name) { fDecoder -> AddData(name); }
void ATTPCDecoderTask::SetNumEvents(Long64_t numEvents) { fNumEvents = numEvents; }

void ATTPCDecoderTask::GetDate()  {kb_info << " This event data : "<< Form("%i-%i-%i , %ih %im %.3fs.",fYear, fMonth, fDay, fHour, fMinute, fSecond) << endl;}

bool ATTPCDecoderTask::IsFPNChannel(Int_t chanIdx) {
    return (chanIdx == 11 || chanIdx == 22 || chanIdx == 45 || chanIdx == 56);
}

Int_t ATTPCDecoderTask::GetPadID(Int_t colIdx, Int_t rowIdx){return colIdx+(32*rowIdx);}

void ATTPCDecoderTask::PadMapping(Int_t &agetIdx, Int_t &chanIdx, Int_t &colIdx, Int_t &rowIdx) {
    if (agetIdx == 0) {
        switch (chanIdx) {
            case 0:
                colIdx = 16;
                rowIdx = 3;
                break;
            case 1:
                colIdx = 17;
                rowIdx = 3;
                break;
            case 2:
                colIdx = 18;
                rowIdx = 3;
                break;
            case 3:
                colIdx = 19;
                rowIdx = 3;
                break;
            case 4:
                colIdx = 20;
                rowIdx = 3;
                break;
            case 5:
                colIdx = 21;
                rowIdx = 3;
                break;
            case 6:
                colIdx = 22;
                rowIdx = 3;
                break;
            case 7:
                colIdx = 23;
                rowIdx = 3;
                break;
            case 8:
                colIdx = 24;
                rowIdx = 3;
                break;
            case 9:
                colIdx = 25;
                rowIdx = 3;
                break;
            case 10:
                colIdx = 26;
                rowIdx = 3;
                break;
            case 12:
                colIdx = 27;
                rowIdx = 3;
                break;
            case 13:
                colIdx = 28;
                rowIdx = 3;
                break;
            case 14:
                colIdx = 29;
                rowIdx = 3;
                break;
            case 15:
                colIdx = 30;
                rowIdx = 3;
                break;
            case 16:
                colIdx = 31;
                rowIdx = 3;
                break;
            case 17:
                colIdx = 16;
                rowIdx = 2;
                break;
            case 18:
                colIdx = 17;
                rowIdx = 2;
                break;
            case 19:
                colIdx = 18;
                rowIdx = 2;
                break;
            case 20:
                colIdx = 19;
                rowIdx = 2;
                break;
            case 21:
                colIdx = 20;
                rowIdx = 2;
                break;
            case 23:
                colIdx = 21;
                rowIdx = 2;
                break;
            case 24:
                colIdx = 22;
                rowIdx = 2;
                break;
            case 25:
                colIdx = 23;
                rowIdx = 2;
                break;
            case 26:
                colIdx = 24;
                rowIdx = 2;
                break;
            case 27:
                colIdx = 25;
                rowIdx = 2;
                break;
            case 28:
                colIdx = 26;
                rowIdx = 2;
                break;
            case 29:
                colIdx = 27;
                rowIdx = 2;
                break;
            case 30:
                colIdx = 28;
                rowIdx = 2;
                break;
            case 31:
                colIdx = 29;
                rowIdx = 2;
                break;
            case 32:
                colIdx = 30;
                rowIdx = 2;
                break;
            case 33:
                colIdx = 31;
                rowIdx = 2;
                break;
            case 34:
                colIdx = 16;
                rowIdx = 1;
                break;
            case 35:
                colIdx = 17;
                rowIdx = 1;
                break;
            case 36:
                colIdx = 18;
                rowIdx = 1;
                break;
            case 37:
                colIdx = 19;
                rowIdx = 1;
                break;
            case 38:
                colIdx = 20;
                rowIdx = 1;
                break;
            case 39:
                colIdx = 21;
                rowIdx = 1;
                break;
            case 40:
                colIdx = 22;
                rowIdx = 1;
                break;
            case 41:
                colIdx = 23;
                rowIdx = 1;
                break;
            case 42:
                colIdx = 24;
                rowIdx = 1;
                break;
            case 43:
                colIdx = 25;
                rowIdx = 1;
                break;
            case 44:
                colIdx = 26;
                rowIdx = 1;
                break;
            case 46:
                colIdx = 27;
                rowIdx = 1;
                break;
            case 47:
                colIdx = 28;
                rowIdx = 1;
                break;
            case 48:
                colIdx = 29;
                rowIdx = 1;
                break;
            case 49:
                colIdx = 30;
                rowIdx = 1;
                break;
            case 50:
                colIdx = 31;
                rowIdx = 1;
                break;
            case 51:
                colIdx = 16;
                rowIdx = 0;
                break;
            case 52:
                colIdx = 17;
                rowIdx = 0;
                break;
            case 53:
                colIdx = 18;
                rowIdx = 0;
                break;
            case 54:
                colIdx = 19;
                rowIdx = 0;
                break;
            case 55:
                colIdx = 20;
                rowIdx = 0;
                break;
            case 57:
                colIdx = 21;
                rowIdx = 0;
                break;
            case 58:
                colIdx = 22;
                rowIdx = 0;
                break;
            case 59:
                colIdx = 23;
                rowIdx = 0;
                break;
            case 60:
                colIdx = 24;
                rowIdx = 0;
                break;
            case 61:
                colIdx = 25;
                rowIdx = 0;
                break;
            case 62:
                colIdx = 26;
                rowIdx = 0;
                break;
            case 63:
                colIdx = 27;
                rowIdx = 0;
                break;
            case 64:
                colIdx = 28;
                rowIdx = 0;
                break;
            case 65:
                colIdx = 29;
                rowIdx = 0;
                break;
            case 66:
                colIdx = 30;
                rowIdx = 0;
                break;
            case 67:
                colIdx = 31;
                rowIdx = 0;
                break;

            default:
                colIdx = -99;
                rowIdx = -99;
                return;
        }
    } else if (agetIdx == 1) {
        switch (chanIdx) {
            case 0:
                colIdx = 16;
                rowIdx = 7;
                break;
            case 1:
                colIdx = 17;
                rowIdx = 7;
                break;
            case 2:
                colIdx = 18;
                rowIdx = 7;
                break;
            case 3:
                colIdx = 19;
                rowIdx = 7;
                break;
            case 4:
                colIdx = 20;
                rowIdx = 7;
                break;
            case 5:
                colIdx = 21;
                rowIdx = 7;
                break;
            case 6:
                colIdx = 22;
                rowIdx = 7;
                break;
            case 7:
                colIdx = 23;
                rowIdx = 7;
                break;
            case 8:
                colIdx = 24;
                rowIdx = 7;
                break;
            case 9:
                colIdx = 25;
                rowIdx = 7;
                break;
            case 10:
                colIdx = 26;
                rowIdx = 7;
                break;
            case 12:
                colIdx = 27;
                rowIdx = 7;
                break;
            case 13:
                colIdx = 28;
                rowIdx = 7;
                break;
            case 14:
                colIdx = 29;
                rowIdx = 7;
                break;
            case 15:
                colIdx = 30;
                rowIdx = 7;
                break;
            case 16:
                colIdx = 31;
                rowIdx = 7;
                break;
            case 17:
                colIdx = 16;
                rowIdx = 6;
                break;
            case 18:
                colIdx = 17;
                rowIdx = 6;
                break;
            case 19:
                colIdx = 18;
                rowIdx = 6;
                break;
            case 20:
                colIdx = 19;
                rowIdx = 6;
                break;
            case 21:
                colIdx = 20;
                rowIdx = 6;
                break;
            case 23:
                colIdx = 21;
                rowIdx = 6;
                break;
            case 24:
                colIdx = 22;
                rowIdx = 6;
                break;
            case 25:
                colIdx = 23;
                rowIdx = 6;
                break;
            case 26:
                colIdx = 24;
                rowIdx = 6;
                break;
            case 27:
                colIdx = 25;
                rowIdx = 6;
                break;
            case 28:
                colIdx = 26;
                rowIdx = 6;
                break;
            case 29:
                colIdx = 27;
                rowIdx = 6;
                break;
            case 30:
                colIdx = 28;
                rowIdx = 6;
                break;
            case 31:
                colIdx = 29;
                rowIdx = 6;
                break;
            case 32:
                colIdx = 30;
                rowIdx = 6;
                break;
            case 33:
                colIdx = 31;
                rowIdx = 6;
                break;
            case 34:
                colIdx = 16;
                rowIdx = 5;
                break;
            case 35:
                colIdx = 17;
                rowIdx = 5;
                break;
            case 36:
                colIdx = 18;
                rowIdx = 5;
                break;
            case 37:
                colIdx = 19;
                rowIdx = 5;
                break;
            case 38:
                colIdx = 20;
                rowIdx = 5;
                break;
            case 39:
                colIdx = 21;
                rowIdx = 5;
                break;
            case 40:
                colIdx = 22;
                rowIdx = 5;
                break;
            case 41:
                colIdx = 23;
                rowIdx = 5;
                break;
            case 42:
                colIdx = 24;
                rowIdx = 5;
                break;
            case 43:
                colIdx = 25;
                rowIdx = 5;
                break;
            case 44:
                colIdx = 26;
                rowIdx = 5;
                break;
            case 46:
                colIdx = 27;
                rowIdx = 5;
                break;
            case 47:
                colIdx = 28;
                rowIdx = 5;
                break;
            case 48:
                colIdx = 29;
                rowIdx = 5;
                break;
            case 49:
                colIdx = 30;
                rowIdx = 5;
                break;
            case 50:
                colIdx = 31;
                rowIdx = 5;
                break;
            case 51:
                colIdx = 16;
                rowIdx = 4;
                break;
            case 52:
                colIdx = 17;
                rowIdx = 4;
                break;
            case 53:
                colIdx = 18;
                rowIdx = 4;
                break;
            case 54:
                colIdx = 19;
                rowIdx = 4;
                break;
            case 55:
                colIdx = 20;
                rowIdx = 4;
                break;
            case 57:
                colIdx = 21;
                rowIdx = 4;
                break;
            case 58:
                colIdx = 22;
                rowIdx = 4;
                break;
            case 59:
                colIdx = 23;
                rowIdx = 4;
                break;
            case 60:
                colIdx = 24;
                rowIdx = 4;
                break;
            case 61:
                colIdx = 25;
                rowIdx = 4;
                break;
            case 62:
                colIdx = 26;
                rowIdx = 4;
                break;
            case 63:
                colIdx = 27;
                rowIdx = 4;
                break;
            case 64:
                colIdx = 28;
                rowIdx = 4;
                break;
            case 65:
                colIdx = 29;
                rowIdx = 4;
                break;
            case 66:
                colIdx = 30;
                rowIdx = 4;
                break;
            case 67:
                colIdx = 31;
                rowIdx = 4;
                break;

            default:
                colIdx = -99;
                rowIdx = -99;
                return;
        }
    } else if (agetIdx == 2) {
        switch (chanIdx) {
            case 0:
                colIdx = 8;
                rowIdx = 7;
                break;
            case 1:
                colIdx = 8;
                rowIdx = 6;
                break;
            case 2:
                colIdx = 8;
                rowIdx = 5;
                break;
            case 3:
                colIdx = 8;
                rowIdx = 4;
                break;
            case 4:
                colIdx = 8;
                rowIdx = 3;
                break;
            case 5:
                colIdx = 8;
                rowIdx = 2;
                break;
            case 6:
                colIdx = 8;
                rowIdx = 1;
                break;
            case 7:
                colIdx = 8;
                rowIdx = 0;
                break;
            case 8:
                colIdx = 9;
                rowIdx = 7;
                break;
            case 9:
                colIdx = 9;
                rowIdx = 6;
                break;
            case 10:
                colIdx = 9;
                rowIdx = 5;
                break;
            case 12:
                colIdx = 9;
                rowIdx = 4;
                break;
            case 13:
                colIdx = 9;
                rowIdx = 3;
                break;
            case 14:
                colIdx = 9;
                rowIdx = 2;
                break;
            case 15:
                colIdx = 9;
                rowIdx = 1;
                break;
            case 16:
                colIdx = 9;
                rowIdx = 0;
                break;
            case 17:
                colIdx = 10;
                rowIdx = 7;
                break;
            case 18:
                colIdx = 10;
                rowIdx = 6;
                break;
            case 19:
                colIdx = 10;
                rowIdx = 5;
                break;
            case 20:
                colIdx = 10;
                rowIdx = 4;
                break;
            case 21:
                colIdx = 10;
                rowIdx = 3;
                break;
            case 23:
                colIdx = 10;
                rowIdx = 2;
                break;
            case 24:
                colIdx = 10;
                rowIdx = 1;
                break;
            case 25:
                colIdx = 10;
                rowIdx = 0;
                break;
            case 26:
                colIdx = 11;
                rowIdx = 7;
                break;
            case 27:
                colIdx = 11;
                rowIdx = 6;
                break;
            case 28:
                colIdx = 11;
                rowIdx = 5;
                break;
            case 29:
                colIdx = 11;
                rowIdx = 4;
                break;
            case 30:
                colIdx = 11;
                rowIdx = 3;
                break;
            case 31:
                colIdx = 11;
                rowIdx = 2;
                break;
            case 32:
                colIdx = 11;
                rowIdx = 1;
                break;
            case 33:
                colIdx = 11;
                rowIdx = 0;
                break;
            case 34:
                colIdx = 12;
                rowIdx = 7;
                break;
            case 35:
                colIdx = 12;
                rowIdx = 6;
                break;
            case 36:
                colIdx = 12;
                rowIdx = 5;
                break;
            case 37:
                colIdx = 12;
                rowIdx = 4;
                break;
            case 38:
                colIdx = 12;
                rowIdx = 3;
                break;
            case 39:
                colIdx = 12;
                rowIdx = 2;
                break;
            case 40:
                colIdx = 12;
                rowIdx = 1;
                break;
            case 41:
                colIdx = 12;
                rowIdx = 0;
                break;
            case 42:
                colIdx = 13;
                rowIdx = 7;
                break;
            case 43:
                colIdx = 13;
                rowIdx = 6;
                break;
            case 44:
                colIdx = 13;
                rowIdx = 5;
                break;
            case 46:
                colIdx = 13;
                rowIdx = 4;
                break;
            case 47:
                colIdx = 13;
                rowIdx = 3;
                break;
            case 48:
                colIdx = 13;
                rowIdx = 2;
                break;
            case 49:
                colIdx = 13;
                rowIdx = 1;
                break;
            case 50:
                colIdx = 13;
                rowIdx = 0;
                break;
            case 51:
                colIdx = 14;
                rowIdx = 7;
                break;
            case 52:
                colIdx = 14;
                rowIdx = 6;
                break;
            case 53:
                colIdx = 14;
                rowIdx = 5;
                break;
            case 54:
                colIdx = 14;
                rowIdx = 4;
                break;
            case 55:
                colIdx = 14;
                rowIdx = 3;
                break;
            case 57:
                colIdx = 14;
                rowIdx = 2;
                break;
            case 58:
                colIdx = 14;
                rowIdx = 1;
                break;
            case 59:
                colIdx = 14;
                rowIdx = 0;
                break;
            case 60:
                colIdx = 15;
                rowIdx = 7;
                break;
            case 61:
                colIdx = 15;
                rowIdx = 6;
                break;
            case 62:
                colIdx = 15;
                rowIdx = 5;
                break;
            case 63:
                colIdx = 15;
                rowIdx = 4;
                break;
            case 64:
                colIdx = 15;
                rowIdx = 3;
                break;
            case 65:
                colIdx = 15;
                rowIdx = 2;
                break;
            case 66:
                colIdx = 15;
                rowIdx = 1;
                break;
            case 67:
                colIdx = 15;
                rowIdx = 0;
                break;

            default:
                colIdx = -99;
                rowIdx = -99;
                return;
        }
    } else if (agetIdx == 3) {
        switch (chanIdx) {
            case 0:
                colIdx = 0;
                rowIdx = 7;
                break;
            case 1:
                colIdx = 0;
                rowIdx = 6;
                break;
            case 2:
                colIdx = 0;
                rowIdx = 5;
                break;
            case 3:
                colIdx = 0;
                rowIdx = 4;
                break;
            case 4:
                colIdx = 0;
                rowIdx = 3;
                break;
            case 5:
                colIdx = 0;
                rowIdx = 2;
                break;
            case 6:
                colIdx = 0;
                rowIdx = 1;
                break;
            case 7:
                colIdx = 0;
                rowIdx = 0;
                break;
            case 8:
                colIdx = 1;
                rowIdx = 7;
                break;
            case 9:
                colIdx = 1;
                rowIdx = 6;
                break;
            case 10:
                colIdx = 1;
                rowIdx = 5;
                break;
            case 12:
                colIdx = 1;
                rowIdx = 4;
                break;
            case 13:
                colIdx = 1;
                rowIdx = 3;
                break;
            case 14:
                colIdx = 1;
                rowIdx = 2;
                break;
            case 15:
                colIdx = 1;
                rowIdx = 1;
                break;
            case 16:
                colIdx = 1;
                rowIdx = 0;
                break;
            case 17:
                colIdx = 2;
                rowIdx = 7;
                break;
            case 18:
                colIdx = 2;
                rowIdx = 6;
                break;
            case 19:
                colIdx = 2;
                rowIdx = 5;
                break;
            case 20:
                colIdx = 2;
                rowIdx = 4;
                break;
            case 21:
                colIdx = 2;
                rowIdx = 3;
                break;
            case 23:
                colIdx = 2;
                rowIdx = 2;
                break;
            case 24:
                colIdx = 2;
                rowIdx = 1;
                break;
            case 25:
                colIdx = 2;
                rowIdx = 0;
                break;
            case 26:
                colIdx = 3;
                rowIdx = 7;
                break;
            case 27:
                colIdx = 3;
                rowIdx = 6;
                break;
            case 28:
                colIdx = 3;
                rowIdx = 5;
                break;
            case 29:
                colIdx = 3;
                rowIdx = 4;
                break;
            case 30:
                colIdx = 3;
                rowIdx = 3;
                break;
            case 31:
                colIdx = 3;
                rowIdx = 2;
                break;
            case 32:
                colIdx = 3;
                rowIdx = 1;
                break;
            case 33:
                colIdx = 3;
                rowIdx = 0;
                break;
            case 34:
                colIdx = 4;
                rowIdx = 7;
                break;
            case 35:
                colIdx = 4;
                rowIdx = 6;
                break;
            case 36:
                colIdx = 4;
                rowIdx = 5;
                break;
            case 37:
                colIdx = 4;
                rowIdx = 4;
                break;
            case 38:
                colIdx = 4;
                rowIdx = 3;
                break;
            case 39:
                colIdx = 4;
                rowIdx = 2;
                break;
            case 40:
                colIdx = 4;
                rowIdx = 1;
                break;
            case 41:
                colIdx = 4;
                rowIdx = 0;
                break;
            case 42:
                colIdx = 5;
                rowIdx = 7;
                break;
            case 43:
                colIdx = 5;
                rowIdx = 6;
                break;
            case 44:
                colIdx = 5;
                rowIdx = 5;
                break;
            case 46:
                colIdx = 5;
                rowIdx = 4;
                break;
            case 47:
                colIdx = 5;
                rowIdx = 3;
                break;
            case 48:
                colIdx = 5;
                rowIdx = 2;
                break;
            case 49:
                colIdx = 5;
                rowIdx = 1;
                break;
            case 50:
                colIdx = 5;
                rowIdx = 0;
                break;
            case 51:
                colIdx = 6;
                rowIdx = 7;
                break;
            case 52:
                colIdx = 6;
                rowIdx = 6;
                break;
            case 53:
                colIdx = 6;
                rowIdx = 5;
                break;
            case 54:
                colIdx = 6;
                rowIdx = 4;
                break;
            case 55:
                colIdx = 6;
                rowIdx = 3;
                break;
            case 57:
                colIdx = 6;
                rowIdx = 2;
                break;
            case 58:
                colIdx = 6;
                rowIdx = 1;
                break;
            case 59:
                colIdx = 6;
                rowIdx = 0;
                break;
            case 60:
                colIdx = 7;
                rowIdx = 7;
                break;
            case 61:
                colIdx = 7;
                rowIdx = 6;
                break;
            case 62:
                colIdx = 7;
                rowIdx = 5;
                break;
            case 63:
                colIdx = 7;
                rowIdx = 4;
                break;
            case 64:
                colIdx = 7;
                rowIdx = 3;
                break;
            case 65:
                colIdx = 7;
                rowIdx = 2;
                break;
            case 66:
                colIdx = 7;
                rowIdx = 1;
                break;
            case 67:
                colIdx = 7;
                rowIdx = 0;
                break;

            default:
                colIdx = -99;
                rowIdx = -99;
                return;
        }
    }
}