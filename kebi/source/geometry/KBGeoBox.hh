#ifndef KBGEOBOX_HH
#define KBGEOBOX_HH

#include "TVector3.h"
#include "TGraph.h"

#include "KBVector3.hh"
#include "KBGeoRotated.hh"
#include "KBGeoLine.hh"
#include "KBGeo2DBox.hh"

typedef KBVector3::Axis kbaxis_t;

class KBGeoBox : public KBGeoRotated
{
  public:
    KBGeoBox();
    KBGeoBox(Double_t xc, Double_t yc, Double_t zc, Double_t dx, Double_t dy, Double_t dz);
    KBGeoBox(TVector3 center, Double_t dx, Double_t dy, Double_t dz);
    virtual ~KBGeoBox() {}

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");
    virtual void Copy(KBGeoBox *box) const;

    virtual void SetBox(Double_t xc, Double_t yc, Double_t zc, Double_t dx, Double_t dy, Double_t dz);
    virtual void SetBox(TVector3 center, Double_t dx, Double_t dy, Double_t dz);

    TVector3 GetCenter() const;
    Double_t GetdX() const;
    Double_t GetdY() const;
    Double_t GetdZ() const;

    TVector3 GetCorner(Int_t idx) const;
    TVector3 GetCorner(Int_t xpm, Int_t ypm, Int_t zpm) const; ///< pm should be 1(high) or -1(low)

    KBGeoLine GetEdge(Int_t idx) const;
    KBGeoLine GetEdge(Int_t idxCorner1, Int_t idxCorner2) const;
    KBGeoLine GetEdge(Int_t xpm, Int_t ypm, Int_t zpm) const; ///< pm of edge axis is 0 while the other two should be 1(high) or -1(low)

    KBGeo2DBox GetFace(kbaxis_t xaxis, kbaxis_t yaxis) const;
    //KBGeo2DBox GetFace(Int_t idx) const;
    //KBGeo2DBox GetFace(kbaxis_t axis) const;

    TGraph *Draw2DBox(kbaxis_t axis1 = KBVector3::kX, kbaxis_t axis2 = KBVector3::kY);

    bool IsInside(TVector3 pos);
    bool IsInside(Double_t x, Double_t y, Double_t z);

  protected:
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fdX;
    Double_t fdY;
    Double_t fdZ;

  ClassDef(KBGeoBox, 1)
};

#endif
