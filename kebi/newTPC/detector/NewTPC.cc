#include "NewTPC.hh"
#include "NewTPCRectnglePad.hh"
#include "NewTPCHoneyCombPad.hh"

ClassImp(NewTPC)

NewTPC::NewTPC()
:KBTpc("newTPC","new cylindrical Active Target - Time Projection Chamber")
{
}

bool NewTPC::Init()
{

  fEFieldAxis = fPar -> GetParAxis("tpcEFieldAxis");

  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

KBVector3::Axis NewTPC::GetEFieldAxis()
{
  return fEFieldAxis;
}

TVector3 NewTPC::GetEField(TVector3)
{
  KBVector3 e(0,0,0);
  e.AddAt(1,fEFieldAxis);
  return e;
}

KBPadPlane *NewTPC::GetDriftPlane(TVector3 pos)
{
  return (KBPadPlane *) fDetectorPlaneArray -> At(0);
}

bool NewTPC::BuildGeometry()
{


  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("NewTPC", "New cylindrical TPC Geometry");
  }
  auto worldX = fPar -> GetParDouble("worldX");
  auto worldY = fPar -> GetParDouble("worldY");
  auto worldZ = fPar -> GetParDouble("worldZ");

  auto tpcR0 = fPar -> GetParDouble("tpcR0");
  auto tpcR1 = fPar -> GetParDouble("tpcR1");
  auto tpcLength = fPar -> GetParDouble("tpcLength");


  TString detMatName = fPar -> GetParString("detMatName");

  TGeoMedium *Gas = new TGeoMedium("p10", 1, new TGeoMaterial("p10"));
  TGeoMedium *Vacuum = new TGeoMedium("Vacuum", 1, new TGeoMaterial("Vacuum"));

  auto top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  TGeoVolume *tpc = new TGeoVolumeAssembly("AT-TPC");
  
  TGeoTranslation *TPC_offset = new TGeoTranslation("TPC_offset", 0., 0., tpcLength);
  TGeoTranslation *Pad_offset = new TGeoTranslation("TPC_offset", 0., 0., -tpcLength);

  TGeoVolume *TPC_volume = fGeoManager -> MakeTube("TPC_volume", Gas ,tpcR0, tpcR1, tpcLength);
  TPC_volume -> SetVisibility(true);
  TPC_volume -> SetLineColor(kBlue-2);
  TPC_volume -> SetTransparency(90);    

  TGeoVolume *Pad_volume = fGeoManager -> MakeTube("Pad_volume", Gas ,tpcR0, tpcR1, 0.1);
  Pad_volume -> SetVisibility(true);
  Pad_volume -> SetLineColor(kBlack);
 
  TGeoVolume *World_volume = fGeoManager -> MakeBox("World_volume", Vacuum ,worldX, worldY, worldZ);
  World_volume -> SetVisibility(true);
  World_volume -> SetLineColor(kBlue-10);
  World_volume -> SetTransparency(99);

  top -> AddNode(tpc, 1, TPC_offset);
  tpc -> AddNode(TPC_volume, 1);
  tpc -> AddNode(Pad_volume, 2, Pad_offset);
  tpc -> AddNode(World_volume, 3);

  FinishGeometry();

  return true;
}

bool NewTPC::BuildDetectorPlane()
{

  KBPadPlane *padplane = nullptr;

  if (fPar->CheckPar("PadPlaneType")){
    if (fPar -> GetParString("PadPlaneType") == "RectanglePad"){
      padplane = new NewTPCRectnglePad();
    }  
    else if(fPar -> GetParString("PadPlaneType") == "HoneyCombPad"){
      padplane = new NewTPCHoneyCombPad();
    }
  }
  else
    padplane = new NewTPCRectnglePad();
  
     
  padplane -> SetParameterContainer(fPar);
  padplane -> SetPlaneID(0);
  auto planeK = 0.;
  padplane -> SetPlaneK(planeK);
  padplane -> SetAxis(KBVector3::kX, KBVector3::kY);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
