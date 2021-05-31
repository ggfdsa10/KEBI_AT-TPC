#ifndef ATTPCRANDOMPRIMARYGENERATE_HH
#define ATTPCRANDOMPRIMARYGENERATE_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "KBG4RunManager.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "TMath.h"
#include "TRandom.h"


class ATTPCRandomPrimaryGenerate : public G4VUserPrimaryGeneratorAction
{
  public:
    ATTPCRandomPrimaryGenerate();    
    virtual ~ATTPCRandomPrimaryGenerate();
    virtual void GeneratePrimaries(G4Event*);         
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  
  private:
    G4double TriggerFunction(G4double x, G4double y);
    G4ParticleGun*  fParticleGun;

  //auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  //auto par = runManager -> GetParameterContainer();
};

#endif
