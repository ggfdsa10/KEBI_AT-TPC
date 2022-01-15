#include "KBTpcHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include <iostream>
#include <iomanip>

ClassImp(KBTpcHit)

void KBTpcHit::Clear(Option_t *option)
{
  KBHit::Clear(option);

  fPadID = -1;
  fSection = -999;
  fRow = -999;
  fLayer = -999;
  fTb = -1;

  fTrackCandArray.clear();
}

void KBTpcHit::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (opts.Index("s")>=0)
    kr_info(0)
      << "HTMP-ID: " << fHitID << ", " << fTrackID << ", " << fMCID << ", " << fPadID
      << "| XYZ: " << fX << ", " << fY << ", " << fZ << " |, Q: " << fW
      << "| SLR: " << fSection << ", "<< fRow << ", "<< fLayer << " | Tb: " << fTb << endl;
  else
    kr_info(0)
      << "HTMP-ID: "
      << setw(4)  << fHitID
      << setw(4)  << fTrackID
      << setw(4)  << fMCID
      << setw(4)  << fPadID
      << "| XYZ: "
      << setw(12) << fX
      << setw(12) << fY
      << setw(12) << fZ
      << "| Q: "
      << setw(12) << fW
      << "| SRL: "
      << setw(4) << fSection
      << setw(4) << fRow
      << setw(4) << fLayer
      << "| Tb: "
      << setw(4) << fTb << endl;

  if (opts.Index(">")>=0)
    fHitArray.PrintHits(1);
}

void KBTpcHit::PrintTpcHit() const
{
  kr_info(0)
    << "HTMP-ID: "
    << setw(4)  << fHitID
    << setw(4)  << fTrackID
    << setw(4)  << fMCID
    << setw(4)  << fPadID
    << "| XYZ: "
    << setw(12) << fX
    << setw(12) << fY
    << setw(12) << fZ
    << "| Q: "
    << setw(12) << fW
    << "| SRL: "
    << setw(4) << fSection
    << setw(4) << fRow
    << setw(4) << fLayer
    << "| Tb: "
    << setw(4) << fTb << endl;
}

void KBTpcHit::CopyFrom(KBTpcHit const *hit)
{
  fMCError   = hit -> GetMCError  ();
  fMCID      = hit -> GetMCID     ();
  fMCPurity  = hit -> GetMCPurity ();
  fX         = hit -> GetX        ();
  fY         = hit -> GetY        ();
  fZ         = hit -> GetZ        ();
  fW         = hit -> GetCharge   ();
  fAlpha     = hit -> GetAlpha    ();
  fHitID     = hit -> GetHitID    ();
  fSortValue = hit -> GetSortValue();
  fTrackID   = hit -> GetTrackID  ();
  fPadID     = hit -> GetPadID    ();
  fSection   = hit -> GetSection  ();
  fRow       = hit -> GetRow      ();
  fLayer     = hit -> GetLayer    ();
  fTb        = hit -> GetTb       ();
}

void KBTpcHit::Copy(TObject &obj) const
{
  //KBHit::Copy(obj);
  auto tpchit = (KBTpcHit &) obj;

  tpchit.fMCError   = fMCError  ;
  tpchit.fMCID      = fMCID     ;
  tpchit.fMCPurity  = fMCPurity ;
  tpchit.fX         = fX        ;
  tpchit.fY         = fY        ;
  tpchit.fZ         = fZ        ;
  tpchit.fW         = fW        ;
  tpchit.fAlpha     = fAlpha    ;
  tpchit.fHitID     = fHitID    ;
  tpchit.fSortValue = fSortValue;
  tpchit.fTrackID   = fTrackID  ;
  tpchit.fPadID     = fPadID    ;
  tpchit.fSection   = fSection  ;
  tpchit.fRow       = fRow      ;
  tpchit.fLayer     = fLayer    ;
  tpchit.fTb        = fTb       ;
}

void KBTpcHit::AddHit(KBTpcHit *hit)
{
  auto w0 = fW;
  KBHit::AddHit((KBHit *) hit);
  fTb = (w0*fTb + hit->GetCharge()*hit->GetTb()) / (w0+hit->GetCharge());
}

TF1 *KBTpcHit::GetPulseFunction(Option_t *)
{
  auto pulseGen = KBPulseGenerator::GetPulseGenerator();
  auto f1 = pulseGen -> GetPulseFunction("pulse");
  f1 -> SetParameters(fW, fTb);
  return f1;
}

void KBTpcHit::SetPadID(Int_t id) { fPadID = id; }
void KBTpcHit::SetSection(Int_t section) { fSection = section; }
void KBTpcHit::SetRow(Int_t row) { fRow = row; }
void KBTpcHit::SetLayer(Int_t layer) { fLayer = layer; }
void KBTpcHit::SetTb(Double_t tb) { fTb = tb; }

Int_t KBTpcHit::GetPadID() const { return fPadID; }
Int_t KBTpcHit::GetSection() const { return fSection; }
Int_t KBTpcHit::GetRow() const { return fRow; }
Int_t KBTpcHit::GetLayer() const { return fLayer; }
Double_t KBTpcHit::GetTb() const { return fTb; }

void KBTpcHit::SetSortByLayer(bool sortEarlierIfSmaller)
{
  if (sortEarlierIfSmaller) fSortValue =  fLayer;
  else                      fSortValue = -fLayer;
}

#ifdef ACTIVATE_EVE
TEveElement *KBTpcHit::CreateEveElement()
{
  auto pointSet = new TEvePointSet("TpcHit");
  pointSet -> SetMarkerColor(kAzure-8);
  pointSet -> SetMarkerSize(0.5);
  pointSet -> SetMarkerStyle(20);

  return pointSet;
}
#endif
