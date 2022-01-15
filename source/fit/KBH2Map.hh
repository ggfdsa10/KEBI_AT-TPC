#ifndef KBH2SAMPLE_HH
#define KBH2SAMPLE_HH

#include "TH2.h"
#include "KBCurve.hh"

class KBH2Point
{
  public:
    KBH2Point() {};
    virtual ~KBH2Point() {};

    void Initialize();

    Int_t NeighborPointID(Int_t ix, Int_t iy);

    void SetIsActive(bool active);
    void SetValue(Double_t v);
    void SetNeighborPoint(Int_t id, KBH2Point *point);

    bool IsActive() const;
    Double_t GetValue() const;
    KBH2Point *GetNeighborPoint(Int_t id) const;
    Double_t GetVNeibor(Int_t id) const;

    Double_t GetVX();
    Double_t GetVY();

  private:
    bool fIsActive = false;
    Double_t fValue = 0;
    KBH2Point *fNeighborPoint[8]; //! <
    Double_t fVNeighbor[8];

    Double_t fVX;
    Double_t fVY;
};

class KBH2Map
{
  public:
    KBH2Map() {};
    KBH2Map(TH2* sample);
    virtual ~KBH2Map() {};

    void SetSample(TH2* sample);
    TH2 *GetSample();

    void FitCurve(KBCurve* curve, Int_t itMax = 20);

    void Draw(Option_t *opt = "");
    void DrawBox(Int_t ix, Int_t iy);
    void DrawField(Double_t headSize = 1, Double_t tailSize = 1);

    void SetNBinsX(Int_t n);
    void SetLowLimitX(Double_t v);
    void SetHighLimitX(Double_t v);
    void SetBinWidthX(Double_t v);

    Int_t    GetNBinsX() const;
    Double_t GetLowLimitX() const;
    Double_t GetHighLimitX() const;
    Double_t GetBinWidthX() const;

    void SetNBinsY(Int_t n);
    void SetLowLimitY(Double_t v);
    void SetHighLimitY(Double_t v);
    void SetBinWidthY(Double_t v);

    Int_t    GetNBinsY() const;
    Double_t GetLowLimitY() const;
    Double_t GetHighLimitY() const;
    Double_t GetBinWidthY() const;

    void SetMaxV(Double_t v);
    Double_t GetMaxV() const;

  private:
    TH2 *fSample;

    KBH2Point ***fH2Points;

    Int_t    fNBinsX    = 0;
    Double_t fLowLimitX  = 0;
    Double_t fHighLimitX  = 0;
    Double_t fBinWidthX = 0;

    Int_t    fNBinsY    = 0;
    Double_t fLowLimitY  = 0;
    Double_t fHighLimitY  = 0;
    Double_t fBinWidthY = 0;

    Double_t fMaxV = 0;
    Double_t fBinWidthXY = 0;


  ClassDef(KBH2Map, 1)
};

#endif
