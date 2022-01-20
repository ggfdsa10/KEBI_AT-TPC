#include "NewTPCPhysicsList.hh"

NewTPCPhysicsList::NewTPCPhysicsList() : G4VModularPhysicsList()
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


NewTPCPhysicsList::~NewTPCPhysicsList()
{
}

void NewTPCPhysicsList::AddParameters()
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
    G4BraggIonModel* BraggIon = new G4BraggIonModel(particle, "BraggionModel");
    G4BetheBlochModel* Bethe = new G4BetheBlochModel(particle, "BetheModel");
    G4IonFluctuations* IonFluc = new G4IonFluctuations();

    config -> SetExtraEmModel("alpha", "ionIoni", BraggIon, "regionworld", 0 *eV, 2*MeV, IonFluc);
    config -> SetExtraEmModel("alpha", "ionIoni", Bethe, "regionworld", 2 *MeV, 100 *GeV, IonFluc);
    config -> SetExtraEmModel("mu-", "muIoni",pai,"regionTPC",10 *eV, 100 *TeV, pai);
    config -> SetExtraEmModel("proton", "hIoni", pai, "regionTPC", 10 *eV, 100 *TeV, pai);
  }
  
}

void NewTPCPhysicsList::SetCuts()
{
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1.3*CLHEP::eV, 10000000 *CLHEP::MeV);
  G4Region* region = G4RegionStore::GetInstance()->GetRegion("regionTPC");
  
  G4ProductionCuts* cuts = new G4ProductionCuts();
  cuts -> SetProductionCut(0. *CLHEP::nm, G4ProductionCuts::GetIndex("e-"));

  region -> SetProductionCuts(cuts);

  DumpCutValuesTable();
  
			   
}

void NewTPCPhysicsList::ConstructParticle()
{
  G4VModularPhysicsList::ConstructParticle();
  
}

void NewTPCPhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
  AddParameters();
}



