#include "KBGeoSphere.hh"
#include <cmath>
#include <iostream>

ClassImp(KBGeoSphere)

KBGeoSphere::KBGeoSphere()
{
}

KBGeoSphere::KBGeoSphere(Double_t x, Double_t y, Double_t z, Double_t r)
{
  SetSphere(x, y, z, r);
}

KBGeoSphere::KBGeoSphere(TVector3 pos, Double_t r)
{
  SetSphere(pos.X(), pos.Y(), pos.Z(), r);
}

void KBGeoSphere::SetSphere(Double_t x, Double_t y, Double_t z, Double_t r)
{
  fX = x;
  fY = y;
  fZ = z;
  fR = r;
}

void KBGeoSphere::SetSphere(TVector3 pos, Double_t r)
{
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
  fR = r;
}

Double_t KBGeoSphere::GetX() const { return fX; }
Double_t KBGeoSphere::GetY() const { return fY; }
Double_t KBGeoSphere::GetZ() const { return fZ; }
Double_t KBGeoSphere::GetR() const { return fR; }

TVector3 KBGeoSphere::GetCenter() const { return TVector3(fX, fY, fZ); }
Double_t KBGeoSphere::GetRadius() const { return fR; }

TGraph *KBGeoSphere::DrawCircleXY(Int_t n, Double_t theta1, Double_t theta2) {
  return KBGeoCircle(fX,fY,fR).DrawCircle(n, theta1, theta2);
}

TGraph *KBGeoSphere::DrawCircleYZ(Int_t n, Double_t theta1, Double_t theta2) {
  return KBGeoCircle(fY,fZ,fR).DrawCircle(n, theta1, theta2);
}

TGraph *KBGeoSphere::DrawCircleZX(Int_t n, Double_t theta1, Double_t theta2) {
  return KBGeoCircle(fZ,fX,fR).DrawCircle(n, theta1, theta2);
}

TVector3 KBGeoSphere::StereographicProjection(Double_t x, Double_t y)
{
  Double_t a = sqrt(x*x + y*y) / (2*fR);
  Double_t b = 1 + a * a;

  Double_t xMap = x / b;
  Double_t yMap = y / b;
  Double_t zMap = 2 * fR * a * a / b;

  return TVector3(xMap,yMap,zMap);
}
