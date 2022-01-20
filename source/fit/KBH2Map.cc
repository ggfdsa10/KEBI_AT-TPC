#include "TLine.h"
#include "TArrow.h"

#include "KBH2Map.hh"

#include <cmath>
#include <iostream>
using namespace std;


Int_t KBH2Point::NeighborPointID(Int_t ix, Int_t iy) 
{
  if (iy == -1) {
         if (ix ==-1) return 6;
    else if (ix == 0) return 3;
    else if (ix == 1) return 7;
  } else if (iy == 0) {
         if (ix ==-1) return 2;
    else if (ix == 1) return 0;
  } else if (iy == 1) {
         if (ix ==-1) return 5;
    else if (ix == 0) return 1;
    else if (ix == 1) return 4;
  }
  return -1;
}

void KBH2Point::Initialize()
{
  if (!fIsActive)
    return;

  for (auto id = 0; id < 4; id++)
    fVNeighbor[id] = fNeighborPoint[id] -> GetValue() - fValue;

  for (auto id = 4; id < 8; id++)
    fVNeighbor[id] = (fNeighborPoint[id] -> GetValue() - fValue);

  if (fVNeighbor[0] < 0 && fVNeighbor[1] < 0 && fVNeighbor[2] < 0 && fVNeighbor[3] < 0) {
    fVX = 0;
    fVY = 0;
  } else {
    auto sq2 = std::sqrt(2); 
    fVX = fVNeighbor[0] - fVNeighbor[2] + fVNeighbor[4]/sq2 - fVNeighbor[5]/sq2 - fVNeighbor[6]/sq2 + fVNeighbor[7]/sq2;
    fVY = fVNeighbor[1] - fVNeighbor[3] + fVNeighbor[4]/sq2 + fVNeighbor[5]/sq2 - fVNeighbor[6]/sq2 - fVNeighbor[7]/sq2;
  }
}

void KBH2Point::SetIsActive(bool active) { fIsActive = active; }
void KBH2Point::SetValue(Double_t v) { fValue = v; }
void KBH2Point::SetNeighborPoint(Int_t id, KBH2Point *point) { fNeighborPoint[id] = point; }

bool KBH2Point::IsActive() const { return fIsActive; }
Double_t KBH2Point::GetValue() const { return fValue; }
KBH2Point *KBH2Point::GetNeighborPoint(Int_t id) const { return fNeighborPoint[id]; }
Double_t KBH2Point::GetVNeibor(Int_t id) const { return fVNeighbor[id]; }

Double_t KBH2Point::GetVX() { return fVX; }
Double_t KBH2Point::GetVY() { return fVY; }

ClassImp(KBH2Map)

KBH2Map::KBH2Map(TH2* sample)
{
  SetSample(sample);
}

void KBH2Map::SetSample(TH2* sample)
{
  fSample = sample;

  fNBinsX = fSample -> GetXaxis() -> GetNbins();
  fNBinsY = fSample -> GetYaxis() -> GetNbins();

  fLowLimitX  = fSample -> GetXaxis() -> GetBinLowEdge(1);
  fHighLimitX = fSample -> GetXaxis() -> GetBinLowEdge(fNBinsX-1);
  fBinWidthX  = fSample -> GetXaxis() -> GetBinWidth(1);

  fLowLimitY  = fSample -> GetYaxis() -> GetBinLowEdge(1);
  fHighLimitY = fSample -> GetYaxis() -> GetBinLowEdge(fNBinsY-1);
  fBinWidthY  = fSample -> GetYaxis() -> GetBinWidth(1);

  fH2Points = new KBH2Point**[fNBinsX];
  for(auto ibinx = 0; ibinx < fNBinsX; ibinx++) {
    fH2Points[ibinx] = new KBH2Point*[fNBinsY];
    for(auto ibiny = 0; ibiny < fNBinsY; ibiny++) {
      fH2Points[ibinx][ibiny] = new KBH2Point();
      auto point = fH2Points[ibinx][ibiny];
      auto bin = fSample -> GetBin(ibinx+1, ibiny+1);
      point -> SetValue(fSample -> GetBinContent(bin));
    }
  }

  for(auto ibinx = 1; ibinx < fNBinsX-1; ibinx++) {
    for(auto ibiny = 1; ibiny < fNBinsY-1; ibiny++) {
      auto point = fH2Points[ibinx][ibiny];

      point -> SetNeighborPoint(0, fH2Points[ibinx+1][ibiny]);
      point -> SetNeighborPoint(1, fH2Points[ibinx][ibiny+1]);
      point -> SetNeighborPoint(2, fH2Points[ibinx-1][ibiny]);
      point -> SetNeighborPoint(3, fH2Points[ibinx][ibiny-1]);
      point -> SetNeighborPoint(4, fH2Points[ibinx+1][ibiny+1]);
      point -> SetNeighborPoint(5, fH2Points[ibinx-1][ibiny+1]);
      point -> SetNeighborPoint(6, fH2Points[ibinx-1][ibiny-1]);
      point -> SetNeighborPoint(7, fH2Points[ibinx+1][ibiny-1]);

      point -> SetIsActive(true);
    }
  }

  for(auto ibinx = 1; ibinx < fNBinsX-1; ibinx++) {
    for(auto ibiny = 1; ibiny < fNBinsY-1; ibiny++) {
      auto point = fH2Points[ibinx][ibiny];
      point -> Initialize();

      auto vx = point -> GetVX();
      auto vy = point -> GetVY();
      auto mag = std::sqrt(vx*vx + vy*vy);

      if (fMaxV < mag)
        fMaxV = mag;
    }
  }

  fBinWidthXY = std::sqrt(fBinWidthX*fBinWidthX + fBinWidthY*fBinWidthY);
}

TH2 *KBH2Map::GetSample() { return fSample; }

void KBH2Map::FitCurve(KBCurve* curve, Int_t itMax)
{
  bool drawBox = false;

  Double_t x, y;
  curve -> GetPoint(0, x, y);
  auto ibinxLow = fSample -> GetXaxis() -> FindBin(x) - 1;
  curve -> GetPoint(curve -> GetN() - 1, x, y);
  auto ibinxHigh = fSample -> GetXaxis() -> FindBin(x) - 1;

  if (ibinxLow < 1)
    ibinxLow = 1;
  if (ibinxHigh > fNBinsX-1)
    ibinxHigh = fNBinsX-1;

  if (drawBox) 
  {
    for(auto ibinx = ibinxLow; ibinx < ibinxHigh; ibinx++) 
    {
      auto xLE = fLowLimitX + fBinWidthX * (ibinx);
      auto xHE = fLowLimitX + fBinWidthX * (ibinx+1);
      auto yAtXLE = curve -> Eval(xLE);
      auto yAtXHE = curve -> Eval(xHE);
      auto yBinAtXLE = Int_t((yAtXLE - fLowLimitY)/fBinWidthY);
      auto yBinAtXHE = Int_t((yAtXHE - fLowLimitY)/fBinWidthY);

      if (yBinAtXLE > yBinAtXHE) {
        auto temp = yBinAtXLE;
        yBinAtXLE = yBinAtXHE;
        yBinAtXHE = temp;
      }
      
      auto numYBins = yBinAtXHE - yBinAtXLE + 1;
      for (auto iy = 0; iy < numYBins; iy++) {
        auto ibiny = yBinAtXLE + iy;
        if (ibiny < 1 || ibiny > fNBinsY-2)
          continue;
        DrawBox(ibinx, ibiny);
      }
    }
  }

  for (auto iter = 0; iter < itMax; iter++)
  {
    for(auto ibinx = ibinxLow; ibinx < ibinxHigh; ibinx++) 
    {
      auto binx = fLowLimitX + fBinWidthX * (0.5 + ibinx);

      auto xLE = fLowLimitX + fBinWidthX * (ibinx);
      auto xHE = fLowLimitX + fBinWidthX * (ibinx+1);
      auto yAtXLE = curve -> Eval(xLE);
      auto yAtXHE = curve -> Eval(xHE);
      auto yBinAtXLE = int((yAtXLE - fLowLimitY)/fBinWidthY);
      auto yBinAtXHE = int((yAtXHE - fLowLimitY)/fBinWidthY);

      //auto yAtCenterOfXBin = curve -> Eval((xLE+xHE)/2.);

      if (yBinAtXLE > yBinAtXHE) {
        auto temp = yBinAtXLE;
        yBinAtXLE = yBinAtXHE;
        yBinAtXHE = temp;
      }
      yBinAtXHE += 2;
      yBinAtXLE -= 2;
      
      auto numYBins = yBinAtXHE - yBinAtXLE + 1;
      for (auto iy = 0; iy < numYBins; iy++) {
        auto ibiny = yBinAtXLE + iy;
        if (ibiny < 1 || ibiny > fNBinsY-2)
          continue;

        auto biny = fLowLimitY + fBinWidthY * (0.5 + ibiny);

        //auto yAtYBin = fLowLimitY + fBinWidthY * (ibiny+.5);
        //auto dY = yAtYBin - yAtCenterOfXBin;

        auto point = fH2Points[ibinx][ibiny];
        //auto vx = point -> GetVX();
        auto vy = point -> GetVY();

        //curve -> Push(binx, biny, vx/fMaxV, vy/fMaxV);
        curve -> Push(binx, biny, 0, vy/fMaxV);
      }
    }
    //auto dx = vxSum * fBinWidthX / fMaxV;
    //auto dy = vySum * fBinWidthY / fMaxV;

    //function -> SetParameter(0, function -> GetParameter(0) + dx);
    //function -> SetParameter(1, function -> GetParameter(1) + dy);
  }
}

void KBH2Map::Draw(Option_t *opt) { fSample -> Draw(opt); }

void KBH2Map::DrawBox(Int_t ix, Int_t iy)
{
  auto xw = fBinWidthX+fLowLimitX;
  auto x1 = ix*fBinWidthX+fLowLimitX;
  auto x2 = x1 + xw;

  auto yw = fBinWidthY+fLowLimitY;
  auto y1 = iy*fBinWidthY+fLowLimitY;
  auto y2 = y1 + yw;

  cout << x1 << " " << y1 << endl;

  TLine *line[4];
  line[0] = new TLine(x1, y1, x2, y1);
  line[1] = new TLine(x1, y2, x2, y2);
  line[2] = new TLine(x1, y1, x1, y2);
  line[3] = new TLine(x2, y1, x2, y2);

  for (auto i = 0; i < 4; i++) {
    line[i] -> SetLineColor(kRed);
    line[i] -> Draw();
  }
}

void KBH2Map::DrawField(Double_t headSize, Double_t tailSize)
{
  for(auto ibinx = 1; ibinx < fNBinsX-1; ibinx++) {
    for(auto ibiny = 1; ibiny < fNBinsY-1; ibiny++) {
      auto point = fH2Points[ibinx][ibiny];
      auto vx = point -> GetVX();
      auto vy = point -> GetVY();

      auto binWidth = std::sqrt(fBinWidthX*fBinWidthX + fBinWidthX*fBinWidthX);

      auto mag = std::sqrt(vx*vx + vy*vy);
      auto scale = .2 * binWidth * std::log(mag) / std::log(fMaxV);

      vx *= scale / mag * tailSize;
      vy *= scale / mag * tailSize;

      auto arrowSize = .008 * scale * headSize;

      auto binx = fLowLimitX + fBinWidthX * (0.5 + ibinx);
      auto biny = fLowLimitY + fBinWidthY * (0.5 + ibiny);

      auto arrow = new TArrow(binx - vx, biny - vy, binx + vx, biny + vy, arrowSize, "|>");
      arrow -> Draw();
    }
  }
}

void KBH2Map::SetNBinsX(Int_t n)        { fNBinsX = n; }
void KBH2Map::SetLowLimitX(Double_t v)  { fLowLimitX = v; }
void KBH2Map::SetHighLimitX(Double_t v) { fHighLimitX = v; }
void KBH2Map::SetBinWidthX(Double_t v)  { fBinWidthX = v; }

Int_t    KBH2Map::GetNBinsX()      const { return fNBinsX; }
Double_t KBH2Map::GetLowLimitX()   const { return fLowLimitX; }
Double_t KBH2Map::GetHighLimitX()  const { return fHighLimitX; }
Double_t KBH2Map::GetBinWidthX()   const { return fBinWidthX; }

void KBH2Map::SetNBinsY(Int_t n)        { fNBinsY = n; }
void KBH2Map::SetLowLimitY(Double_t v)  { fLowLimitY = v; }
void KBH2Map::SetHighLimitY(Double_t v) { fHighLimitY = v; }
void KBH2Map::SetBinWidthY(Double_t v)  { fBinWidthY = v; }

Int_t    KBH2Map::GetNBinsY()      const { return fNBinsY; }
Double_t KBH2Map::GetLowLimitY()   const { return fLowLimitY; }
Double_t KBH2Map::GetHighLimitY()  const { return fHighLimitY; }
Double_t KBH2Map::GetBinWidthY()   const { return fBinWidthY; }

void KBH2Map::SetMaxV(Double_t v) { fMaxV = v; }
Double_t KBH2Map::GetMaxV() const { return fMaxV; }
