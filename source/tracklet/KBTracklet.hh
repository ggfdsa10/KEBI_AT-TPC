#ifndef KBTRACKLET_HH
#define KBTRACKLET_HH

#include "TGraph.h"

#include "KBMCTagged.hh"

#include "KBHitArray.hh"
#include "KBHit.hh"
#include "KBVector3.hh"
#include "KBDetectorPlane.hh"

#include <functional>

class KBTracklet : public KBMCTagged
{
  protected:
    Int_t fTrackID = -1;
    Int_t fParentID = -1;  ///< Vertex ID
    Int_t fPDG = -1;

    KBHitArray fHitArray; //!

    TGraph *fTrajectoryOnPlane = nullptr; //!

  public:
    KBTracklet() {}
    virtual ~KBTracklet() {}

    virtual void Clear(Option_t *option = "");

    virtual void PropagateMC();

    void SetTrackID(Int_t val) { fTrackID = val; }
    Int_t GetTrackID() const { return fTrackID; }

    void SetParentID(Int_t val) { fParentID = val; }
    Int_t GetParentID() const { return fParentID; }

    void SetPDG(Int_t val) { fPDG = val; }
    Int_t GetPDG() const { return fPDG; }

    KBHitArray *GetHitArray() { return &fHitArray; }

    virtual void AddHit(KBHit *hit);
    virtual void RemoveHit(KBHit *hit);

    virtual bool Fit() { return true; }

    virtual TVector3 Momentum(Double_t B = 0.5) const = 0; ///< Momentum of track at head.
    virtual TVector3 PositionAtHead() const = 0; ///< Position at head of helix
    virtual TVector3 PositionAtTail() const = 0; ///< Position at tail of helix
    virtual Double_t TrackLength() const = 0; ///< Length of track calculated from head to tail.

    virtual TVector3 ExtrapolateTo(TVector3 point) const = 0; ///< Extrapolate to POCA from point, returns extrapolated position
    virtual TVector3 ExtrapolateHead(Double_t l) const = 0; ///< Extrapolate head of track about length, returns extrapolated position
    virtual TVector3 ExtrapolateTail(Double_t l) const = 0; ///< Extrapolate tail of track about length, returns extrapolated position
    virtual TVector3 ExtrapolateByRatio(Double_t r) const = 0; ///< Extrapolate by ratio (tail:0, head:1), returns extrapolated position
    virtual TVector3 ExtrapolateByLength(Double_t l) const = 0; ///< Extrapolate by length (tail:0), returns extrapolated position

    virtual Double_t LengthAt(TVector3 point) const = 0; ///< Length at POCA from point, where tail=0, head=TrackLength


#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *, Double_t scale=1);
    virtual void AddToEveSet(TEveElement *eveSet, Double_t scale=1);
#endif

    virtual bool DoDrawOnDetectorPlane();
    virtual TGraph *TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, bool (*fisout)(TVector3 pos), Double_t scale=1);
    virtual TGraph *TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, Double_t scale=1);
    virtual TGraph *TrajectoryOnPlane(KBDetectorPlane *plane, Double_t scale=1);

    virtual TGraph *CrossSectionOnPlane(kbaxis_t, kbaxis_t, Double_t) { return (TGraph *) nullptr; }

  ClassDef(KBTracklet, 3)
};

#endif
