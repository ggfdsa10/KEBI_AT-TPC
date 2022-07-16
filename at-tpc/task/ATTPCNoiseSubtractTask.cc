#include "ATTPCNoiseSubtractTask.hh"

#include <iostream>
#include <fstream>
using namespace std;

ClassImp(ATTPCNoiseSubtractTask)

ATTPCNoiseSubtractTask::ATTPCNoiseSubtractTask()
:KBTask("ATTPCNoiseSubtractTask","")
{
} 

bool ATTPCNoiseSubtractTask::Init()
{
    KBRun *run = KBRun::GetRun();
    fPadArray = (TClonesArray *) run -> GetBranch("Pad");
    fPadFPNArray = (TClonesArray *) run -> GetBranch("FPNPad");

    ATTPC *tpc = (ATTPC *) run -> GetDetector(0);
    fPadPlane = tpc -> GetPadPlane();

    fDecoder = new ATTPCDecoderTask();

    fNumTB = fPar -> GetParInt("NTbs");
    fNumAsAds = fPar -> GetParInt("numAsAds");
    fNumAGETs = fPar -> GetParInt("numAGETs");
    fNumChannels = fPar -> GetParInt("numChannelsMax");

    return true;
}

void ATTPCNoiseSubtractTask::Exec(Option_t*)
{
    const Int_t numTB = fNumTB;
    const Int_t numAsAd = fNumAsAds;
    const Int_t numAGET = fNumAGETs;

    vector<tuple<int, int, double>> vChanId_maxADC_evn_inAGET[numAsAd][numAGET];
    vector<tuple<int, int, double>> vChanId_maxADC_odd_inAGET[numAsAd][numAGET];

    Int_t nPads = fPadArray -> GetEntries();
    if(nPads==0){return;} // skip the event

    for (Int_t iPad = 0; iPad < nPads; iPad++) {
        KBPad *pad = (KBPad *) fPadArray -> At(iPad);
        Double_t out[numTB] = {0};
        memset(out, 0., sizeof(out));

        Int_t asadIdx = pad -> GetAsAdID();
        Int_t agetIdx = pad -> GetAGETID();
        Int_t chanIdx = pad -> GetChannelID();
        Int_t padIdx = pad -> GetPadID();

        auto bufferOut = pad -> GetBufferOut();
        if(fDecoder -> IsFPNChannel(chanIdx)){continue;}
        if(fDecoder -> IsDeadChannel(agetIdx, chanIdx)){continue;}

        // Step 1: Subtract a Fixed Pattern Noise from raw signals
        Int_t idxFPNChanByPads = (fDecoder -> GetPadID(asadIdx, agetIdx, chanIdx)).second;
        Int_t idFPNPad = fDecoder -> GetFPNPadID(asadIdx, agetIdx, idxFPNChanByPads);

        KBPad *padFPN = (KBPad *) fPadFPNArray -> At(idFPNPad);
        auto bufferOutFPN = padFPN -> GetBufferOut();

        Double_t meanADC = 0.;
        for(int tb = int(fTBStart); tb < numTB; tb++){
            out[tb] = bufferOut[tb] - bufferOutFPN[tb];
            if(tb < int(fTBEnd)){meanADC += (out[tb]/fTBEnd);}
        }

        // Step 2: Subtract a mean value from the (FPNC)signals to find noise candidates
        for(int tb = int(fTBStart); tb < numTB; tb++){out[tb] -= meanADC;}

        pad -> SetBufferOut(out);
        
        // Step 3: make noise profiles using 4 noise candidates
        if(fDecoder -> IsEvenChannel(agetIdx, chanIdx)){vChanId_maxADC_evn_inAGET[asadIdx][agetIdx].push_back(make_tuple(pad->GetPadID(), chanIdx, out[1]));}
        else{vChanId_maxADC_odd_inAGET[asadIdx][agetIdx].push_back(make_tuple(pad->GetPadID(), chanIdx, out[1]));}
    }

    for(int asadIdx = 0; asadIdx < numAsAd; asadIdx++){
        for(int agetIdx = 0; agetIdx < numAGET; agetIdx++){
            // Sort by a descending order
            sort(vChanId_maxADC_evn_inAGET[asadIdx][agetIdx].begin(), vChanId_maxADC_evn_inAGET[asadIdx][agetIdx].end(), compareThirdByDescending);
            sort(vChanId_maxADC_odd_inAGET[asadIdx][agetIdx].begin(), vChanId_maxADC_odd_inAGET[asadIdx][agetIdx].end(), compareThirdByDescending);

            Double_t bufferNoiseEvn[numTB];
            Double_t bufferNoiseOdd[numTB];
            memset(bufferNoiseEvn, 0., sizeof(bufferNoiseEvn));
            memset(bufferNoiseOdd, 0., sizeof(bufferNoiseOdd));

            for(int idx = 0; idx < fNoiseFindParNum; idx++){
                Int_t padIDEvn = get<0>(vChanId_maxADC_evn_inAGET[asadIdx][agetIdx].at(idx));
                Int_t padIDOdd = get<0>(vChanId_maxADC_odd_inAGET[asadIdx][agetIdx].at(idx));

                KBPad *padEvn = (KBPad *) fPadArray -> At(padIDEvn);
                KBPad *padOdd = (KBPad *) fPadArray -> At(padIDOdd);

                auto outEvn = padEvn -> GetBufferOut();
                auto outOdd = padOdd -> GetBufferOut();

                for(int tb = 0; tb < numTB; tb++){
                    bufferNoiseEvn[tb] += (1./fNoiseFindParNum) * outEvn[tb];
                    bufferNoiseOdd[tb] += (1./fNoiseFindParNum) * outOdd[tb];
                }
            }

            // Step 4: adjust a pedestal of the (FPNC)signals to 0
            for(int chanIdx = 0; chanIdx < fNumChannels; chanIdx++){
                if(fDecoder -> IsFPNChannel(chanIdx) || fDecoder -> IsDeadChannel(agetIdx, chanIdx)) continue;

                Double_t out[numTB];
                Int_t padID = (fDecoder -> GetPadID(asadIdx, agetIdx, chanIdx)).first;
                KBPad *pad = (KBPad *) fPadArray -> At(padID);
                auto padOut = pad -> GetBufferOut();

                vector<double> bufferADC;
                for(int tb = fTBStart; tb <= fTBEnd; tb++){bufferADC.push_back(padOut[tb]);}
                sort(bufferADC.begin(), bufferADC.end());

                double deviation = 0.;
                for(int tb = 151; tb <= 200; tb++){deviation += bufferADC[tb - 1] / 50.;}
                for(int tb = int(fTBStart); tb < numTB; tb++){
                    out[tb] = padOut[tb] -deviation;

                    // Step 5: subtract a noise profile from the (FPNC)signals
                    if(fDecoder->IsEvenChannel(agetIdx, chanIdx)){out[tb] -= bufferNoiseEvn[tb];}
                    else{out[tb] -= bufferNoiseOdd[tb];}
                }

                for(int tb =0; tb<numTB; tb++){if(out[tb] < 0.){out[tb] = 0.;}}
                pad -> SetBufferOut(out);
                
                // // save the noise template
                // if(chanIdx ==0){
                //     if(fDecoder->IsEvenChannel(agetIdx, chanIdx)){pad -> SetBufferIn(bufferNoiseEvn);}
                //     else{pad -> SetBufferIn(bufferNoiseOdd);}
                // }
            }
        }
    }
    return;
}


void ATTPCNoiseSubtractTask::SetPadPersistency(bool persistence){fPersistency = persistence;}