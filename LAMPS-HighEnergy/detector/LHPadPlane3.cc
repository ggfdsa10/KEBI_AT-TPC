#include "LHPadPlane3.hh"

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

ClassImp(LHPadPlane3)

LHPadPlane3::LHPadPlane3()
:KBPadPlane("PadPlane3", "pad plane with rectangular pads following the circle line for LAMPS-TPC")
{
}

bool LHPadPlane3::Init()
{
  if (fPar -> CheckPar("rMinTPC"))      fRMin         = fPar -> GetParDouble("rMinTPC");
  if (fPar -> CheckPar("rMaxTPC"))      fRMax         = fPar -> GetParDouble("rMaxTPC");
  if (fPar -> CheckPar("padGap"))       fPadGap       = fPar -> GetParDouble("padGap");
  if (fPar -> CheckPar("rTopCut"))      fRTopCut      = fPar -> GetParDouble("rTopCut");
  if (fPar -> CheckPar("padWidth"))     fPadWidth     = fPar -> GetParDouble("padWidth");
  if (fPar -> CheckPar("padHeight"))    fPadHeight    = fPar -> GetParDouble("padHeight");
  if (fPar -> CheckPar("radiusLayer0")) fRadiusLayer0 = fPar -> GetParDouble("radiusLayer0");
  if (fPar -> CheckPar("numLayers"))    fNumLayers    = fPar -> GetParInt("numLayers");

  fXSpacing = fPadGap + fPadWidth;
  fRSpacing = fPadGap + fPadHeight;

  fTanPi1o8 = TMath::Tan(TMath::Pi()*1./8.);
  fTanPi3o8 = TMath::Tan(TMath::Pi()*3./8.);
  fTanPi5o8 = TMath::Tan(TMath::Pi()*5./8.);
  fTanPi7o8 = TMath::Tan(TMath::Pi()*7./8.);
  for (Int_t i = 0; i < 8; i++) {
    fCosPiNo4[i] = TMath::Cos(TMath::Pi()*i/4.);
    fSinPiNo4[i] = TMath::Sin(TMath::Pi()*i/4.);
  }

  Double_t xCorner[5] = {0};
  Double_t yCorner[5] = {0};

  for (int layer=0; layer<fNumLayers; ++layer)
  {
    double radius = fRadiusLayer0 + layer*fRSpacing;
    int isInnerPad = true;

    KBPad *padAtCurrentRow[8][2]; // section, R/L
    KBPad *padAtPreviousRow[8][2]; // section, R/L

    int row = 0;
    for (int iRow=1; isInnerPad; ++iRow)
    {
      double xPad = (iRow -.5) * fXSpacing;
      double yPad = sqrt(radius*radius - xPad*xPad);


      if (yPad > fRTopCut) {
        fNumSkippedHalfRows[layer]++;
        continue;
      }
      else
        row++;

      if (yPad < GetPadCenterYBoundAtX(xPad)) {
        fNumHalfRowsInLayer[layer] = row;
        isInnerPad = false;
      }

      ///////////////////////////////////////////////////////////////////

      int numCorners = 4;
      xCorner[0] = xPad + .5*fPadWidth; yCorner[0] = yPad + .5*fPadHeight;
      xCorner[1] = xPad - .5*fPadWidth; yCorner[1] = yPad + .5*fPadHeight;
      xCorner[2] = xPad - .5*fPadWidth; yCorner[2] = yPad - .5*fPadHeight;
      xCorner[3] = xPad + .5*fPadWidth; yCorner[3] = yPad - .5*fPadHeight;

      bool sideBoundaryCutWasMade = false;

      if (fDoCutSideBoundary && (yCorner[3] < GetPadCutBoundaryYAtX(xCorner[3]))) {
        sideBoundaryCutWasMade = true;
        if (xCorner[0] > GetPadCutBoundaryXAtY(yCorner[0])) {
          xCorner[0] = GetPadCutBoundaryXAtY(yCorner[0]);
          yCorner[2] = GetPadCutBoundaryYAtX(xCorner[2]);
          numCorners = 3;
        }
        else if (yCorner[2] < GetPadCutBoundaryYAtX(xCorner[2])) {
          yCorner[2] = GetPadCutBoundaryYAtX(xCorner[2]);
          yCorner[3] = GetPadCutBoundaryYAtX(xCorner[3]);
        }
        else if (yCorner[3] < GetPadCutBoundaryYAtX(xCorner[3])) {
          xCorner[3] = GetPadCutBoundaryXAtY(yCorner[3]);
          xCorner[4] = xCorner[0];
          yCorner[4] = GetPadCutBoundaryYAtX(xCorner[4]);
          numCorners = 5;
        }
      }
      if (fDoCutTopBoundary && yCorner[0] > fRTopCut) {
        yCorner[0] = fRTopCut;
        yCorner[1] = fRTopCut;
      }

      ///////////////////////////////////////////////////////////////////

      for (auto section=0; section<8; ++section)
      {
        Double_t phiSection = section * TMath::Pi()/4.;

        TVector2 point(xPad, yPad);
        point = point.Rotate(phiSection);

        for (int iRL : {0,1})
        {
          int signRL = iRL==0?1:-1;
          auto pad = NewPad(section, signRL*row, layer);
          double xSum = 0;
          double ySum = 0;
          for (auto iC=0; iC<numCorners; ++iC) {
            point = TVector2(signRL*xCorner[iC],yCorner[iC]);
            point = point.Rotate(phiSection);
            pad -> AddPadCorner(point.X(), point.Y());
            xSum += point.X();
            ySum += point.Y();
          }
          pad -> SetPosition(xSum/numCorners, ySum/numCorners);
          padAtCurrentRow[section][iRL] = pad;
        }
      }

      if (sideBoundaryCutWasMade) {
        SetNeighborPads(padAtCurrentRow[0][1], padAtCurrentRow[1][0]);
        SetNeighborPads(padAtCurrentRow[1][1], padAtCurrentRow[2][0]);
        SetNeighborPads(padAtCurrentRow[2][1], padAtCurrentRow[3][0]);
        SetNeighborPads(padAtCurrentRow[3][1], padAtCurrentRow[4][0]);
        SetNeighborPads(padAtCurrentRow[4][1], padAtCurrentRow[5][0]);
        SetNeighborPads(padAtCurrentRow[5][1], padAtCurrentRow[6][0]);
        SetNeighborPads(padAtCurrentRow[6][1], padAtCurrentRow[7][0]);
        SetNeighborPads(padAtCurrentRow[7][1], padAtCurrentRow[0][0]);
      }

      if (row==1) {
        // row=-1 and row=1 are neighbor
        if (fNumSkippedHalfRows[layer]==0)
          for (auto section=0; section<8; ++section)
            SetNeighborPads(padAtCurrentRow[section][0], padAtCurrentRow[section][1]);
      }
      else {
        // [row] and [row-1] are neighbor
        for (auto section=0; section<8; ++section)
          for (int iRL : {0,1})
            SetNeighborPads(padAtPreviousRow[section][iRL],padAtCurrentRow[section][iRL]);
      }

      for (auto section=0; section<8; ++section)
        for (int iRL : {0,1})
          padAtPreviousRow[section][iRL] = padAtCurrentRow[section][iRL];
    }
  }

  for (auto iLayer=fNumLayers-1; iLayer>=0; --iLayer) {
    fNumPadsDownToLayer[iLayer] = 2*fNumHalfRowsInLayer[iLayer] + fNumPadsDownToLayer[iLayer+1];
  }

  fChannelArray -> Sort(); // see KBPad::Compare();

  for (auto i=0; i<fChannelArray->GetEntries(); ++i)
  {
    auto pad = (KBPad *) fChannelArray -> At(i);
    pad -> SetPadID(i);
  }

  for (auto section=0; section<8; ++section) {
    for (auto layer=0; layer<fNumLayers; ++layer) {
      for (auto row=1; row<=fNumHalfRowsInLayer[layer]; ++row)
      {
        auto padL = GetPad(section,-row,layer);
        auto padR = GetPad(section,+row,layer);

        // pad below
        if (layer>0) {
          auto rowNb = row + fNumSkippedHalfRows[layer] - fNumSkippedHalfRows[layer-1];
          if (rowNb<=0||rowNb>fNumHalfRowsInLayer[layer-1]) continue;
          auto padBelowL = GetPad(section,-rowNb,layer-1);
          auto padBelowR = GetPad(section,+rowNb,layer-1);
          SetNeighborPads(padL,padBelowL);
          SetNeighborPads(padR,padBelowR);
        }
      }
    }
  }

  return true;
}

Int_t LHPadPlane3::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  int idLayer = section*fNumPadsDownToLayer[layer];
  if (layer<fNumLayers) idLayer = idLayer + (8-section)*fNumPadsDownToLayer[layer+1];
  int idRow = ((row>0) ? (fNumHalfRowsInLayer[layer] - row) : (fNumHalfRowsInLayer[layer] - row - 1));
  int id = idLayer + idRow;

  return id;
}

Int_t LHPadPlane3::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i,j);

  Double_t xRotatedToSec0 =  i*fCosPiNo4[section] + j*fSinPiNo4[section];
  Double_t yRotatedToSec0 = -i*fSinPiNo4[section] + j*fCosPiNo4[section];
  Double_t rRotatedToSec0 =  sqrt(xRotatedToSec0*xRotatedToSec0 + yRotatedToSec0*yRotatedToSec0);

  Double_t rFromSectionBottom = rRotatedToSec0 - fRadiusLayer0 + .5*fPadHeight;
  if (rFromSectionBottom < 0)
    return -1;

  Int_t layer = (Int_t)(rFromSectionBottom/fRSpacing);
  if (layer > fNumLayers)
    return -1;

  Int_t pm = 1;
  if (xRotatedToSec0 < 0) {
    xRotatedToSec0 = -xRotatedToSec0;
    pm = -1;
  }

  if (xRotatedToSec0 < .5*fPadGap)
    return -1;

  Double_t xFromRow0LeftEdge = xRotatedToSec0 - .5*fPadGap;
  Int_t row = (Int_t) (xFromRow0LeftEdge / fXSpacing) + 1;
  if (xFromRow0LeftEdge - (row-1)*fXSpacing > fPadWidth)
    return -1;

  row = row - fNumSkippedHalfRows[layer];

  if (row > fNumHalfRowsInLayer[layer])
    return -1;

  return FindPadID(section,pm*row,layer);
}

Double_t LHPadPlane3::PadDisplacement() const
{
  return sqrt(fXSpacing*fXSpacing + fRSpacing*fRSpacing);
}

bool LHPadPlane3::IsInBoundary(Double_t i, Double_t j)
{
  Double_t r = TMath::Sqrt(i*i+j*j);
  if (r < fRMin || r > fRMax)
    return false;

  return true;
}

TH2* LHPadPlane3::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane; 

  TString optionString(option);

  bool removeBinBoundary = false;
  if (optionString.Index("x")>=0) {
    optionString.ReplaceAll("x","");
    removeBinBoundary = true;
  }

  TH2Poly *hist = new TH2Poly();

  Double_t xPoints[6] = {0};
  Double_t yPoints[6] = {0};

  KBPad *pad;
  TIter iterPads(fChannelArray);
  Int_t selectSection = optionString.Atoi();
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

    hist -> AddBin(numPoints+1, xPoints, yPoints);
  }

  TIter nextBin(hist -> GetBins());
  while (auto bin = (TH2PolyBin*) nextBin()) {
    auto graphBin = (TGraph *) bin -> GetPolygon();
    graphBin -> SetLineColor(kGray+1);
    if (removeBinBoundary) graphBin -> SetLineWidth(0);
  }

  hist -> SetStats(0);
  hist -> SetTitle(";x (mm); y (mm)");
  hist -> GetXaxis() -> SetRangeUser(-1.05*fRTopCut,1.05*fRTopCut);
  hist -> GetYaxis() -> SetRangeUser(-1.05*fRTopCut,1.05*fRTopCut);
  hist -> GetXaxis() -> SetTickLength(0);
  hist -> GetYaxis() -> SetTickLength(0);

  fH2Plane = (TH2 *) hist;
  return fH2Plane;
}

void LHPadPlane3::DrawFrame(Option_t *)
{
}

TCanvas *LHPadPlane3::GetCanvas(Option_t *)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,890,750);
  fCanvas -> SetMargin(0.13,0.13,0.08,0.02);

  return fCanvas;
}

KBPad *LHPadPlane3::NewPad(Int_t s, Int_t r, Int_t l)
{
  auto pad = new KBPad();
  pad -> SetSectionRowLayer(s, r, l);
  fChannelArray -> Add(pad);
  return pad;
}

void LHPadPlane3::SetNeighborPads(KBPad *pad0, KBPad *pad1)
{
  pad0 -> AddNeighborPad(pad1);
  pad1 -> AddNeighborPad(pad0);
}

Int_t LHPadPlane3::FindSection(Double_t i, Double_t j)
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
