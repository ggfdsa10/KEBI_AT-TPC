#ifndef NEWTPCMODULARPHYSICSLIST_HH
#define NEWTPCMODULARPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"
#include "KBG4RunManager.hh"
#include "QGSP_BERT_HP.hh"
#include "G4PAIPhotModel.hh"
#include "G4PAIModel.hh"
#include "G4BraggIonModel.hh"
#include "G4BetheBlochModel.hh"
#include "G4IonFluctuations.hh"
#include "G4EmParameters.hh"
#include "G4EmConfigurator.hh"
#include "G4LossTableManager.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCutsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"


class NewTPCPhysicsList : public G4VModularPhysicsList
{
  public:
    NewTPCPhysicsList();
    virtual~NewTPCPhysicsList();
    virtual void SetCuts();
    virtual void ConstructParticle();
    virtual void ConstructProcess();
  
  protected:
    void AddParameters();
  

};

#endif
