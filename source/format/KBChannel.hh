#ifndef KBCHANNEL_HH
#define KBCHANNEL_HH

#include "KBContainer.hh"
#include "KBChannelHit.hh"
#include "KBGeoBox.hh"

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"

class KBChannel : public KBContainer
{
  public:
    KBChannel() { Clear(); }
    virtual ~KBChannel() {}

    virtual void Clear(Option_t *option = "");
    virtual void Copy(TObject &obj) const;

    void  SetID(Int_t id);
    Int_t GetID() const;

    void AddChannelHit(KBChannelHit *channelHit);
    TObjArray *GetChannelHitArray();

  protected:
    Int_t fID = -1;

    TObjArray fChannelHitArray; //!

  ClassDef(KBChannel, 1)
};

#endif
