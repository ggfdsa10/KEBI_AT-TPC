#include "ATTPCPadPlane.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TH2Poly.h"
#include "TCollection.h"
#include "TLine.h"

using namespace std;
using namespace TMath;

ClassImp(ATTPCPadPlane)

ATTPCPadPlane::ATTPCPadPlane()
:KBPadPlane("ATTPCPad1", "Rectngle Pad")
{
}


void ATTPCPadPlane::Draw(Option_t *option)
{
  GetCanvas();
  GetHist() -> Draw(option);
  DrawFrame();
}


bool ATTPCPadPlane::Init()
{
  TObjArray *fInPadArray = new TObjArray();

  fAxis1 = fPar -> GetParAxis("padRefAxis", 0);
  fAxis2 = fPar -> GetParAxis("padRefAxis", 1);
  
  for(Int_t iLayer = 0; iLayer < ColumnNum; iLayer++) {
    fNRowsInLayer[0].push_back(RowNum);
  }

  Int_t channelID = 0;
  Int_t padID = 0;
  Int_t agetID = 0;
  Int_t asadID = 0;
  Int_t nLayers = fNRowsInLayer[0].size();
  
  for (Int_t layer = 0; layer < nLayers; layer++) {
    
    for(Int_t row = 0; row < RowNum; row++) {

      KBPad *pad = new KBPad();
      Double_t posI = fBasePadPos -fPadGap/2 -fPadWidth/2 - row*(fPadWidth + fPadGap);
      Double_t posJ = fBasePadPos -fPadGap/2 -fPadHeight/2 - layer*(fPadHeight + fPadGap);
;

      KBVector3 posPad(fAxis1%fAxis2, posI, posJ, 0);

      pad -> SetPlaneID(fPlaneID);
      pad -> SetPadID(padID);
      pad -> SetAsAdID(asadID);
      pad -> SetAGETID(agetID);
      pad -> SetChannelID(channelID);
      pad -> SetPosition(posPad);
      pad -> SetSectionRowLayer(0, row, layer);

      fChannelArray -> Add(pad);
      
      fNPadsTotal++;
      padID++;
      channelID++;
    }
  }

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

Int_t ATTPCPadPlane::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  if (section < 0 || section >= fNumSections)
    return -1;

  Int_t nLayers = fNRowsInLayer[section].size();
  if (layer < 0 || layer >= nLayers)
    return -1;

  if (row <  0 || row >= fNRowsInLayer[section][layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}


Int_t ATTPCPadPlane::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i, j);
  if (section == -1)
    return -1;
  
  KBVector3 posInput(fAxis1%fAxis2, i, j, 0);
  
  Int_t layer = (-posInput.J() + fBasePadPos)/(fPadGap + fPadHeight);
  Int_t nLayers = fNRowsInLayer[section].size();
  
  if (layer < 0 || layer >= nLayers){
    return -1;
  }
  Int_t Row = (-posInput.I() + fBasePadPos)/(fPadGap + fPadWidth);
  Int_t nRows = fNRowsInLayer[section][layer];
  if(Row < 0 || Row >= nRows){
    return -1;
  }

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(Row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}


Double_t ATTPCPadPlane::PadDisplacement() const
{
  Int_t max = 0;
  if (max < fPadHeight)
    max = fPadHeight;
  
  if (max < fPadWidth)
    max = fPadWidth;

  return max;
}


bool ATTPCPadPlane::IsInBoundary(Double_t i, Double_t j)
{
  if ( i <= fBasePadPos+0.125 && j <= fBasePadPos+0.125)
    return true;
    
  return false;
}

TH2* ATTPCPadPlane::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane;

  TH2Poly *h2 = new TH2Poly();

  Double_t xPoints[5] = {0};
  Double_t yPoints[5] = {0};

  Int_t xSign[5] = {-1, -1, 1, 1, -1};
  Int_t ySign[5] = {-1, 1, 1, -1, -1};
  
  Double_t dH = fPadHeight/2;
  Double_t dW = fPadWidth/2;

  KBPad *pad;
  TIter iterPads(fChannelArray);
  while ((pad = (KBPad *) iterPads.Next())) {
    Double_t i = pad -> GetI();
    Double_t j = pad -> GetJ();

    for (Int_t x = 0; x < 5; x++) {
      xPoints[x] = i + xSign[x] * dW;
      yPoints[x] = j + ySign[x] * dH;

      pad -> AddPadCorner(xPoints[x], yPoints[x]);
    }

    h2 -> AddBin(5, xPoints, yPoints);
  }
  
  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(Form(";%s (mm);%s (mm)",KBVector3::AxisName(fAxis1).Data(),KBVector3::AxisName(fAxis2).Data()));
  
  return fH2Plane;
}

void ATTPCPadPlane::DrawFrame(Option_t *option)
{
  Color_t lineColor = kBlack;

  TLine* line1 = new TLine(fBasePadPos +0.125, 0., fBasePadPos +0.125, fBasePadPos +0.125);
  TLine* line2 = new TLine(0., 0., fBasePadPos +0.125, 0.);
  TLine* line3 = new TLine(0., 0., 0., fBasePadPos +0.125);
  TLine* line4 = new TLine(0., fBasePadPos +0.125, fBasePadPos +0.125, fBasePadPos +0.125);

  line1 -> SetLineColor(lineColor);
  line2 -> SetLineColor(lineColor);
  line3 -> SetLineColor(lineColor);
  line4 -> SetLineColor(lineColor);

  line1 -> Draw("samel");
  line2 -> Draw("samel");
  line3 -> Draw("samel");
  line4 -> Draw("samel");
}

TCanvas *ATTPCPadPlane::GetCanvas(Option_t *option)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,1300,700);
  fCanvas -> SetMargin(0.13,0.13,0.08,0.02);
  
  return fCanvas;
}


Int_t ATTPCPadPlane::FindSection(Double_t i, Double_t j)
{
  if ((i <= fBasePadPos +0.125 || i >= 0) && (j <= fBasePadPos +0.125 || j >= 0))
    {
      return 0;
    }
  else return -1;
}
