#include "LAPPadPlane.hh"

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

ClassImp(LAPPadPlane)

LAPPadPlane::LAPPadPlane()
:KBPadPlane("LAPPadPlane", "")
{
}

void LAPPadPlane::Draw(Option_t *option)
{
  GetCanvas();
  GetHist() -> Draw(option);
  DrawFrame();
}

bool LAPPadPlane::Init()
{
  fAxis1 = fPar -> GetParAxis("ppRefAxis",0);
  fAxis2 = fPar -> GetParAxis("ppRefAxis",1);

  fTanPi1o8 = TMath::Tan(TMath::Pi()*1./8.);
  fTanPi3o8 = TMath::Tan(TMath::Pi()*3./8.);
  fTanPi5o8 = TMath::Tan(TMath::Pi()*5./8.);
  fTanPi7o8 = TMath::Tan(TMath::Pi()*7./8.);

  fRMin = fPar -> GetParDouble("ppRMin");
  fRMax = fPar -> GetParDouble("ppRMax");
  fPadGap = fPar -> GetParDouble("padBoundaryGap");

  fNumSections = fPar -> GetParInt("numSections");

  for (auto section=0; section<fNumSections; ++section) {
    fSectionAngle[section] = fPar -> GetParDouble("sectionAngle",section);
    fRBaseLine[section] = fPar -> GetParDouble("rPadLayer0",section);
    fPadWidth[section] = fPar -> GetParDouble("padWidth",section);
    fPadHeight[section] = fPar -> GetParDouble("padHeight",section);

    auto nLayers = fPar -> GetParN(Form("nRowsInLayer%d",section));
    for (Int_t iLayer = 0; iLayer < nLayers; ++iLayer) {
      fNRowsInLayer[section].push_back(fPar -> GetParInt(Form("nRowsInLayer%d",section),iLayer));
    }
  }

  TString padMapFileName = fPar -> GetParString("padMap");
  ifstream padMapData(padMapFileName.Data());

  Int_t asadID, agetID, channelID, padID;

  for (Int_t section=0; section<fNumSections; section++) {
    Int_t nLayers = fNRowsInLayer[section].size();
    for (Int_t layer = 0; layer < nLayers; layer++) {
      Int_t nRowsHalf = (fNRowsInLayer[section][layer]-1)/2;
      for (Int_t row = nRowsHalf; row >= -nRowsHalf; row--) {

        padMapData >> asadID >> agetID >> channelID >> padID;
        KBPad *pad = new KBPad();

        Double_t posI = row * (fPadWidth[section] + fPadGap);
        Double_t posJ = fRBaseLine[section] + layer * (fPadHeight[section] + fPadGap);
        KBVector3 posPad(fAxis1%fAxis2, posI, posJ, 0);
        posPad.Rotate(fSectionAngle[section]);

        pad -> SetPlaneID(fPlaneID);
        pad -> SetPadID(padID);
        //pad -> SetSortValue(padID);
        pad -> SetAsAdID(asadID);
        pad -> SetAGETID(agetID);
        pad -> SetChannelID(channelID);

        pad -> SetPosition(posPad);
        pad -> SetSectionRowLayer(section, row, layer);

        fChannelArray -> Add(pad);
        fNPadsTotal++;
      }
    }
  }

  //fChannelArray -> Sort();

  Int_t nPads = fChannelArray -> GetEntriesFast();
  for (Int_t iPad = 0; iPad < nPads; iPad++) {
    KBPad *pad = (KBPad *) fChannelArray -> At(iPad);
    pad -> SetPadID(iPad);

    std::vector<Int_t> key;
    key.push_back(pad -> GetSection());
    key.push_back(pad -> GetRow());
    key.push_back(pad -> GetLayer());

    fPadMap.insert(std::pair<std::vector<Int_t>, Int_t>(key,iPad));
  }

  return true;
}

Int_t LAPPadPlane::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  if (section < 0 || section > fNumSections)
    return -1;

  Int_t nLayers = fNRowsInLayer[section].size();
  if (layer < 0 || layer >= nLayers)
    return -1;

  if (std::abs(row) > (fNRowsInLayer[section][layer]-1)/2)
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Int_t LAPPadPlane::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i, j);

  if (section == -1)
    return -1;

  Double_t posRadial = i;
  Double_t posArcDir = j;

  KBVector3 posInput(fAxis1%fAxis2, i, j, 0);
  posInput.Rotate(-fSectionAngle[section]);

  Int_t layer = Int_t((posInput.J()-fRBaseLine[section]+fPadHeight[section]/2.)/(fPadGap+fPadHeight[section]));
  Int_t nLayers = fNRowsInLayer[section].size();
  if (layer < 0 || layer >= nLayers)
    return -1;

  Double_t dx = fPadWidth[section] + fPadGap;
  Double_t x0 = posInput.I() + .5*dx;
  Int_t row;
  if (x0 >= 0) row = Int_t( x0 / dx );
  else         row = Int_t( x0 / dx ) - 1;

  // XXX number of row is alwasy odd number
  int nRowsInLayer = fNRowsInLayer[section][layer];
  Int_t nRowsHalf = (nRowsInLayer-1)/2;
  if (std::abs(row) > nRowsHalf)
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Double_t LAPPadPlane::PadDisplacement() const
{
  Int_t max = 0;
  for (Int_t section=0; section<fNumSections; ++section) {
    if (max < fPadHeight[section])
      max = fPadHeight[section];
    if (max < fPadWidth[section])
      max = fPadWidth[section];
  }

  return max;
}

bool LAPPadPlane::IsInBoundary(Double_t i, Double_t j)
{
  if (TMath::Sqrt(i*i+j*j) < 300.)
    return true;

  return false;
}

TH2* LAPPadPlane::GetHist(Option_t *)
{
  if (fH2Plane != nullptr)
    return fH2Plane; 

  TH2Poly *h2 = new TH2Poly();

  Double_t iPoints[5] = {0};
  Double_t jPoints[5] = {0};
  Int_t iSigns[5] = {-1, -1, 1, 1, -1};
  Int_t jSigns[5] = {-1, 1, 1, -1, -1};

  KBPad *pad;
  TIter iterPads(fChannelArray);
  while ((pad = (KBPad *) iterPads.Next())) {
    Int_t section = pad -> GetSection();
    //if (section > 1) continue;

    Double_t dr = 0.5 * (fPadHeight[section]);
    Double_t dw = 0.5 * (fPadWidth[section]);
    Double_t di, dj;

    if (section == 0 || section == 1) {
      di = dr;
      dj = dw;
    }
    else {
      di = dw;
      dj = dr;
    }

    Double_t i = pad -> GetI();
    Double_t j = pad -> GetJ();

    for (Int_t iPoint = 0; iPoint < 5; iPoint++) {
      iPoints[iPoint] = i + iSigns[iPoint]*di;
      jPoints[iPoint] = j + jSigns[iPoint]*dj;

      pad -> AddPadCorner(iPoints[iPoint], jPoints[iPoint]);
    }

    h2 -> AddBin(5, iPoints, jPoints);
  }

  h2 -> SetLabelOffset(0.005,"X");
  h2 -> SetLabelOffset(0.005,"Y");
  h2 -> SetLabelOffset(0,"Z");
  h2 -> GetXaxis() -> SetTickLength(0.02);
  h2 -> GetYaxis() -> SetTickLength(0.02);
  h2 -> GetXaxis() -> SetTitleOffset(1.2);
  h2 -> GetYaxis() -> SetTitleOffset(1.5); 
  h2 -> GetZaxis() -> SetTitleOffset(-0.2); 
  h2 -> GetXaxis() -> SetTitleSize(0.03); 
  h2 -> GetYaxis() -> SetTitleSize(0.03); 
  h2 -> GetZaxis() -> SetTitleSize(0.03); 
  h2 -> GetXaxis() -> SetLabelSize(0.025); 
  h2 -> GetYaxis() -> SetLabelSize(0.025); 
  h2 -> GetZaxis() -> SetLabelSize(0.023); 

  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(Form(";%s (mm);%s (mm)",KBVector3::AxisName(fAxis1).Data(),KBVector3::AxisName(fAxis2).Data()));

  return fH2Plane;
}

void LAPPadPlane::DrawFrame(Option_t *)
{
  Color_t lineColor = kBlack;

  TEllipse *outerCircle = new TEllipse(0, 0, fRMax, fRMax);
            outerCircle -> SetFillStyle(0);
            outerCircle -> SetLineColor(lineColor);
            outerCircle -> Draw("samel");

  TEllipse *innerCircle = new TEllipse(0, 0, fRMin, fRMin);
            innerCircle -> SetFillStyle(0);
            innerCircle -> SetLineColor(lineColor);
            innerCircle -> Draw("samel");

  for (auto ii : {1,3,5,7,9,11,13,15}) {
    TLine* line = new TLine(
        fRMax*TMath::Cos(TMath::Pi()*ii/8), fRMax*TMath::Sin(TMath::Pi()*ii/8),
        fRMin*TMath::Cos(TMath::Pi()*ii/8), fRMin*TMath::Sin(TMath::Pi()*ii/8));

    line -> SetLineColor(lineColor);
    line -> Draw("samel");
  }
}

Int_t LAPPadPlane::FindSection(Double_t i, Double_t j)
{
  if (j > fTanPi3o8*i) {
    if (j > fTanPi1o8*i) {
      if (j > fTanPi7o8*i) {
        if (j > fTanPi5o8*i) {
          return 2;
        } else return -1;
      } else return 0;
    } else return -1;
  }
  else
  {
    if (j < fTanPi1o8*i) {
      if (j < fTanPi7o8*i) {
        if (j < fTanPi5o8*i) {
          return 3;
        } else return -1;
      } else return 1;
    } else return -1;
  }
}

TCanvas *LAPPadPlane::GetCanvas(Option_t *)
{
  if (fCanvas == nullptr) {
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,1100,1100);
    fCanvas -> SetMargin(0.10, 0.10, 0.08, 0.065);
  }

  return fCanvas;
}
