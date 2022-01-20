#include "KBGDMLDetector.hh"

KBGDMLDetector::KBGDMLDetector(TString fileName)
:KBDetector("KBGDMLDetector","Detector Geometry with GDML"), fFileName(fileName)
{
}

bool KBGDMLDetector::Init()
{
  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

bool KBGDMLDetector::BuildGeometry()
{
  TGeoManager::Import(fFileName);

  return true;
}

bool KBGDMLDetector::BuildDetectorPlane()
{
  return true;
}
