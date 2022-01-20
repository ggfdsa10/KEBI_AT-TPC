#ifndef KBCHANNELBUFFERS_HH
#define KBCHANNELBUFFERS_HH

#include "KBChannelBufferX.hh"
#include "TArrayS.h"
#include "TH1S.h"

class KBChannelBufferS : public KBChannelBufferX, public TArrayS
{
  public:
    KBChannelBufferS() { Clear(); }
    virtual ~KBChannelBufferS() {}

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    TH1 *GetHist(TString name = "");

  ClassDef(KBChannelBufferS, 1)
};

#endif
