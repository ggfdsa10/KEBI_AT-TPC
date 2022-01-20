#include "KBGeoRotated.hh"

ClassImp(KBGeoRotated)

void KBGeoRotated::Clear(Option_t *)
{
  fRotation = TRotation();
}

TVector3 KBGeoRotated::Rotate(TVector3 pos) const
{
  TVector3 center = GetCenter();
  TVector3 tpos = (pos-center).Transform(fRotation);
  tpos = tpos + center;

  return tpos;
}

TVector3 KBGeoRotated::InvRotate(TVector3 pos) const
{
  TVector3 center = GetCenter();
  TVector3 tpos = (pos-center).Transform((fRotation.Inverse()));
  tpos = tpos + center;

  return tpos;
}
