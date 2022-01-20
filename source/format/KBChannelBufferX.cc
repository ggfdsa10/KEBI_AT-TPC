#include "KBChannelBufferX.hh"

ClassImp(KBChannelBufferX)

void KBChannelBufferX::Clear(Option_t *option)
{
  KBChannel::Clear(option);
}

void KBChannelBufferX::Draw(Option_t *option)
{
  auto hist = GetHist();
  hist -> Draw(option);

  //TODO : draw hits
}
