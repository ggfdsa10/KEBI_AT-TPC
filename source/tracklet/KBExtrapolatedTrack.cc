#include "KBExtrapolatedTrack.hh"
#include "KBGeoLine.hh"

#include <iostream>
using namespace std;

ClassImp(KBExtrapolatedTrack)

KBExtrapolatedTrack::KBExtrapolatedTrack() 
{
  Clear();
}

void KBExtrapolatedTrack::Clear(Option_t *)
{
  KBTracklet::Clear();

  fParentID = -99999;
  fTrackID = -99999;

  fPoints.clear();
}

void KBExtrapolatedTrack::Print(Option_t *) const
{
  cout << "KBExtrapolatedTrack " << fTrackID << "(" << fParentID << ") with "
    << fPoints.size() << " points" << endl;;
}

TVector3 KBExtrapolatedTrack::Momentum(Double_t) const { return TVector3(); }
TVector3 KBExtrapolatedTrack::PositionAtHead() const {
  if (GetNumPoints() > 0)
    return fPoints.back();
  return TVector3();
}
TVector3 KBExtrapolatedTrack::PositionAtTail() const {
  if (GetNumPoints() > 0)
    return fPoints.at(0);
  return TVector3();
}
Double_t KBExtrapolatedTrack::TrackLength() const {
  if (GetNumPoints() > 1)
    return fLengths.back() - fLengths.at(0);
  return 0;
}

void KBExtrapolatedTrack::SetParentID(Int_t id) { fParentID = id; }
void KBExtrapolatedTrack::SetTrackID(Int_t id) { fTrackID = id; } 
void KBExtrapolatedTrack::AddPoint(TVector3 point, Double_t length)
{
  if (length >= 0)
    fLengths.push_back(length);
  else if (fPoints.size() > 0) {
    auto last_point = fPoints.back();
    auto last_length = fLengths.back();
    length = last_length + (point-last_point).Mag();
    fLengths.push_back(length);
  }
  else {
    fLengths.push_back(0);
  }
  fPoints.push_back(point);
}

Int_t KBExtrapolatedTrack::GetParentID() const { return fParentID; }
Int_t KBExtrapolatedTrack::GetTrackID() const { return fTrackID; }
vector<TVector3> *KBExtrapolatedTrack::GetPoints() { return &fPoints; }
vector<Double_t> *KBExtrapolatedTrack::GetLengths() { return &fLengths; }

Int_t KBExtrapolatedTrack::GetNumPoints() const { return (Int_t) fPoints.size(); }

TVector3 KBExtrapolatedTrack::ExtrapolateTo(TVector3 pos) const
{
  Int_t idx1 = 0;
  TVector3 pos1;
  Double_t dist1 = DBL_MAX;

  Int_t idx2 = 0;
  TVector3 pos2;
  Double_t dist2 = DBL_MAX;

  Int_t numPoints = fPoints.size();

  if (numPoints == 0)
    return TVector3();
  else if (numPoints == 1)
    return fPoints.at(0);


  for (Int_t iPoint=0; iPoint<numPoints; ++iPoint)
  {
    TVector3 pos_ex = fPoints.at(iPoint);
    Double_t dist_ex = (pos_ex-pos).Mag();

    if (dist1 > dist_ex) {
      idx1 = iPoint;
      pos1 = pos_ex;
      dist1 = dist_ex;
    }
  }

  if (idx1==0) {
    idx2 = 1;
    pos2 = fPoints.at(idx2);
    dist2 = (pos2-pos).Mag();
  }

  if (idx1==numPoints-1) {
    idx2 = numPoints-2;
    pos2 = fPoints.at(idx2);
    dist2 = (pos2-pos).Mag();
  }

  auto poca = KBGeoLine(pos1,pos2).ClosestPointOnLine(pos);
  return poca;
}

TVector3 KBExtrapolatedTrack::ExtrapolateHead(Double_t length) const
{
  Int_t numPoints = fPoints.size();

  if (numPoints == 0)
    return TVector3();
  else if (numPoints == 1)
    return fPoints.at(0);

  TVector3 point1 = fPoints.at(numPoints-1);
  TVector3 point2 = fPoints.at(numPoints-2);

  Double_t dlength = (point1 - point2).Mag();

  auto point = 1./(length-(length+dlength)) * (length*point2 - (length+dlength)*point1);
  return point;
}

TVector3 KBExtrapolatedTrack::ExtrapolateTail(Double_t length) const
{
  Int_t numPoints = fPoints.size();

  if (numPoints == 0)
    return TVector3();
  else if (numPoints == 1)
    return fPoints.at(0);

  TVector3 point1 = fPoints.at(0);
  TVector3 point2 = fPoints.at(1);

  Double_t dlength = (point1 - point2).Mag();

  auto point = 1./(length-(length+dlength)) * (length*point2 - (length+dlength)*point1);
  return point;
}

TVector3 KBExtrapolatedTrack::ExtrapolateByLength(Double_t length) const
{
  Int_t numPoints = fPoints.size();
  if (numPoints < 2)
    return TVector3();

  Double_t length1 = fLengths.at(0);
  Double_t length2 = fLengths.at(1);
  TVector3  point1 =  fPoints.at(0);
  TVector3  point2 =  fPoints.at(1);

  if (length < length1) {
    auto d1 = length - length1;
    auto d2 = length - length2;
    auto point = 1./(d1-d2) * (d1*point2 - d2*point1);
    return point;
  }

  for (auto iPoint=2; iPoint<numPoints; ++iPoint)
  {
    length1 = length2;
     point1 =  point2;

    length2 = fLengths.at(iPoint);
     point2 =  fPoints.at(iPoint);

    if (length2 > length){
      break;
    }
  }

  if (length < length2) {
    auto d1 = length - length1;
    auto d2 = length2 - length;
    auto point = 1./(d1+d2) * (d1*point2 + d2*point1);
    return point;
  }

  auto d1 = length - length1;
  auto d2 = length - length2;
  auto point = 1./(d1-d2) * (d1*point2 - d2*point1);
  return point;
}

TGraph *KBExtrapolatedTrack::TrajectoryOnPlane(KBVector3::Axis axis1, KBVector3::Axis axis2, Double_t)
{
  if (fTrajectoryOnPlane == nullptr) {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);
  }

  fTrajectoryOnPlane -> Set(0);

  Int_t numPoints = GetNumPoints();

  for (auto iPoint=0; iPoint<numPoints; ++iPoint)
  {
    auto pos =  KBVector3(fPoints.at(iPoint),KBVector3::kZ);
    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane->GetN(), pos.At(axis1), pos.At(axis2));
  }

  return fTrajectoryOnPlane;
}

TGraph *KBExtrapolatedTrack::TrajectoryOnPlane(KBVector3::Axis axis1, KBVector3::Axis axis2, bool (*fisout)(TVector3 pos), Double_t)
{
  if (fTrajectoryOnPlane == nullptr) {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);
  }

  fTrajectoryOnPlane -> Set(0);

  Int_t numPoints = GetNumPoints();

  bool isout;
  for (auto iPoint=0; iPoint<numPoints; ++iPoint)
  {
    auto pos =  KBVector3(fPoints.at(iPoint),KBVector3::kZ);

    isout = fisout(pos);
    if (isout)
      break;

    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane->GetN(), pos.At(axis1), pos.At(axis2));
  }

  fTrajectoryOnPlane -> SetLineColor(kRed);
  if (isout)
    fTrajectoryOnPlane -> SetLineColor(kGray+1);

  return fTrajectoryOnPlane;
}
