#include "ATTPCHoneyCombPad.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TCollection.h"
#include "TLine.h"


using namespace std;
using namespace TMath;

ClassImp(ATTPCHoneyCombPad)

ATTPCHoneyCombPad::ATTPCHoneyCombPad()
:KBPadPlane("HoneyCombPad", "HoneyComb Pad")
{    
}


void ATTPCHoneyCombPad::Draw(Option_t *option)
{
  GetCanvas();
  GetHist() -> Draw(option);
  DrawFrame();
}


bool ATTPCHoneyCombPad::Init()
{
  TObjArray *fInPadArray = new TObjArray();
  fPadBoundary = new TH2Poly();
  PadBoundaryConstruct(fPadBoundary);

  fAxis1 = fPar -> GetParAxis("padRefAxis", 0);
  fAxis2 = fPar -> GetParAxis("padRefAxis", 1);

  
  for(Int_t iLayer = 0; iLayer < layerNum; iLayer++) {
    fNRowsInLayer[0].push_back(rowNum);
  }

  Int_t channelID = 0;
  Int_t padID = 0;
  Int_t agetID = 0;
  Int_t asadID = 0;
  Int_t nLayers = fNRowsInLayer[0].size();

  for (Int_t layer = 0; layer < nLayers; layer++) {
    
    for(Int_t row = 0; row < rowNum; row++) {

      KBPad *pad = new KBPad();
        Double_t posI;
        Double_t posJ;

    if(layer%2!=0){
      posI = row*fPadWidth +fPadWidth/2;
      posJ =  layer*fPadHeight;
    }
    else{
      posI = row*fPadWidth;
      posJ =  layer*fPadHeight;
    }

      KBVector3 posPad(fAxis1%fAxis2, posI, posJ, 0.);

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

Int_t ATTPCHoneyCombPad::FindPadID(Int_t section, Int_t row, Int_t layer)
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


Int_t ATTPCHoneyCombPad::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i, j);
  if (section == -1)
    return -1;
  
  KBVector3 posInput(fAxis1%fAxis2, i, j, 0);
  Int_t PadIndex = PadIndexCheck(posInput.I(), posInput.J());
  if (PadIndex < 0){
    return -1;
  }

  Int_t layer = PadIndex/16 ;
  Int_t nLayers = fNRowsInLayer[section].size();

  if (layer < 0 || layer >= nLayers){
    return -1;
  }

  Int_t Row = PadIndex%16;
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


Double_t ATTPCHoneyCombPad::PadDisplacement() const
{
  Int_t max = 0;
  if (max < fPadHeight/2)
    max = fPadHeight;
  
  if (max < fPadWidth/2)
    max = fPadWidth;

  return max;
}


bool ATTPCHoneyCombPad::IsInBoundary(Double_t i, Double_t j)
{
  if ( (i <= fBasePadPos-4 && i >= -3)&& (j <= fBasePadPos-3 && j >= -4))
    return true;
    
  return false;
}

TH2* ATTPCHoneyCombPad::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane;

  TH2Poly *h2 = new TH2Poly();

  Double_t xPoints[7] = {0};
  Double_t yPoints[7] = {0};

  Double_t delY[2];
  delY[0] = 0.5 * (1 - fPadGap / fPadHeight) * (pow(fPadWidth, 2) + pow(0.5 * fPadHeight, 2)) / fPadWidth;
  delY[1] = sqrt(pow(delY[0], 2) - pow(0.5 * fPadHeight - fPadGap, 2));

  KBPad *pad;
  TIter iterPads(fChannelArray);
  while ((pad = (KBPad *) iterPads.Next())) {
    Double_t i = pad -> GetI();
    Double_t j = pad -> GetJ();

    xPoints[0] = i - 0.5 * (fPadWidth - fPadGap);
    yPoints[0] = j - delY[1];
    xPoints[1] = i;
    yPoints[1] = j - delY[0];
    xPoints[2] = i + 0.5 * (fPadWidth - fPadGap);
    yPoints[2] = yPoints[0];
    xPoints[3] = xPoints[2];
    yPoints[3] = j + delY[1];
    xPoints[4] = xPoints[1];
    yPoints[4] = j + delY[0];
    xPoints[5] = xPoints[0];
    yPoints[5] = yPoints[3];
    xPoints[6] = i - 0.5 * (fPadWidth - fPadGap);
    yPoints[6] = j - delY[1];

    pad -> AddPadCorner(xPoints[0], yPoints[0]);
    pad -> AddPadCorner(xPoints[1], yPoints[1]);
    pad -> AddPadCorner(xPoints[2], yPoints[2]);
    pad -> AddPadCorner(xPoints[3], yPoints[3]);
    pad -> AddPadCorner(xPoints[4], yPoints[4]);
    pad -> AddPadCorner(xPoints[5], yPoints[5]);
    pad -> AddPadCorner(xPoints[6], yPoints[6]);
    

    h2 -> AddBin(7, xPoints, yPoints);
  }
  
  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(Form(";%s (mm);%s (mm)",KBVector3::AxisName(fAxis1).Data(),KBVector3::AxisName(fAxis2).Data()));
  
  return fH2Plane;
}

void ATTPCHoneyCombPad::DrawFrame(Option_t *option)
{
}

TCanvas *ATTPCHoneyCombPad::GetCanvas(Option_t *option)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,1300,700);
  fCanvas -> SetMargin(0.13,0.13,0.08,0.02);
  
  return fCanvas;
}


Int_t ATTPCHoneyCombPad::FindSection(Double_t i, Double_t j)
{
  if ((i <= fBasePadPos-4 && i >= -3)&& (j <= fBasePadPos-3 && j >= -4))
  {
    return 0;
  }
  else return -1;
}

void ATTPCHoneyCombPad::PadBoundaryConstruct(TH2Poly *pad){
  double x[6], y[6];
  double delY[2];

  delY[0] = 0.5 * (1 - fPadGap / fPadHeight) * (pow(fPadWidth, 2) + pow(0.5 * fPadHeight, 2)) / fPadWidth;
  delY[1] = sqrt(pow(delY[0], 2) - pow(0.5 * fPadHeight - fPadGap, 2));

  double xCenter = 0, yCenter = 0;

  double boundaryX = fPadGap/2;
  double boundaryY1 = 2.2*(sqrt(3)*fPadGap/2)/5;
  double boundaryY2 = 2.2*(sqrt(3)*fPadGap/2)/5;

  for (int nRow = 0; nRow < 16; nRow++) {
      
    for (int nColumn = 0; nColumn < 16; nColumn++) {
        
      x[0] = xCenter - 0.5 * (fPadWidth - fPadGap) -boundaryX;
      y[0] = yCenter - delY[1] -boundaryY2;
      x[1] = xCenter;
      y[1] = yCenter - delY[0] -boundaryY2;
      x[2] = xCenter + 0.5 * (fPadWidth - fPadGap) +boundaryX;
      y[2] = y[0];
      x[3] = x[2];
      y[3] = yCenter + delY[1] +boundaryY1;
      x[4] = x[1];
      y[4] = yCenter + delY[0] +boundaryY1;
      x[5] = x[0];
      y[5] = y[3];

      pad->AddBin(6, x, y);

      xCenter += fPadWidth;
    }

    yCenter += fPadHeight;
    if (nRow % 2 == 0)
      xCenter = +0.5 * fPadWidth;
    else
      xCenter = 0;
  }  
}

Int_t ATTPCHoneyCombPad::PadIndexCheck(Double_t i, Double_t j){
  Int_t PadIndex = fPadBoundary -> FindBin(i,j);
  return PadIndex -1;
}