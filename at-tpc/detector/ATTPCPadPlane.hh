#ifndef ATTPCPADPLANE_HH
#define ATTPCPADPLANE_HH

#include "KBPadPlane.hh"
#include "KBTpcHit.hh"
#include "TF1.h"

typedef vector<KBTpcHit*> KBTpcHits;

class ATTPCPadPlane : public KBPadPlane
{
  public:
    ATTPCPadPlane();
    virtual ~ATTPCPadPlane() {}

    virtual void Draw(Option_t *option = "colz");
  
    virtual bool Init();

    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer);
    virtual Int_t FindPadID(Double_t i, Double_t j);

    virtual Double_t PadDisplacement() const;
  
    virtual bool IsInBoundary(Double_t i, Double_t j);
  
    virtual TCanvas *GetCanvas(Option_t *option = "");
  
    virtual void DrawFrame(Option_t *option = "");
  
    virtual TH2* GetHist(Option_t *option = "");

  private:
    Int_t FindSection(Double_t i, Double_t j);
  
    Int_t fNumSections = 1;
    Int_t RowNum = 32;
    Int_t ColumnNum = 8;
    Double_t fPadWidth = 2.625; // [mm]
    Double_t fPadHeight = 12; // [mm]
    Double_t fPadGap = 0.5; // [mm]
    Double_t fBasePadPos = 100. - 0.125; // [mm]
  
    std::vector<Int_t> fNRowsInLayer[1];
    std::map<std::vector<Int_t>, Int_t> fPadMap;

    Int_t fNPadsTotal = 0;
  
  
  ClassDef(ATTPCPadPlane, 1)
};

#endif
