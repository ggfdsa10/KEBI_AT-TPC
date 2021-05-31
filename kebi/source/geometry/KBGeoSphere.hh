#ifndef KBGEOSPHERE_HH
#define KBGEOSPHERE_HH

#include "TVector3.h"
#include "TGraph.h"

#include "KBVector3.hh"
#include "KBGeometry.hh"
#include "KBGeoCircle.hh"

class KBGeoSphere : public KBGeometry
{
  public:
    KBGeoSphere();
    KBGeoSphere(Double_t x, Double_t y, Double_t z, Double_t r);
    KBGeoSphere(TVector3 pos, Double_t r);
    virtual ~KBGeoSphere() {}

    void SetSphere(Double_t x, Double_t y, Double_t z, Double_t r);
    void SetSphere(TVector3 pos, Double_t r);

    Double_t GetX() const;
    Double_t GetY() const;
    Double_t GetZ() const;
    Double_t GetR() const;

    virtual TVector3 GetCenter() const;
    Double_t GetRadius() const;

    TGraph *DrawCircleXY(Int_t n = 100, Double_t theta1 = 0, Double_t theta2 = 0);
    TGraph *DrawCircleYZ(Int_t n = 100, Double_t theta1 = 0, Double_t theta2 = 0);
    TGraph *DrawCircleZX(Int_t n = 100, Double_t theta1 = 0, Double_t theta2 = 0);

    TVector3 StereographicProjection(Double_t x, Double_t y);

  protected:
    Double_t fX = 0;
    Double_t fY = 0;
    Double_t fZ = 0;
    Double_t fR = 0;

    KBGeoCircle *circle = nullptr;


  ClassDef(KBGeoSphere, 1)
};

#endif
