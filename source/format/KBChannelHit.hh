#ifndef KBCHANNELHIT_HH
#define KBCHANNELHIT_HH

#include "TObject.h"

class KBChannelHit : public TObject
{
  public:
    KBChannelHit() { Clear(); }
    KBChannelHit(KBChannelHit *hit);
    KBChannelHit(Int_t id, Double_t t, Double_t a);
    virtual ~KBChannelHit() {}

    virtual void Clear(Option_t *option = "");

    void SetID(Int_t val);
    void SetTDC(Double_t val);
    void SetADC(Double_t val);

    Int_t    GetID()  const;
    Double_t GetTDC() const;
    Double_t GetADC() const;

  private:
    Int_t    fID = -1;
    Double_t fTDC = -999;
    Double_t fADC = -999;

    ClassDef(KBChannelHit, 1)
};

#endif
