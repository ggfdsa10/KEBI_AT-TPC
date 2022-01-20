#include "ATTPC.hh"
#include "ATTPCRectanglePad.hh"
#include "ATTPCHoneyCombPad.hh"

ClassImp(ATTPC)

ATTPC::ATTPC()
:KBTpc("AT-TPC","Active Target - Time Projection Chamber")
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
  auto worldX = fPar -> GetParDouble("worldX");
  auto worldY = fPar -> GetParDouble("worldY");
  auto worldZ = fPar -> GetParDouble("worldZ");

  auto tpcX = fPar -> GetParDouble("tpcX");
  auto tpcY = fPar -> GetParDouble("tpcY");
  auto tpcZ = fPar -> GetParDouble("tpcZ");

  Double_t PadWidth;
  Double_t PadHeight;
  Double_t PadGap;

  auto PadPlaneType = fPar -> GetParString("PadPlaneType");
  
  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  else if(PadPlaneType == "RectanglePad"){
    ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  auto triggerDistance = fPar -> GetParDouble("TriggerDistance");
  auto triggerSizeX = fPar -> GetParDouble("WindowSize",0);
  auto triggerSizeZ = fPar -> GetParDouble("WindowSize",1);
  auto triggerHeight = fPar -> GetParDouble("WindowHeight");
  auto triggerShift = fPar -> GetParDouble("WindowShift");

  TString detMatName = fPar -> GetParString("detMatName");

  TGeoMedium *Gas = new TGeoMedium("p10", 1, new TGeoMaterial("p10"));
  TGeoMedium *Vacuum = new TGeoMedium("Vacuum", 1, new TGeoMaterial("Vacuum"));

  auto top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  TGeoVolume *tpc = new TGeoVolumeAssembly("AT-TPC");
  
  TGeoTranslation *TPC_offset = new TGeoTranslation("TPC_offset", tpcX-PadWidth/2-PadGap/2, tpcY-PadHeight/2-PadGap/2, tpcZ);
  TGeoTranslation *World_offset = new TGeoTranslation("World_offset", -tpcX+PadWidth/2, -tpcY-PadHeight/2, -tpcZ);
  TGeoTranslation *trigger_offset1 = new TGeoTranslation("Trigger_offset1", -triggerShift, -triggerDistance+4, triggerHeight);
  TGeoTranslation *trigger_offset2 = new TGeoTranslation("Trigger_offset2", -triggerShift, triggerDistance+4, triggerHeight);
  TGeoTranslation *Pad_offset = new TGeoTranslation("Pad_offset", 0., 0., -tpcZ);

  TGeoVolume *TPC_volume = fGeoManager -> MakeBox("TPC_volume", Gas ,tpcX, tpcY, tpcZ);
  TPC_volume -> SetVisibility(true);
  TPC_volume -> SetLineColor(kBlue-2);
  TPC_volume -> SetTransparency(90);

  TGeoVolume *Pad_volume = fGeoManager -> MakeBox("Pad_volume", Gas ,tpcX, tpcY, 0.1);
  Pad_volume -> SetVisibility(true);
  Pad_volume -> SetLineColor(kBlack);
 
  TGeoVolume *World_volume = fGeoManager -> MakeBox("World_volume", Vacuum ,worldX, worldY, worldZ);
  World_volume -> SetVisibility(true);
  World_volume -> SetLineColor(kBlue-10);
  World_volume -> SetTransparency(99);

  TGeoVolume *Trigger_volume1 = fGeoManager -> MakeBox("Trigger_volume1", Vacuum ,triggerSizeX/2, 1, triggerSizeZ/2);
  Trigger_volume1 -> SetVisibility(true);
  Trigger_volume1 -> SetLineColor(kRed);
  Trigger_volume1 -> SetTransparency(70);

  TGeoVolume *Trigger_volume2 = fGeoManager -> MakeBox("Trigger_volume2", Vacuum ,triggerSizeX/2, 1, triggerSizeZ/2);
  Trigger_volume2 -> SetVisibility(true);
  Trigger_volume2 -> SetLineColor(kRed);
  Trigger_volume2 -> SetTransparency(70);



  top -> AddNode(tpc, 1, TPC_offset);
  tpc -> AddNode(TPC_volume, 1);
  tpc -> AddNode(Pad_volume, 2, Pad_offset);
  tpc -> AddNode(World_volume, 3, World_offset);
  tpc -> AddNode(Trigger_volume1, 4, trigger_offset1);
  tpc -> AddNode(Trigger_volume2, 5, trigger_offset2);

  FinishGeometry();

  return true;
}

bool ATTPC::BuildDetectorPlane()
{

  KBPadPlane *padplane = nullptr;

  if (fPar->CheckPar("PadPlaneType")){
    if (fPar -> GetParString("PadPlaneType") == "RectanglePad"){
      padplane = new ATTPCRectanglePad();
    }  
    else if(fPar -> GetParString("PadPlaneType") == "HoneyCombPad"){
      padplane = new ATTPCHoneyCombPad();
    }
  }
  else
    padplane = new ATTPCRectanglePad();
  
     
  padplane -> SetParameterContainer(fPar);
  padplane -> SetPlaneID(0);
  auto planeK = 0.;
  padplane -> SetPlaneK(planeK);
  padplane -> SetAxis(KBVector3::kX, KBVector3::kY);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
