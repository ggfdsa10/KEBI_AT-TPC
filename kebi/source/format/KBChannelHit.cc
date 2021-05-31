#include "KBChannelHit.hh"

ClassImp(KBChannelHit)

KBChannelHit::KBChannelHit(KBChannelHit *hit)
:fID(hit -> GetID()), fTDC(hit -> GetTDC()), fADC(hit -> GetADC())
{
}

KBChannelHit::KBChannelHit(Int_t id, Double_t t, Double_t a)
:fID(id), fTDC(t), fADC(a)
{
}

void KBChannelHit::Clear(Option_t *option)
{
  fID = -1;
  fTDC = -999;
  fADC = -999;
}

void KBChannelHit::SetID(Int_t val)     { fID = val; }
void KBChannelHit::SetTDC(Double_t val) { fTDC = val; }
void KBChannelHit::SetADC(Double_t val) { fADC = val; }

Int_t    KBChannelHit::GetID()  const { return fID; }
Double_t KBChannelHit::GetTDC() const { return fTDC; }
Double_t KBChannelHit::GetADC() const { return fADC; }
