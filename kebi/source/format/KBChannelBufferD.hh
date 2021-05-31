#ifndef KBCHANNELBUFFERD_HH
#define KBCHANNELBUFFERD_HH

#include "KBChannelBufferX.hh"
#include "TArrayD.h"
#include "TH1D.h"

class KBChannelBufferD : public KBChannelBufferX, public TArrayD
{
  public:
    KBChannelBufferD() { Clear(); }
    virtual ~KBChannelBufferD() {}

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    TH1 *GetHist(TString name = "");

  ClassDef(KBChannelBufferD, 1)
};

#endif
