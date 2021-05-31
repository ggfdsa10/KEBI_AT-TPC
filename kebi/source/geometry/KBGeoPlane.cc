#include "KBGeoPlane.hh"
#include <cmath>

ClassImp(KBGeoPlane)

KBGeoPlane::KBGeoPlane()
{
}

KBGeoPlane::KBGeoPlane(Double_t x, Double_t y, Double_t z, Double_t xn, Double_t yn, Double_t zn)
{
  SetPlane(x, y, z, xn, yn, zn);
}

KBGeoPlane::KBGeoPlane(TVector3 pos, TVector3 nnn)
{
  SetPlane(pos, nnn);
}

KBGeoPlane::KBGeoPlane(Double_t a, Double_t b, Double_t c, Double_t d)
{
  SetPlane(a,b,c,d);
}

void KBGeoPlane::SetPlane(TVector3 pos, TVector3 nnn)
{
  nnn = nnn.Unit();
  fA = nnn.X();
  fB = nnn.Y();
  fC = nnn.Z();
  fD = -nnn.Dot(pos);
}

void KBGeoPlane::SetPlane(Double_t x, Double_t y, Double_t z, Double_t xn, Double_t yn, Double_t zn)
{
  SetPlane(TVector3(x,y,z), TVector3(xn,yn,zn));
}

void KBGeoPlane::SetPlane(Double_t a, Double_t b, Double_t c, Double_t d)
{
  auto mag = TMath::Sqrt(a*a+b*b+c*c);
  fA = a/mag;
  fB = b/mag;
  fC = c/mag;
  fD = d/mag;
}

Double_t KBGeoPlane::GetA() const { return fA; }
Double_t KBGeoPlane::GetB() const { return fB; }
Double_t KBGeoPlane::GetC() const { return fC; }
Double_t KBGeoPlane::GetD() const { return fD; }
TVector3 KBGeoPlane::GetNormal() const { return TVector3(fA,fB,fC); }

Double_t KBGeoPlane::GetX(Double_t y, Double_t z) const { return -(fD+fB*y+fC*z)/fA; }
Double_t KBGeoPlane::GetY(Double_t z, Double_t x) const { return -(fD+fC*z+fA*x)/fB; }
Double_t KBGeoPlane::GetZ(Double_t x, Double_t y) const { return -(fD+fA*x+fB*y)/fC; }

TVector3 KBGeoPlane::ClosestPointOnPlane(TVector3 pos) const
{
  TVector3 nnn(fA,fB,fC);
  TVector3 point = pos - nnn.Dot(TVector3(fA,fB,fC)) * nnn;

  return point;
}

//Double_t KBGeoPlane::DistanceToPlane(Double_t x, Double_t y, Double_t z) const
//{
//  return DistanceToPlane(TVector3(x,y,z));
//}
