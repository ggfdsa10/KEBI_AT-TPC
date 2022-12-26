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
#include <time.h>


class ATTPCRandomPrimaryGenerate : public G4VUserPrimaryGeneratorAction
{
  public:
    ATTPCRandomPrimaryGenerate();    
    virtual ~ATTPCRandomPrimaryGenerate();
    virtual void GeneratePrimaries(G4Event*);         
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    G4int GetNumberOfPrimary()  {return fNumberOfPrimary; } 
  
  private:
    void ExternalTriggerPMT(G4Event* event);
    void ProtonBeam(G4Event* event);

    TRandom3* fRandom = nullptr;
    KBG4RunManager* fRunManager = nullptr;
    KBParameterContainer* fPar = nullptr;
    G4ParticleDefinition* fParticle = nullptr;
    G4ParticleTable* fParticleTable = nullptr;
    G4IonTable* fIontable = nullptr;

    G4ParticleGun*  fParticleGun;

    Double_t PositionX, PositionY, PositionZ;
    Double_t DirectionX, DirectionY, DirectionZ;

    G4int fNumberOfPrimary = 0;
};

#endif
