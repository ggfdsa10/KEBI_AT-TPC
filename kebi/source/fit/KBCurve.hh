#ifndef KBCURVE_HH
#define KBCURVE_HH

#include "TGraph.h"

class KBCurve : public TGraph
{ 
  public:
    KBCurve();
    KBCurve(Int_t nPoints);
    virtual ~KBCurve() {};

    void SetUnitLength(Double_t unitLength);
    Double_t GetUnitLength();

    void SetTension(Double_t tension);
    Double_t GetTension();

    Double_t GetXLow();
    Double_t GetXHigh();

    void Push(Double_t x, Double_t y, Double_t fx, Double_t fy);

    KBCurve *GetDifferentialCurve();

    Double_t DistanceToCurve(Double_t x, Double_t y);

  private:
    Double_t fUnitLength = 1;
    Double_t fTension = 0.01;

  ClassDef(KBCurve, 1)
};

#endif
