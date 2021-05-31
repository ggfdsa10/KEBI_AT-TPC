#include "KBTpc.hh"

#include <iostream>
using namespace std;

ClassImp(KBTpc)

KBTpc::KBTpc()
:KBTpc("KBTpc","TPC")
{
}

KBTpc::KBTpc(const char *name, const char *title)
:KBDetector(name, title)
{
}

KBPadPlane *KBTpc::GetPadPlane(Int_t idx) { return (KBPadPlane *) GetDetectorPlane(idx); }

TVector3 KBTpc::GetEField(Double_t x, Double_t y, Double_t z) { return GetEField(TVector3(x,y,z)); }
KBPadPlane *KBTpc::GetDriftPlane(Double_t x, Double_t y, Double_t z) { return GetDriftPlane(TVector3(x,y,z)); }
