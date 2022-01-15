#include "KBTracklet.hh"
#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

ClassImp(KBTracklet)

void KBTracklet::PropagateMC()
{
  vector<Int_t> mcIDs;
  vector<Int_t> counts;

  TIter next(&fHitArray);
  KBHit *component;
  while ((component = (KBHit *) next()))
  {
    auto mcIDCoponent = component -> GetMCID();

    Int_t numMCIDs = mcIDs.size();
    Int_t idxFound = -1;
    for (Int_t idx = 0; idx < numMCIDs; ++idx) {
      if (mcIDs[idx] == mcIDCoponent) {
        idxFound = idx;
        break;
      }
    }
    if (idxFound == -1) {
      mcIDs.push_back(mcIDCoponent);
      counts.push_back(1);
    }
    else {
      counts[idxFound] = counts[idxFound] + 1;
    }
  }

  auto maxCount = 0;
  for (auto count : counts)
    if (count > maxCount)
      maxCount = count;

  vector<Int_t> iIDCandidates;
  for (auto iID = 0; iID < Int_t(counts.size()); ++iID)
    if (counts[iID] == maxCount)
      iIDCandidates.push_back(iID);


  //TODO @todo
  if (iIDCandidates.size() == 1)
  {
    auto iID = iIDCandidates[0];
    auto mcIDFinal = mcIDs[iID];

    auto errorFinal = 0.;
    next.Begin();
    while ((component = (KBHit *) next()))
      if (component -> GetMCID() == mcIDFinal)
        errorFinal += component -> GetMCError();

    errorFinal = errorFinal/counts[iID];
    Double_t purity = Double_t(counts[iID])/fHitArray.GetNumHits();
    SetMCTag(mcIDFinal, errorFinal, purity);
  }
  else
  {
    auto mcIDFinal = 0;
    auto errorFinal = DBL_MAX;
    Double_t purity = -1;

    for (auto iID : iIDCandidates) {
      auto mcIDCand = mcIDs[iID];

      auto errorCand = 0.;
      next.Begin();
      while ((component = (KBHit *) next()))
        if (component -> GetMCID() == mcIDCand)
          errorCand += component -> GetMCError();
      errorCand = errorCand/counts[iID];

      if (errorCand < errorFinal) {
        mcIDFinal = mcIDCand;
        errorFinal = errorCand;
        purity = Double_t(counts[iID])/fHitArray.GetNumHits();
      }
    }
    SetMCTag(mcIDFinal, errorFinal, purity);
  }
}

void KBTracklet::Clear(Option_t *option)
{
  TObject::Clear(option);

  fTrackID = -1;
  fParentID = -1;
  fPDG = -1;

  fHitArray.Clear();  //!
}

void KBTracklet::AddHit(KBHit *hit)
{
  fHitArray.AddHit(hit);
}

void KBTracklet::RemoveHit(KBHit *hit)
{
  fHitArray.RemoveHit(hit);
}

#ifdef ACTIVATE_EVE
bool KBTracklet::DrawByDefault() { return true; }
bool KBTracklet::IsEveSet() { return false; }

TEveElement *KBTracklet::CreateEveElement()
{
  auto element = new TEveLine();

  return element;
}

void KBTracklet::SetEveElement(TEveElement *element, Double_t scale)
{
  auto line = (TEveLine *) element;
  line -> SetElementName("Tracklet");
  line -> Reset();

  line -> SetElementName(Form("Tracklet%d",fTrackID));

  if (fParentID > -1)
    line -> SetLineColor(kPink);
  else
    line -> SetLineColor(kGray);

  auto dr = 0.02;
  if (dr < 5./TrackLength())
    dr = 5./TrackLength();

  for (Double_t r = 0.; r < 1.0001; r += dr) {
    auto pos = scale*ExtrapolateByRatio(r);
    line -> SetNextPoint(pos.X(), pos.Y(), pos.Z());
  }
}

void KBTracklet::AddToEveSet(TEveElement *, Double_t)
{
}
#endif

bool KBTracklet::DoDrawOnDetectorPlane()
{
  return true;
}

TGraph *KBTracklet::TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, Double_t scale)
{
  if (fTrajectoryOnPlane == nullptr) {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);
  }

  fTrajectoryOnPlane -> Set(0);

  for (Double_t r = 0.; r < 1.001; r += 0.02) {
    auto pos = scale * KBVector3(ExtrapolateByRatio(r),KBVector3::kZ);
    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane->GetN(), pos.At(axis1), pos.At(axis2));
  }

  return fTrajectoryOnPlane;
}

TGraph *KBTracklet::TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, bool (*fisout)(TVector3 pos), Double_t scale)
{
  if (fTrajectoryOnPlane == nullptr) {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);
  }

  fTrajectoryOnPlane -> Set(0);

  bool isout;
  for (Double_t r = 0.; r < 100.; r += 0.05) {
    auto pos = scale * KBVector3(ExtrapolateByRatio(r),KBVector3::kZ);
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

TGraph *KBTracklet::TrajectoryOnPlane(KBDetectorPlane *plane, Double_t scale)
{
  return TrajectoryOnPlane(plane->GetAxis1(), plane->GetAxis2(), scale);
}
