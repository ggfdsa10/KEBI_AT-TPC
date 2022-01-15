#ifndef KBGEOPLANEWITHCENTER_HH
#define KBGEOPLANEWITHCENTER_HH

#include "KBGeoPlane.hh"

class KBGeoPlaneWithCenter : public KBGeoPlane
{
  public:
    KBGeoPlaneWithCenter();
    KBGeoPlaneWithCenter(TVector3 pos, TVector3 nnn);
    KBGeoPlaneWithCenter(Double_t x, Double_t y, Double_t z, Double_t xn, Double_t yn, Double_t zn);
    virtual ~KBGeoPlaneWithCenter() {}

    virtual void SetPlane(TVector3 pos, TVector3 nnn);
    virtual void SetPlane(Double_t x, Double_t y, Double_t z, Double_t xn, Double_t yn, Double_t zn);

    TVector3 GetCenter() const;

    Double_t GetX() const;
    Double_t GetY() const;
    Double_t GetZ() const;

    TVector3 GetVectorU() const;
    TVector3 GetVectorV() const;

  protected:
    Double_t fX = 0;
    Double_t fY = 0;
    Double_t fZ = 0;

  ClassDef(KBGeoPlaneWithCenter, 1)
};

#endif
