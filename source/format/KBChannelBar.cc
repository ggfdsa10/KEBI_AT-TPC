#include "KBChannelBar.hh"

ClassImp(KBChannelBar)

KBChannelBar::KBChannelBar()
{
  Clear();

  fChannel1 = new KBChannelBufferD();
  fChannel2 = new KBChannelBufferD();
}

void KBChannelBar::Clear(Option_t *option)
{
  KBChannel::Clear(option);

  TString opt(option); opt.ToLower();

  fChannel1 -> Clear(option);
  fChannel2 -> Clear(option);
}

void KBChannelBar::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto channel = (KBChannelBar &) obj;

  channel.SetID(fID);

  fChannel1 -> Copy(*(channel.Channel1()));
  fChannel2 -> Copy(*(channel.Channel2()));
}

KBChannelBufferD *KBChannelBar::Channel1() const { return fChannel1; }
KBChannelBufferD *KBChannelBar::Channel2() const { return fChannel2; }
