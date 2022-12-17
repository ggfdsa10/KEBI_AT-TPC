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

    void SetInteractionPoint(TVector3 pos, TVector3 mom);
    Double_t GetInteractionEnergy(){return InteractionEnergy;}

    G4ParticleGun*  fParticleGun;

  private:
    void Particle();
    void HoyleState(G4Event* event);

    TRandom3* fRandom = nullptr;
    KBG4RunManager* fRunManager = nullptr;
    KBParameterContainer* fPar = nullptr;
    G4ParticleDefinition* fParticle = nullptr;
    G4ParticleTable* fParticleTable = nullptr;
    G4IonTable* fIontable = nullptr;

    Double_t PositionX, PositionY, PositionZ;
    Double_t DirectionX, DirectionY, DirectionZ;
    Double_t InteractionEnergy = 0;

    TVector3 fInteractionPos;
    TVector3 fInteractionMom;
};

#endif
