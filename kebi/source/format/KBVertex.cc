#include "KBVertex.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif

ClassImp(KBVertex)

KBVertex::KBVertex()
{
  Clear();
}

void KBVertex::Clear(Option_t *option)
{
  KBHit::Clear(option);

  fTrackArray.clear();
  fTrackIDArray.clear();
}

void KBVertex::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (opts.Index("s")>=0)
    kr_info(0) << "Vertex at (" << fX << "," << fY << "," << fZ
      << ") [mm] containing " << GetNumTracks() << "tracks" << endl;
  else //if (opts.Index("a")>=0)
    kr_info(0) << "Vertex at (" << setw(12) << fX <<"," << setw(12) << fY <<"," << setw(12) << fZ
      << ") [mm] containing " << GetNumTracks() << "tracks" << endl;
}

void KBVertex::Copy(TObject &obj) const
{
  KBHit::Copy(obj);

  auto vertex = (KBVertex &) obj;

  for (auto track : fTrackArray)
    vertex.AddTrack(track);
}

void KBVertex::AddTrack(KBTracklet* track)
{
  fTrackArray.push_back(track);
  fTrackIDArray.push_back(track->GetTrackID());
}

#ifdef ACTIVATE_EVE
bool KBVertex::DrawByDefault() { return true; }

bool KBVertex::IsEveSet() { return true; }

TEveElement *KBVertex::CreateEveElement() {
  auto pointSet = new TEvePointSet("Vertex");
  pointSet -> SetMarkerColor(kBlack);
  pointSet -> SetMarkerSize(2.5);
  pointSet -> SetMarkerStyle(20);
  return pointSet;
}

void KBVertex::AddToEveSet(TEveElement *eveSet, Double_t scale) {
  auto pointSet = (TEvePointSet *) eveSet;
  pointSet -> SetNextPoint(scale*fX, scale*fY, scale*fZ);
}
#endif
