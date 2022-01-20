#include "KBGeoHelix.hh"
#include "TMath.h"
#include "TRandom.h"

ClassImp(KBGeoHelix)

KBGeoHelix::KBGeoHelix()
{
}

KBGeoHelix::KBGeoHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
                       Double_t t, Double_t h, kbaxis_t a)
{
  SetHelix(i,j,r,s,k,t,h,a);
}

void KBGeoHelix::Print(Option_t *option) const
{
}

TVector3 KBGeoHelix::GetCenter() const { return KBVector3(fA,fI,fJ,.5*(fH*fS+fK + fT*fS+fK)).GetXYZ(); }

void KBGeoHelix::SetHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
                          Double_t t, Double_t h, kbaxis_t a)
{
  fI = i;
  fJ = j;
  fR = r;
  fS = s;
  fK = k;
  fT = t;
  fH = h;
  fA = a;
}

    void KBGeoHelix::SetRMSR(Double_t val) { fRMSR = val; }
    void KBGeoHelix::SetRMST(Double_t val) { fRMST = val; }
Double_t KBGeoHelix::GetRMSR()       const { return fRMSR; }
Double_t KBGeoHelix::GetRMST()       const { return fRMST; }

TVector3 KBGeoHelix::GetRandomPoint(Double_t sigma)
{
  if (sigma > 0) {
    Double_t val;
    if (fH > fT) val = gRandom -> Uniform(fH-fT) + fT;
    if (fT > fH) val = gRandom -> Uniform(fT-fH) + fH;

    Double_t dr = gRandom -> Gaus(0,sigma);
    fR = fR + dr;
    auto pos = PositionAtAlpha(val);
    fR = fR - dr;
    return pos;
  }

  Double_t val;
  if (fH > fT) val = gRandom -> Uniform(fH-fT) + fT;
  if (fT > fH) val = gRandom -> Uniform(fT-fH) + fH;
  return PositionAtAlpha(val);
}

void KBGeoHelix::SetI(Double_t val) { fI = val; }
void KBGeoHelix::SetJ(Double_t val) { fJ = val; }
void KBGeoHelix::SetR(Double_t val) { fR = val; }
void KBGeoHelix::SetS(Double_t val) { fS = val; }
void KBGeoHelix::SetK(Double_t val) { fK = val; }
void KBGeoHelix::SetT(Double_t val) { fT = val; }
void KBGeoHelix::SetH(Double_t val) { fH = val; }
void KBGeoHelix::SetA(kbaxis_t val) { fA = val; }

Double_t KBGeoHelix::GetI() const { return fI; }
Double_t KBGeoHelix::GetJ() const { return fJ; }
Double_t KBGeoHelix::GetR() const { return fR; }
Double_t KBGeoHelix::GetS() const { return fS; }
Double_t KBGeoHelix::GetK() const { return fK; }
Double_t KBGeoHelix::GetT() const { return fT; }
Double_t KBGeoHelix::GetH() const { return fH; }
kbaxis_t KBGeoHelix::GetA() const { return fA; }

   Int_t KBGeoHelix::Helicity()            const { return fS > 0 ? 1 : -1; }
Double_t KBGeoHelix::DipAngle()            const { return (fR <= 0 ? -999 : TMath::ATan(fS/fR)); }
Double_t KBGeoHelix::CosDip()              const { return TMath::Cos(DipAngle()); }
Double_t KBGeoHelix::AngleFromCenterAxis() const { return TMath::Pi()/2 - DipAngle(); }
Double_t KBGeoHelix::LengthInPeriod()      const { return 2*TMath::Pi()*fR/CosDip(); }
Double_t KBGeoHelix::KLengthInPeriod()     const { return TMath::Abs(2*TMath::Pi()*fS); }

Double_t KBGeoHelix::TravelLengthAtAlpha(Double_t alpha)  const { return alpha*fR/CosDip(); }
Double_t KBGeoHelix::AlphaAtTravelLength(Double_t tlen) const { return tlen*CosDip()/fR; }

TVector3 KBGeoHelix::PositionAtAlpha(Double_t alpha) const {
  return KBVector3(fA,fR*TMath::Cos(alpha)+fI,fR*TMath::Sin(alpha)+fJ,alpha*fS+fK).GetXYZ(); 
}

TVector3 KBGeoHelix::Direction(Double_t alpha) const
{
  Double_t alphaPointer = alpha;
  if (fT < fH) alphaPointer += TMath::Pi()/2.;
  else         alphaPointer -= TMath::Pi()/2.;

  KBVector3 direction = KBVector3(PositionAtAlpha(alphaPointer),fA) - KBVector3(fA,fI,fJ,0);
  auto directionZ = direction.Z();
  direction.SetK(0);
  direction.SetMag(2*TMath::Pi()*fR);
  if (directionZ > 0) direction.SetK(+abs(KLengthInPeriod()));
  else                direction.SetK(-abs(KLengthInPeriod()));

  return direction.GetXYZ().Unit();
}

TVector3 KBGeoHelix::HelicoidMap(TVector3 pos, Double_t alpha) const
{
  Double_t tlenH = TravelLengthAtAlpha(fH);
  Double_t tlenT = TravelLengthAtAlpha(fT);
  Double_t tlen0 = tlenH;
  if (tlenH > tlenT)
    tlen0 = tlenT;

  KBVector3 posr(pos, fA);
  KBVector3 poca(PositionAtAlpha(alpha), fA);

  Double_t di = posr.I() - fI;
  Double_t dj = posr.J() - fJ;
  Double_t dr = sqrt(di*di + dj*dj) - fR;
  Double_t dk = posr.K() - poca.K();

  return TVector3(dr, dk/CosDip(), dk*TMath::Sin(DipAngle()) + TravelLengthAtAlpha(alpha) - tlen0);
}
