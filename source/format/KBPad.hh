#ifndef KBPAD_HH
#define KBPAD_HH

#include "KBChannel.hh"
#include "KBTpcHit.hh"
#include "KBHitArray.hh"
#include "KBVector3.hh"

#include "TObject.h"
#include "TH1D.h"
#include "TVector2.h"

#include <vector>
using namespace std;

class KBPad : public KBChannel
{
  public:
    KBPad() { Clear(); }
    virtual ~KBPad() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;

    /// option (default is "")
    /// - p : Add Pad[ID] to main title
    /// - a : Add AsAd,AGET,Channel-IDs to main title
    /// - mc: Draw MCID and line at corresponding tb
    /// - o : Draw output buffer
    /// - r : Draw raw buffer
    /// - i : Draw input buffer (not written by default)
    /// - h : Draw hit
    virtual void Draw(Option_t *option = "mcoh");

    void DrawMCID(Option_t *option = "mc");
    void DrawHit(Option_t *option = "h");

    virtual Bool_t IsSortable() const;
    virtual Int_t Compare(const TObject *obj) const;


    void SetPad(KBPad* padRef); // copy from padRef
    void CopyPadData(KBPad* padRef); // copy from padRef

    void SetActive(bool active = true);
    bool IsActive() const;

    void SetPadID(Int_t id);
    Int_t GetPadID() const;

    void SetPlaneID(Int_t id);
    Int_t GetPlaneID() const;

    void SetAsAdID(Int_t id);
    Int_t GetAsAdID() const;

    void SetAGETID(Int_t id);
    Int_t GetAGETID() const;

    void SetChannelID(Int_t id);
    Int_t GetChannelID() const;

    void SetBaseLine(Double_t baseLine);
    Double_t GetBaseLine() const;

    void SetNoiseAmplitude(Double_t gain);
    Double_t GetNoiseAmplitude() const;

    void SetPosition(KBVector3 pos);
    void SetPosition(Double_t i, Double_t j);
    void GetPosition(Double_t &i, Double_t &j) const;
    KBVector3 GetPosition() const;
    Double_t GetI() const;
    Double_t GetJ() const;
    Double_t GetK() const;
    Double_t GetX() const;
    Double_t GetY() const;
    Double_t GetZ() const;

    void AddPadCorner(Double_t i, Double_t j);
    vector<TVector2> *GetPadCorners();

    void SetSectionRowLayer(Int_t section, Int_t row, Int_t layer);
    void GetSectionRowLayer(Int_t &section, Int_t &row, Int_t &layer) const;
    Int_t GetSection() const;
    Int_t GetRow() const;
    Int_t GetLayer() const;

    void FillBufferIn(Int_t idx, Double_t val, Int_t trackID = -1);
    void SetBufferIn(Double_t *buffer);
    Double_t *GetBufferIn();

    void SetBufferRaw(Short_t *buffer);
    Short_t *GetBufferRaw();

    void SetBufferOut(Double_t *buffer);
    Double_t *GetBufferOut();

    void AddNeighborPad(KBPad *pad);
    vector<KBPad *> *GetNeighborPadArray();

    void AddHit(KBTpcHit *hit);
    Int_t GetNumHits() const;
    KBTpcHit *GetHit(Int_t idx);

    void ClearHits();
    KBTpcHit *PullOutNextFreeHit();
    void PullOutHits(vector<KBTpcHit *> *hits);
    void PullOutHits(KBHitArray *hist);

    bool IsGrabed() const;
    void Grab();
    void LetGo();

    /// option (default is "")
    /// * p : Add Pad[ID] to main title
    /// * a : Add AsAd,AGET,Channel-IDs to main title
    /// * mc: Draw MCID and line at corresponding tb
    /// * o : Draw output buffer
    /// * r : Draw raw buffer
    /// * i : Draw input buffer (not written by default)
    /// * h : Draw hit
    void SetHist(TH1D *hist, Option_t *option = "mcoh");
    TH1D *GetHist(Option_t *option = "");

    Int_t             GetNumMCIDs()      { return fMCIDArray.size(); }
    vector<Int_t>    *GetMCIDArray()     { return &fMCIDArray; }
    vector<Double_t> *GetMCWeightArray() { return &fMCWeightArray; }
    vector<Double_t> *GetMCTbArray()     { return &fMCTbArray; }

    void SetSortValue(Double_t value) { fSortValue = value; }
    Double_t GetSortValue() { return fSortValue; }

  private:
    bool fActive = false; //!

    Int_t fPlaneID = 0;
    Int_t fAsAdID = -1;
    Int_t fAGETID = -1;
    Int_t fChannelID = -1;

    Double_t fBaseLine = 0;
    Double_t fNoiseAmp = 0;

    KBVector3 fPosition = KBVector3(KBVector3::kZ);

    vector<TVector2> fPadCorners; //!

    Int_t fSection = -999;
    Int_t fRow = -999;
    Int_t fLayer = -999;

    Double_t fBufferIn[512]; //!
    Short_t  fBufferRaw[512];
    Double_t fBufferOut[512];

    vector<KBPad *> fNeighborPadArray; //!
    vector<KBTpcHit *> fHitArray; //!

    vector<Int_t>    fMCIDArray;      ///< MC Track-ID
    vector<Double_t> fMCWeightArray;  ///< Sum of weight of corresponding Track-ID
    vector<Double_t> fMCTbArray;      ///< Tb of corresponding Track-ID

    bool fGrabed = false; //!

    Double_t fSortValue = -999; //!

  ClassDef(KBPad, 1)
};

#endif
