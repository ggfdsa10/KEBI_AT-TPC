#include "KBRun.hh"
#include "NewTPCElectronics.hh"

using namespace std;

ClassImp(NewTPCElectronics)

NewTPCElectronics::NewTPCElectronics(Bool_t usePointChargeMC)
:KBTask("NewTPCElectronics",""), fUsePointChargeMC(usePointChargeMC)
{
}

bool NewTPCElectronics::Init()
{
  KBRun *run = KBRun::GetRun();

  KBParameterContainer *par = run -> GetParameterContainer();
  LHTpc *det = (LHTpc *) (run -> GetDetectorSystem() -> GetTpc());

  fNPlanes = det -> GetNumPlanes();
  fNTbs = par -> GetParInt("NTbs");
  fDynamicRange = par -> GetParDouble("DynamicRange");
  fNoiseOn = par -> GetParBool("NoiseOn");

  fEChargeToADC = fElectronCharge/(fDynamicRange *1.0e-15)*fADCMaxAmp;

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  KBPulseGenerator *pulseGen = new KBPulseGenerator();
  fPulseFunction = pulseGen -> GetPulseFunction();
  fPulseFunction -> SetParameters(fEChargeToADC,0);

  fRandom = new TRandom3(time(0));
   
  return true;
}

void NewTPCElectronics::Exec(Option_t*)
{
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
    
    auto saturated = false;
    Int_t saturatedFrom = 100000;
    Int_t saturatedTo = 100000;
    for (Int_t iTb = 0; iTb < fNTbs; iTb++) {
      if (!saturated && out[iTb] > fADCMaxAmp) {
        saturated = true;
        saturatedFrom = iTb;
      }
      if (saturated && out[iTb] < fADCMaxAmp) {
        saturatedTo = iTb;
        break;
      }
    }

    if (saturatedTo-saturatedFrom>=5)
    for (Int_t iTb = saturatedFrom+5; iTb < fNTbs; iTb++)
      out[iTb] = 0;

    if(fNoiseOn == true){
      for (Int_t i = 0; i< 512; i++){
	      Double_t noise = fRandom -> Gaus(434,42);
	      out[i] += noise;
      }
    }
    pad -> SetBufferOut(out);
  }
  
  kb_info << endl;
  
  return;
}
