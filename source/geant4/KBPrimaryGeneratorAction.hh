#ifndef KBPRIMARYGENERATORACTION_HH
#define KBPRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "KBMCEventGenerator.hh"

class KBPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    KBPrimaryGeneratorAction();
    KBPrimaryGeneratorAction(const char *fileName);
    virtual ~KBPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);         

		void GeneratePrimariesMode0(G4Event*);         
		void GeneratePrimariesMode1(G4Event*);         
  
    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    void SetEventGenerator(const char *fileName);
  
  private:
    G4ParticleGun* fParticleGun;
    KBMCEventGenerator *fEventGenerator;
    bool fReadMomentumOrEnergy;
};

#endif