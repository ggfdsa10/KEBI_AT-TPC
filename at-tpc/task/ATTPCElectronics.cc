#include "KBRun.hh"
#include "ATTPCElectronics.hh"

using namespace std;

ClassImp(ATTPCElectronics)

ATTPCElectronics::ATTPCElectronics(Bool_t usePointChargeMC)
:KBTask("ATTPCElectronics",""), fUsePointChargeMC(usePointChargeMC)
{
}

bool ATTPCElectronics::Init()
{
  KBRun *run = KBRun::GetRun();

  KBParameterContainer *par = run -> GetParameterContainer();
  
  ATTPC *det = (ATTPC *) (run -> GetDetectorSystem() -> GetTpc());

  fNPlanes = det -> GetNumPlanes();
  fNTbs = par -> GetParInt("NTbs");
  fDynamicRange = par -> GetParDouble("DynamicRange");
  fNoiseOn = par -> GetParBool("NoiseOn");

  if(fPar -> GetParString("PadPlaneType") == "StripPad"){
    fStripPad = (ATTPCStripPad *) det -> GetPadPlane();
    fStripView = true;
  }
  fEChargeToADC = fElectronCharge/(fDynamicRange *1.0e-15)*fADCMaxAmp;

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");
  fFPNPadArray = (TClonesArray *) run -> GetBranch("FPNPad");

  KBPulseGenerator *pulseGen = new KBPulseGenerator("pulserForATTPC.dat");
  fPulseFunction = pulseGen -> GetPulseFunction();
  fPulseFunction -> SetParameters(fEChargeToADC,0);

  if(fNoiseOn == true){
    auto noiseFile = new TFile("$KEBIPATH/at-tpc/macros/input/noise.root","read");
    noiseTree = (TTree*)noiseFile -> Get("noise");
    noiseTree->SetBranchAddress("noiseArray", &noiseArray);
    fDecoder = new ATTPCDecoderTask();
  }
  
  return true;
}

void ATTPCElectronics::Exec(Option_t*)
{
  gRandom -> SetSeed(0);

  if(fNoiseOn == true){
    Int_t nFPNPads = fFPNPadArray -> GetEntries();

    for(int iPad = 0; iPad < nFPNPads; iPad++){
      KBPad *pad = (KBPad *) fFPNPadArray -> At(iPad);
      Double_t out[512] = {0};
      int noiseEvent = gRandom->Uniform(0, noiseTree->GetEntries());
      noiseTree->GetEntry(noiseEvent);
      int iAGET = pad -> GetAGETID();
      Double_t baseLine = gRandom -> Gaus(450., 30.);

      for(int tb=0; tb<512; tb++){
        out[tb] = noiseArray[iAGET][tb] + baseLine;
      }
      pad -> SetBufferOut(out);
    }
  }

  Int_t nPads = fPadArray -> GetEntries();
  for (Int_t iPad = 0; iPad < nPads; iPad++) {
    KBPad *pad = (KBPad *) fPadArray -> At(iPad);
    Double_t out[512] = {0};

    if (fUsePointChargeMC)
    {
      auto tbs = pad -> GetMCTbArray();
      auto evs = pad -> GetMCWeightArray();
      Int_t numMCIDs = tbs -> size();
      for (auto iMC = 0; iMC < numMCIDs; ++iMC)
      {
        auto tb1 = tbs -> at(iMC);
        auto ev1 = evs -> at(iMC);
        fPulseFunction -> SetParameters(fEChargeToADC,tb1);
        for (Int_t tb = 0; tb < 512; ++tb) {
          out[tb] += ev1 * fPulseFunction -> Eval(tb+0.5);
          if (out[tb] > fADCMaxAmp)
            out[tb] = fADCMaxAmp+1;
        }
      }
    }

    else { 
      auto tbs = pad -> GetMCTbArray();
      auto evs = pad -> GetMCWeightArray();
      Int_t numMCIDs = tbs -> size();
      for (auto iMC = 0; iMC < numMCIDs; ++iMC)
      {
        auto tb1 = tbs -> at(iMC);
        auto ev1 = evs -> at(iMC);
        for (Int_t tb = 0; tb < 512; ++tb) {
          out[tb] += ev1;
          if (out[tb] > fADCMaxAmp)
            out[tb] = fADCMaxAmp+1;
        }
      }
    }


    if(fNoiseOn == true){ 
      Int_t iAsAd = pad -> GetAsAdID();
      Int_t iAGET = pad -> GetAGETID();
      Int_t iChannel = pad -> GetChannelID();
      Int_t idxFPNChanByPads = (fDecoder -> GetPadID(iAsAd, iAGET, iChannel)).second;
      Int_t idFPNPad = fDecoder -> GetFPNPadID(iAsAd, iAGET, idxFPNChanByPads);

      KBPad *padFPN = (KBPad *) fFPNPadArray -> At(idFPNPad);
      auto bufferNoiseOut = padFPN -> GetBufferOut();

      int noiseEvent1 = gRandom->Uniform(0, noiseTree->GetEntries());
      int noiseEvent2 = gRandom->Uniform(0, noiseTree->GetEntries());
      noiseTree->GetEntry(noiseEvent1);
      Double_t noiseByPad[512];

      for (Int_t i = 0; i< 512; i++){
        noiseByPad[i] = noiseArray[iAGET][i];
      }
      noiseTree->GetEntry(noiseEvent2);
      for (Int_t i = 0; i< 512; i++){
        out[i] += (noiseArray[iAGET][i] + noiseByPad[i])/2.;
        out[i] += bufferNoiseOut[i];
      }
    }


    // pad -> SetBufferIn(out);
    pad -> SetBufferOut(out);
  }



  if(fStripPad != nullptr){
    if(fStripView){
      auto stripPadCh0 = fStripPad->GetPadByChan(0);
      auto stripPadCh1 = fStripPad->GetPadByChan(1);
      auto stripPadCh2 = fStripPad->GetPadByChan(2);

      for(int channal = 0; channal < 3; channal++){
        auto stripPadArray = stripPadCh0;
        if(channal ==1){stripPadArray = stripPadCh1;}
        if(channal ==2){stripPadArray = stripPadCh2;}

        int layerNum = stripPadArray.size();
        for(int layer = 0; layer < layerNum; layer++){
          int rowNum = stripPadArray.at(layer).size();
          Double_t outByStrip[512] = {0};

          for(int row = 0; row < rowNum; row++){
            int padIndex = get<0>(stripPadArray.at(layer).at(row));
            KBPad *pad = (KBPad *) fPadArray -> At(padIndex);
            auto bufferOut = pad -> GetBufferIn();

            for (Int_t tb = 0; tb < 512; ++tb) {
              outByStrip[tb] += bufferOut[tb];
            }
          }
          for(int row = 0; row < rowNum; row++){
            int padIndex = get<0>(stripPadArray.at(layer).at(row));
            KBPad *pad = (KBPad *) fPadArray -> At(padIndex);

            pad -> SetBufferOut(outByStrip);
          }
        }
      }
    }
  }

  kb_info << endl;
  
  return;
}
