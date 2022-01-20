#include "KBDetectorSystem.hh"
#include "TBrowser.h"

#include <iostream>
using namespace std;

ClassImp(KBDetectorSystem)

KBDetectorSystem::KBDetectorSystem()
:KBDetectorSystem("KBDetectorSystem")
{
}

KBDetectorSystem::KBDetectorSystem(const char *name)
{
  fName = name;
}

void KBDetectorSystem::SetParameterContainer(KBParameterContainer *par) {
  KBGear::SetParameterContainer(par);
  SetDetectorPar();
}

void KBDetectorSystem::SetParameterContainer(TString fname) {
  KBGear::SetParameterContainer(fname);
  SetDetectorPar();
}

void KBDetectorSystem::AddParameterContainer(KBParameterContainer *par) {
  KBGear::AddParameterContainer(par);
  SetDetectorPar();
}

void KBDetectorSystem::AddParameterContainer(TString fname) {
  KBGear::AddParameterContainer(fname);
  SetDetectorPar();
}

void KBDetectorSystem::Print(Option_t *) const
{
  TIter next(this);
  KBDetector *detector;
  while ((detector = (KBDetector *) next()))
    detector -> Print();
}

bool KBDetectorSystem::Init()
{
  TString title("Detector System containing");
  SetDetectorPar();

  TGeoVolume *top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  TIter next(this);
  KBDetector *detector;
  while ((detector = (KBDetector *) next())) {
    SetDetector(detector);
    detector -> Init();
    title = title + ", " + detector -> GetName();
  }
  top = fGeoManager -> GetTopVolume();
  top -> CheckOverlaps();

  if (!fGeoManager -> IsClosed()) {
    fGeoManager -> SetTitle(title);
    fGeoManager -> CloseGeometry();
  }

  kb_info << title << " initialized"<< endl;

  return true;
}

TGeoManager *KBDetectorSystem::GetGeoManager() const { return fGeoManager; }
TGeoVolume *KBDetectorSystem::GetGeoTopVolume() const { return fGeoManager -> GetTopVolume(); }
void KBDetectorSystem::SetGeoManager(TGeoManager *man)
{
  fGeoManager = man;

  TIter next(this);
  KBDetector *detector;
  while ((detector = (KBDetector *) next()))
    detector -> SetGeoManager(man);
}

void KBDetectorSystem::SetTransparency(Int_t transparency)
{
  TObjArray* listVolume = gGeoManager -> GetListOfVolumes();
  Int_t nVolumes = listVolume -> GetEntries();
  for (Int_t iVolume = 0; iVolume < nVolumes; iVolume++)
    ((TGeoVolume*) listVolume -> At(iVolume)) -> SetTransparency(transparency);
}

void KBDetectorSystem::AddDetector(KBDetector *detector)
{
  SetDetector(detector);
  detector -> SetPar(fPar);
  Add(detector);
}

Int_t KBDetectorSystem::GetNumDetectors() const { return GetEntries(); }
KBDetector *KBDetectorSystem::GetDetector(Int_t idx) const { return (KBDetector *) At(idx); }

KBTpc *KBDetectorSystem::GetTpc() const
{
  TIter next(this);
  TObject *detector;
  while ((detector = next()))
    if (detector -> InheritsFrom("KBTpc"))
      return (KBTpc *) detector;

  return (KBTpc *) nullptr;
}

Int_t KBDetectorSystem::GetNumPlanes() const
{
  Int_t numPlanes = 0;

  TIter next(this);
  KBDetector *detector;
  while ((detector = (KBDetector *)next()))
    numPlanes += detector -> GetNumPlanes();

  return numPlanes;
}

KBDetectorPlane *KBDetectorSystem::GetDetectorPlane(Int_t idx) const
{
  Int_t countPlanes = 0;

  TIter next(this);
  KBDetector *detector;
  while ((detector = (KBDetector *)next()))
  {
    Int_t numPlanes0 = countPlanes;
    countPlanes += detector -> GetNumPlanes();
    if (idx>countPlanes-1)
      continue;
    return (KBDetectorPlane *) detector -> GetDetectorPlane(idx-numPlanes0);
  }

  return (KBDetectorPlane *) nullptr;
}

void KBDetectorSystem::SetDetector(KBDetector *detector)
{
  if (fGeoManager == nullptr) {
    kb_info << "Closing Geometry" << endl;
    fGeoManager = new TGeoManager();
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetName(fName);
  }

  detector -> SetGeoManager(fGeoManager);
  detector -> SetRank(fRank+1);
  detector -> SetParent(this);
}

void KBDetectorSystem::SetDetectorPar()
{
  TIter next(this);
  KBDetector *detector;
  while ((detector = (KBDetector *) next()))
    detector -> SetPar(fPar);
}
