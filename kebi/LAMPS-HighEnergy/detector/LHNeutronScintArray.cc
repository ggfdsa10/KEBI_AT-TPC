#include "LHNeutronScintArray.hh"
#include "LHPadPlane.hh"
#include "LHPadPlaneRPad.hh"
#include "KBGeoBoxStack.hh"
#include "KBGeoBox.hh"

ClassImp(LHNeutronScintArray)

LHNeutronScintArray::LHNeutronScintArray()
:KBDetector("LAMPS-Scint-Array","neutron scintillator array")
{
}

bool LHNeutronScintArray::Init()
{
  if (BuildGeometry() == false)
    return false;

  return true;
}

bool LHNeutronScintArray::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("LAMPS-Scint-Array", "LAMPS Neutron Scintillator Array");
  }

  auto top = CreateGeoTop();

  TGeoMedium *xylen = new TGeoMedium("xylen", 1, new TGeoMaterial("xylen"));

  auto numWall = fPar -> GetParInt("numNeutronWall");
  for (auto iwall = 0; iwall < numWall; ++iwall) {
    auto naStackAxis = fPar -> GetParAxis(Form("naStackAxis%d",iwall));
    auto naNumStack = fPar -> GetParInt(Form("naNumStack%d",iwall));
    auto nadX = fPar -> GetParDouble(Form("nadX%d",iwall));
    auto nadY = fPar -> GetParDouble(Form("nadY%d",iwall));
    auto nadZ = fPar -> GetParDouble(Form("nadZ%d",iwall));
    auto naXOffset = fPar -> GetParDouble(Form("naXOffset%d",iwall));
    auto naYOffset = fPar -> GetParDouble(Form("naYOffset%d",iwall));
    auto naZOffset = fPar -> GetParDouble(Form("naZOffset%d",iwall));

    KBGeoBoxStack boxStack(naXOffset,naYOffset,naZOffset,nadX,nadY,nadZ,naNumStack,naStackAxis,KBVector3::kZ);

    TGeoVolume *wall = new TGeoVolumeAssembly(Form("NeutronWall_%d",iwall));
    top -> AddNode(wall, top->GetNdaughters()+1);

    for (auto copy = 0; copy < naNumStack; ++copy)
    {
      Int_t id = 10000+copy+iwall*100;
      auto box = boxStack.GetBox(copy);
      auto pos = box.GetCenter();

      TGeoVolume *scint = fGeoManager -> MakeBox(Form("scint_%d_%d",iwall,copy),xylen,0.5*box.GetdX(), 0.5*box.GetdY(), 0.5*box.GetdZ());
      TGeoTranslation *offset = new TGeoTranslation(Form("offset_%d_%d",iwall,copy),pos.X(),pos.Y(),pos.Z());

      scint -> SetVisibility(true);
      scint -> SetLineColor(kGray);
      scint -> SetTransparency(90);

      wall -> AddNode(scint, copy+1, offset);
    }
  }

  FinishGeometry();

  return true;
}

bool LHNeutronScintArray::BuildDetectorPlane()
{
  return true;
}
