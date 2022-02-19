#ifndef KBHIT_HH
#define KBHIT_HH

#include "KBWPoint.hh"
#include "KBContainer.hh"
#include "KBHitArray.hh"

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

#include "TVector3.h"
#include "TMath.h"
#include "TF1.h"

#include <vector>
using namespace std;

typedef KBVector3::Axis kbaxis;

class KBTracklet;

class KBHit : public KBWPoint
{
  protected:
    Int_t fHitID = -1;
    Int_t fTrackID = -1;
    Double_t fAlpha = -999; // TODO the polar angle
    Double_t fDX = -999;
    Double_t fDY = -999;
    Double_t fDZ = -999;
    Double_t fSortValue = 0; //! sort earlier if smaller, latter if larger

    KBHitArray fHitArray; //!

    vector<Int_t> fTrackCandArray;  //!

  public :
    KBHit() { Clear(); }
    KBHit(Double_t x, Double_t y, Double_t z, Double_t q) { Clear(); Set(x,y,z,q); }
    virtual ~KBHit() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "at") const;
    virtual void Copy (TObject &object) const;
    void CopyFrom(KBHit *hit);

    void SetSortValue(Double_t val);
    void SetSortByX(bool sortEarlierIfSmaller);
    void SetSortByY(bool sortEarlierIfSmaller);
    void SetSortByR(bool sortEarlierIfSmaller);
    void SetSortByZ(bool sortEarlierIfSmaller);
    void SetSortByCharge(bool sortEarlierIfSmaller);
    void SetSortByDistanceTo(TVector3 point, bool sortEarlierIfCloser);
    virtual Bool_t IsSortable() const;
    virtual Int_t Compare(const TObject *obj) const;

    virtual void PropagateMC();

    void SetHitID(Int_t id);
    void SetTrackID(Int_t id);
    void SetAlpha(Double_t a);
    void SetDPosition(TVector3 dpos);
    void SetDX(Double_t dx);
    void SetDY(Double_t dy);
    void SetDZ(Double_t dz);
    void SetX(Double_t x);
    void SetY(Double_t y);
    void SetZ(Double_t z);
    void SetCharge(Double_t charge);

    virtual void AddHit(KBHit *hit);
    virtual void RemoveHit(KBHit *hit);

    Double_t GetSortValue() const;

    KBHitArray *GetHitArray() { return &fHitArray; }

       Int_t GetNumHits() const;

       Int_t GetHitID()   const;
       Int_t GetTrackID() const;
    Double_t GetAlpha()   const;
    TVector3 GetDPosition() const;
    Double_t GetDX()      const;
    Double_t GetDY()      const;
    Double_t GetDZ()      const;
    Double_t GetX()       const;
    Double_t GetY()       const;
    Double_t GetZ()       const;
    Double_t GetCharge()  const;

    TVector3 GetMean()          const;
    TVector3 GetVariance()      const;
    TVector3 GetCovariance()    const;
    TVector3 GetStdDev()        const;
    TVector3 GetSquaredMean()   const;
    TVector3 GetCoSquaredMean() const;

    KBVector3 GetMean(kbaxis)          const;
    KBVector3 GetVariance(kbaxis)      const;
    KBVector3 GetCovariance(kbaxis)    const;
    KBVector3 GetStdDev(kbaxis)        const;
    KBVector3 GetSquaredMean(kbaxis)   const;
    KBVector3 GetCoSquaredMean(kbaxis) const;

    vector<Int_t> *GetTrackCandArray();
    Int_t GetNumTrackCands();
    void AddTrackCand(Int_t id);
    void RemoveTrackCand(Int_t trackID);

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *, Double_t scale=1);
    virtual void AddToEveSet(TEveElement *eveSet, Double_t scale=1);
#endif

  ClassDef(KBHit, 3)
};

class KBHitSortDirection {
  public:
    KBHitSortDirection(TVector3 p):fP(p) {}
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Dot(fP) > h2 -> GetPosition().Dot(fP); }
  private:
    TVector3 fP;
};

class KBHitSortThetaFromP {
  public:
    KBHitSortThetaFromP(TVector3 p):fP(p) {}
    bool operator() (KBHit* h1, KBHit* h2) {
      fP1 = h1 -> GetPosition() - fP;
      fP2 = h2 -> GetPosition() - fP;
      return TMath::ATan2(fP1.Z(),fP1.X()) > TMath::ATan2(fP2.Z(),fP2.X());
    }
  private:
    TVector3 fP, fP1, fP2;
};

class KBHitSortTheta {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { 
      Double_t t1 = TMath::ATan2(h1 -> GetPosition().Z(), h1 -> GetPosition().X());
      Double_t t2 = TMath::ATan2(h2 -> GetPosition().Z(), h2 -> GetPosition().X());
      return t1 > t2;
    }
};

class KBHitSortThetaInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      Double_t t1 = TMath::ATan2(h1 -> GetPosition().Z(), h1 -> GetPosition().X());
      Double_t t2 = TMath::ATan2(h2 -> GetPosition().Z(), h2 -> GetPosition().X());
      return t1 < t2;
    }
};

class KBHitSortR {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Mag() > h2 -> GetPosition().Mag(); }
};

class KBHitSortRInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Mag() < h2 -> GetPosition().Mag(); }
};

class KBHitSortRInvFromP {
  public:
    KBHitSortRInvFromP(TVector3 p):fP(p) {}
    bool operator() (KBHit* h1, KBHit* h2){
      fP1 = h1 -> GetPosition() - fP;
      fP2 = h2 -> GetPosition() - fP;
      if (fP1.Mag() == fP2.Mag()) {
        if (fP1.X() == fP2.X()) {
          if (fP1.Y() == fP2.Y()) {
            return fP1.Z() > fP2.Z();
          } return fP1.Y() > fP2.Z();
        } return fP1.X() > fP2.X();
      } return fP1.Mag() < fP2.Mag(); 
    }

  private:
    TVector3 fP;
    TVector3 fP1;
    TVector3 fP2;
};

class KBHitSortZ {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Z() > h2 -> GetPosition().Z(); }
};

class KBHitSortZInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Z() < h2 -> GetPosition().Z(); }
};

class KBHitSortX {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().X() > h2 -> GetPosition().X(); }
};

class KBHitSortXInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().X() < h2 -> GetPosition().X(); }
};

class KBHitSortY {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Y() > h2 -> GetPosition().Y(); }
};

class KBHitSortYInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Y() < h2 -> GetPosition().Y(); }
};

class KBHitSortCharge {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      if (h1 -> GetCharge() == h2 -> GetCharge())
        return h1 -> GetY() > h2 -> GetY();
      return h1 -> GetCharge() > h2 -> GetCharge();
    }
};

class KBHitSortChargeInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetCharge() < h2 -> GetCharge(); }
};

class KBHitSortXYZInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      if (h1 -> GetPosition().Z() == h2 -> GetPosition().Z()) {
        if (h1 -> GetPosition().X() == h2 -> GetPosition().X())
          return h1 -> GetPosition().Y() < h2 -> GetPosition().Y(); 
        else 
          return h1 -> GetPosition().X() < h2 -> GetPosition().X(); 
      }
      return h1 -> GetPosition().Z() < h2 -> GetPosition().Z(); 
    }
};

class KBHitSortRho {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      Double_t rho1 = (h1 -> GetPosition().X() * h1 -> GetPosition().X() + h1 -> GetPosition().Z() * h1 -> GetPosition().Z());
      Double_t rho2 = (h2 -> GetPosition().X() * h2 -> GetPosition().X() + h2 -> GetPosition().Z() * h2 -> GetPosition().Z());
      return rho1 > rho2;
    }
};

class KBHitSortRhoInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      Double_t rho1 = (h1 -> GetPosition().X() * h1 -> GetPosition().X() + h1 -> GetPosition().Z() * h1 -> GetPosition().Z());
      Double_t rho2 = (h2 -> GetPosition().X() * h2 -> GetPosition().X() + h2 -> GetPosition().Z() * h2 -> GetPosition().Z());
      return rho1 < rho2;
    }
};

class KBHitSortByDistanceTo {
  public:
    KBHitSortByDistanceTo(TVector3 p):fP(p) {}
    bool operator() (KBHit* a, KBHit* b) { return (a->GetPosition()-fP).Mag() < (b->GetPosition()-fP).Mag(); }
  private:
    TVector3 fP;
};

#endif
