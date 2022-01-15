#include "DUMMYDetectorPlane.hh"
#include "TCanvas.h"
#include <iostream>
using namespace std;

ClassImp(DUMMYDetectorPlane)

DUMMYDetectorPlane::DUMMYDetectorPlane()
//:KBPadPlane("DUMMYDetectorPlane", "")
:KBDetectorPlane("DUMMYDetectorPlane", "")
{
}

bool DUMMYDetectorPlane::Init()
{
  return true;
}


bool DUMMYDetectorPlane::IsInBoundary(Double_t i, Double_t j)
{
  return true;
}

Int_t DUMMYDetectorPlane::FindChannelID(Double_t i, Double_t j)
{
  return -1;
}

TH2* DUMMYDetectorPlane::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane;

  fH2Plane = new TH2D(fName+Form("%d",fPlaneID),";x (mm);y (mm)",100,-10,10,100,-10,10);
  return fH2Plane;
}

void DUMMYDetectorPlane::DrawFrame(Option_t *option)
{
}

TCanvas *DUMMYDetectorPlane::GetCanvas(Option_t *option)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,1100,600);

  return fCanvas;
}
