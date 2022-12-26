#include "ATTPCPhysicsList.hh"

//general process model
#include "G4hMultipleScattering.hh"
#include "G4NuclearStopping.hh"
#include "G4PAIModel.hh"
#include "G4PAIPhotModel.hh"
#include "G4hBremsstrahlung.hh"
#include "G4WentzelVIModel.hh"
#include "G4CoulombScattering.hh"
#include "G4ComptonScattering.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4hIonisation.hh"
#include "G4hPairProduction.hh"
#include "G4hCoulombScatteringModel.hh"
#include "G4UniversalFluctuation.hh"

// step and track limiter process
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

// particle constructors
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

// process model for gamma
#include "G4PhotoElectricEffect.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4GammaConversion.hh"
#include "G4LivermoreGammaConversionModel.hh"
#include "G4RayleighScattering.hh"

// EM LiveMore model for e-, e+
#include "G4eMultipleScattering.hh"
#include "G4eCoulombScatteringModel.hh"
#include "G4eIonisation.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4LivermoreBremsstrahlungModel.hh"
#include "G4eBremsstrahlung.hh"
#include "G4UrbanMscModel.hh"
#include "G4eplusAnnihilation.hh"

// ionization process model for muon
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuMultipleScattering.hh"

// ionization process model for generic ions 
#include "G4ionIonisation.hh"
#include "G4BraggIonModel.hh"
#include "G4BetheBlochModel.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4IonCoulombScatteringModel.hh"


ATTPCPhysicsList::ATTPCPhysicsList() : G4VModularPhysicsList()
{
  G4int verb =1;
  SetVerboseLevel(verb);
  defaultCutValue = 0.1 *CLHEP::nm;
  QGSP_BERT_HP* physics = new QGSP_BERT_HP;

  for(G4int i =1;;++i) {
    G4VPhysicsConstructor* elem = const_cast<G4VPhysicsConstructor*>(physics->GetPhysics(i));
    if (elem == NULL) break;
    RegisterPhysics(elem);
  }
}

ATTPCPhysicsList::~ATTPCPhysicsList()
{
}

void ATTPCPhysicsList::ConstructParticle()
{
  G4BosonConstructor bosonConstructor;
  bosonConstructor.ConstructParticle();

  G4LeptonConstructor leptonConstructor;
  leptonConstructor.ConstructParticle();

  G4BaryonConstructor baryonConstructor;
  baryonConstructor.ConstructParticle();

  G4IonConstructor ionConstructor;
  ionConstructor.ConstructParticle();
}

void ATTPCPhysicsList::ConstructProcess()
{
  AddTransportation();
  AddProcessParameter();
  AddLimiterProcess();
}

void ATTPCPhysicsList::SetCuts()
{
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1.*CLHEP::eV, 10000000 *CLHEP::MeV);
  G4Region* region = G4RegionStore::GetInstance()->GetRegion("regionTPC");

  G4ProductionCuts* cuts = new G4ProductionCuts();
  cuts -> SetProductionCut(0.1 *CLHEP::nm, G4ProductionCuts::GetIndex("e-"));

  region -> SetProductionCuts(cuts);
}

void ATTPCPhysicsList::AddProcessParameter()
{
  G4MuBremsstrahlung* mub = new G4MuBremsstrahlung();
  G4MuPairProduction* mup = new G4MuPairProduction();
  G4hBremsstrahlung* pb = new G4hBremsstrahlung();
  G4hPairProduction* pp = new G4hPairProduction();

  G4MuMultipleScattering* mumsc = new G4MuMultipleScattering();
  mumsc->AddEmModel(0, new G4WentzelVIModel());

  G4hMultipleScattering* pmsc = new G4hMultipleScattering();
  G4hMultipleScattering* ionmsc = new G4hMultipleScattering("ionmsc");

  G4CoulombScattering* pcsc = new G4CoulombScattering();
  pcsc->AddEmModel(0, new G4hCoulombScatteringModel());
  G4CoulombScattering* ioncsc = new G4CoulombScattering();
  ioncsc->AddEmModel(0, new G4IonCoulombScatteringModel());

  G4NuclearStopping* ionnuc = new G4NuclearStopping();
  G4NuclearStopping* pnuc = new G4NuclearStopping();

  G4PAIModel* pai = new G4PAIModel();
  G4Region* region = G4RegionStore::GetInstance()->GetRegion("regionTPC");

  G4double highEnergyLimit = 100.*MeV;
  G4double LivermoreHighEnergyLimit = GeV;

  auto ph = G4PhysicsListHelper::GetPhysicsListHelper();
  auto pIterator = GetParticleIterator();
  pIterator->reset();

  while((*pIterator)()){
    G4ParticleDefinition *particle = pIterator->value();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {
      G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
      G4LivermorePhotoElectricModel* theLivermorePhotoElectricModel = new G4LivermorePhotoElectricModel();
      thePhotoElectricEffect->SetEmModel(theLivermorePhotoElectricModel);
      ph->RegisterProcess(thePhotoElectricEffect, particle);

      G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
      G4LivermoreComptonModel* theLivermoreComptonModel = new G4LivermoreComptonModel();
      theLivermoreComptonModel->SetHighEnergyLimit(LivermoreHighEnergyLimit);
      theComptonScattering->SetEmModel(theLivermoreComptonModel, 1);
      ph->RegisterProcess(theComptonScattering, particle);

      G4GammaConversion* theGammaConversion = new G4GammaConversion();
      G4VEmModel* theLivermoreGammaConversionModel = new G4LivermoreGammaConversionModel();
      theGammaConversion->SetEmModel(theLivermoreGammaConversionModel, 1);
      ph->RegisterProcess(theGammaConversion, particle);

      G4RayleighScattering* theRayleigh = new G4RayleighScattering();
      ph->RegisterProcess(theRayleigh, particle);
    }
                
    if (particleName == "e-"){
      G4eMultipleScattering* msc = new G4eMultipleScattering;
      msc->SetStepLimitType(fUseDistanceToBoundary);
      G4UrbanMscModel* msc1 = new G4UrbanMscModel();
      G4WentzelVIModel* msc2 = new G4WentzelVIModel();
      msc1->SetHighEnergyLimit(highEnergyLimit);
      msc2->SetLowEnergyLimit(highEnergyLimit);
//       msc->SetRangeFactor(0.01); // this function for GEANT4 10.6 version
      msc->AddEmModel(0, msc1);
      msc->AddEmModel(0, msc2);

      G4CoulombScattering* ss = new G4CoulombScattering();
      G4eCoulombScatteringModel* ssm = new G4eCoulombScatteringModel(); 
      ss->SetEmModel(ssm, 1); 
      ss->SetMinKinEnergy(highEnergyLimit);
      ssm->SetLowEnergyLimit(highEnergyLimit);
      ssm->SetActivationLowEnergyLimit(highEnergyLimit);
  
      G4eIonisation* eIoni = new G4eIonisation();
      G4LivermoreIonisationModel* theIoniLivermore = new G4LivermoreIonisationModel();
      theIoniLivermore->SetHighEnergyLimit(0.1*MeV); 
      eIoni->AddEmModel(0, theIoniLivermore, new G4UniversalFluctuation() );
      eIoni->SetStepFunction(0.2, 100*um); //     
  
      G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
      G4VEmModel* theBremLivermore = new G4LivermoreBremsstrahlungModel();
      theBremLivermore->SetHighEnergyLimit(1*GeV);
      eBrem->SetEmModel(theBremLivermore,1);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(eIoni, particle);
      ph->RegisterProcess(eBrem, particle);
      ph->RegisterProcess(ss, particle);
    }

    else if (particleName == "e+") {
      G4eMultipleScattering* msc = new G4eMultipleScattering;
      msc->SetStepLimitType(fUseDistanceToBoundary);

      G4UrbanMscModel* msc1 = new G4UrbanMscModel();
      G4WentzelVIModel* msc2 = new G4WentzelVIModel();
      msc1->SetHighEnergyLimit(highEnergyLimit);
      msc2->SetLowEnergyLimit(highEnergyLimit);
//       msc->SetRangeFactor(0.01); // this function for GEANT4 10.6 version
      msc->AddEmModel(0, msc1);
      msc->AddEmModel(0, msc2);

      G4CoulombScattering* ss = new G4CoulombScattering();
      G4eCoulombScatteringModel* ssm = new G4eCoulombScatteringModel(); 
      ss->SetEmModel(ssm, 1); 
      ss->SetMinKinEnergy(highEnergyLimit);
      ssm->SetLowEnergyLimit(highEnergyLimit);
      ssm->SetActivationLowEnergyLimit(highEnergyLimit);

      G4eIonisation* eIoni = new G4eIonisation();
      eIoni->SetStepFunction(0.2, 100*um);      

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(eIoni, particle);
      ph->RegisterProcess(new G4eBremsstrahlung(), particle);
      ph->RegisterProcess(new G4eplusAnnihilation(), particle);
      ph->RegisterProcess(ss, particle);
    }

    else if(particleName == "mu+" || particleName == "mu-"){
      G4MuIonisation* muIoni = new G4MuIonisation();
      muIoni->AddEmModel(0, pai, pai, region);
      muIoni->SetStepFunction(0.2, 50*um);          

      ph->RegisterProcess(mumsc, particle);
      ph->RegisterProcess(muIoni, particle);
      ph->RegisterProcess(mub, particle);
      ph->RegisterProcess(mup, particle);
    }
    else if (particleName == "proton" || particleName == "anti_proton") {
      G4hIonisation* hIoni = new G4hIonisation();
      hIoni->AddEmModel(0, pai, pai, region);
      hIoni->SetStepFunction(0.2, 50*um);

      ph->RegisterProcess(pmsc, particle);
      ph->RegisterProcess(hIoni, particle);
      ph->RegisterProcess(pb, particle);
      ph->RegisterProcess(pp, particle);
      ph->RegisterProcess(pnuc, particle);
      ph->RegisterProcess(pcsc, particle);
    }

    else if(particleName == "GenericIon"){
      G4ionIonisation* ionIoni = new G4ionIonisation();
      ionIoni->SetEmModel(new G4IonParametrisedLossModel());
      ionIoni->SetStepFunction(0.1, 1*um);

      ph->RegisterProcess(ionmsc, particle);
      ph->RegisterProcess(ionIoni, particle);
      ph->RegisterProcess(ionnuc, particle);
      ph->RegisterProcess(ioncsc, particle);
    }

    else if(particleName == "alpha" || particleName == "He3"){
      G4hMultipleScattering* msc = new G4hMultipleScattering();

      G4BraggIonModel* BraggIonModel = new G4BraggIonModel();
      BraggIonModel->SetActivationHighEnergyLimit(2.*MeV);
      G4BetheBlochModel* BetheModel = new G4BetheBlochModel();
      BetheModel->SetActivationLowEnergyLimit(2.*MeV);

      G4ionIonisation* ionIoni = new G4ionIonisation();
      ionIoni->AddEmModel(0, BraggIonModel, new G4IonFluctuations());
      ionIoni->AddEmModel(0, BetheModel, new G4IonFluctuations());
      ionIoni->SetStepFunction(0.1, 10*um);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(ionIoni, particle);
      ph->RegisterProcess(ionnuc, particle);
      ph->RegisterProcess(ioncsc, particle);
    }
  }
}

void ATTPCPhysicsList::AddLimiterProcess()
{
  auto ph = G4PhysicsListHelper::GetPhysicsListHelper();
  auto pIterator = GetParticleIterator();
  pIterator->reset();
  G4StepLimiter *slt = new G4StepLimiter();
  G4UserSpecialCuts *usc = new G4UserSpecialCuts();

  while((*pIterator)()){
    G4ParticleDefinition *particle = pIterator->value();
    G4String particleName = particle->GetParticleName();
    if(particleName=="e-"){
      ph->RegisterProcess(slt, particle);
      ph->RegisterProcess(usc, particle);
    }
  }
}

