#ifndef KBGEOPLANE_HH
#define KBGEOPLANE_HH

#include "TVector3.h"

#include "KBVector3.hh"
#include "KBGeometry.hh"

/// fA*x + fB*y + fC*z + fD = 0
class KBGeoPlane : public KBGeometry
{
  public:
    KBGeoPlane();
    KBGeoPlane(TVector3 pos, TVector3 nnn);
    KBGeoPlane(Double_t x, Double_t y, Double_t z, Double_t xn, Double_t yn, Double_t zn);
    KBGeoPlane(Double_t a, Double_t b, Double_t c, Double_t d);
    virtual ~KBGeoPlane() {}

    void SetPlane(TVector3 pos, TVector3 nnn);
    void SetPlane(Double_t x, Double_t y, Double_t z, Double_t xn, Double_t yn, Double_t zn);
    void SetPlane(Double_t a, Double_t b, Double_t c, Double_t d);

    Double_t GetA() const;
    Double_t GetB() const;
    Double_t GetC() const;
    Double_t GetD() const;
    TVector3 GetNormal() const;

    Double_t GetX(Double_t y, Double_t z) const;
    Double_t GetY(Double_t z, Double_t x) const;
    Double_t GetZ(Double_t x, Double_t y) const;

    virtual TVector3 ClosestPointOnPlane(TVector3 pos) const;

    //virtual TVector3 ClosestPointOnPlane(TVector3 pos) const;
    //virtual Double_t DistanceToPlane(Double_t x, Double_t y, Double_t z) const;
    //virtual Double_t DistanceToPlane(TVector3 pos) const;

  protected:
    Double_t fA = 0;
    Double_t fB = 0;
    Double_t fC = 0;
    Double_t fD = 0;

  ClassDef(KBGeoPlane, 1)
};

#endif
