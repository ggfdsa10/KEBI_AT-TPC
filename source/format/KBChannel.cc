#include "KBChannel.hh"

ClassImp(KBChannel)

void KBChannel::Clear(Option_t *option)
{
  KBContainer::Clear(option);

  TString opt(option); opt.ToLower();
  if (opt.Index("ch")>=0) fID = -1;
}

void KBChannel::Copy(TObject &obj) const
{
  KBContainer::Copy(obj);
  auto channel = (KBChannel &) obj;

  channel.SetID(fID);
}

void KBChannel::SetID(Int_t id) { fID = id; }
Int_t KBChannel::GetID() const { return fID; }

void KBChannel::AddChannelHit(KBChannelHit *channelHit) { fChannelHitArray.Add(channelHit); }
TObjArray *KBChannel::GetChannelHitArray() { return &fChannelHitArray; }
