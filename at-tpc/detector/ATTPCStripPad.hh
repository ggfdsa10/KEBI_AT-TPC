#ifndef ATTPCSTRIPPAD_HH
#define ATTPCSTRIPPAD_HH

#include "KBPadPlane.hh"
#include "KBTpcHit.hh"
#include "TF1.h"
#include "KBRun.hh"
#include "KBParameterContainer.hh"
#include "TH2Poly.h"
#include <tuple>

typedef vector<KBTpcHit*> KBTpcHits;

class ATTPCStripPad : public KBPadPlane
{
  public:
    ATTPCStripPad();
    virtual ~ATTPCStripPad() {}

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
    virtual std::vector<std::vector<std::tuple<Int_t, Double_t, Double_t>>> GetPadByChan(int ch){return fPadChArray[ch];}

  private:
    void PadBoundaryConstruct(TH2Poly *pad);
    std::vector<Int_t> PadNeighborIndex(KBVector3 posPad);
    Int_t PadIndexCheck(Double_t i, Double_t j);
    Int_t FindChannal(Int_t padIndex);
    Int_t LayerIndexCheck(Int_t padIndex, Int_t Channal);
    Int_t RowIndexCheck(Int_t padIndex, Int_t Channal, Int_t LayerIndex);

    TH2Poly *fPadBoundary = nullptr;
    Int_t fNumSections = 3;
    Int_t fStripChannals = 3;
    Double_t fBasePadPos = 200.; // [mm]
    Double_t fStripNumByCh = 85; // strip number
    Double_t fPadActiveSize = 190.;  // [mm]
    Double_t fPadBTWLength = 0.; // [mm]
    Double_t fPadWidth = 0.; // [mm]
    Double_t fPadHeight = 0.; // [mm]
    Double_t fPadGap = 0.1; // [mm]
  
    std::vector<std::vector<std::tuple<Int_t, Double_t, Double_t>>> fPadChArray[3]; //[section][layers][Rows][posX, posY]
    std::map<std::vector<Int_t>, Int_t> fPadMap;

    Int_t fNPadsTotal = 0;

  
  ClassDef(ATTPCStripPad, 1)
};

#endif
