#ifndef KBCHANNELBUFFERX_HH
#define KBCHANNELBUFFERX_HH

#include "KBChannel.hh"
#include "TH1.h"

class KBChannelBufferX : public KBChannel
{
  public:
    KBChannelBufferX() {}
    virtual ~KBChannelBufferX() {}

    virtual void Clear(Option_t *option = "");

    virtual void Draw(Option_t *option = "");

    virtual TH1 *GetHist(TString name = "") = 0;

  ClassDef(KBChannelBufferX, 1)
};

#endif
