
#include "ATTPCDecoderTask.hh"

#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"


ClassImp(ATTPCDecoderTask)

ATTPCDecoderTask::ATTPCDecoderTask()
:KBTask("ATTPCDecoderTask","")
{
    fDecoder = new GETDecoder();
    NewPadIDMapping();
} 

bool ATTPCDecoderTask::Init()
{
    KBRun *run = KBRun::GetRun();
    ATTPC *tpc = (ATTPC *) run -> GetDetector(0);
    fPadPlane = tpc -> GetPadPlane();
    if(fFileToKEBIForm){
        fPadArray = new TClonesArray("KBPad");
        fPadFPNArray = new TClonesArray("KBPad");

        run -> RegisterBranch("Pad", fPadArray, fPersistency);
        run -> RegisterBranch("FPNPad", fPadFPNArray, fPersistency);
    } 

    TString pathToRawData  = fPar -> GetParString("pathToRawData");
    TString pathToMetaData = "";

    if(fPar -> CheckPar("pathToMetaData")){
        auto pathToMetaData = fPar -> GetParString("pathToMetaData");
    }
    LoadData(pathToRawData, pathToMetaData);
    

    fNumAsAds = fPar -> GetParInt("numAsAds");
    fNumAGETs = fPar -> GetParInt("numAGETs");
    fNumChannels = fPar -> GetParInt("numChannelsMax");
    
    fPar -> SetPar("isExpData", true);

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

    if (fIsOnline){
        fDecoder -> GoToEnd();
        fNumEvents = fDecoder -> GetNumFrames();
        run -> SetEntries(1);
    }

    return true;
}

void ATTPCDecoderTask::Exec(Option_t*)
{
    if(fFileToKEBIForm){fPadArray -> Clear("C");}

    Long64_t currentEntry = KBRun::GetRun() -> GetCurrentEventID();
    Int_t countChannels = 0;
    Int_t idFPNPad = 0;

    GETBasicFrame *frame = fDecoder -> GetBasicFrame(currentEntry);

    if(currentEntry==0 || fEventIdx==0){
        fEventTime = 0;
        fEventDiffTime = 0;
    }

    if(fIsOnline){
        frame = fDecoder -> GetBasicFrame(fEventIdx);
        if(fEventIdx!=0){
            GETBasicFrame *prevFrame = fDecoder -> GetBasicFrame(fEventIdx-1);
            fEventTime = prevFrame->GetEventTime();
        }
    }

    fEventDiffTime = frame->GetEventTime() - fEventTime;
    fEventTime = frame->GetEventTime();

    RunEventChecker(currentEntry, frame);
    if(IsFakeEvent() || IsSparkEvent()){
        
        kb_info << "this event is skip! " << "(IsFakeEvent: " << IsFakeEvent() << " / IsSparkEvent: "<< IsSparkEvent()<< ")"<< endl;
        return;
    }


    for (Int_t iAsAd = 0; iAsAd < fNumAsAds; iAsAd++) {
        Int_t AsAdID = frame -> GetAsadID();
        for (Int_t iAGET = 0; iAGET < fNumAGETs; iAGET++) {
            for (Int_t iChannel = 0; iChannel < fNumChannels; iChannel++) {
                Int_t *sample = frame -> GetSample(iAGET, iChannel);

                Short_t copy[512] = {0};
                Double_t copy2[512] = {0};
                for (Int_t iTb = 0; iTb < 512; iTb++) {
                    Short_t value = sample[iTb];
                    copy[iTb] = value;
                    copy2[iTb] = (Double_t) value;

                    if(IsDeadChannel(iAGET, iChannel)){
                        copy[iTb] = 0.;
                        copy2[iTb] = 0.;
                    }
                }

                if(IsFPNChannel(iChannel)){
                    KBPad *padFPN = fPadPlane -> GetPad(idFPNPad);
                    if (padFPN == nullptr)
                    continue;
                    auto FPNpadSave = (KBPad *) fPadFPNArray -> ConstructedAt(idFPNPad);
                    FPNpadSave -> SetPad(padFPN);
                    FPNpadSave -> SetAsAdID(iAsAd);
                    FPNpadSave -> SetAGETID(iAGET);
                    FPNpadSave -> SetChannelID(iChannel);
                    FPNpadSave -> SetPadID(idFPNPad);
                    FPNpadSave -> SetBufferRaw(copy);
                    FPNpadSave -> SetBufferOut(copy2);
                    FPNpadSave -> SetSortValue(idFPNPad);
                    idFPNPad++;
                }

                else{
                    Int_t padID = GetPadID(iAsAd, iAGET, iChannel).first;
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


    fPadFPNArray -> Sort();
    fPadArray -> Sort();
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

void ATTPCDecoderTask::ExcuteOnline(Int_t eventIdx){
    fEventIdx = eventIdx;
    fIsOnline = true;
}

void ATTPCDecoderTask::RunEventChecker(Long64_t currentEvent, GETBasicFrame *frame) 
{
    if (currentEvent == 0) {
        fIsFakeEvent = false;
        fIsSparkEvent= false;
    }

    // *** Check whether it's fake or not. ***
    // if 10 ms < diffTime < 30 ms, this event is a fake trigger event.
    fIsFakeEvent = false;
    if (1E6 < GetEventDiffTime() && GetEventDiffTime() < 3E6) {
        fIsFakeEvent = true;
        fNumFakeEvent++;
    }
    if (currentEvent == 0) fIsFakeEvent = false;
    
    // *** Check whether it's spark or not. ***
    // if At least one thing is mimimum ADC < 100, this event is a spark event.
    fIsSparkEvent = false;
    for(int asadIdx = 0; asadIdx < fNumAsAds; asadIdx++){
        for(int agetIdx = 0; agetIdx < fNumAGETs; agetIdx++){
            for(int chanIdx = 0; chanIdx < fNumChannels; chanIdx++){
                if(IsFPNChannel(chanIdx) || IsDeadChannel(agetIdx, chanIdx)) continue;
                Int_t *sample = frame->GetSample(agetIdx, chanIdx);
                for(int tb = 10; tb<500; tb++){
                    if(sample[tb] < 100){
                        fIsSparkEvent = true;
                        fNumSparkEvent++;
                        return;
                    }
                }
            }
        }
    }
}

void ATTPCDecoderTask::GetDate(){kb_info << " This event date : "<< Form("%i-%i-%i , %ih %im %.3fs.",fYear, fMonth, fDay, fHour, fMinute, fSecond) << endl;}

pair<Int_t, Int_t> ATTPCDecoderTask::GetPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanIdx)
{
    vector<Int_t> key;
    key.push_back(asadIdx);
    key.push_back(agetIdx);
    key.push_back(chanIdx);

    Int_t PadID = get<0>(fPadIdxArray[key]) + (32 * get<1>(fPadIdxArray[key]));
    pair<Int_t, Int_t> value = make_pair(PadID, get<2>(fPadIdxArray[key]));
    return value;
}

Int_t ATTPCDecoderTask::GetFPNPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanFPNIdx){
    vector<Int_t> key;
    key.push_back(asadIdx);
    key.push_back(agetIdx);
    key.push_back(fFPNChanArray[chanFPNIdx]);
    return fPadFPNIdxArray[key];
}

bool ATTPCDecoderTask::IsFPNChannel(Int_t chanIdx) {return (chanIdx == fFPNChanArray[0] || chanIdx == fFPNChanArray[1] || chanIdx == fFPNChanArray[2] || chanIdx == fFPNChanArray[3]) ? true : false;}

bool ATTPCDecoderTask::IsEvenChannel(Int_t agetIdx, Int_t chanIdx) {
    if (IsFPNChannel(chanIdx) || chanIdx < 0 || chanIdx > 67 || agetIdx < 0 || agetIdx > 3) {
        kb_error << "Out of range of agetId or chanId!" << std::endl;
        exit(-1);
    }
    if (chanIdx < 11 || (22 < chanIdx && chanIdx < 45) || 56 < chanIdx) {
        return (agetIdx == 0) ? (chanIdx % 2 == 0) : (chanIdx % 2 == 1);
    } else {
        return (agetIdx == 0) ? (chanIdx % 2 == 1) : (chanIdx % 2 == 0);
    }
}

bool ATTPCDecoderTask::IsDeadChannel(Int_t agetIdx, Int_t chanIdx) {
    return false;
    // if (chanIdx < 0 || chanIdx > 67 || agetIdx < 0 || agetIdx > 3) {
    //     kb_error << "Out of range of agetId or chanId!" << std::endl;
    //     exit(-1);
    // }
    // if (IsFPNChannel(chanIdx)) return false;
    // // 52 (15 evens + 37 odds) channels is dead.
    // if (agetIdx == 0) {
    //     switch (chanIdx) {  // AGET 0 even channels : 5 channels is dead.
    //         case 17:
    //             return true;
    //         case 32:
    //             return true;
    //         case 55:
    //             return true;
    //         case 58:
    //             return true;
    //         case 60:
    //             return true;
    //         default:
    //             return false;
    //     }
    // } else if (agetIdx == 1) {
    //     if (IsEvenChannel(agetIdx, chanIdx)) {
    //         switch (chanIdx) {  // AGET 1 even channels : 9 channels is dead.
    //             case 3:
    //                 return true;
    //             case 5:
    //                 return true;
    //             case 7:
    //                 return true;
    //             case 9:
    //                 return true;
    //             case 12:
    //                 return true;
    //             case 14:
    //                 return true;
    //             case 16:
    //                 return true;
    //             case 18:
    //                 return true;
    //             case 23:
    //                 return true;
    //             default:
    //                 return false;
    //         }
    //     } else {
    //         switch (chanIdx) {  // AGET 1 odd channels : 22 channels is dead.
    //             case 0:
    //                 return true;
    //             case 2:
    //                 return true;
    //             case 8:
    //                 return true;
    //             case 13:
    //                 return true;
    //             case 15:
    //                 return true;
    //             case 17:
    //                 return true;
    //             case 21:
    //                 return true;
    //             case 24:
    //                 return true;
    //             case 26:
    //                 return true;
    //             case 28:
    //                 return true;
    //             case 30:
    //                 return true;
    //             case 32:
    //                 return true;
    //             case 34:
    //                 return true;
    //             case 36:
    //                 return true;
    //             case 38:
    //                 return true;
    //             case 40:
    //                 return true;
    //             case 44:
    //                 return true;
    //             case 47:
    //                 return true;
    //             case 49:
    //                 return true;
    //             case 51:
    //                 return true;
    //             case 55:
    //                 return true;
    //             case 58:
    //                 return true;
    //             default:
    //                 return false;
    //         }
    //     }
    // } else if (agetIdx == 2) {  // AGET 2 : all channels are alive.
    //     return false;
    // } else {
    //     if (IsEvenChannel(agetIdx, chanIdx)) {  // AGET 3 even channels : only chanId 39 is dead in even channel.
    //         switch (chanIdx) {
    //             case 39:
    //                 return true;
    //             case 61:
    //                 return true;
    //             case 65:
    //                 return true;
    //             default:
    //                 return false;
    //         }
    //     } else {
    //         switch (chanIdx) {  // AGET 3 odd channels : 15 channels is dead.
    //             case 0:
    //                 return true;
    //             case 2:
    //                 return true;
    //             case 6:
    //                 return true;
    //             case 8:
    //                 return true;
    //             case 10:
    //                 return true;
    //             case 13:
    //                 return true;
    //             case 15:
    //                 return true;
    //             case 17:
    //                 return true;
    //             case 21:
    //                 return true;
    //             case 24:
    //                 return true;
    //             case 26:
    //                 return true;
    //             case 28:
    //                 return true;
    //             case 34:
    //                 return true;
    //             case 38:
    //                 return true;
    //             case 40:
    //                 return true;
    //             case 42:
    //                 return true;
    //             default:
    //                 return false;
    //         }
    //     }
    // }
}

void ATTPCDecoderTask::PadIDMapping() 
{
    fPadIdxArray.clear();
    fPadFPNIdxArray.clear();

    Int_t xIdx, yIdx, idFPNPad = 0;
    for(int asadIdx=0; asadIdx<fNumAsAds; asadIdx++){
        for(int agetIdx = 0; agetIdx < fNumAGETs; agetIdx++){
            switch (agetIdx) {
                case 0:
                    xIdx = 16;
                    yIdx = 3;
                    break;
                case 1:
                    xIdx = 16;
                    yIdx = 7;
                    break;
                case 2:
                    xIdx = 8;
                    yIdx = 7;
                    break;
                case 3:
                    xIdx = 0;
                    yIdx = 7;
                    break;
            }

            Int_t nChanId = 0;
            for(int chanIdx = 0; chanIdx < fNumChannels; chanIdx++){
                vector<Int_t> key;
                key.push_back(asadIdx);
                key.push_back(agetIdx);
                key.push_back(chanIdx);

                if (IsFPNChannel(chanIdx)) {
                    fPadIdxArray.insert(make_pair(key, make_tuple(-1, -1, -1)));
                    fPadFPNIdxArray.insert(make_pair(key, idFPNPad));
                    idFPNPad++;
                    continue;
                }

                nChanId++;
                Int_t idxFPN = (nChanId - 1) / 16;
                if (agetIdx == 0 || agetIdx == 1) {
                    fPadIdxArray.insert(make_pair(key, make_tuple(xIdx++, yIdx, idxFPN)));
                    if (xIdx == 32) {
                        xIdx = 16;
                        yIdx--;
                    }
                } 
                else if (agetIdx == 2 || agetIdx == 3) {
                    fPadIdxArray.insert(make_pair(key, make_tuple(xIdx, yIdx--, idxFPN)));
                    if (yIdx == -1) {
                        xIdx++;
                        yIdx = 7;
                    }
                }
            }
        }
    }
}

void ATTPCDecoderTask::NewPadIDMapping(){
    fPadIdxArray.clear();
    fPadFPNIdxArray.clear();

    Int_t xIdx, yIdx, idFPNPad = 0;
    for(int asadIdx=0; asadIdx<fNumAsAds; asadIdx++){
        for(int agetIdx = 0; agetIdx < fNumAGETs; agetIdx++){
            xIdx = 16;
            yIdx = (fColumnNum-2)-(2*agetIdx);

            Int_t nChanId = 0;
            Int_t padSorterIdx = 0;
            for(int chanIdx = 0; chanIdx < fNumChannels; chanIdx++){
                vector<Int_t> key;
                key.push_back(asadIdx);
                key.push_back(agetIdx);
                key.push_back(chanIdx);

                int sign = (chanIdx%2 == 1) ? 1 : -1;
                xIdx = xIdx + sign*(padSorterIdx);
                if (IsFPNChannel(chanIdx)) {
                    fPadIdxArray.insert(make_pair(key, make_tuple(-1, -1, -1)));
                    fPadFPNIdxArray.insert(make_pair(key, idFPNPad));
                    idFPNPad++;
                    if(chanIdx < 33){xIdx = (sign==1) ? xIdx-1 : xIdx+1;}
                    if(chanIdx > 34){xIdx = (sign==1) ? xIdx+1 : xIdx-1;}
                    continue;
                }
                nChanId++;
                Int_t idxFPN = (nChanId - 1) / 16;

                padSorterIdx = (chanIdx > 33) ? padSorterIdx-1 : padSorterIdx+1;
                if(chanIdx==33 || chanIdx==34){
                    if(chanIdx==34){yIdx++;}
                    fPadIdxArray.insert(make_pair(key, make_tuple(0, yIdx, idxFPN)));
                    continue;
                }

                fPadIdxArray.insert(make_pair(key, make_tuple(xIdx, yIdx, idxFPN)));
            }
        }
    }
}