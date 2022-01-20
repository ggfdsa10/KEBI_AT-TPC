#ifndef KBGEOROTATED_HH
#define KBGEOROTATED_HH

#include "KBGeometry.hh"
#include "TRotation.h"

class KBGeoRotated : public KBGeometry
{
  protected:
    TRotation fRotation;

  public:
    KBGeoRotated() {}
    virtual ~KBGeoRotated() {}

    virtual void Clear(Option_t *option = "");

    virtual TVector3 GetCenter() const { return TVector3(); }

    TVector3 Rotate(TVector3 pos) const; //< rotate due to center position
    TVector3 InvRotate(TVector3 pos) const; //< inverse rotate due to center position

    void SetRotation(TRotation rot) { fRotation = rot; }
    TRotation GetRotation() const { return fRotation; }

  ClassDef(KBGeoRotated, 1)
};

#endif
