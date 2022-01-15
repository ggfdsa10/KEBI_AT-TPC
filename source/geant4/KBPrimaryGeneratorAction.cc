#include "KBPrimaryGeneratorAction.hh"
#include "KBG4RunManager.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include <G4strstreambuf.hh>

KBPrimaryGeneratorAction::KBPrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun();
}

KBPrimaryGeneratorAction::KBPrimaryGeneratorAction(const char *fileName)
{
  fParticleGun = new G4ParticleGun();
  fEventGenerator = new KBMCEventGenerator(fileName);
  fReadMomentumOrEnergy = fEventGenerator -> ReadMomentumOrEnergy();
}

KBPrimaryGeneratorAction::~KBPrimaryGeneratorAction()
{
  delete fParticleGun;
}

void KBPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4int pdg;
  G4double vx, vy, vz, px, py, pz;

  fEventGenerator -> ReadNextEvent(vx, vy, vz);

  fParticleGun -> SetParticlePosition(G4ThreeVector(vx,vy,vz));

  while (fEventGenerator -> ReadNextTrack(pdg, px, py, pz))
  {
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable() -> FindParticle(pdg);
    fParticleGun -> SetParticleDefinition(particle);

    G4ThreeVector momentum(px,py,pz);
    fParticleGun -> SetParticleMomentumDirection(momentum.unit());

    G4strstreambuf* oldBuffer = dynamic_cast<G4strstreambuf*>(G4cout.rdbuf(0));
    // Removing print outs in between here ------------->
    if (fReadMomentumOrEnergy) fParticleGun -> SetParticleMomentum(momentum.mag()*MeV);
    else                       fParticleGun -> SetParticleEnergy(momentum.mag()*MeV);
    // <------------- to here
    G4cout.rdbuf(oldBuffer);

    fParticleGun -> GeneratePrimaryVertex(anEvent);
  }
}

void KBPrimaryGeneratorAction::SetEventGenerator(const char *fileName)
{
  fEventGenerator = new KBMCEventGenerator(fileName);
  fReadMomentumOrEnergy = fEventGenerator -> ReadMomentumOrEnergy();
  ((KBG4RunManager *) KBG4RunManager::GetRunManager()) -> SetNumEvents(fEventGenerator -> GetNumEvents());
}
