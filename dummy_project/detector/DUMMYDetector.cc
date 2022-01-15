#include "DUMMYDetector.hh"
#include "DUMMYDetectorPlane.hh"

DUMMYDetector::DUMMYDetector()
:KBDetector("DUMMYDetector","DUMMY Detector")
{
}

bool DUMMYDetector::Init()
{
  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

bool DUMMYDetector::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("DUMMY Detector", "DUMMY Detector Geometry");
  }

  Double_t detector_size = 20;
  Double_t detector_offset_z = 30;

  auto top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  auto detector = new TGeoVolumeAssembly("Detector");
  auto offset = new TGeoTranslation("offset",0,0,detector_offset_z);

  auto water = new TGeoMedium("water", 1, new TGeoMaterial("water"));
  auto detector_volume = fGeoManager -> MakeBox("detector",water,detector_size/2,detector_size/2,detector_size/2);
  detector_volume -> SetVisibility(true);
  detector_volume -> SetLineColor(kBlue-10);
  detector_volume -> SetTransparency(90);
  auto rotation = new TGeoRotation("rotation",0,90,0); 

  top -> AddNode(detector, 1, offset);
  detector -> AddNode(detector_volume, 1, rotation);

  fGeoManager -> CloseGeometry();

  return true;
}

bool DUMMYDetector::BuildDetectorPlane()
{
  DUMMYDetectorPlane *plane = new DUMMYDetectorPlane();
  plane -> SetParameterContainer(fPar);
  plane -> SetPlaneID(0);
  plane -> Init();

  AddPlane(plane);

  return true;
}
