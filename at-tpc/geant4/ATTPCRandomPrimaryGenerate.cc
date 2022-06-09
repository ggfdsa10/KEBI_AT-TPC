#include "ATTPCRandomPrimaryGenerate.hh"
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
}

ATTPCRandomPrimaryGenerate::~ATTPCRandomPrimaryGenerate()
{
  delete fParticleGun;
}

void ATTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{   
  gRandom -> SetSeed(0);
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* iontable = G4IonTable::GetIonTable();
  G4String particleName;
  G4ParticleDefinition* particle = nullptr;

  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString RandomParticle = par -> GetParString("RandomParticle");
  ParticleEnergy = par -> GetParDouble("ParticleEnergy");

  if (RandomParticle == "mu-"){
    particle = particleTable -> FindParticle(particleName = "mu-");
  }

  else if (RandomParticle == "proton"){
    particle = particleTable -> FindParticle(particleName = "proton");
  }

  else if (RandomParticle == "alpha"){
    particle = iontable ->FindIon(2,4,0); //helium
  }

  else if (RandomParticle == "ion"){
    Int_t IonZ = par -> GetParInt("IonInfo", 0);
    Int_t IonA = par -> GetParInt("IonInfo", 1);
    Int_t IonE = par -> GetParInt("IonInfo", 2);

    particle = iontable ->GetIon(IonZ, IonA, IonE);
    g4_info << " Primary particle : " << iontable ->GetIonName(IonZ, IonA, IonE) << endl;
  }

  else if (RandomParticle == "gamma"){
    particle = particleTable -> FindParticle(particleName = "gamma");
  }

  else{
    g4_warning << " Particle name is not valid. " << RandomParticle << endl;
  }
  if(RandomParticle != "ion"){
    g4_info << " Primary particle : " << RandomParticle << endl;
  }

  PaiAngle = (90.+ gRandom->Uniform(-30., 30.))*TMath::Pi()/180.;
  ThetaAngle = (90.+ gRandom->Uniform(-30., 30.))*TMath::Pi()/180.;

  PositionX = 32. + gRandom -> Uniform(-5., 5.);
  PositionZ = 60. + gRandom -> Uniform(-2., 2.);

  for(int i=0; i<2; i++){
    int eventID = anEvent->GetEventID() - 1;


    ProtonBeam(eventID, i);

    fParticleGun -> SetParticleDefinition(particle);
    fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
    fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
    fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
    fParticleGun -> GeneratePrimaryVertex(anEvent);
  }


  fNumberOfPrimary = anEvent -> GetNumberOfPrimaryVertex();

}

void ATTPCRandomPrimaryGenerate::TriggerFunction()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");

  G4double windowX = par -> GetParDouble("WindowSize", 0);
  G4double windowZ = par -> GetParDouble("WindowSize", 1);
  G4double WindowHeight = par -> GetParDouble("WindowHeight");
  G4double WindowShift = par -> GetParDouble("WindowShift");
  G4double TriggerDistance = par -> GetParDouble("TriggerDistance");
  TString PadPlaneType = par -> GetParString("PadPlaneType");

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

  
  PositionX = gRandom -> Uniform(windowX/2 -15.*3., windowX/2+15.*3.); //22
  PositionZ = gRandom -> Uniform(windowZ/2 -15.*3., windowZ/2+15.*3.); // 23


  G4double arctanX1 = ATan(PositionX / (2 * TriggerDistance));
  G4double arctanX2 = ATan((windowX - PositionX) / (2 * TriggerDistance));

  G4double arctanZ1 = ATan(PositionZ / (2 * TriggerDistance));
  G4double arctanZ2 = ATan((windowZ - PositionZ) / (2 * TriggerDistance));

  G4double ThetaAngle = gRandom -> Uniform(-arctanZ2, arctanZ1) +Pi()/2;
  G4double PaiAngle = gRandom -> Uniform(-arctanX2, arctanX1) +Pi()/2;

  PositionX = PositionX -WindowShift;
  PositionY = -30.;
  PositionZ = PositionZ +WindowHeight;

  DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  DirectionZ = Cos(ThetaAngle);

}

void ATTPCRandomPrimaryGenerate::ProtonBeam(int eventID, int trackNum)
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString PadPlaneType = par -> GetParString("PadPlaneType");
  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");
  G4double PadWidth;
  G4double PadHeight;
  G4double PadGap;

  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  if(PadPlaneType == "HoneyCombPad"){
    ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  if(PadPlaneType == "20RectanglePad"){
    ATTPC20RectanglePad *fPadPlane = new ATTPC20RectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  if(PadPlaneType == "StripPad"){
    ATTPCStripPad *fPadPlane = new ATTPCStripPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  if(trackNum==0){
    PositionY = -PadHeight/2.;
    DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
    DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
    DirectionZ = Cos(ThetaAngle);

    ParticleEnergy = 75.;
  }
  if(trackNum==1){
    // double padInDegree = 90.-double(indexPai*5);
    // double thetaInDegree = 90.+double((indexTheta -10*indexPai)*5);

    // PaiAngle = padInDegree*TMath::Pi()/180.;
    // ThetaAngle = thetaInDegree*TMath::Pi()/180.;

    // double tanAnglePai = (90. - padInDegree)*TMath::Pi()/180.;
    // double tanAngleTheta = (thetaInDegree -90.)*TMath::Pi()/180.;

    // PositionX = 32.-(48.*Tan(tanAnglePai));
    // PositionZ = 75.+(48.*Tan(tanAngleTheta));
    // PositionY = -PadHeight/2.;

    // PositionX = 20.+double(indexPai*5)*0.25;
    // PositionZ = 40.+double((indexTheta -10*indexPai/2)*3)*0.25;
    PositionY = -PadHeight/2.;
    PositionZ = PositionZ + gRandom->Uniform(0.,50.);

    DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
    DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
    DirectionZ = Cos(ThetaAngle);

    ParticleEnergy = 75.;
  }

}

void ATTPCRandomPrimaryGenerate::AlphaScattering(int eventID, int trackNum)
{

  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString PadPlaneType = par -> GetParString("PadPlaneType");
  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");
  G4double PadWidth;
  G4double PadHeight;
  G4double PadGap;

  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  if(PadPlaneType == "HoneyCombPad"){
    ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  if(PadPlaneType == "20RectanglePad"){
    ATTPC20RectanglePad *fPadPlane = new ATTPC20RectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  ParticleEnergy = 10.;
  Double_t Atomic_Mass = 931.5016;
  Double_t Corbon_KE = ParticleEnergy;
  Double_t Corbon_hoyle = 7.65;
  Double_t Corbon_Mass = 12*Atomic_Mass;
  Double_t Corbon_E = Corbon_hoyle +Corbon_Mass;

  
  Double_t Alpha_E = Corbon_E/3.;
  Double_t Alpha_Mass = 4*Atomic_Mass;
  Double_t Alpha_KE = Alpha_E - Alpha_Mass;
  Double_t Alpha_Mom = Sqrt(Alpha_E*Alpha_E - Alpha_Mass*Alpha_Mass);


  Double_t PlaneAnglePsi = gRandom -> Uniform(0,120);
  Double_t angleTheta = 0.;
  Double_t anglePai = 90.;

  TVector3 AlphaPos1, AlphaPos2, AlphaPos3;
  TVector3 Alpha1, Alpha2, Alpha3;
  TLorentzVector Alpha1_4Vec, Alpha2_4Vec, Alpha3_4Vec;

  Alpha1.SetXYZ(Alpha_Mom * Cos(PlaneAnglePsi * Pi() / 180.), Alpha_Mom * Sin(PlaneAnglePsi * Pi() / 180.), Alpha_Mom * 0);
  Alpha2.SetXYZ(Alpha_Mom * Cos((PlaneAnglePsi - 120.) * Pi() / 180.), Alpha_Mom * Sin((PlaneAnglePsi - 120.) * Pi() / 180.), Alpha_Mom * 0);
  Alpha3.SetXYZ(Alpha_Mom * Cos((PlaneAnglePsi + 120.) * Pi() / 180.), Alpha_Mom * Sin((PlaneAnglePsi + 120.) * Pi() / 180.), Alpha_Mom * 0);

  Alpha1.RotateZ(angleTheta * Pi() / 180.);
  Alpha2.RotateZ(angleTheta * Pi() / 180.);
  Alpha3.RotateZ(angleTheta * Pi() / 180.);

  Alpha1.RotateX(anglePai * Pi() / 180.);
  Alpha2.RotateX(anglePai * Pi() / 180.);
  Alpha3.RotateX(anglePai * Pi() / 180.);

  Alpha1_4Vec.SetPxPyPzE(Alpha1.X(), Alpha1.Y(), Alpha1.Z(), Alpha_E);
  Alpha2_4Vec.SetPxPyPzE(Alpha2.X(), Alpha2.Y(), Alpha2.Z(), Alpha_E);
  Alpha3_4Vec.SetPxPyPzE(Alpha3.X(), Alpha3.Y(), Alpha3.Z(), Alpha_E);

  Double_t Corbon_Beta = Sqrt(1 - 1 / Power(1 + (Corbon_KE / Corbon_Mass), 2));

  TVector3 AlphaBoost;
  AlphaBoost.SetXYZ(0, Corbon_Beta, 0);

  Alpha1_4Vec.Boost(AlphaBoost);
  Alpha2_4Vec.Boost(AlphaBoost);
  Alpha3_4Vec.Boost(AlphaBoost);

  if(trackNum ==0){
    // particle = iontable -> FindIon(6,12,0);

    PositionX = 45.;
    PositionZ = 75.;
    PositionY = -PadHeight/2.;

    DirectionX = 0.;
    DirectionY = 1.;
    DirectionZ = 0.;
  }

  if(trackNum ==1){
    // particle = iontable -> FindIon(2,4,0);

    PositionX = 45.;
    PositionZ = 75.;
    PositionY = 80.;

    auto Direction = Alpha1_4Vec.Vect().Unit();
    DirectionX = Direction.X();
    DirectionY = Direction.Y();
    DirectionZ = Direction.Z();

    ParticleEnergy = Alpha1_4Vec.E() - Alpha_Mass;
  }

  if(trackNum ==2){
    // particle = iontable -> FindIon(2,4,0);

    PositionX = 55.;
    PositionZ = 75.;
    PositionY = 80.;

    auto Direction = Alpha2_4Vec.Vect().Unit();
    DirectionX = Direction.X();
    DirectionY = Direction.Y();
    DirectionZ = Direction.Z();

    ParticleEnergy = Alpha2_4Vec.E() - Alpha_Mass;
  }

  if(trackNum ==3){
    // particle = iontable -> FindIon(2,4,0);

    PositionX = 45.;
    PositionZ = 75.;
    PositionY = 80.;

    auto Direction = Alpha3_4Vec.Vect().Unit();
    DirectionX = Direction.X();
    DirectionY = Direction.Y();
    DirectionZ = Direction.Z();

    ParticleEnergy = Alpha3_4Vec.E() - Alpha_Mass;
  }





}