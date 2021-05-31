
#include "ATTPCRandomPrimaryGenerate.hh"

using namespace std;

ATTPCRandomPrimaryGenerate::ATTPCRandomPrimaryGenerate() : G4VUserPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable -> FindParticle(particleName = "mu-");

  fParticleGun -> SetParticleDefinition(particle);
  fParticleGun -> SetParticlePosition(G4ThreeVector(5.15*cm, 0., 7.5*cm));
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(0., 1., 0.));
  fParticleGun -> SetParticleEnergy(4.*GeV);
}

ATTPCRandomPrimaryGenerate::~ATTPCRandomPrimaryGenerate()
{
  delete fParticleGun;
}

void ATTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{
  fParticleGun -> GeneratePrimaryVertex(anEvent);
}

G4double ATTPCRandomPrimaryGenerate::TriggerFunction(G4double x, G4double y)
{
  /*
  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");
  G4double windowX = par -> GetParDouble("window", 0);
  G4double windowZ = par -> GetParDouble("window", 1);

  G4double arctanX1 = TMath::ATan((abs(x)+windowX)/tpcY);
  G4double arctanX2 = TMath::ATan(windowX-(abs(x))/tpcY);

  G4double arctanZ1 = TMath::ATan(
    
  */

}

