#ifndef ATTPCRANDOMPRIMARYGENERATE_HH
#define ATTPCRANDOMPRIMARYGENERATE_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "KBG4RunManager.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include "globals.hh"

#include "TMath.h"
#include "TRandom3.h"


class ATTPCRandomPrimaryGenerate : public G4VUserPrimaryGeneratorAction
{
  public:
    ATTPCRandomPrimaryGenerate();    
    virtual ~ATTPCRandomPrimaryGenerate();
    virtual void GeneratePrimaries(G4Event*);         
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  
  private:
    G4double TriggerFunction();
    G4ParticleGun*  fParticleGun;
    G4double PositionX = 0.;
    G4double PositionY = 0.;
    G4double PositionZ = 0.;
    G4double DirectionX = 0.;
    G4double DirectionY = 0.;
    G4double DirectionZ = 0.;
};

#endif
