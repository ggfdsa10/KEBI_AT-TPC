#include "KBScintWall.hh"
#include "TCanvas.h"

#include <iostream>
using namespace std;

ClassImp(KBScintWall)

KBScintWall::KBScintWall()
:KBScintWall("KBScintWall","wall of scintillator array")
{
}

KBScintWall::KBScintWall(const char *name, const char *title)
:KBDetectorPlane(name,title)
{
}

void KBScintWall::Clear(Option_t *option)
{
  KBDetectorPlane::Clear(option);
}

void KBScintWall::Print(Option_t *) const
{
  kb_info << "containing " << fChannelArray -> GetEntries() << " pads" << endl;
}

bool KBScintWall::Init()
{
  fType = fPar -> GetParString(Form("SW%d_type",fPlaneID));
  fType.ToLower();
  fType.ReplaceAll("_","");

  if (fType == "bararray") {
    SetNameTitle("KBScintWall_BA", "wall of scintillator bar array");

    fBarAxis =   fPar -> GetParAxis(Form("SW%d_barAxis",fPlaneID));
    fStackAxis = fPar -> GetParAxis(Form("SW%d_stackAxis",fPlaneID));
    fStackDisplacement.SetReferenceAxis(fStackAxis);

    fNumStacks = fPar -> GetParInt(Form("SW%d_numStacks",fPlaneID));

    auto ds = fPar -> GetParDouble(Form("SW%d_stackDisplacement",fPlaneID));
    fStackDisplacement.SetIJK(0,0,ds);

    auto dx = fPar -> GetParDouble(Form("SW%d_dx",fPlaneID));
    auto dy = fPar -> GetParDouble(Form("SW%d_dy",fPlaneID));
    auto dz = fPar -> GetParDouble(Form("SW%d_dz",fPlaneID));
    fScintDXYZ.SetXYZ(dx,dy,dz);

    auto x0 = fPar -> GetParDouble(Form("SW%d_x0",fPlaneID));
    auto y0 = fPar -> GetParDouble(Form("SW%d_y0",fPlaneID));
    auto z0 = fPar -> GetParDouble(Form("SW%d_z0",fPlaneID));
    fScintPos0.SetXYZ(x0,y0,z0);

    /*
    auto numChannels = 0;
    for (auto layer = 0; layer < fNumStacks; ++layer)
    {
      KBVector3 pos = fScintPos0 + layer * fStackDisplacement;
      auto x = pos.At(fBarAxis);
      auto y = pos.At(fStackAxis);

      Double_t xPoints[5] = {x-dx, x-dx, x+dx, x+dx, x-dx};
      Double_t yPoints[5] = {y-dy, y+dy, y+dy, y-dy, y-dy};

      h2 -> AddBin(5, xPoints, yPoints);

      auto channel = new KBChannelBufferD();
      channel -> SetID(numChannels++);
      AddChannel(channel);

      for (auto iCorner = 0; iCorner < 4; ++iCorner) {
        if (xPoints[iCorner] > fMaxX) fMaxX = xPoints[iCorner];
        if (xPoints[iCorner] < fMinX) fMinX = xPoints[iCorner];

        if (yPoints[iCorner] > fMaxY) fMaxY = yPoints[iCorner];
        if (yPoints[iCorner] < fMinY) fMinY = yPoints[iCorner];
      }
    }
    */

    // DO NOT SORT CHANNELARRAY!!

    return true;
  }
  else {
    if (!fType.IsNull())
      kb_error << "Type [" << fType << "] is not supported" << endl;
    kb_error << "No parameter set for scintillator wall" << endl;
    return false;
  }
}

bool KBScintWall::IsInBoundary(Double_t i, Double_t j)
{
  if (fType == "bararray") {
    if (i > fMaxX || i < fMinX || j > fMaxY || j < fMaxY)
      return false;
    return true;
  }
  return false;
}

Int_t KBScintWall::FindChannelID(Double_t i, Double_t j)
{
  //@todo
  Int_t layer = -1;
  if (fType == "bararray") {
    if (fStackAxis == KBVector3::kX || fStackAxis == KBVector3::kMX) {
      layer = (i - fScintPos0.X()) / fStackDisplacement.X();
    } else if (fStackAxis == KBVector3::kY || fStackAxis == KBVector3::kMY) {
      layer = (j - fScintPos0.Y()) / fStackDisplacement.Y();
    }
  }
  else
    return -1;
  return layer;
}

TCanvas *KBScintWall::GetCanvas(Option_t *)
{
  //@todo
  return new TCanvas();
}

void KBScintWall::DrawFrame(Option_t *)
{
  //@todo
  return;
}

TH2* KBScintWall::GetHist(Option_t *)
{
  //@todo
  return (TH2 *) nullptr;
}
