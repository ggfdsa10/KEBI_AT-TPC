#include "KBDetectorPlane.hh"

#include <iostream>
using namespace std;

ClassImp(KBDetectorPlane)

KBDetectorPlane::KBDetectorPlane()
:KBDetectorPlane("KBDetectorPlane","default detector-plane class")
{
}

KBDetectorPlane::KBDetectorPlane(const char *name, const char *title)
:TNamed(name, title), fChannelArray(new TObjArray())
{
}

void KBDetectorPlane::Clear(Option_t *)
{
  KBChannel *channel;
  TIter iterChannels(fChannelArray);
  while ((channel = (KBChannel *) iterChannels.Next()))
    channel -> Clear();
}

void KBDetectorPlane::Print(Option_t *option) const
{
  kb_info << fName << " plane-" << fPlaneID << " containing " << fChannelArray -> GetEntries() << " channels" << endl;
}

void KBDetectorPlane::AddChannel(KBChannel *channel) { fChannelArray -> Add(channel); }

KBChannel *KBDetectorPlane::GetChannelFast(Int_t idx) { return (KBChannel *) fChannelArray -> At(idx); }

KBChannel *KBDetectorPlane::GetChannel(Int_t idx)
{
  TObject *obj = nullptr;
  if (idx != -1 && idx < fChannelArray -> GetEntriesFast())
    obj = fChannelArray -> At(idx); 

  return (KBChannel *) obj;
}

void KBDetectorPlane::SetPlaneID(Int_t id) { fPlaneID = id; }
Int_t KBDetectorPlane::GetPlaneID() const { return fPlaneID; }

void KBDetectorPlane::SetPlaneK(Double_t k) { fPlaneK = k; }
Double_t KBDetectorPlane::GetPlaneK() { return fPlaneK; }

Int_t KBDetectorPlane::GetNChannels() { return fChannelArray -> GetEntriesFast(); }

TObjArray *KBDetectorPlane::GetChannelArray() { return fChannelArray; }

TCanvas *KBDetectorPlane::GetCanvas(Option_t *)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,800,800);
  return fCanvas;
}

void KBDetectorPlane::DrawFrame(Option_t *)
{
}

void KBDetectorPlane::SetAxis(KBVector3::Axis axis1, KBVector3::Axis axis2)
{
  fAxis1 = axis1;
  fAxis2 = axis2;
}

KBVector3::Axis KBDetectorPlane::GetAxis1() { return fAxis1; }
KBVector3::Axis KBDetectorPlane::GetAxis2() { return fAxis2; }
