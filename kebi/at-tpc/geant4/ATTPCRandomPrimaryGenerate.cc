#include "ATTPCRandomPrimaryGenerate.hh"

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
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = nullptr;

  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString RandomParticle = par -> GetParString("RandomParticle");
  G4double ParticleEnergy = par -> GetParDouble("ParticleEnergy");

  if (RandomParticle == "mu-"){
    particle = particleTable -> FindParticle(particleName = "mu-");
  }

  else if (RandomParticle == "proton"){
    particle = particleTable -> FindParticle(particleName = "proton");
  }

  else if (RandomParticle == "alpha"){
    G4IonTable* iontable = G4IonTable::GetIonTable();
    particle = iontable ->FindIon(2,4,0); //helium
  }

  else if (RandomParticle == "gamma"){
    particle = particleTable -> FindParticle(particleName = "gamma");
  }

  else{
    g4_warning << " Particle name is not valid. " << RandomParticle << endl;
  }
  g4_info << " Primary particle : " << RandomParticle << endl;

  TriggerFunction();

  fParticleGun -> SetParticleDefinition(particle);
  fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
  fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
  fParticleGun -> GeneratePrimaryVertex(anEvent);
}

G4double ATTPCRandomPrimaryGenerate::TriggerFunction()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  G4double windowX = par -> GetParDouble("WindowSize", 0);
  G4double windowZ = par -> GetParDouble("WindowSize", 1);
  G4double WindowHeight = par -> GetParDouble("WindowHeight");
  G4double TriggerDistance = par -> GetParDouble("TriggerDistance");

  TRandom3* REngine = new TRandom3(0);
  
  PositionX = REngine -> Uniform(0, windowX);
  PositionZ = REngine -> Uniform(0, windowZ);

  G4double arctanX1 = ATan(PositionX / (2 * TriggerDistance));
  G4double arctanX2 = ATan((windowX - PositionX) / (2 * TriggerDistance));

  G4double arctanZ1 = ATan(PositionZ / (2 * TriggerDistance));
  G4double arctanZ2 = ATan((windowZ - PositionZ) / (2 * TriggerDistance));

  G4double ThetaAngle = REngine -> Uniform(-arctanZ2, arctanZ1) + Pi()/2;
  G4double PaiAngle = REngine -> Uniform(-arctanX2, arctanX1) + Pi()/2;

  PositionX += 20.;
  PositionY += 0.;
  PositionZ += WindowHeight;

  DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  DirectionZ = Cos(ThetaAngle);
}
  
