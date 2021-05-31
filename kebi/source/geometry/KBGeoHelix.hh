#ifndef KBGEOHELIX_HH
#define KBGEOHELIX_HH

#include "TVector3.h"
#include "TGraph2D.h"
#include "TGraph.h"

#include "KBVector3.hh"
#include "KBGeometry.hh"

typedef KBVector3::Axis kbaxis_t;

class KBGeoHelix : public KBGeometry
{
  protected:
    Double_t fRMSR = -1; // radial RMS
    Double_t fRMST = -1; // axial RMS

    Double_t fI = 0; ///< axis i-position
    Double_t fJ = 0; ///< axis j-position
    Double_t fR = 0; ///< Helix radius
    Double_t fS = 0; ///< k = fS * alpha + fK
    Double_t fK = 0; ///< k = fS * alpha + fK
    Double_t fT = 0; ///< Alpha angle at tail
    Double_t fH = 0; ///< Alpha angle at head

    kbaxis_t fA = KBVector3::kZ; ///< helix axis

  public:
    KBGeoHelix();
    KBGeoHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
               Double_t t, Double_t h, kbaxis_t a);

    virtual ~KBGeoHelix() {}

    virtual void Print(Option_t *option = "") const;
    virtual TVector3 GetCenter() const;

    void SetHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
                  Double_t t, Double_t h, kbaxis_t a);

    TVector3 GetRandomPoint(Double_t sigma = 0);

    void SetRMSR(Double_t);
    void SetRMST(Double_t);

    Double_t GetRMSR() const;
    Double_t GetRMST() const;

    void SetI(Double_t val);
    void SetJ(Double_t val);
    void SetR(Double_t val);
    void SetS(Double_t val);
    void SetK(Double_t val);
    void SetT(Double_t val);
    void SetH(Double_t val);
    void SetA(kbaxis_t val);

    Double_t GetI() const;
    Double_t GetJ() const;
    Double_t GetR() const;
    Double_t GetS() const;
    Double_t GetK() const;
    Double_t GetT() const;
    Double_t GetH() const;
    kbaxis_t GetA() const;

       Int_t Helicity() const; ///< Helicity of track +/- @todo TODO
    Double_t DipAngle() const; ///< =artan(-fS/fR) (=0 on xz plane) : Angle between p and pt
    Double_t CosDip()   const; ///< cos(dip)
    Double_t AngleFromCenterAxis() const;

    Double_t LengthInPeriod() const;                ///< Length of track in one period
    Double_t KLengthInPeriod() const;               ///< y-length of track in one period

    Double_t TravelLengthAtAlpha(Double_t alpha) const;  ///< Length of track in change of alpha
    Double_t AlphaAtTravelLength(Double_t tlen)  const;  ///< Angle alpha in change of length

    TVector3 PositionAtAlpha(Double_t alpha) const; ///< Position at angle alpha [mm]
    TVector3 Direction(Double_t alpha) const;       ///< Direction at angle alpha

    TVector3 HelicoidMap(TVector3 pos, Double_t alpha) const; ///< Map pos to helicoid map coordinate

    /*
    //TVector3 ClosestPointToHelix(TVector3);
    //Double_t DistanceToHelix(TVector3);

    KBGeoCircle CircleProjection();
    //KBGeoLine   HelicoidMapLine();

    TGraph2D *DrawHelix();
    TGraph   *Draw2D(kbaxis, kbaxis);
    TGraph   *DrawIJ();
    TGraph   *DrawKI();
    TGraph   *DrawKJ();
    */

  ClassDef(KBGeoHelix, 1)
};

#endif
