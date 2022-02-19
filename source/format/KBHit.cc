#include "KBHit.hh"
#include "KBTracklet.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include "TMath.h"
#include <iostream>
#include <iomanip>

ClassImp(KBHit)

void KBHit::Clear(Option_t *option)
{
  KBWPoint::Clear(option);

  fHitID = -1;
  fTrackID = -1;
  fAlpha = -999;
  fDX = -999;
  fDY = -999;
  fDZ = -999;
  fSortValue = 0;

  fHitArray.Clear("C");
  fTrackCandArray.clear();
}

void KBHit::Print(Option_t *option) const
{
  TString opts = TString(option);

  Int_t rank = 0;
  if (TString(opts[0]).IsDec())
    rank = TString(opts[0]).Atoi();

  TString title;
  if (opts.Index("t")>=0) title += "HTM-ID|XYZ|Q: ";

  if (opts.Index("s")>=0)
    kr_info(rank) << title
      << fHitID << ", " << fTrackID << ", " << fMCID << " | "
      << fX << ", " << fY << ", " << fZ << " | "
      << fW << endl;
  else //if (opts.Index("a")>=0)
    kr_info(rank) << title
      << setw(4)  << fHitID
      << setw(4)  << fTrackID
      << setw(4)  << fMCID << " |"
      << setw(12) << fX
      << setw(12) << fY
      << setw(12) << fZ << " |"
      << setw(12) << fW << endl;

  if (opts.Index(">")>=0)
    fHitArray.PrintHits(1);
}

void KBHit::Copy(TObject &obj) const
{
  KBWPoint::Copy(obj);
  auto hit = (KBHit &) obj;

  hit.SetHitID(fHitID);
  hit.SetTrackID(fTrackID);

  /* TODO
   auto numHits = fHitArray.GetNumHits();
   for (auto i = 0; i < numHits; ++i)
   hit.AddHit(fHitArray.GetHit(i));
   */
}

void KBHit::CopyFrom(KBHit *hit)
{
  fMCError   = hit -> GetMCError  ();
  fMCID      = hit -> GetMCID     ();
  fMCPurity  = hit -> GetMCPurity ();
  fDX        = hit -> GetDX       ();
  fDY        = hit -> GetDY       ();
  fDZ        = hit -> GetDZ       ();
  fX         = hit -> GetX        ();
  fY         = hit -> GetY        ();
  fZ         = hit -> GetZ        ();
  fW         = hit -> GetCharge   ();
  fAlpha     = hit -> GetAlpha    ();
  fHitID     = hit -> GetHitID    ();
  fSortValue = hit -> GetSortValue();
  fTrackID   = hit -> GetTrackID  ();
}

void KBHit::SetSortValue(Double_t val) { fSortValue = val; }
Double_t KBHit::GetSortValue() const { return fSortValue; }

void KBHit::SetSortByX(bool sortEarlierIfSmaller) {
  if (sortEarlierIfSmaller) fSortValue =  fX;
  else                      fSortValue = -fX;
}

void KBHit::SetSortByY(bool sortEarlierIfSmaller) {
  if (sortEarlierIfSmaller) fSortValue =  fY;
  else                      fSortValue = -fY;
}

void KBHit::SetSortByR(bool sortEarlierIfSmaller) {
  if (sortEarlierIfSmaller) fSortValue =  sqrt(fX*fX+fY*fY);
  else                      fSortValue = -sqrt(fX*fX+fY*fY);
}

void KBHit::SetSortByZ(bool sortEarlierIfSmaller) {
  if (sortEarlierIfSmaller) fSortValue =  fZ;
  else                      fSortValue = -fZ;
}

void KBHit::SetSortByCharge(bool sortEarlierIfSmaller)
{
  if (sortEarlierIfSmaller) fSortValue =  fW;
  else                      fSortValue = -fW;
}

void KBHit::SetSortByDistanceTo(TVector3 point, bool sortEarlierIfCloser)
{
  auto dist = (point - GetPosition()).Mag();
  if (sortEarlierIfCloser) fSortValue =  dist;
  else                     fSortValue = -dist;
}

Bool_t KBHit::IsSortable() const { return true; }

Int_t KBHit::Compare(const TObject *obj) const
{
  auto hitCompare = (KBHit *) obj;

  int sortEarlier = 1;
  int sortLatter = -1;
  int sortSame = 0;

  if (fSortValue < hitCompare -> GetSortValue()) return sortEarlier;
  else if (fSortValue > hitCompare -> GetSortValue()) return sortLatter;

  return sortSame;
}

void KBHit::PropagateMC()
{
  if (fHitArray.GetNumHits()==0)
    return;

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

void KBHit::SetHitID(Int_t id) { fHitID = id; }
void KBHit::SetTrackID(Int_t id) { fTrackID = id; }
void KBHit::SetAlpha(Double_t a) { fAlpha = a; }
void KBHit::SetDPosition(TVector3 dpos) { fDX = dpos.X(); fDY = dpos.Y(); fDZ = dpos.Z(); }
void KBHit::SetDX(Double_t dx) { fDX = dx; }
void KBHit::SetDY(Double_t dy) { fDY = dy; }
void KBHit::SetDZ(Double_t dz) { fDZ = dz; }
void KBHit::SetX(Double_t x) { fX = x; }
void KBHit::SetY(Double_t y) { fY = y; }
void KBHit::SetZ(Double_t z) { fZ = z; }
void KBHit::SetCharge(Double_t charge) { fW = charge; }

void KBHit::AddHit(KBHit *hit)
{
  fHitArray.AddHit(hit);
  fX = fHitArray.GetMeanX();
  fY = fHitArray.GetMeanY();
  fZ = fHitArray.GetMeanZ();
  fW = fHitArray.GetW();
}

void KBHit::RemoveHit(KBHit *hit)
{
  fHitArray.RemoveHit(hit);
  fX = fHitArray.GetMeanX();
  fY = fHitArray.GetMeanY();
  fZ = fHitArray.GetMeanZ();
  fW = fHitArray.GetW();
}

   Int_t KBHit::GetHitID()   const { return fHitID; }
   Int_t KBHit::GetTrackID() const { return fTrackID; }
Double_t KBHit::GetAlpha()   const { return fAlpha; }
TVector3 KBHit::GetDPosition() const { return TVector3(fDX,fDY,fDZ); }
Double_t KBHit::GetDX()      const { return fDX; }
Double_t KBHit::GetDY()      const { return fDY; }
Double_t KBHit::GetDZ()      const { return fDZ; }
Double_t KBHit::GetX()       const { return fX; }
Double_t KBHit::GetY()       const { return fY; }
Double_t KBHit::GetZ()       const { return fZ; }
Double_t KBHit::GetCharge()  const { return fW; }


TVector3 KBHit::GetMean()          const { return fHitArray.GetMean();          }
TVector3 KBHit::GetVariance()      const { return fHitArray.GetVariance();      }
TVector3 KBHit::GetCovariance()    const { return fHitArray.GetCovariance();    }
TVector3 KBHit::GetStdDev()        const { return fHitArray.GetStdDev();        }
TVector3 KBHit::GetSquaredMean()   const { return fHitArray.GetSquaredMean();   }
TVector3 KBHit::GetCoSquaredMean() const { return fHitArray.GetCoSquaredMean(); }

KBVector3 KBHit::GetMean(kbaxis ref)          const { return fHitArray.GetMean(ref);          }
KBVector3 KBHit::GetVariance(kbaxis ref)      const { return fHitArray.GetVariance(ref);      }
KBVector3 KBHit::GetCovariance(kbaxis ref)    const { return fHitArray.GetCovariance(ref);    }
KBVector3 KBHit::GetStdDev(kbaxis ref)        const { return fHitArray.GetStdDev(ref);        }
KBVector3 KBHit::GetSquaredMean(kbaxis ref)   const { return fHitArray.GetSquaredMean(ref);   }
KBVector3 KBHit::GetCoSquaredMean(kbaxis ref) const { return fHitArray.GetCoSquaredMean(ref); }


std::vector<Int_t> *KBHit::GetTrackCandArray() { return &fTrackCandArray; }
Int_t KBHit::GetNumTrackCands() { return fTrackCandArray.size(); }
void KBHit::AddTrackCand(Int_t id) { fTrackCandArray.push_back(id); }

void KBHit::RemoveTrackCand(Int_t trackID)
{
  Int_t n = fTrackCandArray.size();
  for (auto i = 0; i < n; i++) {
    if (fTrackCandArray[i] == trackID) {
      fTrackCandArray.erase(fTrackCandArray.begin()+i); 
      return;
    }
  }
  fTrackCandArray.push_back(-1);
}

#ifdef ACTIVATE_EVE
bool KBHit::DrawByDefault() { return true; }
bool KBHit::IsEveSet() { return true; }

TEveElement *KBHit::CreateEveElement()
{
  auto pointSet = new TEvePointSet("Hit");
  pointSet -> SetMarkerColor(kAzure-8);
  pointSet -> SetMarkerStyle(20);
  pointSet -> SetMarkerSize(0.5);
  //pointSet -> SetMarkerColor(kBlack);
  //pointSet -> SetMarkerSize(1.0);
  //pointSet -> SetMarkerStyle(38);

  return pointSet;
}

void KBHit::SetEveElement(TEveElement *, Double_t)
{
}

void KBHit::AddToEveSet(TEveElement *eveSet, Double_t scale)
{
  auto pointSet = (TEvePointSet *) eveSet;
  pointSet -> SetNextPoint(scale*fX, scale*fY, scale*fZ);
}
#endif
