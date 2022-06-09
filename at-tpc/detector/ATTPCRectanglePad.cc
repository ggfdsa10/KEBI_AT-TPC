#include "ATTPCRectanglePad.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TH2Poly.h"
#include "TCollection.h"
#include "TLine.h"

using namespace std;
using namespace TMath;

ClassImp(ATTPCRectanglePad)

ATTPCRectanglePad::ATTPCRectanglePad()
:KBPadPlane("RectanglePad", "Rectangle Pad")
{    
}


void ATTPCRectanglePad::Draw(Option_t *option)
{
  GetCanvas();
  GetHist() -> Draw(option);
  DrawFrame();
}


bool ATTPCRectanglePad::Init()
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
      Double_t posI = row*(fPadWidth + fPadGap);
      Double_t posJ =  layer*(fPadHeight + fPadGap);

      KBVector3 posPad(fAxis1%fAxis2, posI, posJ, 0.);

      pad -> SetPlaneID(fPlaneID);
      pad -> SetPadID(padID);

      pad -> SetAsAdID(asadID);
      pad -> SetAGETID(agetID);
      pad -> SetChannelID(channelID);
      pad -> SetPosition(posPad);
      pad -> SetSectionRowLayer(0, row, layer);

      auto neighborIndex = PadNeighborIndex(layer, row, padID);
      for(int index = 0; index<neighborIndex.size(); index++){
        KBPad *padNeighbor = (KBPad *) fChannelArray -> At(neighborIndex[index]);
        padNeighbor -> AddNeighborPad(pad);
        pad -> AddNeighborPad(padNeighbor);
      }

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

    Int_t layerID = pad -> GetLayer();
    Int_t rowID = pad -> GetRow();

    std::vector<Int_t> key;
    key.push_back(pad -> GetSection());
    key.push_back(pad -> GetRow());
    key.push_back(pad -> GetLayer());

    fPadMap.insert(std::pair<std::vector<Int_t>, Int_t>(key,iPad));
  }

  

  return true;
}

Int_t ATTPCRectanglePad::FindPadID(Int_t section, Int_t row, Int_t layer)
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


Int_t ATTPCRectanglePad::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i, j);
  if (section == -1)
    return -1;
  
  KBVector3 posInput(fAxis1%fAxis2, i, j, 0);
  
  Int_t layer = (posInput.J()+fPadHeight/2)/(fPadGap + fPadHeight) ;
  if(layer < 0) layer = 0;
  Int_t nLayers = fNRowsInLayer[section].size();

  if (layer < 0 || layer >= nLayers){
    return -1;
  }
  Int_t Row = (posInput.I()+fPadWidth/2)/(fPadGap + fPadWidth);
  if(Row < 0) Row = 0;
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


Double_t ATTPCRectanglePad::PadDisplacement() const
{
  Int_t max = 0;
  if (max < fPadHeight)
    max = fPadHeight;
  
  if (max < fPadWidth)
    max = fPadWidth;

  return max;
}


bool ATTPCRectanglePad::IsInBoundary(Double_t i, Double_t j)
{
  if ( (i <= fBasePadPos-(fPadWidth/2+fPadGap/2) && i >= -(fPadWidth/2+fPadGap/2))&& (j <= fBasePadPos-(fPadWidth/2+fPadGap/2) && j >= -(fPadHeight/2+fPadGap/2)))
    return true;
    
  return false;
}

TH2* ATTPCRectanglePad::GetHist(Option_t *option)
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

void ATTPCRectanglePad::DrawFrame(Option_t *option)
{
}

TCanvas *ATTPCRectanglePad::GetCanvas(Option_t *option)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,800,800);
  fCanvas -> SetMargin(0.13,0.13,0.08,0.02);
  
  return fCanvas;
}


Int_t ATTPCRectanglePad::FindSection(Double_t i, Double_t j)
{
  if ((i <= fBasePadPos-(fPadWidth/2+fPadGap/2) && i >= -(fPadWidth/2+fPadGap/2)) && (j <= fBasePadPos-(fPadWidth/2+fPadGap/2) && j >= -(fPadHeight/2+fPadGap/2)))
  {
    return 0;
  }
  else return -1;
}


std::vector<Int_t> ATTPCRectanglePad::PadNeighborIndex(Int_t layer, Int_t Row, Int_t PadID)
{
    // this method returns to the previous set neighbor pad index.
    // for set the neighbor pad, do register current pad and previous pads each other.
    std::vector<Int_t> indexArray;

    if(layer==0 && Row==0){return indexArray;}

    else if(layer==0 && Row!=0){
        indexArray.push_back(PadID-1);
    }
    else if(layer!=0 && Row==0){
        indexArray.push_back(PadID-RowNum);
        indexArray.push_back(PadID-RowNum+1);
    }
    else{
        indexArray.push_back(PadID-1);
        indexArray.push_back(PadID-RowNum);
        indexArray.push_back(PadID-RowNum-1);

        if(Row==RowNum-1){return indexArray;}

        indexArray.push_back(PadID-RowNum+1);
    }
    return indexArray;
}