#ifndef ATTPC20RECTANGLEPAD_HH
#define ATTPC20RECTANGLEPAD_HH

#include "KBPadPlane.hh"
#include "KBTpcHit.hh"
#include "TF1.h"
#include "KBRun.hh"
#include "KBParameterContainer.hh"

typedef vector<KBTpcHit*> KBTpcHits;

class ATTPC20RectanglePad : public KBPadPlane
{
  public:
    ATTPC20RectanglePad();
    virtual ~ATTPC20RectanglePad() {}

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

  private:
    Int_t FindSection(Double_t i, Double_t j);

    std::vector<Int_t> PadNeighborIndex(Int_t layer, Int_t Row, Int_t PadID);
  
    Int_t fNumSections = 1;
    Int_t RowNum = 32;
    Int_t ColumnNum = 32;
    Double_t fPadWidth = 5.5; // [mm]     
    Double_t fPadHeight = 5.5; // [mm]     
    Double_t fPadGap = 0.5; // [mm]         
    Double_t fBasePadPos = 200.; // [mm]
  
    std::vector<Int_t> fNRowsInLayer[1];
    std::map<std::vector<Int_t>, Int_t> fPadMap;

    Int_t fNPadsTotal = 0;

  
  ClassDef(ATTPC20RectanglePad, 1)
};

#endif
