#include "ATTPCPSA.hh"

ClassImp(ATTPCPSA)

void ATTPCPSA::AnalyzeChannel(Double_t *buffer, vector<KBChannelHit> *hitArray, vector<vector<pair<double, double>>> *TBArray)
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

void ATTPCPSA::SetTbRange(Int_t tbi, Int_t tbf)
{ 
  fTbStart = tbi;
  fTbEnd = tbf;
}

void ATTPCPSA::SetThreshold(Double_t val) { fThreshold = val; }

void ATTPCPSA::SetParameters(KBParameterContainer *par)
{
  fThreshold = par -> GetParDouble("ADCThreshold");
}