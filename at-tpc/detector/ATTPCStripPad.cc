#include "ATTPCStripPad.hh"
#include "TCanvas.h"
#include "TMath.h"
#include "TCollection.h"
#include "TLine.h"


using namespace std;
using namespace TMath;

ClassImp(ATTPCStripPad)

ATTPCStripPad::ATTPCStripPad()
:KBPadPlane("HoneyCombPad", "HoneyComb Pad")
{    

}


void ATTPCStripPad::Draw(Option_t *option)
{
  GetCanvas();
  GetHist() -> Draw(option);
  DrawFrame();
}


bool ATTPCStripPad::Init()
{

  TObjArray *fInPadArray = new TObjArray();
  fPadBoundary = new TH2Poly();

  PadBoundaryConstruct(fPadBoundary);

  fAxis1 = fPar -> GetParAxis("padRefAxis", 0);
  fAxis2 = fPar -> GetParAxis("padRefAxis", 1);

  Int_t padID = 0;
  Int_t agetID = 0;
  Int_t asadID = 0;

  for(Int_t channal = 0; channal < fStripChannals; channal++){

    Int_t nLayers = fPadChArray[channal].size();
    for (Int_t layer = 0; layer < nLayers; layer++) {
      
      Int_t nRows = fPadChArray[channal].at(layer).size();
      for(Int_t row = 0; row < nRows; row++) {

        KBPad *pad = new KBPad();
        Double_t posI = get<1>(fPadChArray[channal].at(layer).at(row));
        Double_t posJ = get<2>(fPadChArray[channal].at(layer).at(row));

        KBVector3 posPad(fAxis1%fAxis2, posI, posJ, 0.);

        pad -> SetPlaneID(fPlaneID);
        pad -> SetPadID(padID);
        pad -> SetAsAdID(asadID);
        pad -> SetAGETID(agetID);
        pad -> SetChannelID(channal);
        pad -> SetPosition(posPad);
        pad -> SetSectionRowLayer(channal, row, layer);

        fChannelArray -> Add(pad);
        
        fNPadsTotal++;
        padID++;
      }
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

    auto padPos = pad -> GetPosition();
    auto neighborIndex = PadNeighborIndex(padPos);

    for(int index = 0; index<neighborIndex.size(); index++){
      KBPad *padNeighbor = (KBPad *) fChannelArray -> At(neighborIndex[index]);
      padNeighbor -> AddNeighborPad(pad);
      pad -> AddNeighborPad(padNeighbor);
    }
  }

  return true;
}

Int_t ATTPCStripPad::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  if (section < 0 || section >= fNumSections)
    return -1;

  Int_t nLayers = fPadChArray[section].size();
  if (layer < 0 || layer >= nLayers)
    return -1;

  if (row <  0 || row >= fPadChArray[section][layer].size())
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}


Int_t ATTPCStripPad::FindPadID(Double_t i, Double_t j)
{

  KBVector3 posInput(fAxis1%fAxis2, i, j, 0);
  Int_t PadIndex = PadIndexCheck(posInput.I(), posInput.J());
  if (PadIndex < 0){
    return -1;
  }

  Int_t section = FindChannal(PadIndex);
  if (section == -1 || section >= fStripChannals)
    return -1;

  Int_t layer = LayerIndexCheck(PadIndex, section);
  Int_t nLayers = fPadChArray[section].size();

  if (layer < 0 || layer >= nLayers){
    return -1;
  }

  Int_t Row = RowIndexCheck(PadIndex, section, layer);
  Int_t nRows = fPadChArray[section][layer].size();

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


Double_t ATTPCStripPad::PadDisplacement() const
{
  Int_t max = 0;
  if (max < fPadHeight/2)
    max = fPadHeight;
  
  if (max < fPadWidth/2)
    max = fPadWidth;

  return max;
}


bool ATTPCStripPad::IsInBoundary(Double_t i, Double_t j)
{
  if ( (i <= fBasePadPos-50 && i >= -50)&& (j <= fBasePadPos-3 && j >= -4))
    return true;
    
  return false;
}

TH2* ATTPCStripPad::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane;

  TH2Poly *h2 = new TH2Poly();

  fPadBTWLength = fPadActiveSize/(fStripNumByCh-1.);
  fPadWidth = (fPadBTWLength-sqrt(3)*fPadGap)/3.;
  fPadHeight = sqrt(3)*fPadWidth;

  Double_t xPoints[6] = {0};
  Double_t yPoints[6] = {0};

  KBPad *pad;
  TIter iterPads(fChannelArray);
  while ((pad = (KBPad *) iterPads.Next())) {
    Double_t i = pad -> GetI();
    Double_t j = pad -> GetJ();

    xPoints[0] = i -fPadWidth/2.;
    yPoints[0] = j +fPadHeight/2.;
    xPoints[1] = i +fPadWidth/2.;
    yPoints[1] = j +fPadHeight/2.;
    xPoints[2] = i +fPadWidth;
    yPoints[2] = j;
    xPoints[3] = i +fPadWidth/2.;
    yPoints[3] = j -fPadHeight/2.;
    xPoints[4] = i -fPadWidth/2.;
    yPoints[4] = j -fPadHeight/2.;
    xPoints[5] = i -fPadWidth;
    yPoints[5] = j;
    // xPoints[6] = i -fPadWidth/2.;
    // yPoints[6] = j +fPadHeight/2.;

    pad -> AddPadCorner(xPoints[0], yPoints[0]);
    pad -> AddPadCorner(xPoints[1], yPoints[1]);
    pad -> AddPadCorner(xPoints[2], yPoints[2]);
    pad -> AddPadCorner(xPoints[3], yPoints[3]);
    pad -> AddPadCorner(xPoints[4], yPoints[4]);
    pad -> AddPadCorner(xPoints[5], yPoints[5]);
    // pad -> AddPadCorner(xPoints[6], yPoints[6]);
    
    h2 -> AddBin(6, xPoints, yPoints);
  }
  
  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(Form(";%s (mm);%s (mm)",KBVector3::AxisName(fAxis1).Data(),KBVector3::AxisName(fAxis2).Data()));
  
  return fH2Plane;
}

void ATTPCStripPad::DrawFrame(Option_t *option)
{
}

TCanvas *ATTPCStripPad::GetCanvas(Option_t *option)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,1300,700);
  fCanvas -> SetMargin(0.13,0.13,0.08,0.02);
  
  return fCanvas;
}


void ATTPCStripPad::PadBoundaryConstruct(TH2Poly *pad){
    Double_t x[6], y[6];

    // for pad boundary
    Double_t padGap = 0.;
    fPadBTWLength = fPadActiveSize/(fStripNumByCh-1.);
    fPadWidth = (fPadBTWLength-sqrt(3)*padGap)/3.;
    fPadHeight = sqrt(3)*fPadWidth;

    Int_t Row = fPadActiveSize/2.*sqrt(3)/(fPadWidth/2. +fPadHeight/2.);
    Int_t Col = fPadActiveSize/2./fPadBTWLength;

    Int_t ColumnByLayers = Col;
    Double_t xCenter = 0, yCenter = 0;
    Int_t padIndex = 0;

    // 0 channal construct
    double ch0PosX = 0.;    
    for (int layer = 0; layer < fStripNumByCh; layer++) {
          std::vector<std::tuple<Int_t, Double_t, Double_t>> nRowInLayers;
        for (int row = 0; row < ColumnByLayers; row++) {
            x[0] = xCenter -fPadWidth/2.;
            y[0] = yCenter +fPadHeight/2.;
            x[1] = xCenter +fPadWidth/2.;
            y[1] = yCenter +fPadHeight/2.;
            x[2] = xCenter +fPadWidth;
            y[2] = yCenter;
            x[3] = xCenter +fPadWidth/2.;
            y[3] = yCenter -fPadHeight/2.;
            x[4] = xCenter -fPadWidth/2.;
            y[4] = yCenter -fPadHeight/2.;
            x[5] = xCenter -fPadWidth;
            y[5] = yCenter;

            pad->AddBin(6, x, y);
            nRowInLayers.push_back(make_tuple(padIndex, xCenter, yCenter));
            padIndex++;
            xCenter += fPadBTWLength;
        }
        fPadChArray[0].push_back(nRowInLayers);

        if(layer < fStripNumByCh/2){
            ch0PosX -=(sqrt(3)*padGap/2. +3.*fPadWidth/2.);
            ColumnByLayers++;
        }
        else{
            ch0PosX +=(sqrt(3)*padGap/2. +3.*fPadWidth/2.);
            ColumnByLayers--;
        }
        xCenter = ch0PosX;
        yCenter += 3*(padGap/2. + fPadHeight/2.);
    }

    // // 1 channal construct
    ColumnByLayers = Col+1;
    double ch1PosX = (ColumnByLayers-1)*fPadBTWLength - (sqrt(3)*padGap/2. +3.*fPadWidth/2.);
    double ch1PosY = (padGap/2. + fPadHeight/2.);
    xCenter = ch1PosX, yCenter = ch1PosY;

    for (int layer = 0; layer < fStripNumByCh; layer++) {
        std::vector<std::tuple<Int_t, Double_t, Double_t>> nRowInLayers;
        for (int row = 0; row < ColumnByLayers; row++) {

            x[0] = xCenter -fPadWidth/2.;
            y[0] = yCenter +fPadHeight/2.;
            x[1] = xCenter +fPadWidth/2.;
            y[1] = yCenter +fPadHeight/2.;
            x[2] = xCenter +fPadWidth;
            y[2] = yCenter;
            x[3] = xCenter +fPadWidth/2.;
            y[3] = yCenter -fPadHeight/2.;
            x[4] = xCenter -fPadWidth/2.;
            y[4] = yCenter -fPadHeight/2.;
            x[5] = xCenter -fPadWidth;
            y[5] = yCenter;

            pad->AddBin(6, x, y);
            nRowInLayers.push_back(make_tuple(padIndex, xCenter, yCenter));
            padIndex++;
            xCenter += (sqrt(3)*padGap/2. +3.*fPadWidth/2.);
            yCenter += 3*(padGap/2. + fPadHeight/2.);
        }
        fPadChArray[1].push_back(nRowInLayers);

        if(layer < fStripNumByCh/2-1){
            ch1PosX -=fPadBTWLength;
            ColumnByLayers++;
        }
        else{
            ch1PosX -=(sqrt(3)*padGap/2. +3.*fPadWidth/2.);
            ch1PosY += 3*(padGap/2. + fPadHeight/2.);
            ColumnByLayers--;
        }
        xCenter = ch1PosX;
        yCenter = ch1PosY;
    }

    // 2 channal construct
    ColumnByLayers = Col+1;
    double ch2PosX = (ColumnByLayers-1)*fPadBTWLength - (sqrt(3)*padGap/2. +3.*fPadWidth/2.) +(ColumnByLayers-1)*(sqrt(3)*padGap/2. +3.*fPadWidth/2.) -(sqrt(3)*padGap/2. +3*fPadWidth/2.);
    double ch2PosY = (padGap/2. + fPadHeight/2.) +(ColumnByLayers-1)*3*(padGap/2. + fPadHeight/2.) +(padGap/2. + fPadHeight/2.);
    xCenter = ch2PosX, yCenter = ch2PosY;

    for (int layer = 0; layer < fStripNumByCh; layer++) {
        std::vector<std::tuple<Int_t, Double_t, Double_t>> nRowInLayers;
        for (int row = 0; row < ColumnByLayers; row++) {

            x[0] = xCenter -fPadWidth/2.;
            y[0] = yCenter +fPadHeight/2.;
            x[1] = xCenter +fPadWidth/2.;
            y[1] = yCenter +fPadHeight/2.;
            x[2] = xCenter +fPadWidth;
            y[2] = yCenter;
            x[3] = xCenter +fPadWidth/2.;
            y[3] = yCenter -fPadHeight/2.;
            x[4] = xCenter -fPadWidth/2.;
            y[4] = yCenter -fPadHeight/2.;
            x[5] = xCenter -fPadWidth;
            y[5] = yCenter;

            pad->AddBin(6, x, y);

            nRowInLayers.push_back(make_tuple(padIndex, xCenter, yCenter));
            padIndex++;
            xCenter -= (sqrt(3)*padGap/2. +3*fPadWidth/2.);
            yCenter += 3*(padGap/2. + fPadHeight/2.);
        }
        fPadChArray[2].push_back(nRowInLayers);

        if(layer < fStripNumByCh/2-1){
            ch2PosX -= (sqrt(3)*padGap/2. +3.*fPadWidth/2.);
            ch2PosY -= 3*(padGap/2. + fPadHeight/2.);
            ColumnByLayers++;
        }
        else{
            ch2PosX -=fPadBTWLength;
            ColumnByLayers--;
        }
        xCenter = ch2PosX;
        yCenter = ch2PosY;
    }
}

std::vector<Int_t> ATTPCStripPad::PadNeighborIndex(KBVector3 posPad)
{
  std::vector<Int_t> indexArray;

  for(int i=0; i<3; i++){
    Double_t thisPadX = posPad.I();
    Double_t thisPadY = posPad.J();

    int xIndex = i%3 -1;
    int yIndex = i%2 +1;
    if(i>=3){yIndex= -yIndex;}
    Double_t neighborPadX = thisPadX + xIndex*(sqrt(3)*fPadGap/2. +3.*fPadWidth/2.);
    Double_t neighborPadY = thisPadY + yIndex*(fPadGap/2. + fPadHeight/2.);

    Int_t neighborIndex = PadIndexCheck(neighborPadX, neighborPadY);
    if(neighborIndex > 0){
      indexArray.push_back(neighborIndex);
    }
  }
  return indexArray;
}

Int_t ATTPCStripPad::PadIndexCheck(Double_t i, Double_t j)
{
  Int_t PadIndex = fPadBoundary -> FindBin(i,j);
  return PadIndex -1;
}

Int_t ATTPCStripPad::FindChannal(Int_t padIndex)
{
  for(int channal = 0; channal< fStripChannals; channal++){
    Int_t nLayers = fPadChArray[channal].size(); 
    for(int layer = 0; layer < nLayers; layer++){
      Int_t nRows = fPadChArray[channal].at(layer).size(); 
      for(int row = 0; row < nRows; row++){
        int findPadIndex = get<0>(fPadChArray[channal].at(layer).at(row));
        if(findPadIndex == padIndex){return channal;}
      }
    }
  }
  return -1;
}

Int_t ATTPCStripPad::LayerIndexCheck(Int_t padIndex, Int_t Channal)
{
  Int_t nLayers = fPadChArray[Channal].size(); 
  for(int layer = 0; layer < nLayers; layer++){
    Int_t nRows = fPadChArray[Channal].at(layer).size(); 
    for(int row = 0; row < nRows; row++){
      int findPadIndex = get<0>(fPadChArray[Channal].at(layer).at(row));
      if(findPadIndex == padIndex){return layer;}
    }
  }

  return -1;
}

Int_t ATTPCStripPad::RowIndexCheck(Int_t padIndex, Int_t Channal, Int_t LayerIndex)
{
  Int_t nRows = fPadChArray[Channal].at(LayerIndex).size(); 
  for(int row = 0; row < nRows; row++){
    int findPadIndex = get<0>(fPadChArray[Channal].at(LayerIndex).at(row));
    if(findPadIndex == padIndex){return row;}
  }
  return -1;
}