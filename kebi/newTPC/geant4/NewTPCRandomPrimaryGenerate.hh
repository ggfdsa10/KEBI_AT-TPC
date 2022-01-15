#ifndef NEWTPCRANDOMPRIMARYGENERATE_HH
#define NEWTPCRANDOMPRIMARYGENERATE_HH

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
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TRandom3.h"
#include <time.h>


class NewTPCRandomPrimaryGenerate : public G4VUserPrimaryGeneratorAction
{
  public:
    NewTPCRandomPrimaryGenerate();    
    virtual ~NewTPCRandomPrimaryGenerate();
    virtual void GeneratePrimaries(G4Event*);         
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  
  private:
    void AlphaScattering();
    void Particle();
    G4ParticleGun*  fParticleGun;


    TVector3 AlphaPos1, AlphaPos2, AlphaPos3;
    TVector3 Alpha1, Alpha2, Alpha3;
    TLorentzVector Alpha1_4Vec, Alpha2_4Vec, Alpha3_4Vec;
    Double_t KEnergy1, KEnergy2, KEnergy3;
    Double_t PositionX, PositionY, PositionZ;
    Double_t DirectionX, DirectionY, DirectionZ;

};

#endif
