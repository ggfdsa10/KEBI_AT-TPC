#include "KBCurve.hh"
#include "KBGeoLine.hh"

#include <float.h>
#include <iostream>
using namespace std;

ClassImp(KBCurve)

KBCurve::KBCurve()
:TGraph()
{
}

KBCurve::KBCurve(Int_t nPoints)
:TGraph(nPoints)
{
}

void KBCurve::SetUnitLength(Double_t unitLength) { fUnitLength = unitLength; }
Double_t KBCurve::GetUnitLength() { return fUnitLength; }

void KBCurve::SetTension(Double_t tension) { fTension = tension; }
Double_t KBCurve::GetTension() { return fTension; }

void KBCurve::Push(Double_t x, Double_t y, Double_t /*fx*/, Double_t fy)
{
  for (auto iPoint = 0; iPoint < fNpoints; ++iPoint) {
    auto dx = x - fX[iPoint];
    auto dy = y - fY[iPoint];
    auto dr = sqrt(dx * dx + dy * dy);

    //fX[iPoint] += fx/dr;
    fY[iPoint] += fy/dr;
  }

  for (auto iPoint = 1; iPoint < fNpoints-1; ++iPoint) {
    auto x0 = fX[iPoint];
    auto y0 = fY[iPoint];
    auto xb = fX[iPoint-1];
    auto yb = fY[iPoint-1];
    auto xa = fX[iPoint+1];
    auto ya = fY[iPoint+1];

    auto dxb = xb - x0;
    auto dyb = yb - y0;
    auto dxa = xa - x0;
    auto dya = ya - y0;

    auto drb = sqrt(dxb*dxb + dyb*dyb);
    auto dra = sqrt(dxa*dxa + dya*dya);

    dxb = dxb*drb/fUnitLength*fTension;
    dyb = dyb*drb/fUnitLength*fTension;
    dxa = dxa*dra/fUnitLength*fTension;
    dya = dya*dra/fUnitLength*fTension;

    //fX[iPoint] += (dxb + dxa);
    fY[iPoint] += (dyb + dya);
  }

  /*
  auto iPoint = 0;
  {
    auto x = fX[iPoint];
    auto y = fY[iPoint];
    auto xa = fX[iPoint+1];
    auto ya = fY[iPoint+1];

    auto dxa = xa - x;
    auto dya = ya - y;

    auto dra = sqrt(dxa*dxa + dya*dya);

    dxa = dxa*dra/fUnitLength*fTension;
    dya = dya*dra/fUnitLength*fTension;

    //fX[iPoint] += (dxa);
    fY[iPoint] += (dya);
  }
  
  iPoint = fNpoints-1;
  {
    auto x = fX[iPoint];
    auto y = fY[iPoint];
    auto xb = fX[iPoint-1];
    auto yb = fY[iPoint-1];

    auto dxb = xb - x;
    auto dyb = yb - y;

    auto drb = sqrt(dxb*dxb + dyb*dyb);

    dxb = dxb*drb/fUnitLength*fTension;
    dyb = dyb*drb/fUnitLength*fTension;

    fY[iPoint] += (dyb);
  }
  */
}

KBCurve *KBCurve::GetDifferentialCurve()
{
  auto diff = new KBCurve(fNpoints-1);

  for (auto iPoint = 0; iPoint < fNpoints-1; ++iPoint) {
    auto xMid = (fX[iPoint] + fX[iPoint+1])/2;
    auto dx = fX[iPoint] - fX[iPoint+1];
    auto dy = fY[iPoint] - fY[iPoint+1];
    diff -> SetPoint(iPoint, xMid, dy/dx);
  }

  return diff;
}

Double_t KBCurve::DistanceToCurve(Double_t x, Double_t y)
{
  Double_t drSumMin = DBL_MAX;
  Int_t idxAtMin = -1;

  for (auto iPoint = 0; iPoint < fNpoints-1; ++iPoint) {
    auto x1 = fX[iPoint];
    auto y1 = fY[iPoint];
    auto dr1 = sqrt((x1-x)*(x1-x)+(y1-y)*(y1-y));

    auto x2 = fX[iPoint+1];
    auto y2 = fY[iPoint+1];
    auto dr2 = sqrt((x2-x)*(x2-x)+(y2-y)*(y2-y));
    
    if (dr1+ dr2 < drSumMin) {
      drSumMin = dr1 + dr2;
      idxAtMin = iPoint;
    }
  }

  KBGeoLine line(fX[idxAtMin],fY[idxAtMin],0,fX[idxAtMin+1],fY[idxAtMin+1],0); 
  return line.DistanceToLine(x,y,0);
}
