#ifndef KBMCRECOMATCHING_HH
#define KBMCRECOMATCHING_HH

#include "KBContainer.hh"
#include "TVector3.h"
#include <vector>
using namespace std;

class KBMCRecoMatching : public KBContainer
{
  public:
    KBMCRecoMatching();
    virtual ~KBMCRecoMatching() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual void Copy (TObject &object) const;

    /**
     * KBMCRecoMatching::Status
     * - kNotFound: only MC is set
     * - kFake: only Reco is set
     * - kFound: both MC and Reco is set
     */

    enum Status { kNotFound, kMatched, kFake };

    void SetIsMatched()         { fStatus = kMatched; }
    void SetIsNotFound()        { fStatus = kNotFound; }
    void SetIsFake()            { fStatus = kFake; }
    void SetStatus(Status stat) { fStatus = stat; }

    bool IsMatched()   const { return fStatus == kMatched  ? true : false; }
    bool IsNotFound()  const { return fStatus == kNotFound ? true : false; }
    bool IsFake()      const { return fStatus == kFake     ? true : false; }
    Status GetStatus() const { return fStatus; }

    /** */

    void SetMCID(Int_t id)           { fMCID = id; }
    void SetMCMomentum(TVector3 p)   { fMCMomentum = p; }
    void SetRecoID(Int_t id)         { fRecoID = id; }
    void SetRecoMomentum(TVector3 p) { fRecoMomentum = p; }

    void Set    (Int_t mcid, TVector3 mcp, Int_t recoid, TVector3 recop);
    void SetMC  (Int_t id, TVector3 p);
    void SetReco(Int_t id, TVector3 p);

       Int_t GetMCID()         const { return fMCID; }
    TVector3 GetMCMomentum()   const { return fMCMomentum; }
       Int_t GetRecoID()       const { return fRecoID; }
    TVector3 GetRecoMomentum() const { return fRecoMomentum; }

    /**
     * If reco candidate exist, one can add candidate by AddRecoCand(id, p)
     */

    void AddRecoCand(Int_t id, TVector3 p);
    void SetRecoCand(Int_t idx, Int_t id, TVector3 p);

    Int_t GetNumRecoCand() const { return fRecoIDCand.size(); }
    Int_t GetRecoIDCand(Int_t idx) const { return fRecoIDCand.at(idx); }
    TVector3 GetRecoMomentumCand(Int_t idx) const { return fRecoMomentumCand.at(idx); }

  private:
    Status fStatus; ///< Matching status

       Int_t fMCID;          ///< position in array of KBMCTrack 
    TVector3 fMCMomentum;    ///< Original momentum

       Int_t fRecoID;        ///< position in array of KBTrack 
    TVector3 fRecoMomentum;  ///< Reconstructed momentum

    vector<Int_t>    fRecoIDCand;        ///< candidate reco ID
    vector<TVector3> fRecoMomentumCand;  ///< candidate reco momentum

  ClassDef(KBMCRecoMatching, 2)
};

#endif
