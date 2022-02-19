#include "LHPadPlaneRPad.hh"

#include "TGraph.h"
#include "TMath.h"
#include "TVector2.h"
#include "TH2Poly.h"
#include "TCollection.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TCanvas.h"

#include <iostream>
using namespace std;

ClassImp(LHPadPlaneRPad)

LHPadPlaneRPad::LHPadPlaneRPad()
:KBPadPlane("PadPlaneR", "pad plane with rectangular pads for LAMPS-TPC")
{
}

bool LHPadPlaneRPad::Init()
{
  fRMin = fPar -> GetParDouble("TPCrMin");
  fRMax = fPar -> GetParDouble("TPCrMax");
  fPadGap = fPar -> GetParDouble("TPCPadGap");
  fPadWid = fPar -> GetParDouble("TPCPadWidth");
  fPadHei = fPar -> GetParDouble("TPCPadHeight");
  fYPPMin = fPar -> GetParDouble("TPCYPPMin");
  if (fYPPMin+.5*fPadHei > fRMin) {
    kb_error << "'YPPMin' + 'fPadHei'/2 should be larger than 'rMinTPC'!  :  " << fYPPMin+.5*fPadHei << " < " << fRMin << endl;
    return false;
  }
  fYPPMax = fPar -> GetParDouble("TPCYPPMax");
  fWPPBot = fPar -> GetParDouble("TPCWPPBottom");
  fPadAreaLL = fPar -> GetParDouble("TPCPadAreaLL");
  fRemoveCuttedPad = fPar -> GetParBool("TPCremoveCuttedPad");

  fTanPi1o8 = TMath::Tan(TMath::Pi()*1./8.);
  fTanPi3o8 = TMath::Tan(TMath::Pi()*3./8.);
  fTanPi5o8 = TMath::Tan(TMath::Pi()*5./8.);
  fTanPi7o8 = TMath::Tan(TMath::Pi()*7./8.);
  for (Int_t i = 0; i < 8; i++) {
    fCosPiNo4[i] = TMath::Cos(TMath::Pi()*i/4.);
    fSinPiNo4[i] = TMath::Sin(TMath::Pi()*i/4.);
  }

  fFuncXRightBound
    = new TF1("RightBound",Form("%f*(x-%f)+%f",fTanPi3o8,.5*fWPPBot,fYPPMin),0,500);
  fFuncXRightBoundInverse
    = new TF1("RightBoundInverse",Form("(x-%f)/%f+%f",fYPPMin,fTanPi3o8,.5*fWPPBot),0,500);

  fXSpacing = fPadGap + fPadWid;
  fYSpacing = fPadGap + fPadHei;

  for (Int_t section = 0; section < 8; section++)
  {
    Double_t phiSection = section * TMath::Pi()/4.;

    for (Int_t layer = 0; true; ++layer) 
    {
      auto yPadBot = fYPPMin+fYSpacing*layer;
      if (yPadBot > fYPPMax) {
        fLayerMax = layer-1;
        break;
      }

      bool cuttedTop = false;
      auto yPadTop = fYPPMin+fPadHei+fYSpacing*layer;
      if (yPadTop > fYPPMax) {
        yPadTop = fYPPMax;
        cuttedTop = true;
      }

      auto breakFromRow = false;
      for (Int_t row = 1; true; ++row)
      {
        if (breakFromRow)
          break;

        for (auto pm : {1, -1}) // right-half and left-half
        {
          auto xPadInn = .5*fPadGap+fXSpacing*(row-1);
          auto xPadOut = .5*fPadGap+fXSpacing*(row-1)+fPadWid;

          bool cuttedTo3 = false;
          bool cuttedTo4 = false;
          bool cuttedTo5 = false;

          if (yPadTop < fFuncXRightBound->Eval(xPadInn)) {
            if (section == 0)
              fHalfRowMax.push_back(row-1);
            breakFromRow = true;
            break;
          }
          else if (yPadTop < fFuncXRightBound->Eval(xPadOut)) cuttedTo3 = true;
          else if (yPadBot < fFuncXRightBound->Eval(xPadInn)) cuttedTo4 = true;
          else if (yPadBot < fFuncXRightBound->Eval(xPadOut)) cuttedTo5 = true;

          if (fRemoveCuttedPad && (cuttedTo3 || cuttedTo4 || cuttedTo5)) {
            if (section == 0)
              fHalfRowMax.push_back(row-1);
            breakFromRow = true;
            break;
          }

          xPadInn = pm*xPadInn;
          xPadOut = pm*xPadOut;

          TVector2 posCenter(.5*(xPadInn+xPadOut), .5*(yPadBot+yPadTop));
          auto r = sqrt(posCenter.X()*posCenter.X() + posCenter.Y()*posCenter.Y());
          if (r>fRMax) {
            if (section == 0)
              fHalfRowMax.push_back(row-1);
            breakFromRow = true;
            break;
          }
          posCenter = posCenter.Rotate(phiSection);

          TVector2 posCorner0(xPadInn, yPadTop);
          if (sqrt(xPadInn*xPadInn + yPadTop*yPadTop) > fRMax)
            posCorner0.Set(xPadInn,sqrt(fRMax*fRMax-xPadInn*xPadInn));
          posCorner0 = posCorner0.Rotate(phiSection);
          if (cuttedTo3)
          {
            TVector2 posCorner1(xPadInn, fFuncXRightBound->Eval(pm*xPadInn));
            posCorner1 = posCorner1.Rotate(phiSection);

            TVector2 posCorner2(pm*fFuncXRightBoundInverse->Eval(yPadTop), yPadTop);
            posCorner2 = posCorner2.Rotate(phiSection);
            auto d01 = posCorner0 - posCorner1;
            auto d02 = posCorner0 - posCorner2;
            auto a = d01.X()*d01.X() + d01.Y()*d01.Y();
            auto b = d02.X()*d02.X() + d02.Y()*d02.Y();
            if (.5*a*b < fPadAreaLL) {
              if (section == 0)
                fHalfRowMax.push_back(row-1);
              breakFromRow = true;
              break;
            }

            auto pad = NewPad(section, pm*row, layer);
            pad -> AddPadCorner(posCorner0.X(), posCorner0.Y());
            pad -> AddPadCorner(posCorner1.X(), posCorner1.Y());
            pad -> AddPadCorner(posCorner2.X(), posCorner2.Y());

            auto xCenter = (posCorner0.X()+posCorner1.X()+posCorner2.X())/3.;
            auto yCenter = (posCorner0.Y()+posCorner1.Y()+posCorner2.Y())/3.;
            pad -> SetPosition(xCenter, yCenter);

            continue;
          }

          TVector2 posCorner1(xPadOut, yPadTop);
          if (sqrt(xPadOut*xPadOut + yPadTop*yPadTop) > fRMax)
            posCorner1.Set(xPadOut,sqrt(fRMax*fRMax-xPadOut*xPadOut));
          posCorner1 = posCorner1.Rotate(phiSection);
          if (cuttedTo4)
          {
            TVector2 posCorner2(xPadOut, fFuncXRightBound->Eval(pm*xPadOut));
            posCorner2 = posCorner2.Rotate(phiSection);

            TVector2 posCorner3(xPadInn, fFuncXRightBound->Eval(pm*xPadInn));
            posCorner3 = posCorner3.Rotate(phiSection);

            auto pad = NewPad(section, pm*row, layer);
            pad -> AddPadCorner(posCorner0.X(), posCorner0.Y());
            pad -> AddPadCorner(posCorner1.X(), posCorner1.Y());
            pad -> AddPadCorner(posCorner2.X(), posCorner2.Y());
            pad -> AddPadCorner(posCorner3.X(), posCorner3.Y());

            auto xCenter = (posCorner0.X()+posCorner1.X()+posCorner2.X()+posCorner3.X())/4.;
            auto yCenter = (posCorner0.Y()+posCorner1.Y()+posCorner2.Y()+posCorner3.Y())/4.;
            pad -> SetPosition(xCenter, yCenter);

            continue;
          }

          if (cuttedTo5)
          {
            TVector2 posCorner2(xPadOut, fFuncXRightBound->Eval(pm*xPadOut));
            posCorner2 = posCorner2.Rotate(phiSection);

            TVector2 posCorner3(pm*fFuncXRightBoundInverse->Eval(yPadBot), yPadBot);
            posCorner3 = posCorner3.Rotate(phiSection);

            TVector2 posCorner4(xPadInn, yPadBot);
            if (sqrt(xPadInn*xPadInn + yPadBot*yPadBot) < fRMin)
              posCorner4.Set(xPadInn,sqrt(fRMin*fRMin-xPadInn*xPadInn));
            posCorner4 = posCorner4.Rotate(phiSection);

            auto pad = NewPad(section, pm*row, layer);
            pad -> AddPadCorner(posCorner0.X(), posCorner0.Y());
            pad -> AddPadCorner(posCorner1.X(), posCorner1.Y());
            pad -> AddPadCorner(posCorner2.X(), posCorner2.Y());
            pad -> AddPadCorner(posCorner3.X(), posCorner3.Y());
            pad -> AddPadCorner(posCorner4.X(), posCorner4.Y());

            auto xCenter = (posCorner0.X()+posCorner1.X()+posCorner2.X()+posCorner3.X()+posCorner4.X())/5.;
            auto yCenter = (posCorner0.Y()+posCorner1.Y()+posCorner2.Y()+posCorner3.Y()+posCorner4.Y())/5.;
            pad -> SetPosition(xCenter, yCenter);
          }
          else // normal rectangular pad
          {
            TVector2 posCorner2(xPadOut, yPadBot);
            if (sqrt(xPadOut*xPadOut + yPadBot*yPadBot) < fRMin)
              posCorner2.Set(xPadOut,sqrt(fRMin*fRMin-xPadOut*xPadOut));
            posCorner2 = posCorner2.Rotate(phiSection);

            TVector2 posCorner3(xPadInn, yPadBot);
            if (sqrt(xPadInn*xPadInn + yPadBot*yPadBot) < fRMin)
              posCorner3.Set(xPadInn,sqrt(fRMin*fRMin-xPadInn*xPadInn));
            posCorner3 = posCorner3.Rotate(phiSection);

            auto pad = NewPad(section, pm*row, layer);
            pad -> AddPadCorner(posCorner0.X(), posCorner0.Y());
            pad -> AddPadCorner(posCorner1.X(), posCorner1.Y());
            pad -> AddPadCorner(posCorner2.X(), posCorner2.Y());
            pad -> AddPadCorner(posCorner3.X(), posCorner3.Y());

            auto xCenter = (posCorner0.X()+posCorner1.X()+posCorner2.X()+posCorner3.X())/4.;
            auto yCenter = (posCorner0.Y()+posCorner1.Y()+posCorner2.Y()+posCorner3.Y())/4.;
            pad -> SetPosition(xCenter, yCenter);
          }
        }
      }
    }
  }

  fChannelArray -> Sort();

  Int_t numPads = fChannelArray -> GetEntriesFast();
  for (Int_t padID = 0; padID < numPads; ++padID) {
    auto pad = (KBPad *) fChannelArray -> At(padID);
    pad -> SetPadID(padID);

    std::vector<Int_t> key;
    key.push_back(pad -> GetSection());
    key.push_back(pad -> GetRow());
    key.push_back(pad -> GetLayer());
    fPadMap.insert(std::pair<std::vector<Int_t>, Int_t>(key,padID));
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer <= fLayerMax; iLayer++) {
      Int_t nHalfRows = fHalfRowMax[iLayer];
      for (Int_t iRow = -nHalfRows; iRow < nHalfRows; iRow++) {
        if (iRow == 0)
          continue;

        std::vector<Int_t> key0;
        key0.push_back(iSection);
        key0.push_back(iRow);
        key0.push_back(iLayer);
        auto pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

        Int_t row1 = iRow+1;
        if (iRow == -1)
          row1 = 1;

        std::vector<Int_t> key1;
        key1.push_back(iSection);
        key1.push_back(row1);
        key1.push_back(iLayer);
        auto pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);
        SetNeighborPads(pad0, pad1);
      }
    }
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer <= fLayerMax-1; iLayer++) {
      Int_t nHalfRows0 = fHalfRowMax[iLayer];
      Int_t nHalfRows1 = fHalfRowMax[iLayer+1];
      for (Int_t iRow = -nHalfRows0; iRow <= nHalfRows0; iRow++) {
        if (iRow == 0)
          continue;

        std::vector<Int_t> key0;
        key0.push_back(iSection);
        key0.push_back(iRow);
        key0.push_back(iLayer);
        auto pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

        if (iRow<=nHalfRows1 && iRow>=-nHalfRows1) {
          std::vector<Int_t> key1;
          key1.push_back(iSection);
          key1.push_back(iRow);
          key1.push_back(iLayer+1);
          auto pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);
          SetNeighborPads(pad0, pad1);
        }

        auto nextRow = iRow+1 == 0 ?  1 : iRow+1;
        if (nextRow<=nHalfRows1 && nextRow>=-nHalfRows1) {
          std::vector<Int_t> key2;
          key2.push_back(iSection);
          key2.push_back(nextRow);
          key2.push_back(iLayer+1);
          auto pad2 = (KBPad *) fChannelArray -> At(fPadMap[key2]);
          SetNeighborPads(pad0, pad2);
        }

        auto prevRow = iRow-1 == 0 ? -1 : iRow-1;
        if (prevRow<=nHalfRows1 && prevRow>=-nHalfRows1) {
          std::vector<Int_t> key2;
          key2.push_back(iSection);
          key2.push_back(prevRow);
          key2.push_back(iLayer+1);
          auto pad2 = (KBPad *) fChannelArray -> At(fPadMap[key2]);
          SetNeighborPads(pad0, pad2);
        }
      }
    }
  }

  for (Int_t iLayer = 0; iLayer <= fLayerMax; iLayer++) {
    Int_t nHalfRows = fHalfRowMax[iLayer];
    if (iLayer != 0 && fHalfRowMax[iLayer-1] > nHalfRows)
      break;
    for (Int_t iSection = 0; iSection < 8; iSection++) {
      std::vector<Int_t> key0;
      key0.push_back(iSection);
      key0.push_back(nHalfRows);
      key0.push_back(iLayer);
      auto pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

      Int_t section1 = iSection-1;
      if (iSection == 0)
        section1 = 7;

      std::vector<Int_t> key1;
      key1.push_back(section1);
      key1.push_back(-nHalfRows);
      key1.push_back(iLayer);
      auto pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);

      SetNeighborPads(pad0, pad1);
    }
  }

  return true;
}

Int_t LHPadPlaneRPad::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  if (layer < 0 || layer >= fLayerMax)
    return -1;

  if (row == 0 || std::abs(row) > fHalfRowMax[layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Int_t LHPadPlaneRPad::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i,j);

  Double_t xRot =  i*fCosPiNo4[section] + j*fSinPiNo4[section];
  Double_t yRot = -i*fSinPiNo4[section] + j*fCosPiNo4[section];

  Double_t yFromMin = yRot - fYPPMin;
  if (yFromMin < 0)
    return -1;

  Int_t layer = (Int_t)(yFromMin/fYSpacing);
  if (yFromMin - layer*fYSpacing > fPadHei)
    return -1;

  Int_t pm = 1;
  if (xRot < 0) {
    xRot = -xRot;
    pm = -1;
  }

  if (xRot < .5*fPadGap)
    return -1;

  Double_t xFromMin = xRot - .5*fPadGap;
  Int_t row = (Int_t)(xFromMin/fXSpacing)+1;
  if (xFromMin - (row-1)*fXSpacing > fPadWid)
    return -1;

  if (row > fHalfRowMax[layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(pm*row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Double_t LHPadPlaneRPad::PadDisplacement() const
{
  return sqrt(fXSpacing*fXSpacing + fYSpacing*fYSpacing);
}

bool LHPadPlaneRPad::IsInBoundary(Double_t i, Double_t j)
{
  Double_t r = TMath::Sqrt(i*i+j*j);
  if (r < fRMin || r > fRMax)
    return false;

  return true;
}

TH2* LHPadPlaneRPad::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane; 

  TH2Poly *h2 = new TH2Poly();

  Double_t xPoints[6] = {0};
  Double_t yPoints[6] = {0};

  KBPad *pad;
  TIter iterPads(fChannelArray);
  Int_t selectSection = TString(option).Atoi();
  while ((pad = (KBPad *) iterPads.Next())) 
  {
    if (selectSection != -1 && selectSection != pad -> GetSection())
      continue;

    if (pad -> GetPadID() == -1)
      continue;

    auto corners = pad -> GetPadCorners();

    Int_t numPoints = corners->size();
    for (auto iCorner = 0; iCorner < numPoints; ++iCorner)
    {
      TVector2 corner = corners->at(iCorner);
      xPoints[iCorner] = corner.X();
      yPoints[iCorner] = corner.Y();
    }
    TVector2 corner = corners->at(0);
    xPoints[numPoints] = corner.X();
    yPoints[numPoints] = corner.Y();

    h2 -> AddBin(numPoints+1, xPoints, yPoints);
  }

  if (true) {
    TList *bins = h2->GetBins(); TIter Next(bins);
    TObject *obj;
    TH2PolyBin *b;
    TGraph *g;
    while ((obj = Next())) {
      b = (TH2PolyBin*)obj;
      g = (TGraph*)b->GetPolygon();
      if (g) g->SetLineWidth(1);
      if (g) g->SetLineColor(kGray+2);
    }
  }

  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(";x (mm); y (mm)");
  fH2Plane -> GetXaxis() -> CenterTitle();
  fH2Plane -> GetYaxis() -> CenterTitle();

  return fH2Plane;
}

void LHPadPlaneRPad::DrawFrame(Option_t *)
{
  Color_t lineColor = kGray;

  TEllipse *circleOut = new TEllipse(0, 0, fRMax, fRMax);
  circleOut -> SetFillStyle(0);
  circleOut -> SetLineColor(lineColor);
  circleOut -> Draw("samel");

  TEllipse *circleIn = new TEllipse(0, 0, fRMin, fRMin);
  circleIn -> SetFillStyle(0);
  circleIn -> SetLineColor(lineColor);
  circleIn -> Draw("samel");

  TLine* line1 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*1/8),  fRMax*TMath::Sin(TMath::Pi()*1/8),
                fRMin*TMath::Cos(TMath::Pi()*1/8),  fRMin*TMath::Sin(TMath::Pi()*1/8));
  TLine* line2 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*3/8),  fRMax*TMath::Sin(TMath::Pi()*3/8),
                fRMin*TMath::Cos(TMath::Pi()*3/8),  fRMin*TMath::Sin(TMath::Pi()*3/8));
  TLine* line3 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*5/8),  fRMax*TMath::Sin(TMath::Pi()*5/8),
                fRMin*TMath::Cos(TMath::Pi()*5/8),  fRMin*TMath::Sin(TMath::Pi()*5/8));
  TLine* line4 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*7/8),  fRMax*TMath::Sin(TMath::Pi()*7/8),
                fRMin*TMath::Cos(TMath::Pi()*7/8),  fRMin*TMath::Sin(TMath::Pi()*7/8));
  TLine* line5 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*9/8),  fRMax*TMath::Sin(TMath::Pi()*9/8),
                fRMin*TMath::Cos(TMath::Pi()*9/8),  fRMin*TMath::Sin(TMath::Pi()*9/8));
  TLine* line6 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*11/8), fRMax*TMath::Sin(TMath::Pi()*11/8),
                fRMin*TMath::Cos(TMath::Pi()*11/8), fRMin*TMath::Sin(TMath::Pi()*11/8));
  TLine* line7 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*13/8), fRMax*TMath::Sin(TMath::Pi()*13/8),
                fRMin*TMath::Cos(TMath::Pi()*13/8), fRMin*TMath::Sin(TMath::Pi()*13/8));
  TLine* line8 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*15/8), fRMax*TMath::Sin(TMath::Pi()*15/8),
                fRMin*TMath::Cos(TMath::Pi()*15/8), fRMin*TMath::Sin(TMath::Pi()*15/8));

  line1 -> SetLineColor(lineColor);
  line2 -> SetLineColor(lineColor);
  line3 -> SetLineColor(lineColor);
  line4 -> SetLineColor(lineColor);
  line5 -> SetLineColor(lineColor);
  line6 -> SetLineColor(lineColor);
  line7 -> SetLineColor(lineColor);
  line8 -> SetLineColor(lineColor);

  line1 -> Draw("samel");
  line2 -> Draw("samel");
  line3 -> Draw("samel");
  line4 -> Draw("samel");
  line5 -> Draw("samel");
  line6 -> Draw("samel");
  line7 -> Draw("samel");
  line8 -> Draw("samel");
}

TCanvas *LHPadPlaneRPad::GetCanvas(Option_t *)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,890,750);
  fCanvas -> SetMargin(0.13,0.13,0.08,0.02);

  return fCanvas;
}

KBPad *LHPadPlaneRPad::NewPad(Int_t s, Int_t r, Int_t l)
{
  auto pad = new KBPad();
  pad -> SetSectionRowLayer(s, r, l);
  fChannelArray -> Add(pad);
  return pad;
}

void LHPadPlaneRPad::SetNeighborPads(KBPad *pad0, KBPad *pad1)
{
  pad0 -> AddNeighborPad(pad1);
  pad1 -> AddNeighborPad(pad0);
}

Int_t LHPadPlaneRPad::FindSection(Double_t i, Double_t j)
{
  if (j > fTanPi3o8*i) {
    if (j > fTanPi1o8*i) {
      if (j > fTanPi7o8*i) {
        if (j > fTanPi5o8*i) {
          return 0;
        } else return 1;
      } else return 2;
    } else return 3;
  }
  else
  {
    if (j < fTanPi1o8*i) {
      if (j < fTanPi7o8*i) {
        if (j < fTanPi5o8*i) {
          return 4;
        } else return 5;
      } else return 6;
    } else return 7;
  }
}
