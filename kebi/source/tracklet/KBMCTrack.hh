#ifndef KBMCTRACK_HH
#define KBMCTRACK_HH

#include "TObject.h"
#include "TVector3.h"
#include "KBMCStep.hh"
#include "KBTracklet.hh"
#include <vector>
using namespace std;

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

/**
 * Eve naming concept : mc_pnm:tid(pid)[mom]{dnm;nvx}
 * - pnm: particle name or PDG encoding number
 * - tid: track id
 * - pid: parent id
 * - mom: momentum value in MeV/c
 * - dnm: detector name or detector id
 * - nvx: number of vertex
 */

class KBMCTrack : public KBTracklet
{
  public:
    KBMCTrack();
    virtual ~KBMCTrack() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option="") const;

    void SetPX(Double_t val);
    void SetPY(Double_t val);
    void SetPZ(Double_t val);
    void SetVX(Double_t val);
    void SetVY(Double_t val);
    void SetVZ(Double_t val);
    void SetKE(Double_t val);
    void SetDetectorID(Int_t id);
    void SetCreatorProcessID(Int_t id);

  void SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Int_t detectorID = 0, Double_t vx = 0, Double_t vy = 0, Double_t vz = 0, Double_t ke = 0, Int_t processID = -1);
    void AddVertex(Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz);

    Int_t GetNumVertices() const;

    Double_t GetPX(Int_t idx = 0) const;
    Double_t GetPY(Int_t idx = 0) const;
    Double_t GetPZ(Int_t idx = 0) const;
    TVector3 GetMomentum(Int_t idx = 0) const;

    Double_t GetVX(Int_t idx = 0) const;
    Double_t GetVY(Int_t idx = 0) const;
    Double_t GetVZ(Int_t idx = 0) const;
    TVector3 GetVertex(Int_t idx = 0) const;

    Double_t GetKE(Int_t idx = 0) const;

    Int_t GetDetectorID(Int_t idx = 0) const;

    Int_t GetCreatorProcessID() const;

    TVector3 GetPrimaryPosition() const;
       Int_t GetPrimaryDetectorID() const;

    void AddStep(KBMCStep *hit);
    vector<KBMCStep *> *GetStepArray();

    virtual TVector3 Momentum(Double_t B = 0.5) const;
    virtual TVector3 PositionAtHead() const;
    virtual TVector3 PositionAtTail() const;
    virtual Double_t TrackLength() const;

    virtual TVector3 ExtrapolateTo(TVector3 point) const;
    virtual TVector3 ExtrapolateHead(Double_t l) const;
    virtual TVector3 ExtrapolateTail(Double_t l) const;
    virtual TVector3 ExtrapolateByRatio(Double_t r) const;
    virtual TVector3 ExtrapolateByLength(Double_t l) const;

    virtual Double_t LengthAt(TVector3 point) const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *, Double_t scale=1);
    virtual void AddToEveSet(TEveElement *, Double_t scale=1);
#endif

    virtual TGraph *TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, Double_t scale=1);
    virtual TGraph *TrajectoryOnPlane(kbaxis_t axis1, kbaxis_t axis2, bool (*fisout)(TVector3 pos), Double_t scale=1);

  protected:
    vector<Double_t> fPX;
    vector<Double_t> fPY;
    vector<Double_t> fPZ;
    vector<Double_t> fVX;
    vector<Double_t> fVY;
    vector<Double_t> fVZ;
    vector<Double_t> fKE; // Kinetic energy
    vector<Int_t> fDetectorID; ///< detector ID (= copyNo)

    vector<KBMCStep *> fStepArray; //!

    Int_t fCreatorProcessID = 0;
  
  ClassDef(KBMCTrack, 4)
};

#endif
