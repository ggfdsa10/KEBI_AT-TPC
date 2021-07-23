#include "ATTPCPhysicsList.hh"

ATTPCPhysicsList::ATTPCPhysicsList() : G4VModularPhysicsList()
{
  G4int verb =0;
  SetVerboseLevel(verb);
  defaultCutValue = 0. *CLHEP::nm;
  QGSP_BERT_HP* physics = new QGSP_BERT_HP;

  for(G4int i =0;;++i) {
    G4VPhysicsConstructor* elem = const_cast<G4VPhysicsConstructor*>(physics->GetPhysics(i));

    if (elem == NULL) break;
    RegisterPhysics(elem);
  }
}


ATTPCPhysicsList::~ATTPCPhysicsList()
{
}

void ATTPCPhysicsList::AddParameters()
{
  auto theParticleIterator = GetParticleIterator();
  theParticleIterator -> reset();
  
  while((*theParticleIterator)()){
    G4ParticleDefinition* particle = theParticleIterator->value();
    
    G4EmConfigurator* config = G4LossTableManager::Instance()->EmConfigurator();
    G4LossTableManager::Instance()->SetVerbose(1);

    
    G4String particleName = particle -> GetParticleName();

    
    G4PAIModel* pai = new G4PAIModel(particle, "PAIModel");
    G4PAIPhotModel* paiphot = new G4PAIPhotModel(particle, "G4PAIModel");
    
    //config -> SetExtraEmModel("alpha", "ionIoni", pai, "regionTPC", 10 *eV, 100 *TeV, pai);
    config -> SetExtraEmModel("mu-", "muIoni",pai,"regionTPC",10 *eV, 100 *TeV, pai);
    config -> SetExtraEmModel("proton", "hIoni", pai, "regionTPC", 10 *eV, 100 *TeV, pai);
    //config -> SetExtraEmModel("e-", "eIoni", pai, "regionTPC", 26 *eV, 100 *TeV, pai);
  }
  
}

void ATTPCPhysicsList::SetCuts()
{
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1.3*CLHEP::eV, 10000000 *CLHEP::MeV);
  G4Region* region = G4RegionStore::GetInstance()->GetRegion("regionTPC");
  
  G4ProductionCuts* cuts = new G4ProductionCuts();
  cuts -> SetProductionCut(0. *CLHEP::nm, G4ProductionCuts::GetIndex("e-"));

  region -> SetProductionCuts(cuts);

  DumpCutValuesTable();
  
			   
}

void ATTPCPhysicsList::ConstructParticle()
{
  G4VModularPhysicsList::ConstructParticle();
  
}

void ATTPCPhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
  AddParameters();
}



