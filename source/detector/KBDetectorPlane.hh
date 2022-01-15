#ifndef KBDETECTORPLANE_HH
#define KBDETECTORPLANE_HH

#include "KBChannel.hh"
#include "KBGear.hh"
#include "KBVector3.hh"

#include "TH2.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TNamed.h"
#include "TObjArray.h"
#include "TClonesArray.h"

class KBDetectorPlane : public TNamed, public KBGear
{
  public:
    KBDetectorPlane();
    KBDetectorPlane(const char *name, const char *title);
    virtual ~KBDetectorPlane() {};

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual bool Init() = 0;

    virtual bool IsInBoundary(Double_t i, Double_t j) = 0;

    virtual Int_t FindChannelID(Double_t i, Double_t j) = 0;

    virtual TCanvas *GetCanvas(Option_t *option = "");
    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "-1") = 0;

  public:
    void SetPlaneID(Int_t id);
    Int_t GetPlaneID() const;

    void SetPlaneK(Double_t k);
    Double_t GetPlaneK();

    void AddChannel(KBChannel *channel);

    KBChannel *GetChannelFast(Int_t idx);
    KBChannel *GetChannel(Int_t idx);

    Int_t GetNChannels();
    TObjArray *GetChannelArray();

    void SetAxis(KBVector3::Axis axis1, KBVector3::Axis axis2);
    KBVector3::Axis GetAxis1();
    KBVector3::Axis GetAxis2();

  protected:
    TObjArray *fChannelArray = nullptr;

    Int_t fPlaneID = -1;
    Double_t fPlaneK = -999;

    TCanvas *fCanvas = nullptr;
    TH2 *fH2Plane = nullptr;

    KBVector3::Axis fAxis1 = KBVector3::kX;
    KBVector3::Axis fAxis2 = KBVector3::kY;


  ClassDef(KBDetectorPlane, 1)
};

#endif
