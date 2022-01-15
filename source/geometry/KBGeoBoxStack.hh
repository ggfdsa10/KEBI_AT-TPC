#ifndef KBGEOBOXSTACK_HH
#define KBGEOBOXSTACK_HH

#include "TVector3.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TH2D.h"
#include "TH2Poly.h"

#include "KBVector3.hh"
#include "KBGeometry.hh"
#include "KBGeoBox.hh"

typedef KBVector3::Axis kbaxis_t;

class KBGeoBoxStack : public KBGeometry
{
  public:
    KBGeoBoxStack();
    KBGeoBoxStack(Double_t x,  Double_t y,  Double_t z,
                  Double_t dx, Double_t dy, Double_t dz,
                  Int_t n,     kbaxis_t as, kbaxis_t af = KBVector3::kZ);

    virtual ~KBGeoBoxStack() {}

    virtual void Print(Option_t *option = "") const;

    virtual TVector3 GetCenter() const;

    void SetBoxStack(Double_t x,  Double_t y,  Double_t z,
                     Double_t dx, Double_t dy, Double_t dz,
                     Int_t n,     kbaxis_t as, kbaxis_t af = KBVector3::kZ);

    kbaxis_t GetStackAxis() const;
    kbaxis_t GetFaceAxis() const;

    Double_t GetStackAxisCenter() const;
    Double_t GetFaceAxisCenter() const;
    Double_t GetLongAxisCenter() const;

    Double_t GetStackAxisMax() const;
    Double_t GetFaceAxisMax() const;
    Double_t GetLongAxisMax() const;

    Double_t GetStackAxisMin() const;
    Double_t GetFaceAxisMin() const;
    Double_t GetLongAxisMin() const;

    Double_t GetStackAxisDisplacement() const;
    Double_t GetFaceAxisDisplacement() const;
    Double_t GetLongAxisDisplacement() const;

    KBGeoBox GetBox(Int_t idx) const;

    TMultiGraph *DrawStackGraph   (kbaxis_t a1 = KBVector3::kNon, kbaxis_t a2 = KBVector3::kNon);
    TH2D        *DrawStackHist    (TString name="", TString title="", kbaxis_t a1 = KBVector3::kNon, kbaxis_t a2 = KBVector3::kNon);
    TH2Poly     *DrawStackHistPoly(TString name="", TString title="", kbaxis_t a1 = KBVector3::kNon, kbaxis_t a2 = KBVector3::kNon);

    Int_t FindBoxIndex(TVector3 pos) const;
    Int_t FindBoxIndex(Double_t x, Double_t y, Double_t z) const;

  protected:
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fdX;
    Double_t fdY;
    Double_t fdZ;
       Int_t fNumStacks;
    kbaxis_t fStackAxis;
    kbaxis_t fFaceAxis;

  ClassDef(KBGeoBoxStack, 1)
};

#endif
