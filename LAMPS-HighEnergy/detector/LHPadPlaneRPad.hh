#ifndef LAMPSPADPLANERPAD_HH
#define LAMPSPADPLANERPAD_HH

#include "KBPadPlane.hh"
#include "TF1.h"

class LHPadPlaneRPad : public KBPadPlane
{
  public:
    LHPadPlaneRPad();
    virtual ~LHPadPlaneRPad() {}

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

    //XXX
    Double_t fPadGap = 0.5;
    Double_t fPadWid = 3.;
    Double_t fPadHei = 10.;
    Double_t fYPPMin = 92.64;
    Double_t fYPPMax = 499.75;
    Double_t fWPPBot = 76.2;
    Double_t fPadAreaLL = 3;

    Double_t fRMin = 0;
    Double_t fRMax = 0;

    Double_t fTanPi1o8;
    Double_t fTanPi3o8;
    Double_t fTanPi5o8;
    Double_t fTanPi7o8;
    Double_t fCosPiNo4[8];
    Double_t fSinPiNo4[8];

    std::map<std::vector<Int_t>, Int_t> fPadMap;

    Int_t fLayerMax;
    std::vector<Int_t> fHalfRowMax;
    Double_t fXSpacing;
    Double_t fYSpacing;

    Bool_t fRemoveCuttedPad = false;

  public:
    Double_t GetPadGap()          const { return fPadGap; }
    Double_t GetPadWid()          const { return fPadWid; }
    Double_t GetPadHei()          const { return fPadHei; }
    Double_t GetYPPMin()          const { return fYPPMin; }
    Double_t GetYPPMax()          const { return fYPPMax; }
    Double_t GetWPPBot()          const { return fWPPBot; }
    Double_t GetPadAreaLL()       const { return fPadAreaLL; }
    Double_t GetRMin()            const { return fRMin; }
    Double_t GetRMax()            const { return fRMax; }
       Int_t GetLayerMax()        const { return fLayerMax; }
    Double_t GetXSpacing()        const { return fXSpacing; }
    Double_t GetYSpacing()        const { return fYSpacing; }
      Bool_t GetRemoveCuttedPad() const { return fRemoveCuttedPad; }

  ClassDef(LHPadPlaneRPad, 1)
};

#endif
