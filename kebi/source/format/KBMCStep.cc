#include "KBMCStep.hh"

#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBMCStep)

KBMCStep::KBMCStep()
{
  Clear();
}

KBMCStep::~KBMCStep()
{
}

void KBMCStep::Print(Option_t *option) const
{
  TString opts = TString(option);

  TString title;
  if (opts.Index("t")>=0) title += "ID|XYZ|TE: ";

  if (opts.Index("s")>=0)
    kr_info(0) << title << fTrackID << " | " << fX << ", " << fY << ", " << fZ
      << " | " << fTime << ", " << fEdep << endl;
  else //if (opts.Index("a")>=0)
    kr_info(0) << title
      << setw(4)  << fTrackID
      << setw(12) << fX
      << setw(12) << fY
      << setw(12) << fZ << " | "
      << setw(12) << fTime
      << setw(12) << fEdep << endl;
}

void KBMCStep::Clear(Option_t *option) {
  fTrackID = -1;
  fX = -999;
  fY = -999;
  fZ = -999;
  fTime = -999;
  fEdep = -999;
}

void KBMCStep::SetTrackID(Int_t val)  { fTrackID = val; }
void KBMCStep::SetX(Double_t val)     { fX = val; }
void KBMCStep::SetY(Double_t val)     { fY = val; }
void KBMCStep::SetZ(Double_t val)     { fZ = val; }
void KBMCStep::SetTime(Double_t val)  { fTime = val; }
void KBMCStep::SetEdep(Double_t val)  { fEdep = val; }

void KBMCStep::SetMCStep(Int_t trackID, Double_t x, Double_t y, Double_t z, Double_t time, Double_t edep)
{
  fTrackID = trackID;
  fX = x;
  fY = y;
  fZ = z;
  fTime = time;
  fEdep = edep;
}

Int_t KBMCStep::GetTrackID()  const { return fTrackID; }
Double_t KBMCStep::GetX()     const { return fX; }
Double_t KBMCStep::GetY()     const { return fY; }
Double_t KBMCStep::GetZ()     const { return fZ; }
Double_t KBMCStep::GetTime()  const { return fTime; }
Double_t KBMCStep::GetEdep()  const { return fEdep; }



#ifdef ACTIVATE_EVE
bool KBMCStep::DrawByDefault() { return false; }
bool KBMCStep::IsEveSet() { return true; }

TEveElement *KBMCStep::CreateEveElement()
{
  auto pointSet = new TEvePointSet("MCStep");
  pointSet -> SetMarkerColor(kRed-5);
  pointSet -> SetMarkerSize(0.2);
  pointSet -> SetMarkerStyle(20);

  return pointSet;
}

void KBMCStep::SetEveElement(TEveElement *, Double_t scale)
{
}

void KBMCStep::AddToEveSet(TEveElement *eveSet, Double_t scale)
{
  auto pointSet = (TEvePointSet *) eveSet;
  pointSet -> SetNextPoint(scale*fX, scale*fY, scale*fZ);
}
#endif
