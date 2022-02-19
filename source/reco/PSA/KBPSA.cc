#include "KBPSA.hh"

ClassImp(KBPSA)

void KBPSA::AnalyzeChannel(Double_t *buffer, vector<KBChannelHit> *hitArray)
{
  Double_t valMax = 0;
  Double_t tbAtMax = 0;

  for (Int_t iTb = fTbStart; iTb < fTbEnd; iTb++) {
    if (buffer[iTb] > valMax) {
      valMax = buffer[iTb];
      tbAtMax = iTb;
    }
  }

  if (valMax > fThreshold)
    hitArray -> push_back(KBChannelHit(0, tbAtMax, valMax));
}

void KBPSA::SetTbRange(Int_t tbi, Int_t tbf)
{ 
  fTbStart = tbi;
  fTbEnd = tbf;
}

void KBPSA::SetThreshold(Double_t val) { fThreshold = val; }

void KBPSA::SetParameters(KBParameterContainer *par)
{
  fTbStart = par -> GetParInt("TPCtbStart");
  auto nTbs = par -> GetParInt("TPCnTbs");
  fTbEnd = fTbStart + nTbs - 1;
  fThreshold = par -> GetParDouble("TPCADCThreshold");
  fDynamicRange = par -> GetParDouble("TPCdynamicRange");
}
