#include "KBMCTrack.hh"
#include "TVector3.h"
#include "TDatabasePDG.h"

#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBMCTrack)

KBMCTrack::KBMCTrack()
{
  Clear();
}

void KBMCTrack::Clear(Option_t *option)
{
  KBTracklet::Clear(option);

  fPX.clear(); fPX.push_back(-999);
  fPY.clear(); fPY.push_back(-999);
  fPZ.clear(); fPZ.push_back(-999);
  fVX.clear(); fVX.push_back(-999);
  fVY.clear(); fVY.push_back(-999);
  fVZ.clear(); fVZ.push_back(-999);
  fKE.clear(); fKE.push_back(-999);
  fDetectorID.clear(); fDetectorID.push_back(-999);

  fCreatorProcessID = 0;
}

void KBMCTrack::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (TString(option).Index("all")>=0) {
    kr_info(0) << "MC-" << setw(3) << fTrackID << "(" << setw(3) << fParentID << ") " << setw(11) << fPDG << "[" << setw(2) << fCreatorProcessID << "]" << endl;
    Int_t n = fPX.size();
    for (auto i=0; i<n; ++i) {
      kr_info(0) << "  " << i
        <<  "> mom=(" << setw(12) << fPX[i] << "," << setw(12) << fPY[i] << "," << setw(12) << fPZ[i] << "),"
        <<   " det="  << setw(12) << fDetectorID[i] << ","
        <<   " pos=(" << setw(12) << fVX[i] << "," << setw(12) << fVY[i] << "," << setw(12) << fVZ[i] << ")" << endl;
    }
  }
  else {
    kr_info(0) << "MC-" << setw(3) << fTrackID << "(" << setw(3) << fParentID << ") " << setw(11) << fPDG
      << "[" << setw(2) << fCreatorProcessID << "]"
      << " mom=(" << setw(12) << fPX[0] << "," << setw(12) << fPY[0] << "," << setw(12) << fPZ[0] << "),"
      << " det="  << setw(12) << fDetectorID[0] << ","
      << " pos=(" << setw(12) << fVX[0] << "," << setw(12) << fVY[0] << "," << setw(12) << fVZ[0] << ")" << endl;
  }
}

void KBMCTrack::SetPX(Double_t val)     { fPX[0] = val; }
void KBMCTrack::SetPY(Double_t val)     { fPY[0] = val; }
void KBMCTrack::SetPZ(Double_t val)     { fPZ[0] = val; }
void KBMCTrack::SetVX(Double_t val)     { fVX[0] = val; }
void KBMCTrack::SetVY(Double_t val)     { fVY[0] = val; }
void KBMCTrack::SetVZ(Double_t val)     { fVZ[0] = val; }
void KBMCTrack::SetKE(Double_t val)     { fKE[0] = val; }
void KBMCTrack::SetDetectorID(Int_t id) { fDetectorID[0] = id; }
void KBMCTrack::SetCreatorProcessID(Int_t id)  { fCreatorProcessID = id; }

void KBMCTrack::SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz, Double_t ke, Int_t processID)
{
  fTrackID = trackID;
  fParentID = parentID;
  fPDG = pdg;
  fPX[0] = px;
  fPY[0] = py;
  fPZ[0] = pz;
  fVX[0] = vx;
  fVY[0] = vy;
  fVZ[0] = vz;
  fKE[0] = ke;
  fDetectorID[0] = detectorID;
  fCreatorProcessID = processID;
}

void KBMCTrack::AddVertex(Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz)
{
  fPX.push_back(px);
  fPY.push_back(py);
  fPZ.push_back(pz);
  fVX.push_back(vx);
  fVY.push_back(vy);
  fVZ.push_back(vz);
  fDetectorID.push_back(detectorID);
}

Int_t KBMCTrack::GetNumVertices() const { return (Int_t) fPX.size(); }

Double_t KBMCTrack::GetPX(Int_t idx) const { return fPX[idx]; }
Double_t KBMCTrack::GetPY(Int_t idx) const { return fPY[idx]; }
Double_t KBMCTrack::GetPZ(Int_t idx) const { return fPZ[idx]; }
TVector3 KBMCTrack::GetMomentum(Int_t idx) const { return TVector3(fPX[idx], fPY[idx], fPZ[idx]); }

Double_t KBMCTrack::GetVX(Int_t idx) const { return fVX[idx]; }
Double_t KBMCTrack::GetVY(Int_t idx) const { return fVY[idx]; }
Double_t KBMCTrack::GetVZ(Int_t idx) const { return fVZ[idx]; }
TVector3 KBMCTrack::GetVertex(Int_t idx) const { return TVector3(fVX[idx], fVY[idx], fVZ[idx]); }

Double_t KBMCTrack::GetKE(Int_t idx) const { return fKE[idx]; }

Int_t KBMCTrack::GetDetectorID(Int_t idx) const { return fDetectorID[idx]; }

Int_t KBMCTrack::GetCreatorProcessID() const { return fCreatorProcessID; }

TVector3 KBMCTrack::GetPrimaryPosition() const { return TVector3(fVX[0], fVY[0], fVZ[0]); }
   Int_t KBMCTrack::GetPrimaryDetectorID() const { return fDetectorID[0]; }

void KBMCTrack::AddStep(KBMCStep *hit) { fStepArray.push_back(hit); }
vector<KBMCStep *> *KBMCTrack::GetStepArray() { return &fStepArray; }

TVector3 KBMCTrack::Momentum(Double_t) const { return GetMomentum(); }
TVector3 KBMCTrack::PositionAtHead() const { return GetPrimaryPosition() + GetMomentum(); }
TVector3 KBMCTrack::PositionAtTail() const { return GetPrimaryPosition(); }
Double_t KBMCTrack::TrackLength() const
{
  Int_t nPairs = fVX.size() - 1;
  if (nPairs < 2)
    return 0.;

  Double_t length = 0.;

  for (auto iPair = 0; iPair < nPairs; ++iPair) {
    TVector3 a(fVX[iPair], fVY[iPair], fVZ[iPair]);
    TVector3 b(fVX[iPair+1], fVY[iPair+1], fVZ[iPair+1]);
    length += (b-a).Mag();
  }

  return length;
}

TVector3 KBMCTrack::ExtrapolateTo(TVector3)       const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateHead(Double_t)     const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateTail(Double_t)     const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateByRatio(Double_t)  const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateByLength(Double_t) const { return TVector3(); } //@todo
Double_t KBMCTrack::LengthAt(TVector3)            const { return 0; }          //@todo

#ifdef ACTIVATE_EVE
bool KBMCTrack::DrawByDefault() { return true; }
bool KBMCTrack::IsEveSet() { return false; }

TEveElement *KBMCTrack::CreateEveElement()
{
  auto element = new TEveLine();

  return element;
}

void KBMCTrack::SetEveElement(TEveElement *element, Double_t scale)
{
  auto line = (TEveLine *) element;
  line -> Reset();

  auto particle = TDatabasePDG::Instance() -> GetParticle(fPDG);

  TString pName;
  TString pClass;
  Int_t pCharge = 0;

  if (particle == nullptr)
    pName = Form("%d",fPDG);
  else {
    pName = particle -> GetName();
    pClass = particle -> ParticleClass();
    pCharge = particle -> Charge();
  }

  Color_t color = kGray+2;
  Color_t width = 1;

  if (pClass == "Lepton") {
         if (pCharge < 0) color = kOrange-3;
    else if (pCharge > 0) color = kAzure+7;
    else                  color = kGray+1;
  }
  else if (pClass == "Meson") {
         if (pCharge < 0) color = kPink-1;
    else if (pCharge > 0) color = kCyan+1;
    else                  color = kGray+1;
  }
  else if (pClass == "Baryon") {
         if (pCharge < 0) color = kRed-4;
    else if (pCharge > 0) color = kBlue-4;
    else                  color = kOrange;
  }
  else if (pClass == "Ion") {
    width = 2;
         if (pCharge < 0) color = kRed;
    else if (pCharge > 0) color = kBlue;
    else                  color = kGray+1;
  }
  else if (pClass == "GaugeBoson")
    color = kGreen;

  line -> SetLineColor(color);
  line -> SetLineWidth(width);

  TString eveName = Form("mc_%s:%d(%d)[%.1f]{%d;%d}",pName.Data(),fTrackID,fParentID,Momentum().Mag(),fDetectorID.at(0),GetNumVertices());
  line -> SetElementName(eveName);

  if (fParentID != 0)
    line -> SetLineStyle(2);

  Int_t numVertices = fPX.size();
  if (numVertices==1) {
    TVector3 pos0 = scale*GetPrimaryPosition();
    TVector3 pos1 = scale*pos0 + scale*GetMomentum().Unit();

    line -> SetNextPoint(pos0.X(), pos0.Y(), pos0.Z());
    line -> SetNextPoint(pos1.X(), pos1.Y(), pos1.Z());
  }
  else {
    for (auto idx = 0; idx < numVertices; ++idx)
      line -> SetNextPoint(scale*fVX[idx], scale*fVY[idx], scale*fVZ[idx]);
  }
}

void KBMCTrack::AddToEveSet(TEveElement *, Double_t)
{
}
#endif

TGraph *KBMCTrack::TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, Double_t scale)
{
  if (fTrajectoryOnPlane == nullptr)
  {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);

    auto particle = TDatabasePDG::Instance() -> GetParticle(fPDG);

    TString pClass;
    Int_t pCharge = 0;

    if (particle == nullptr) {}
    else {
      pClass = particle -> ParticleClass();
      pCharge = particle -> Charge();
    }

    Color_t color = kGray+2;
    Color_t width = 1;

    if (pClass == "Lepton") {
      if (pCharge < 0) color = kOrange-3;
      else if (pCharge > 0) color = kAzure+7;
      else                  color = kGray+1;
    }
    else if (pClass == "Meson") {
      if (pCharge < 0) color = kPink-1;
      else if (pCharge > 0) color = kCyan+1;
      else                  color = kGray+1;
    }
    else if (pClass == "Baryon") {
      if (pCharge < 0) color = kRed-4;
      else if (pCharge > 0) color = kBlue-4;
      else                  color = kOrange;
    }
    else if (pClass == "Ion") {
      width = 2;
      if (pCharge < 0) color = kRed;
      else if (pCharge > 0) color = kBlue;
      else                  color = kGray+1;
    }
    else if (pClass == "GaugeBoson")
      color = kGreen;

    fTrajectoryOnPlane  -> SetLineColor(color);
    fTrajectoryOnPlane  -> SetLineWidth(width);
  }

  fTrajectoryOnPlane -> Set(0);

  if (fParentID != 0)
    fTrajectoryOnPlane  -> SetLineStyle(2);



  Int_t numVertices = fPX.size();
  if (numVertices==1) {
    auto pos0 = scale * KBVector3(GetPrimaryPosition());
    auto pos1 = scale * (pos0 + KBVector3(GetMomentum().Unit()));

    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane -> GetN(), pos0.At(axis1), pos0.At(axis2));
    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane -> GetN(), pos1.At(axis1), pos1.At(axis2));
  }
  else {
    for (auto idx = 0; idx < numVertices; ++idx) {
      auto pos = scale * KBVector3(fVX[idx], fVY[idx], fVZ[idx]);
      fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane -> GetN(), pos.At(axis1), pos.At(axis2));
    }
  }

  return fTrajectoryOnPlane;
}

TGraph *KBMCTrack::TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, bool (*fisout)(TVector3 pos), Double_t scale)
{
  if (fTrajectoryOnPlane == nullptr)
  {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);

    auto particle = TDatabasePDG::Instance() -> GetParticle(fPDG);

    TString pClass;
    Int_t pCharge = 0;

    if (particle == nullptr) {}
    else {
      pClass = particle -> ParticleClass();
      pCharge = particle -> Charge();
    }

    Color_t color = kGray+2;
    Color_t width = 1;

    if (pClass == "Lepton") {
      if (pCharge < 0) color = kOrange-3;
      else if (pCharge > 0) color = kAzure+7;
      else                  color = kGray+1;
    }
    else if (pClass == "Meson") {
      if (pCharge < 0) color = kPink-1;
      else if (pCharge > 0) color = kCyan+1;
      else                  color = kGray+1;
    }
    else if (pClass == "Baryon") {
      if (pCharge < 0) color = kRed-4;
      else if (pCharge > 0) color = kBlue-4;
      else                  color = kOrange;
    }
    else if (pClass == "Ion") {
      width = 2;
      if (pCharge < 0) color = kRed;
      else if (pCharge > 0) color = kBlue;
      else                  color = kGray+1;
    }
    else if (pClass == "GaugeBoson")
      color = kGreen;

    fTrajectoryOnPlane  -> SetLineColor(color);
    fTrajectoryOnPlane  -> SetLineWidth(width);
  }

  fTrajectoryOnPlane -> Set(0);

  if (fParentID != 0)
    fTrajectoryOnPlane  -> SetLineStyle(2);



  Int_t numVertices = fPX.size();
  if (numVertices==1) {
    auto pos0 = scale * KBVector3(GetPrimaryPosition());
    auto pos1 = scale * (pos0 + KBVector3(GetMomentum().Unit()));

    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane -> GetN(), pos0.At(axis1), pos0.At(axis2));
    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane -> GetN(), pos1.At(axis1), pos1.At(axis2));
  }
  else {
    bool isout;
    for (auto idx = 0; idx < numVertices; ++idx) {
      auto pos = scale * KBVector3(fVX[idx], fVY[idx], fVZ[idx]);
      isout = fisout(pos);
      if (isout)
        break;

      fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane -> GetN(), pos.At(axis1), pos.At(axis2));
    }
  }

  return fTrajectoryOnPlane;
}
