#ifndef KBSCINTBARARRAY_HH
#define KBSCINTBARARRAY_HH

#include "KBDetectorPlane.hh"
#include "KBVector3.hh"
//#include "KBChannel.hh"

#include "TH2.h"
#include "TCanvas.h"
#include "TObjArray.h"

class KBScintWall : public KBDetectorPlane
{
  public:
    KBScintWall();
    KBScintWall(const char *name, const char *title);
    virtual ~KBScintWall() {};

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual bool Init();

    virtual bool IsInBoundary(Double_t i, Double_t j);

    virtual Int_t FindChannelID(Double_t i, Double_t j);

    virtual TCanvas *GetCanvas(Option_t *option = "");
    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "-1");

  private:
    void ChannelID();

  private:
    /// type of scintillator wall
    /// - barArray
    TString fType;

    KBVector3::Axis fBarAxis;   ///< for type:barArray, long-side axis of the scintillator bar.
    KBVector3::Axis fStackAxis; ///< for type:barArray, axis of bar being stacked.
    Int_t fNumStacks;  ///< number of stacks
    /// stack displacement vector
    /// this vector is added when stacking scintillator from reference
    KBVector3 fStackDisplacement;
    KBVector3 fScintPos0; ///< reference scintillator position x
    KBVector3 fScintDXYZ; ///< scintillator dimension length

    Double_t fMaxX = -DBL_MAX;
    Double_t fMinX =  DBL_MAX;
    Double_t fMaxY = -DBL_MAX;
    Double_t fMinY =  DBL_MAX;

    /*
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
    */


  ClassDef(KBScintWall, 1)
};

#endif
