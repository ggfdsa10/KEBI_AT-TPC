#include "KBRun.hh"
#include "LHElectronicsTask.hh"

#include <iostream>
using namespace std;

ClassImp(LHElectronicsTask)

LHElectronicsTask::LHElectronicsTask(Bool_t usePointChargeMC)
:KBTask("LHElectronicsTask",""), fUsePointChargeMC(usePointChargeMC)
{
}

bool LHElectronicsTask::Init()
{
  KBRun *run = KBRun::GetRun();

  KBParameterContainer *par = run -> GetParameterContainer();
  LHTpc *det = (LHTpc *) (run -> GetDetectorSystem() -> GetTpc());

  fNPlanes = det -> GetNumPlanes();
  fNTbs = par -> GetParInt("TPCnTbs");
  feVToADC = par -> GetParDouble("TPCeVToADC");
  fDynamicRange = par -> GetParDouble("TPCdynamicRange");

  fPadArray = (TClonesArray *) run -> GetBranch("TPCPad");

  KBPulseGenerator *pulseGen = new KBPulseGenerator();
  fPulseFunction = pulseGen -> GetPulseFunction();
  fPulseFunction -> SetParameters(feVToADC,0);

  return true;
}

void LHElectronicsTask::Exec(Option_t*)
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
        fPulseFunction -> SetParameters(feVToADC,tb1);
        for (Int_t tb = 0; tb < 512; ++tb) {
          out[tb] += ev1 * fPulseFunction -> Eval(tb+0.5);
          if (out[tb] > fDynamicRange)
            out[tb] = fDynamicRange+1;
        }
      }
    }
    else {
      Double_t *in = pad -> GetBufferIn();
      for (Int_t iTb = 0; iTb < fNTbs; iTb++) {
        if (in[iTb] == 0)
          continue;

        for (Int_t iTb2 = 0; iTb2 < 100; iTb2++) {
          Int_t tb = iTb+iTb2;
          if (tb >= fNTbs)
            break;

          out[tb] += in[iTb] * fPulseFunction -> Eval(iTb2+0.5);
          if (out[tb] > fDynamicRange)
            out[tb] = fDynamicRange+1;
        }
      }
    }

    auto saturated = false;
    Int_t saturatedFrom = 100000;
    Int_t saturatedTo = 100000;
    for (Int_t iTb = 0; iTb < fNTbs; iTb++) {
      if (!saturated && out[iTb] > fDynamicRange) {
        saturated = true;
        saturatedFrom = iTb;
      }
      if (saturated && out[iTb] < fDynamicRange) {
        saturatedTo = iTb;
        break;
      }
    }

    if (saturatedTo-saturatedFrom>=5)
    for (Int_t iTb = saturatedFrom+5; iTb < fNTbs; iTb++)
      out[iTb] = 0;

    pad -> SetBufferOut(out);
  }

  kb_info << endl;
  
  return;
}
