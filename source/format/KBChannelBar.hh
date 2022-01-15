#ifndef KBCHANNELBAR_HH
#define KBCHANNELBAR_HH

#include "KBChannelBufferD.hh"
#include "TVector3.h"
#include "KBVector3.hh"

class KBChannelBar : public KBChannel
{
  public:
    KBChannelBar();
    virtual ~KBChannelBar() {}

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    KBChannelBufferD *Channel1() const;
    KBChannelBufferD *Channel2() const;

  private:
    KBChannelBufferD *fChannel1 = nullptr; //->
    KBChannelBufferD *fChannel2 = nullptr; //->

  ClassDef(KBChannelBar, 1)
};

#endif
