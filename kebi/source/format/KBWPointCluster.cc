#include "KBWPointCluster.hh"
#include "TString.h"
#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBWPointCluster)

KBWPointCluster::KBWPointCluster() { Clear(); }
KBWPointCluster::~KBWPointCluster() {}

void KBWPointCluster::Print(Option_t *option) const
{
  TString opts = TString(option);

  KBWPoint::Print(option);

  TString title1;
  if (opts.Index("t")>=0) title1 += "Cov-XX,YY,ZZ: ";

  TString title2;
  if (opts.Index("t")>=0) title2 += "Cov-XY,YZ,ZX: ";

  if (opts.Index("s")>=0) {
    kr_info(1) << title1 << fCov[0][0] << ", " << fCov[1][1] << ", " << fCov[2][2] << endl;
    kr_info(1) << title2 << fCov[0][1] << ", " << fCov[1][2] << ", " << fCov[2][0] << endl;
  }
  else //if (opts.Index("a")>=0)
  {
    kr_info(1) << title1 << setw(12) << fCov[0][0] << setw(12) << fCov[1][1] << setw(12) << fCov[2][2] << endl;
    kr_info(1) << title2 << setw(12) << fCov[0][1] << setw(12) << fCov[1][2] << setw(12) << fCov[2][0] << endl;
  }
}

void KBWPointCluster::Clear(Option_t *opt)
{
  KBWPoint::Clear(opt);

  for (auto i = 0; i < 3; ++i)
  for (auto j = 0; j < 3; ++j)
    fCov[i][j] = 0;
}

void KBWPointCluster::Copy(TObject &obj) const
{
  KBWPoint::Copy(obj);
  auto wpc = (KBWPointCluster &) obj;

  for (auto i = 0; i < 3; ++i)
  for (auto j = 0; j < 3; ++j)
    wpc.SetCov(i, j, fCov[i][j]);
}

void KBWPointCluster::Add(KBWPoint wp)
{
  auto wi = wp.w();
  auto ww = fW + wi;

  for (int i = 0; i < 3; ++i)
    operator[](i) = (fW*operator[](i) + wi*wp[i])/ww;

  if (fW == 0) {
    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      fCov[i][j] = 0;
  }

  else {
    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      fCov[i][j] = fW*fCov[i][j]/ww + wi*(operator[](i)-wp[i])*(operator[](j)-wp[j])/fW;
  }

  fW = ww;
}

void KBWPointCluster::Remove(KBWPoint wp)
{
  auto wi = -wp.w();
  auto ww = fW + wi;

  for (int i = 0; i < 3; ++i)
    operator[](i) = (fW*operator[](i) + wi*wp[i])/ww;

  if (fW == 0) {
    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      fCov[i][j] = 0;
  }

  else {
    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      fCov[i][j] = fW*fCov[i][j]/ww + wi*(operator[](i)-wp[i])*(operator[](j)-wp[j])/fW;
  }

  fW = ww;
}

void KBWPointCluster::SetCov(Double_t cov[][3])
{
  for (int i = 0; i < 3; ++i)
  for (int j = 0; j < 3; ++j)
    fCov[i][j] = cov[i][j];
}

void KBWPointCluster::SetCov(Int_t i, Int_t j, Double_t cov)
{
  fCov[i][j] = cov;
}

void KBWPointCluster::SetPosSigma(Double_t sigx, Double_t sigy, Double_t sigz)
{
  fCov[0][0] = sigx * sigx;
  fCov[1][1] = sigy * sigy;
  fCov[2][2] = sigz * sigz;
}

void KBWPointCluster::SetPosSigma(TVector3 sig)
{
  fCov[0][0] = sig.X() * sig.X();
  fCov[1][1] = sig.Y() * sig.Y();
  fCov[2][2] = sig.Z() * sig.Z();
}

TVector3 KBWPointCluster::GetPosSigma()
{
  return TVector3(sqrt(fCov[0][0]), sqrt(fCov[1][1]), sqrt(fCov[2][2]));
}

#ifdef ACTIVATE_EVE
TEveElement *KBWPointCluster::CreateEveElement()
{
  auto pointSet = new TEvePointSet("WPointCluster");
  pointSet -> SetMarkerColor(kViolet);
  pointSet -> SetMarkerSize(1);
  pointSet -> SetMarkerStyle(20);

  return pointSet;
}
#endif
