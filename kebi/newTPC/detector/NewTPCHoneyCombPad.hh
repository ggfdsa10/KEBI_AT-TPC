#ifndef NEWTPCHONEYCOMBPAD_HH
#define NEWTPCHONEYCOMBPAD_HH

#include "KBPadPlane.hh"
#include "KBTpcHit.hh"
#include "TF1.h"
#include "KBRun.hh"
#include "KBParameterContainer.hh"
#include "TH2Poly.h"

typedef vector<KBTpcHit*> KBTpcHits;

class NewTPCHoneyCombPad : public KBPadPlane
{
  public:
    NewTPCHoneyCombPad();
    virtual ~NewTPCHoneyCombPad() {}

    virtual void Draw(Option_t *option = "colz");
  
    virtual bool Init();

    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer);
    virtual Int_t FindPadID(Double_t i, Double_t j);

    virtual Double_t PadDisplacement() const;
  
    virtual bool IsInBoundary(Double_t i, Double_t j);
  
    virtual TCanvas *GetCanvas(Option_t *option = "");
  
    virtual void DrawFrame(Option_t *option = "");
  
    virtual TH2* GetHist(Option_t *option = "");
    
    virtual double GetPadWidth()  const { return fPadWidth;}
    virtual double GetPadHeight()  const { return fPadHeight;}
    virtual double GetPadGap()  const { return fPadGap;}

    virtual Int_t PadIndexCheck(Double_t i, Double_t j);
    virtual Int_t LayerIndexCheck(Int_t padIndex);
    virtual Int_t RowIndexCheck(Int_t padIndex, Int_t LayerIndex);

  private:
    void PadBoundaryConstruct(TH2Poly *pad);
    Int_t FindSection(Double_t i, Double_t j);

  
    TH2Poly *fPadBoundary = new TH2Poly();
    Double_t fTpcR0;
    Double_t fTpcR1;
    Double_t fTpcLength;
    Int_t fNumSections = 1;
    Int_t fLayerNum = 65;
    Int_t fRowNum[65] ={0,};
    Double_t fPadWidth = 6.; // [mm]
    Double_t fPadHeight = 6.; // [mm]
    Double_t fPadGap = 0.5; // [mm]
  
    std::vector<Int_t> fNRowsInLayer[1];
    std::map<std::vector<Int_t>, Int_t> fPadMap;

    Int_t fNPadsTotal = 0;

  
  ClassDef(NewTPCHoneyCombPad, 1)
};

#endif
