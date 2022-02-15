#include "NewTPCRandomPrimaryGenerate.hh"
#include "NewTPCRectnglePad.hh"
#include "NewTPCHoneyCombPad.hh"


using namespace std;
using namespace TMath;

NewTPCRandomPrimaryGenerate::NewTPCRandomPrimaryGenerate() : G4VUserPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);
}

NewTPCRandomPrimaryGenerate::~NewTPCRandomPrimaryGenerate()
{
  delete fParticleGun;

}

void NewTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{   
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* iontable = G4IonTable::GetIonTable();
  G4String particleName;
  G4ParticleDefinition* particle = nullptr;


  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString RandomParticle = par -> GetParString("RandomParticle");

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
  if(RandomParticle != "ion")
    g4_info << " Primary particle : " << RandomParticle << endl;


  AlphaScattering();
  // Particle();

  fParticleGun -> SetParticleDefinition(particle);
  fParticleGun -> SetParticlePosition(G4ThreeVector(AlphaPos1.X() *mm, AlphaPos1.Y() *mm, AlphaPos1.Z() *mm));
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha1_4Vec.X(), Alpha1_4Vec.Y(), Alpha1_4Vec.Z()).unit());
  fParticleGun -> SetParticleMomentum(G4ThreeVector(Alpha1_4Vec.X(), Alpha1_4Vec.Y(), Alpha1_4Vec.Z()).mag() *MeV);
  fParticleGun -> GeneratePrimaryVertex(anEvent);

  fParticleGun -> SetParticleDefinition(particle);
  fParticleGun -> SetParticlePosition(G4ThreeVector(AlphaPos2.X() *mm, AlphaPos2.Y() *mm, AlphaPos2.Z() *mm));
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha2_4Vec.X(), Alpha2_4Vec.Y(), Alpha2_4Vec.Z()).unit());
  fParticleGun -> SetParticleMomentum(G4ThreeVector(Alpha2_4Vec.X(), Alpha2_4Vec.Y(), Alpha2_4Vec.Z()).mag() *MeV);
  fParticleGun -> GeneratePrimaryVertex(anEvent);

  fParticleGun -> SetParticleDefinition(particle);
  fParticleGun -> SetParticlePosition(G4ThreeVector(AlphaPos3.X() *mm, AlphaPos3.Y() *mm, AlphaPos3.Z() *mm));
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha3_4Vec.X(), Alpha3_4Vec.Y(), Alpha3_4Vec.Z()).unit());
  fParticleGun -> SetParticleMomentum(G4ThreeVector(Alpha3_4Vec.X(), Alpha3_4Vec.Y(), Alpha3_4Vec.Z()) *MeV);
  fParticleGun -> GeneratePrimaryVertex(anEvent);
  
}

void NewTPCRandomPrimaryGenerate::AlphaScattering()
{
  auto fRandom = new TRandom3(0);
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  G4double tpcR0 = par -> GetParDouble("tpcR0");
  G4double tpcR1 = par -> GetParDouble("tpcR1");
  G4double tpcLength = par -> GetParDouble("tpcLength");
  G4double ParticleEnergy = par -> GetParDouble("ParticleEnergy");

  Double_t PlaneAnglePsi = par -> GetParDouble("PlaneAnglePsi");
  Double_t angleTheta = par -> GetParDouble("angleTheta");
  Double_t anglePai = par -> GetParDouble("anglePai");

  Double_t Atomic_Mass = 931.5016;
  Double_t Corbon_KE = ParticleEnergy;
  Double_t Corbon_hoyle = 7.65;
  Double_t Corbon_Mass = 12*Atomic_Mass;
  Double_t Corbon_E = Corbon_hoyle +Corbon_Mass;

  Double_t Alpha_E = Corbon_E/3.;
  Double_t Alpha_Mass = 4*Atomic_Mass;
  Double_t Alpha_KE = Alpha_E - Alpha_Mass;
  Double_t Alpha_Mom = Sqrt(Alpha_E*Alpha_E - Alpha_Mass*Alpha_Mass);

  Double_t InitPosX = 0.;
  Double_t InitPosY = 0.;
  Double_t InitPosZ = tpcLength;

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

  Double_t Corbon_Beta = Sqrt(1 - 1 / Power(1 + (Corbon_KE / Corbon_Mass), 2));

  TVector3 AlphaBoost;
  AlphaBoost.SetXYZ(0, 0, -Corbon_Beta);

  Alpha1_4Vec.Boost(AlphaBoost);
  Alpha2_4Vec.Boost(AlphaBoost);
  Alpha3_4Vec.Boost(AlphaBoost);

  KEnergy1 = Alpha1_4Vec.E() - Alpha_Mass;
  KEnergy2 = Alpha1_4Vec.E() - Alpha_Mass;
  KEnergy3 = Alpha1_4Vec.E() - Alpha_Mass;

  AlphaPos1.SetXYZ(InitPosX, InitPosY, InitPosZ);
  AlphaPos2.SetXYZ(InitPosX, InitPosY, InitPosZ);
  AlphaPos3.SetXYZ(InitPosX, InitPosY, InitPosZ);
}
  
void NewTPCRandomPrimaryGenerate::Particle(){
  auto fRandom = new TRandom3(0);
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  G4double tpcR0 = par -> GetParDouble("tpcR0");
  G4double tpcR1 = par -> GetParDouble("tpcR1");
  G4double tpcLength = par -> GetParDouble("tpcLength");
  G4double ParticleEnergy = par -> GetParDouble("ParticleEnergy");

  Double_t PlaneAnglePsi = par -> GetParDouble("PlaneAnglePsi");
  Double_t angleTheta = par -> GetParDouble("angleTheta");
  Double_t anglePai = par -> GetParDouble("anglePai");

  Double_t mass = 105.7;
  Double_t Total_E = ParticleEnergy + mass;
  Double_t Momentum = Sqrt(Total_E*Total_E - mass*mass);

  AlphaPos1.SetXYZ(0,0,250);
  Alpha1_4Vec.SetXYZT(0,Momentum,0,Total_E);

}