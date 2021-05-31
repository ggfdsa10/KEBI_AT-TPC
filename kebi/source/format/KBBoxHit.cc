#include "KBBoxHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEveBoxSet.h"
#endif
#include "TMath.h"
#include <iostream>
#include <iomanip>

ClassImp(KBBoxHit)

void KBBoxHit::Clear(Option_t *option)
{
  KBHit::Clear(option);

  fdX = 0;
  fdY = 0;
  fdZ = 0;
}

void KBBoxHit::Copy(TObject &obj) const
{
  KBHit::Copy(obj);
  auto box = (KBBoxHit &) obj;

  box.SetdX(fdX);
  box.SetdY(fdY);
  box.SetdZ(fdZ);
}

void KBBoxHit::Set(Double_t x, Double_t y, Double_t z,
    Double_t dx, Double_t dy, Double_t dz, Double_t q)
{
  KBWPoint::Set(x,y,z,q);
  fdX = dx;
  fdY = dy;
  fdZ = dz;
}

void KBBoxHit::SetdX(Double_t dx) { fdX = dx; }
void KBBoxHit::SetdY(Double_t dy) { fdY = dy; }
void KBBoxHit::SetdZ(Double_t dz) { fdZ = dz; }

Double_t KBBoxHit::GetdX() const { return fdX; }
Double_t KBBoxHit::GetdY() const { return fdY; }
Double_t KBBoxHit::GetdZ() const { return fdZ; }


#ifdef ACTIVATE_EVE
bool KBBoxHit::DrawByDefault() { return true; }
bool KBBoxHit::IsEveSet() { return true; }

TEveElement *KBBoxHit::CreateEveElement()
{
  auto pointSet = new TEveBoxSet("Hit");
  pointSet -> Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, 20);
  pointSet -> SetOwnIds(kTRUE);
  pointSet -> SetAntiFlick(kTRUE);
  pointSet -> SetPalette(new TEveRGBAPalette(0, 4096));
  pointSet -> SetDefDepth(fdX);
  pointSet -> SetDefHeight(fdY);
  pointSet -> SetDefWidth(fdZ);
  return pointSet;
}

void KBBoxHit::SetEveElement(TEveElement *, Double_t)
{
}

void KBBoxHit::AddToEveSet(TEveElement *eveSet, Double_t scale)
{
  auto pointSet = (TEveBoxSet *) eveSet;
  pointSet -> AddBox(
      scale * (fX - pointSet -> GetDefWidth()/2. ),
      scale * (fY - pointSet -> GetDefHeight()/2.),
      scale * (fZ - pointSet -> GetDefDepth()/2. ));
  pointSet -> DigitValue(fW);
}
#endif
