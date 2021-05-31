#include "ATTPC.hh"
#include "ATTPCPadPlane.hh"

ClassImp(ATTPC)

ATTPC::ATTPC()
:KBTpc("AP-TPC","Active Target - Time Projection Chamber")
{
}

bool ATTPC::Init()
{

  fEFieldAxis = fPar -> GetParAxis("tpcEFieldAxis");

  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

KBVector3::Axis ATTPC::GetEFieldAxis()
{
  return fEFieldAxis;
}

TVector3 ATTPC::GetEField(TVector3)
{
  KBVector3 e(0,0,0);
  e.AddAt(1,fEFieldAxis);
  return e;
}

KBPadPlane *ATTPC::GetDriftPlane(TVector3 pos)
{
  return (KBPadPlane *) fDetectorPlaneArray -> At(0);
}

bool ATTPC::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("AT-TPC", "AT-TPC Geometry");
  }

  auto tpcX = fPar -> GetParDouble("tpcX");
  auto tpcY = fPar -> GetParDouble("tpcY");
  auto tpcZ = fPar -> GetParDouble("tpcZ");
  TString detMatName = fPar -> GetParString("detMatName");

  TGeoMedium *Gas = new TGeoMedium("p10", 1, new TGeoMaterial("p10"));

  auto top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  TGeoVolume *tpc = new TGeoVolumeAssembly("AT-TPC");
  
  TGeoTranslation *offset = new TGeoTranslation("offset", tpcX, tpcY, tpcZ);

  TGeoVolume *tpc_volume = fGeoManager -> MakeBox("tpc_volume", Gas ,tpcX, tpcY, tpcZ);
  tpc_volume -> SetVisibility(true);
  tpc_volume -> SetLineColor(kBlue-10);
  tpc_volume -> SetTransparency(90);
 

  top -> AddNode(tpc, 1, offset);
  tpc -> AddNode(tpc_volume, 1);

  FinishGeometry();

  return true;
}

bool ATTPC::BuildDetectorPlane()
{

  KBPadPlane *padplane = nullptr;

  if (fPar->CheckPar("PadPlaneType")){
    if (fPar -> GetParString("PadPlaneType") == "ATTPCPadPlane"){
      padplane = new ATTPCPadPlane();
    }  
    
  }
  else
    padplane = new ATTPCPadPlane();
  
     
  padplane -> SetParameterContainer(fPar);
  padplane -> SetPlaneID(0);
  auto planeK = 0.;
  padplane -> SetPlaneK(planeK);
  padplane -> SetAxis(KBVector3::kX, KBVector3::kY);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
