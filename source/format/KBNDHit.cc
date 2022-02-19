#include "KBNDHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include <iostream>
#include <iomanip>

ClassImp(KBNDHit)

void KBNDHit::Clear(Option_t *option)
{
  KBHit::Clear(option);

  fLayer = -999;
  fSubLayer = -999;
  fModule = -999;

  fTrackCandArray.clear();
}

void KBNDHit::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (opts.Index("s")>=0)
    kr_info(0)
      << "HTMP-ID: " << fHitID << ", " << fTrackID << ", " << fMCID 
      << "| XYZ: " << fX << ", " << fY << ", " << fZ << " |, Q: " << fW
      << "| LSM: " << fLayer << ", "<< fSubLayer << ", "<< fModule << endl;
  else
    kr_info(0)
      << "HTMP-ID: "
      << setw(4)  << fHitID
      << setw(4)  << fTrackID
      << setw(4)  << fMCID
      << "| XYZ: "
      << setw(12) << fX
      << setw(12) << fY
      << setw(12) << fZ
      << "| Q: "
      << setw(12) << fW
      << "| T: "
      << setw(12) << fTime
      << "| LSM: "
      << setw(4) << fLayer
      << setw(4) << fSubLayer
      << setw(4) << fModule;

  if (opts.Index(">")>=0)
    fHitArray.PrintHits(1);
}

void KBNDHit::PrintNDHit() const
{
  kr_info(0)
    << "HTMP-ID: "
    << setw(4)  << fHitID
    << setw(4)  << fTrackID
    << setw(4)  << fMCID
    << "| XYZ: "
    << setw(12) << fX
    << setw(12) << fY
    << setw(12) << fZ
    << "| Q: "
    << setw(12) << fW
    << "| T: "
    << setw(12) << fTime
    << "| LSM: "
    << setw(4) << fLayer
    << setw(4) << fSubLayer
    << setw(4) << fModule;
}

void KBNDHit::CopyFrom(KBNDHit const *hit)
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
  fLayer     = hit -> GetLayer    ();
  fSubLayer  = hit -> GetSubLayer ();
  fModule    = hit -> GetModule   ();
  fTime		   = hit -> GetTime		  ();
}

void KBNDHit::Copy(TObject &obj) const
{
  //KBHit::Copy(obj);
  auto tpchit = (KBNDHit &) obj;

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
  tpchit.fLayer     = fLayer    ;
  tpchit.fSubLayer  = fSubLayer ;
  tpchit.fModule    = fModule   ;
  tpchit.fTime	    = fTime	    ;
}

/*
void KBNDHit::AddHit(KBNDHit *hit)
{
  KBHit::AddHit((KBHit *) hit);
}
*/

void KBNDHit::SetLayer(Int_t layer) { fLayer = layer; }
void KBNDHit::SetSubLayer(Int_t sublayer) { fSubLayer = sublayer; }
void KBNDHit::SetModule(Int_t module) { fModule = module; }
void KBNDHit::SetTime(Float_t time) { fTime = time; }

Int_t KBNDHit::GetLayer() const { return fLayer; }
Int_t KBNDHit::GetSubLayer() const { return fSubLayer; }
Int_t KBNDHit::GetModule() const { return fModule; }
Float_t KBNDHit::GetTime() const { return fTime; }

/*
void KBNDHit::SetSortByLayer(bool sortEarlierIfSmaller)
{
  if (sortEarlierIfSmaller) fSortValue =  fLayer;
  else                      fSortValue = -fLayer;
}
*/

#ifdef ACTIVATE_EVE
TEveElement *KBNDHit::CreateEveElement()
{
  auto pointSet = new TEvePointSet("NDHit");
  pointSet -> SetMarkerColor(kAzure-8);
  pointSet -> SetMarkerSize(0.5);
  pointSet -> SetMarkerStyle(20);

  return pointSet;
}
#endif
