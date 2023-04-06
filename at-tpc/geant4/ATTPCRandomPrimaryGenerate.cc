#include "ATTPCRandomPrimaryGenerate.hh"
#include "ATTPCEventAction.hh"
#include "ATTPCRectanglePad.hh"
#include "ATTPCHoneyCombPad.hh"
#include "ATTPC20RectanglePad.hh"
#include "ATTPCStripPad.hh"
#include "TVector3.h"
#include "TLorentzVector.h"

using namespace std;
using namespace TMath;

ATTPCRandomPrimaryGenerate::ATTPCRandomPrimaryGenerate() : G4VUserPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  fRunManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
  fRandom = new TRandom3(0);

  fParticleTable = G4ParticleTable::GetParticleTable();
  fIontable = G4IonTable::GetIonTable();

  // test
  auto file = new TFile("input/muonFlux.root","read");
  muonFluxH1 = (TH1D*)file -> Get("MuFlux");

}

ATTPCRandomPrimaryGenerate::~ATTPCRandomPrimaryGenerate()
{
  delete fParticleGun;
}

void ATTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{   
  fRandom -> SetSeed(0);
  TString RandomParticle = fPar -> GetParString("RandomParticle");
  G4String particleName;
  if (RandomParticle == "mu-"){
    fParticle = fParticleTable -> FindParticle(particleName = "mu-");
  }

  else if (RandomParticle == "proton"){
    fParticle = fParticleTable -> FindParticle(particleName = "proton");
  }

  else if (RandomParticle == "alpha"){
    fParticle = fIontable ->FindIon(2,4,0); //helium
  }

  else if (RandomParticle == "ion"){
    Int_t IonZ = fPar -> GetParInt("IonInfo", 0);
    Int_t IonA = fPar -> GetParInt("IonInfo", 1);
    Int_t IonE = fPar -> GetParInt("IonInfo", 2);

    fParticle = fIontable ->GetIon(IonZ, IonA, IonE);
    g4_info << " Primary particle : " << fIontable ->GetIonName(IonZ, IonA, IonE) << endl;
  }

  else if (RandomParticle == "gamma"){
    fParticle = fParticleTable -> FindParticle(particleName = "gamma");
  }

  else{
    g4_warning << " Particle name is not valid. " << RandomParticle << endl;
  }
  if(RandomParticle != "ion"){
    g4_info << " Primary particle : " << RandomParticle << endl;
  }


  // ExternalTriggerPMT(anEvent);
  // HIMACSetup(anEvent);
  HoyleState(anEvent);

  fNumberOfPrimary = anEvent -> GetNumberOfPrimaryVertex();
}

void ATTPCRandomPrimaryGenerate::ExternalTriggerPMT(G4Event* event)
{
  G4double tpcX = fPar -> GetParDouble("tpcX");
  G4double tpcY = fPar -> GetParDouble("tpcY");
  G4double tpcZ = fPar -> GetParDouble("tpcZ");

  G4double windowX = fPar -> GetParDouble("WindowSize", 0);
  G4double windowZ = fPar -> GetParDouble("WindowSize", 1);
  G4double WindowHeight = fPar -> GetParDouble("WindowHeight");
  G4double WindowShift = fPar -> GetParDouble("WindowShift");
  G4double TriggerDistance = fPar -> GetParDouble("TriggerDistance");
  TString PadPlaneType = fPar -> GetParString("PadPlaneType");

  G4double PadWidth;
  G4double PadHeight;
  G4double PadGap;

  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  else if(PadPlaneType == "HoneycombPad"){
    ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  PositionX = gRandom -> Uniform(-windowX/2., windowX/2.) +tpcX-PadWidth/2-PadGap/2 +WindowShift;
  PositionY = -TriggerDistance +tpcY-PadHeight/2-PadGap/2;
  // PositionZ = gRandom -> Uniform(-windowZ/2., windowZ/2.) +tpcZ +WindowHeight;

  int zdistance = int(double(event->GetEventID())/2000.);
  PositionZ = 10. + double(zdistance)*10.;

  G4double arctanX1 = ATan(PositionX / (2 * TriggerDistance));
  G4double arctanX2 = ATan((windowX - PositionX) / (2 * TriggerDistance));

  G4double arctanZ1 = ATan(PositionZ / (2 * TriggerDistance));
  G4double arctanZ2 = ATan((windowZ - PositionZ) / (2 * TriggerDistance));

  G4double ThetaAngle = gRandom -> Uniform(-arctanZ2, arctanZ1) +Pi()/2;
  G4double PaiAngle = gRandom -> Uniform(-arctanX2, arctanX1) +Pi()/2;

  // DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  // DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  // DirectionZ = Cos(ThetaAngle);

  DirectionX = Cos(PaiAngle);
  DirectionY = Sin(PaiAngle);
  DirectionZ = 0.;

  // G4double ParticleEnergy = fPar -> GetParDouble("ParticleEnergy");
  G4double ParticleEnergy = muonFluxH1->GetRandom(fRandom)*1.e3;
  if(ParticleEnergy < 0.01){while(ParticleEnergy > 0.01){ParticleEnergy = muonFluxH1->GetRandom(fRandom)*1.e3;}}
  cout << ParticleEnergy << " MeV "  << endl;

  fParticleGun -> SetParticleDefinition(fParticle);
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
  fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
  fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
  fParticleGun -> GeneratePrimaryVertex(event);
}

void ATTPCRandomPrimaryGenerate::ProtonBeam(G4Event* event)
{
  G4double tpcX = fPar -> GetParDouble("tpcX");
  G4double tpcY = fPar -> GetParDouble("tpcY");
  G4double tpcZ = fPar -> GetParDouble("tpcZ");

  G4double windowX = fPar -> GetParDouble("WindowSize", 0);
  G4double windowZ = fPar -> GetParDouble("WindowSize", 1);
  G4double WindowHeight = fPar -> GetParDouble("WindowHeight");
  G4double WindowShift = fPar -> GetParDouble("WindowShift");
  G4double TriggerDistance = fPar -> GetParDouble("TriggerDistance");
  TString PadPlaneType = fPar -> GetParString("PadPlaneType");

  G4double PadWidth;
  G4double PadHeight;
  G4double PadGap;

  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  // if(trackNum==0){
  //   PositionX = 0.;
  //   PositionY = 0.;
  //   PositionZ = 0.;
  //   DirectionX = 0.;
  //   DirectionY = 1.;
  //   DirectionZ = 0.;

  //   ParticleEnergy = 4000.;
  // }

}

void ATTPCRandomPrimaryGenerate::HIMACSetup(G4Event* event)
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString PadPlaneType = par -> GetParString("PadPlaneType");
  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");

  ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
  G4double PadWidth = fPadPlane ->GetPadWidth();
  G4double PadHeight = fPadPlane ->GetPadHeight();
  G4double PadGap = fPadPlane ->GetPadGap();

  // HIMAC exp parameter for AT-TPC, ScI
  Double_t TPCCenter = 550.; // [mm]
  Double_t TPCCenterToOrigin = (PadHeight*8. + PadGap*8.)/2.-PadHeight/2.-PadGap/2.; //[mm]
  Double_t distanceToScISurface = 753.65 - 25.; // [mm]
  Double_t CsILengthX = 100.; // [mm]
  Double_t CsILengthY = 50.; // [mm]
  Double_t TPCOriginShiftZ = 82.75;
  Double_t TPCOriginShiftX = (PadWidth*32. + PadGap*32.)/2.-PadWidth/2.-PadGap/2.;

  Double_t YZMaxAngle = TMath::ATan((CsILengthX/2.)/distanceToScISurface);
  Double_t XYMaxAngle = TMath::ATan(((CsILengthY+5.)/2.)/distanceToScISurface);

  PositionX = TPCOriginShiftX;
  PositionY = -(TPCCenter - TPCCenterToOrigin);
  PositionZ = TPCOriginShiftZ;

  Double_t PaiAngle = fRandom -> Uniform(-XYMaxAngle, XYMaxAngle) +Pi()/2;;
  Double_t ThetaAngle = fRandom -> Uniform(-YZMaxAngle, YZMaxAngle) +Pi()/2;;

  DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  DirectionZ = Cos(ThetaAngle);

  G4double ParticleEnergy = fPar -> GetParDouble("ParticleEnergy");
  fParticleGun -> SetParticleDefinition(fParticle);
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
  fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
  fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
  fParticleGun -> GeneratePrimaryVertex(event);

}

void ATTPCRandomPrimaryGenerate::SetInteractionPoint(TVector3 pos, TVector3 mom)
{
  fInteractionPos = pos;
  fInteractionMom = mom;
}

void ATTPCRandomPrimaryGenerate::HoyleState(G4Event* event)
{
  G4double tpcX = fPar -> GetParDouble("tpcX");
  G4double tpcY = fPar -> GetParDouble("tpcY");
  G4double tpcZ = fPar -> GetParDouble("tpcZ");
  G4double ParticleEnergy = fPar -> GetParDouble("ParticleEnergy");

  Double_t Atomic_Mass = 931.5016;
  Double_t minimumInteractionEnergy = 7.654; // hoyle state
  // Double_t minimumInteractionEnergy = 0.5; // hoyle state

  Int_t eventID = event -> GetEventID();

  if(eventID%2 == 0){
    // direct mode.
    // PositionX = 100.;
    // PositionY = 100.;
    // PositionZ = 2.*tpcZ;
    // DirectionX = 0.;
    // DirectionY = 0.;
    // DirectionZ = -1.;

    // dipole mode.
    PositionX = 20.;
    PositionY = -10.;
    PositionZ = 78.;
    DirectionX = 0.;
    DirectionY = 1.;
    DirectionZ = 0.;

    fParticleGun -> SetParticleDefinition(fParticle);
    fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
    fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
    fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
    fParticleGun -> GeneratePrimaryVertex(event);

    InteractionEnergy = fRandom->Uniform(minimumInteractionEnergy, ParticleEnergy);
  }

  if(eventID%2 == 1){
    TVector3 Alpha1, Alpha2, Alpha3;
    TLorentzVector Alpha1_4Vec, Alpha2_4Vec, Alpha3_4Vec;
    Double_t KEnergy1, KEnergy2, KEnergy3;

    // Double_t PlaneAnglePsi = fPar -> GetParDouble("PlaneAnglePsi");
    // Double_t angleTheta = fPar -> GetParDouble("angleTheta");
    // Double_t anglePai = fPar -> GetParDouble("anglePai");

    Double_t PlaneAnglePsi = -1.;
    Double_t angleTheta = -1.;
    Double_t anglePai = -1.;

    Double_t Corbon_Mass = 12*Atomic_Mass;

    Double_t Alpha_Mass = 4 * Atomic_Mass;
    Double_t Alpha_KE = 0.126; // hoyle to 3alpha state
    Double_t Alpha_E = Alpha_KE + Alpha_Mass;
    Double_t Alpha_Mom = Sqrt(Alpha_E*Alpha_E - Alpha_Mass*Alpha_Mass);
    
    if(PlaneAnglePsi == -1.)
      PlaneAnglePsi = fRandom -> Uniform(0,120);
    if(angleTheta == -1.)
      angleTheta = fRandom -> Uniform(0,90);
    if(anglePai == -1.)
      anglePai = fRandom -> Uniform(0,90);

    Alpha1.SetXYZ(Alpha_Mom * Cos(PlaneAnglePsi * Pi() / 180.), Alpha_Mom * Sin(PlaneAnglePsi * Pi() / 180.), Alpha_Mom * 0);
    Alpha2.SetXYZ(Alpha_Mom * Cos((PlaneAnglePsi - 120.) * Pi() / 180.), Alpha_Mom * Sin((PlaneAnglePsi - 120.) * Pi() / 180.), Alpha_Mom * 0);
    Alpha3.SetXYZ(Alpha_Mom * Cos((PlaneAnglePsi + 120.) * Pi() / 180.), Alpha_Mom * Sin((PlaneAnglePsi + 120.) * Pi() / 180.), Alpha_Mom * 0);

    Alpha1.RotateX(angleTheta * Pi() / 180.);
    Alpha2.RotateX(angleTheta * Pi() / 180.);
    Alpha3.RotateX(angleTheta * Pi() / 180.);

    Alpha1.RotateY(anglePai * Pi() / 180.);
    Alpha2.RotateY(anglePai * Pi() / 180.);
    Alpha3.RotateY(anglePai * Pi() / 180.);

    Alpha1_4Vec.SetPxPyPzE(Alpha1.X(), Alpha1.Y(), Alpha1.Z(), Alpha_E);
    Alpha2_4Vec.SetPxPyPzE(Alpha2.X(), Alpha2.Y(), Alpha2.Z(), Alpha_E);
    Alpha3_4Vec.SetPxPyPzE(Alpha3.X(), Alpha3.Y(), Alpha3.Z(), Alpha_E);

    Double_t Corbon_Beta = Sqrt(1 - 1 / Power(1 + (ParticleEnergy / Corbon_Mass), 2));

    TVector3 AlphaBoost;
    TVector3 motherParticleDirection;
    motherParticleDirection.SetXYZ(fInteractionMom.x(), fInteractionMom.y(), fInteractionMom.z());
    motherParticleDirection = motherParticleDirection.Unit();
    motherParticleDirection.SetXYZ(motherParticleDirection.X()*Corbon_Beta, motherParticleDirection.Y()*Corbon_Beta, motherParticleDirection.Z()*Corbon_Beta);

    Alpha1_4Vec.Boost(motherParticleDirection);
    Alpha2_4Vec.Boost(motherParticleDirection);
    Alpha3_4Vec.Boost(motherParticleDirection);

    KEnergy1 = Alpha1_4Vec.E() - Alpha_Mass;
    KEnergy2 = Alpha2_4Vec.E() - Alpha_Mass;
    KEnergy3 = Alpha3_4Vec.E() - Alpha_Mass;

    Alpha1.SetXYZ(Alpha1_4Vec.Px(), Alpha1_4Vec.Py(), Alpha1_4Vec.Pz());
    Alpha2.SetXYZ(Alpha2_4Vec.Px(), Alpha2_4Vec.Py(), Alpha2_4Vec.Pz());
    Alpha3.SetXYZ(Alpha3_4Vec.Px(), Alpha3_4Vec.Py(), Alpha3_4Vec.Pz());

    PositionX = fInteractionPos.x();
    PositionY = fInteractionPos.y();
    PositionZ = fInteractionPos.z();

    fParticle = fIontable ->FindIon(2,4,0); //helium

    fParticleGun -> SetParticleDefinition(fParticle);
    fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha1.Unit().X(), Alpha1.Unit().Y(), Alpha1.Unit().Z()));
    fParticleGun -> SetParticleEnergy(KEnergy1 *MeV);
    fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
    
    fParticleGun -> GeneratePrimaryVertex(event);
    fParticleGun -> SetParticleDefinition(fParticle);
    fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha2.Unit().X(), Alpha2.Unit().Y(), Alpha2.Unit().Z()));
    fParticleGun -> SetParticleEnergy(KEnergy2 *MeV);
    fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
    fParticleGun -> GeneratePrimaryVertex(event);

    fParticleGun -> SetParticleDefinition(fParticle);
    fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha3.Unit().X(), Alpha3.Unit().Y(), Alpha3.Unit().Z()));
    fParticleGun -> SetParticleEnergy(KEnergy3 *MeV);
    fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
    fParticleGun -> GeneratePrimaryVertex(event);
  }
}
  