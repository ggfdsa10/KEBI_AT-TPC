#include "LHTpc.hh"
#include "LHPadPlane.hh"
#include "LHPadPlaneRPad.hh"
#include "LHPadPlane3.hh"

ClassImp(LHTpc)

LHTpc::LHTpc()
:KBTpc("LAMPS-TPC","cylinderical time projection chamber")
{
}

bool LHTpc::Init()
{
  fEFieldAxis = fPar -> GetParAxis("TPCEFieldAxis");

  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

KBVector3::Axis LHTpc::GetEFieldAxis()
{
  return fEFieldAxis;
}

TVector3 LHTpc::GetEField(TVector3)
{
  KBVector3 e(0,0,0);
  e.AddAt(1,fEFieldAxis);
  return e;
}

KBPadPlane *LHTpc::GetDriftPlane(TVector3 pos)
{
  return (KBPadPlane *) fDetectorPlaneArray -> At(0);
}

bool LHTpc::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("LAMPS TPC", "LAMPS TPC Geometry");
  }

	auto rMinTPC = fPar -> GetParDouble("TPCrMin");
	auto rMaxTPC = fPar -> GetParDouble("TPCrMax");
	auto tpcLength = fPar -> GetParDouble("TPCLength");
  auto zOffset = fPar -> GetParDouble("zOffset");

	TString gasPar = "p10";
	if (fPar -> CheckPar("gasPar")) {
		gasPar = fPar -> GetParString("gasPar");
		gasPar.ToLower();
		if (gasPar.Index("p10")>=0) gasPar = "p10";
		else if (gasPar.Index("p20")>=0) gasPar = "p20";
		else gasPar = "p10";
	}

	TGeoMedium *pMed = new TGeoMedium(gasPar.Data(), 1, new TGeoMaterial(gasPar.Data()));

  auto top = CreateGeoTop();

  TGeoVolume *tpc = new TGeoVolumeAssembly("TPC");
  TGeoTranslation *offTPC = new TGeoTranslation("TPC offset",0,0,zOffset);

  TGeoVolume *gas = fGeoManager -> MakeTube("gas",pMed,rMinTPC,rMaxTPC,tpcLength/2);
  gas -> SetVisibility(true);
  gas -> SetLineColor(kBlue-10);
  gas -> SetTransparency(90);

  top -> AddNode(tpc, top->GetNdaughters()+1, offTPC);
  tpc -> AddNode(gas, 1);

  FinishGeometry();

  return true;
}

bool LHTpc::BuildDetectorPlane()
{
  KBPadPlane *padplane = nullptr;

  if (fPar->CheckPar("TPCPadPlaneType"))
  {
    if (fPar->GetParString("TPCPadPlaneType") == "LHPadPlaneRPad")
      padplane = new LHPadPlaneRPad();
    else if (fPar->GetParString("TPCPadPlaneType") == "LHPadPlane3")
      padplane = new LHPadPlane3();
  }
  else
    padplane = new LHPadPlane();

  padplane -> SetParameterContainer(fPar);
  padplane -> SetPlaneID(0);
	padplane -> SetPlaneK(fPar -> GetParDouble("TPCPadPlaneK0"));
  padplane -> SetAxis(KBVector3::kX, KBVector3::kY);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
