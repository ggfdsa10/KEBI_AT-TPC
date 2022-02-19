#ifndef KBPADPLANE
#define KBPADPLANE

#include "KBPad.hh"
#include "KBDetectorPlane.hh"
#include "KBHitArray.hh"

#include "TVector2.h"
#include "TH2.h"

/*!

  Any PadPlane class should inherit this class and implement following methods:

  - bool Init()
  : Initial parameters and setting should be done in this method.
  This method is called when KBRun::Init() is called.
  If the class is used separately, the method should be called manually.

  - bool IsInBoundary(Double_t i, Double_t j)
  : Check if the position (i,j) is inside the pad-plane boundary and return true if so, and return false else.

  - TH2* GetHist(Option_t *option)
  : This method is not essential but returned histogram is used in EVE.

  - Int_t FindPadID(Double_t i, Double_t j)
  : Find and return padID from the given position (i,j)

  - Int_t FindPadID(Int_t section, Int_t row, Int_t layer)
  : Find and return padID from the given (section, row, layer)

  - Double_t PadDisplacement() const
  : Should return approximate displacements between pads. Doesn't have to be exact.
  One example of use of this method is calculation of the track continuity in track finding.

 */

class KBPadPlane : public KBDetectorPlane
{
  public:
    KBPadPlane();
    KBPadPlane(const char *name, const char *title);
    virtual ~KBPadPlane() {};

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    virtual Int_t FindPadID(Double_t i, Double_t j) = 0;
    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer) = 0;

    virtual Int_t FindChannelID(Double_t i, Double_t j);

    virtual Double_t PadDisplacement() const = 0; ///< Rough (maximum) value of displacements between pads

  public:
    KBPad *GetPadFast(Int_t padID);
    KBPad *GetPad(Int_t padID);

    KBPad *GetPad(Double_t i, Double_t j)                { return GetPad(FindPadID(i,j)); }
    KBPad *GetPad(Int_t section, Int_t row, Int_t layer) { return GetPad(FindPadID(section, row, layer));  }

    //KBPad *GetPadByPadID(Int_t padID);

    void SetPadArray(TClonesArray *padArray);
    void SetHitArray(TClonesArray *hitArray);
    Int_t GetNumPads();

    void FillBufferIn(Double_t i, Double_t j, Double_t tb, Double_t val, Int_t trackID = -1);
    void FillDataToHist(Option_t *option = "out");

    void SetPlaneK(Double_t k);
    Double_t GetPlaneK();

    virtual void ResetHitMap();
    virtual void ResetEvent();
    void AddHit(KBTpcHit *hit);

    virtual KBTpcHit *PullOutNextFreeHit();
    void PullOutNeighborHits(vector<KBTpcHit*> *hits, vector<KBTpcHit*> *neighborHits);
    void PullOutNeighborHits(TVector2 p, Int_t range, vector<KBTpcHit*> *neighborHits);
    void PullOutNeighborHits(Double_t x, Double_t y, Int_t range, vector<KBTpcHit*> *neighborHits);

    void PullOutNeighborHits(KBHitArray *hits, KBHitArray *neighborHits);
    void PullOutNeighborHits(Double_t x, Double_t y, Int_t range, KBHitArray *neighborHits);

    void GrabNeighborPads(vector<KBPad*> *pads, vector<KBPad*> *neighborPads);
    TObjArray *GetPadArray();

    bool PadPositionChecker(bool checkCorners = true);
    bool PadNeighborChecker();

  protected:
    Int_t fEFieldAxis = -1;
    Double_t fPlaneK = -999;

    Int_t fFreePadIdx = 0;

  ClassDef(KBPadPlane, 1)
};

#endif
