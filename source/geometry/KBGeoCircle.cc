#include "KBGeoCircle.hh"
#include "KBGlobal.hh"

#include "TMath.h"
#include "TRandom.h"
#include <iostream>

ClassImp(KBGeoCircle)

KBGeoCircle::KBGeoCircle()
{
}

KBGeoCircle::KBGeoCircle(Double_t x, Double_t y, Double_t r)
{
  SetCircle(x, y, r);
}

void KBGeoCircle::SetCircle(Double_t x, Double_t y, Double_t r)
{
  fX = x;
  fY = y;
  fR = r;
}

TVector3 KBGeoCircle::GetRandomPoint()
{
  auto val = gRandom -> Uniform(2*TMath::Pi());
  return TVector3(fX+fR*TMath::Cos(val), fY+fR*TMath::Sin(val), fZ);
}

void KBGeoCircle::Print(Option_t *) const
{
  KBLog("KBGeoCircle","Print",1,2) << "Center=(" << fX << "," << fY << "), R=" << fR << std::endl;
}

TVector3 KBGeoCircle::GetCenter() const { return TVector3(fX, fY, 0); }

Double_t KBGeoCircle::GetX() const { return fX; }
Double_t KBGeoCircle::GetY() const { return fY; }
Double_t KBGeoCircle::GetZ() const { return fZ; }
Double_t KBGeoCircle::GetR() const { return fR; }
Double_t KBGeoCircle::GetRadius() const { return fR; }

TGraph *KBGeoCircle::DrawCircle(Int_t n, Double_t theta1, Double_t theta2)
{
  if (theta1 == theta2 && theta1 == 0)
    theta2 = 2*TMath::Pi();

  auto graph = new TGraph();
  TVector3 center(fX,fY,0);
  for (auto i=0; i<=n; ++i)
  {
    TVector3 pointer(fR,0,0);
    pointer.RotateZ(i*(theta2-theta1)/n);
    auto point = center + pointer;
    graph -> SetPoint(graph->GetN(), point.X(), point.Y());
  }

  return graph;
}

TVector3 KBGeoCircle::ClosestPointToCircle(Double_t x, Double_t y)
{
  auto phi = Phi(x,y);
  return TVector3(TMath::Cos(phi),TMath::Sin(phi),0);
}

TVector3 KBGeoCircle::PointAtPhi(Double_t phi)
{
  return TVector3(TMath::Cos(phi),TMath::Sin(phi),fZ);
}

Double_t KBGeoCircle::Phi(Double_t x, Double_t y)
{
  x = x - fX;
  y = y - fY;
  return TMath::ATan2(y,x);
}
