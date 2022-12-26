#include "NewTPCRandomPrimaryGenerate.hh"
#include "NewTPCRectnglePad.hh"
#include "NewTPCHoneyCombPad.hh"

using namespace std;
using namespace TMath;

NewTPCRandomPrimaryGenerate::NewTPCRandomPrimaryGenerate() : G4VUserPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  fRunManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
  fRandom = new TRandom3(0);

  fParticleTable = G4ParticleTable::GetParticleTable();
  fIontable = G4IonTable::GetIonTable();
}

NewTPCRandomPrimaryGenerate::~NewTPCRandomPrimaryGenerate()
{
  delete fParticleGun;

}

void NewTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{   
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

  // for Generation
  bool isPhysics = fPar -> GetParBool("Physics");

  if(isPhysics == false){ // just one particle test
    // Particle();
    // fParticleGun -> SetParticleDefinition(fParticle);
    // fParticleGun -> SetParticlePosition(G4ThreeVector(AlphaPos1.X() *mm, AlphaPos1.Y() *mm, AlphaPos1.Z() *mm));
    // fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(Alpha1_4Vec.X(), Alpha1_4Vec.Y(), Alpha1_4Vec.Z()).unit());
    // fParticleGun -> SetParticleMomentum(G4ThreeVector(Alpha1_4Vec.X(), Alpha1_4Vec.Y(), Alpha1_4Vec.Z()).mag() *MeV);
    // fParticleGun -> GeneratePrimaryVertex(anEvent);
  }

  if(isPhysics == true){
    TString physicsType = fPar -> GetParString("PhysicsType");

    if(physicsType == "HoyleState"){HoyleState(anEvent);}
  }
}

void NewTPCRandomPrimaryGenerate::SetInteractionPoint(TVector3 pos, TVector3 mom)
{
  fInteractionPos = pos;
  fInteractionMom = mom;
}

void NewTPCRandomPrimaryGenerate::HoyleState(G4Event* event)
{
  G4double tpcR0 = fPar -> GetParDouble("tpcR0");
  G4double tpcR1 = fPar -> GetParDouble("tpcR1");
  G4double tpcLength = fPar -> GetParDouble("tpcLength");
  G4double ParticleEnergy = fPar -> GetParDouble("ParticleEnergy");
  Double_t Atomic_Mass = 931.5016;
  // Double_t minimumInteractionEnergy = 7.654; // hoyle state

  Double_t minimumInteractionEnergy = 0.5; // hoyle state

  Int_t eventID = event -> GetEventID();

  if(eventID%2 == 0){
    PositionX = 0.;
    PositionY = 0.;
    PositionZ = 499.;
    DirectionX = 0.;
    DirectionY = 0.;
    DirectionZ = -1.;


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

    Double_t PlaneAnglePsi = fPar -> GetParDouble("PlaneAnglePsi");
    Double_t angleTheta = fPar -> GetParDouble("angleTheta");
    Double_t anglePai = fPar -> GetParDouble("anglePai");

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
  
void NewTPCRandomPrimaryGenerate::Particle(){
  // G4double tpcR0 = fPar -> GetParDouble("tpcR0");
  // G4double tpcR1 = fPar -> GetParDouble("tpcR1");
  // G4double tpcLength = fPar -> GetParDouble("tpcLength");
  // G4double ParticleEnergy = fPar -> GetParDouble("ParticleEnergy");

  // Double_t PlaneAnglePsi = fPar -> GetParDouble("PlaneAnglePsi");
  // Double_t angleTheta = fPar -> GetParDouble("angleTheta");
  // Double_t anglePai = fPar -> GetParDouble("anglePai");

  // Double_t Atomic_Mass = 931.5016;
  // Double_t mass = 12*Atomic_Mass;
  // Double_t Total_E = ParticleEnergy + mass;
  // Double_t Momentum = Sqrt(Total_E*Total_E - mass*mass);

  // AlphaPos1.SetXYZ(0,0,470);
  // Alpha1_4Vec.SetXYZT(0,0,-Momentum,Total_E);

}