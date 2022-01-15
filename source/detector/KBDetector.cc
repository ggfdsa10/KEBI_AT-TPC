#include "KBDetectorSystem.hh"
#include "KBDetector.hh"

#include <iostream>
using namespace std;

ClassImp(KBDetector)

KBDetector::KBDetector()
:KBDetector("KBDetector","default detector class")
{
}

KBDetector::KBDetector(const char *name, const char *title)
:TNamed(name, title), fDetectorPlaneArray(new TObjArray())
{
}

void KBDetector::Print(Option_t *) const
{
  kb_info << fTitle << endl;
  for (auto iPlane = 0; iPlane < fNumPlanes; ++iPlane) {
    auto plane = (KBDetectorPlane *) fDetectorPlaneArray -> At(iPlane);
    plane -> Print();
  }
}

TGeoManager *KBDetector::GetGeoManager() { return fGeoManager; }
void KBDetector::SetGeoManager(TGeoManager *man) { fGeoManager = man; }

void KBDetector::SetTransparency(Int_t transparency)
{
  TObjArray* listVolume = gGeoManager -> GetListOfVolumes();
  Int_t nVolumes = listVolume -> GetEntries();
  for (Int_t iVolume = 0; iVolume < nVolumes; iVolume++)
    ((TGeoVolume*) listVolume -> At(iVolume)) -> SetTransparency(transparency);
}

void KBDetector::AddPlane(KBDetectorPlane *plane)
{
  plane -> SetRank(fRank+1);
  fDetectorPlaneArray -> Add(plane);
  fNumPlanes = fDetectorPlaneArray -> GetEntries();
}

Int_t KBDetector::GetNumPlanes() { return fNumPlanes; }

KBDetectorPlane *KBDetector::GetDetectorPlane(Int_t idx) { return (KBDetectorPlane *) fDetectorPlaneArray -> At(idx); }

KBDetectorSystem *KBDetector::GetParent() { return fParent; }
void KBDetector::SetParent(KBDetectorSystem *system) { fParent = system; }

TGeoVolume *KBDetector::CreateGeoTop(TString name)
{
  if (fParent == nullptr) {
    TGeoVolume *top = new TGeoVolumeAssembly(name);
    fGeoManager -> SetTopVolume(top);
    fGeoManager -> SetTopVisible(true);
    kb_info << "Creating Geometry " << name << endl;
    return top;
  }

  return fParent -> GetGeoTopVolume();
}
void KBDetector::FinishGeometry()
{
  if (fParent == nullptr) {
    if (fGeoManager -> IsClosed())
      kb_info << "Geometry is closed already" << endl;
    else {
      fGeoManager -> CloseGeometry();
      kb_info << "Closing geometry " << endl;
    }
  }
}
