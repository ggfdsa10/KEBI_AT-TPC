
#include "ATTPCRootDecoder.hh"

#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"


ClassImp(ATTPCRootDecoder)

ATTPCRootDecoder::ATTPCRootDecoder()
:KBTask("ATTPCRootDecoder","")
{
    NewPadIDMapping();
    BoardIDMapping();
} 

bool ATTPCRootDecoder::Init()
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
    LoadRootFile(pathToRawData);
    fPar -> SetPar("isExpData", true);

    if(fNumEvents==-1){run -> SetEntries(rootTree->GetEntries());}
    else{run -> SetEntries(fNumEvents);}

    return true;
}

void ATTPCRootDecoder::Exec(Option_t*)
{
    if(fFileToKEBIForm){fPadArray -> Clear("C");}
    rootTree -> GetEntry(fEventIdx);
    Int_t countChannels = 0;
    Int_t idFPNPad = 0;

    if(fIsBroken == 1){
        fEventIdx++;
        return;
    }

    for (Int_t iAsAd = 0; iAsAd < fNumAsAds; iAsAd++) {
        for (Int_t iAGET = 0; iAGET < fNumAGETs; iAGET++) {
            for (Int_t iChannel = 0; iChannel < fNumChannels; iChannel++) {
                Short_t copy[512] = {0};
                Double_t copy2[512] = {0};

                for (Int_t iTb = 0; iTb < 512; iTb++) {
                    Short_t value = fADC[iAsAd][iAGET][iChannel][iTb];
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
                    // FPNpadSave -> SetBufferRaw(copy);
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
                    // padSave -> SetBufferRaw(copy);
                    padSave -> SetBufferOut(copy2);
                    padSave -> SetSortValue(padID);
                    countChannels++;
                }
            }
        }
    }

    fPadFPNArray -> Sort();
    fPadArray -> Sort();
    fEventIdx++;
    kb_info << "found " << countChannels << " channels." << endl;
    return;
}

void ATTPCRootDecoder::SetPadPersistency(bool persistence) { fPersistency = persistence; }
void ATTPCRootDecoder::SaveKEBIData(bool fileToKEBIForm) {fFileToKEBIForm = fileToKEBIForm;}

void ATTPCRootDecoder::SetNumEvents(Int_t numEvents){fNumEvents = numEvents;}

void ATTPCRootDecoder::LoadRootFile(TString pathToRawData)
{
   rootFile = new TFile(Form("%s",pathToRawData.Data()), "read");
   rootTree = (TTree*)rootFile -> Get("treeOfADC");
   rootTree -> SetBranchAddress("ADC", &fADC);
   rootTree -> SetBranchAddress("RunID", &fNumRun);
   rootTree -> SetBranchAddress("EventID", &fNumEvents);
   rootTree -> SetBranchAddress("EventTime", &fEventTime);
   rootTree -> SetBranchAddress("EventDiffTime", &fEventDiffTime);
   rootTree -> SetBranchAddress("IsBroken", &fIsBroken);
}

pair<Int_t, Int_t> ATTPCRootDecoder::GetPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanIdx)
{
    vector<Int_t> key;
    key.push_back(asadIdx);
    key.push_back(agetIdx);
    key.push_back(chanIdx);

    Int_t PadID = get<0>(fPadIdxArray[key]) + (32 * get<1>(fPadIdxArray[key]));
    pair<Int_t, Int_t> value = make_pair(PadID, get<2>(fPadIdxArray[key]));
    return value;
}

Int_t ATTPCRootDecoder::GetFPNPadID(Int_t asadIdx, Int_t agetIdx, Int_t chanFPNIdx){
    vector<Int_t> key;
    key.push_back(asadIdx);
    key.push_back(agetIdx);
    key.push_back(fFPNChanArray[chanFPNIdx]);
    return fPadFPNIdxArray[key];
}

tuple<Int_t, Int_t, Int_t> ATTPCRootDecoder::GetBoardID(Int_t padID){return fBoardIdxArray[padID];}
tuple<Int_t, Int_t, Int_t> ATTPCRootDecoder::GetBoardFPNID(Int_t padID){return fBoardFPNIdxArray[padID];}

bool ATTPCRootDecoder::IsFPNChannel(Int_t chanIdx) {return (chanIdx == fFPNChanArray[0] || chanIdx == fFPNChanArray[1] || chanIdx == fFPNChanArray[2] || chanIdx == fFPNChanArray[3]) ? true : false;}

bool ATTPCRootDecoder::IsEvenChannel(Int_t agetIdx, Int_t chanIdx) {
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

bool ATTPCRootDecoder::IsDeadChannel(Int_t agetIdx, Int_t chanIdx) {
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

void ATTPCRootDecoder::PadIDMapping() 
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

void ATTPCRootDecoder::NewPadIDMapping(){
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

                if (IsFPNChannel(chanIdx)) {
                    fPadIdxArray.insert(make_pair(key, make_tuple(-1, -1, -1)));
                    fPadFPNIdxArray.insert(make_pair(key, idFPNPad));
                    idFPNPad++;
                    continue;
                }
                nChanId++;
                Int_t idxFPN = (nChanId - 1) / 16;
                if(chanIdx==33 || chanIdx==34 || chanIdx==35){
                    xIdx=0;
                    if(chanIdx==34){
                        xIdx=1;    
                        yIdx++;
                        padSorterIdx = 14;
                    }
                    fPadIdxArray.insert(make_pair(key, make_tuple(xIdx, yIdx, idxFPN)));
                    continue;
                }

                int sign = -1;
                if(nChanId%2 == 0) {
                    sign = 1;
                    padSorterIdx++;
                } else if(chanIdx >= 38) {
                    padSorterIdx -= 2;
                }
                xIdx = 16 + sign*(padSorterIdx);


                fPadIdxArray.insert(make_pair(key, make_tuple(xIdx, yIdx, idxFPN)));
            }
        }
    }
}

void ATTPCRootDecoder::BoardIDMapping()
{
    fBoardIdxArray.clear();
    Int_t fpnPadIdx = 0;
    for(int asadIdx=0; asadIdx<fNumAsAds; asadIdx++){
        for(int agetIdx = 0; agetIdx < fNumAGETs; agetIdx++){
            for(int chanIdx = 0; chanIdx < fNumChannels; chanIdx++){
                Int_t padID = GetPadID(asadIdx, agetIdx, chanIdx).first;
                fBoardIdxArray.insert(make_pair(padID, make_tuple(asadIdx, agetIdx, chanIdx)));

                if(IsFPNChannel(chanIdx)){
                    fBoardFPNIdxArray.insert(make_pair(fpnPadIdx, make_tuple(asadIdx, agetIdx, chanIdx)));
                    fpnPadIdx++;
                }
            }
        }
    }
}
