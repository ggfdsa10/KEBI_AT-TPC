#ifndef LAMPSHIGHPADPLANE3_HH
#define LAMPSHIGHPADPLANE3_HH

#include "KBPadPlane.hh"
#include "TF1.h"

class LHPadPlane3 : public KBPadPlane
{
  public:
    LHPadPlane3();
    virtual ~LHPadPlane3() {}

    virtual bool Init();

    virtual Int_t FindPadID(Double_t i, Double_t j);
    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer);

    virtual Double_t PadDisplacement() const;

    virtual bool IsInBoundary(Double_t i, Double_t j);

    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "-1");

    virtual TCanvas *GetCanvas(Option_t *optiont = "");

  private:
    KBPad *NewPad(Int_t s, Int_t r, Int_t l);
    void SetNeighborPads(KBPad *pad0, KBPad *pad1);
    Int_t FindSection(Double_t i, Double_t j);

  private:
    TF1* fFuncXRightBound = nullptr;
    TF1* fFuncXRightBoundInverse = nullptr;

    Double_t fRMin = 100.;
    Double_t fRMax = 510.;
    Double_t fPadGap = 0.5;
    Double_t fPadWidth = 3.;
    Double_t fPadHeight = 10.;
    Double_t fRadiusLayer0 = 110.;
    Double_t fRTopCut = 506.;
    Int_t fNumLayers = 42;

    Double_t fTanPi1o8;
    Double_t fTanPi3o8;
    Double_t fTanPi5o8;
    Double_t fTanPi7o8;
    Double_t fCosPiNo4[8];
    Double_t fSinPiNo4[8];

    int fNumHalfRowsInLayer[50] = {0};
    int fNumPadsDownToLayer[50] = {0}; ///< in single section
    int fNumSkippedHalfRows[50] = {0};

    Double_t fXSpacing;
    Double_t fRSpacing;

    const Bool_t fDoCutTopBoundary = true;
    const Bool_t fDoCutSideBoundary = true;

  public:
    double GetPadCutBoundaryYAtX(double x) { return fTanPi3o8*(x+7./2./fTanPi3o8); };
    double GetPadCutBoundaryXAtY(double y) { return (y/fTanPi3o8)-7./2./fTanPi3o8; };
    double GetPadCenterYBoundAtX(double x) { return fTanPi3o8*(x-.5) + 10; };

    Double_t GetRMin()            const { return fRMin; }
    Double_t GetRMax()            const { return fRMax; }
    Double_t GetPadGap()          const { return fPadGap; }
    Double_t GetPadWidth()        const { return fPadWidth; }
    Double_t GetPadHeight()       const { return fPadHeight; }
       Int_t GetNumLayer()        const { return fNumLayers; }
    Double_t GetPadXSpacing()     const { return fXSpacing; }
    Double_t GetPadRSpacing()     const { return fRSpacing; }

  ClassDef(LHPadPlane3, 1)
};

#endif
