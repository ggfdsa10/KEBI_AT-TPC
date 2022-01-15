#ifndef DUMMYDETECTORPLANE_HH
#define DUMMYDETECTORPLANE_HH

//#include "KBPadPlane.hh"
#include "KBDetectorPlane.hh"

//class DUMMYDetectorPlane : public KBPadPlane
class DUMMYDetectorPlane : public KBDetectorPlane
{
  public:
    DUMMYDetectorPlane();
    virtual ~DUMMYDetectorPlane() {}

    virtual bool Init();
    virtual bool IsInBoundary(Double_t i, Double_t j);
    virtual Int_t FindChannelID(Double_t i, Double_t j);
    virtual TCanvas *GetCanvas(Option_t *option = "");
    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "");

  ClassDef(DUMMYDetectorPlane, 1)
};

#endif
