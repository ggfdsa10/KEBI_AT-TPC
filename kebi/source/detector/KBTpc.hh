#ifndef KBTPC_HH
#define KBTPC_HH

#include "KBDetector.hh"
#include "KBPadPlane.hh"

#include "TVector3.h"
#include "KBVector3.hh"

class KBTpc : public KBDetector
{
  public:
    KBTpc();
    KBTpc(const char *name, const char *title);
    virtual ~KBTpc() {};

    virtual KBPadPlane *GetPadPlane(Int_t idx = 0);

    virtual KBVector3::Axis GetEFieldAxis() = 0;

            TVector3 GetEField(Double_t x, Double_t y, Double_t z);
    virtual TVector3 GetEField(TVector3 pos) = 0;

            KBPadPlane *GetDriftPlane(Double_t x, Double_t y, Double_t z);
    virtual KBPadPlane *GetDriftPlane(TVector3 pos) = 0;

  ClassDef(KBTpc, 1)
};

#endif
