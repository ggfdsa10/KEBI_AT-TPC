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
    void TriggerFunction();
    void ProtonBeam(int eventID, int trackNum);
    void AlphaScattering(int eventID, int trackNum);
    G4ParticleGun*  fParticleGun;
    G4double ParticleEnergy = 0.;
    G4double PositionX = 0.;
    G4double PositionY = 0.;
    G4double PositionZ = 0.;
    G4double DirectionX = 0.;
    G4double DirectionY = 0.;
    G4double DirectionZ = 0.;
    G4double PaiAngle, ThetaAngle;

    G4int fNumberOfPrimary = 0;
};

#endif
