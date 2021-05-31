#ifndef KBWPOINTCLUSTER_HH
#define KBWPOINTCLUSTER_HH

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#include "TEvePointSet.h"
#endif

#include "KBWPoint.hh"
#include <vector>
#include <cmath>

/* Cluster of TWPoint.
 * Adding KBWPoint to KBWPointCluster will calculate mean, covariance.
 * MeanError(i,j) returns covariance/weight_sum.
 */

class KBWPointCluster : public KBWPoint
{
  public:
    KBWPointCluster();
    virtual ~KBWPointCluster();

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    void Add(KBWPoint wp);
    void Remove(KBWPoint wp);

    void SetCov(Double_t cov[][3]);
    void SetCov(Int_t i, Int_t j, Double_t cov);

    void SetPosSigma(Double_t sigx, Double_t sigy, Double_t sigz);
    void SetPosSigma(TVector3 sig);
    TVector3 GetPosSigma();

    Double_t StandardDeviation(Int_t i)   { return sqrt(fCov[i][i]); }
    Double_t Covariance(Int_t i, Int_t j) { return fCov[i][j]; }
    Double_t  MeanError(Int_t i, Int_t j) { return fCov[i][j]/fW; }

#ifdef ACTIVATE_EVE
    virtual TEveElement *CreateEveElement();
#endif

  protected:
    Double_t fCov[3][3];

  ClassDef(KBWPointCluster, 1)
};

#endif
