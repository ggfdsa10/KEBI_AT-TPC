#include <time.h>
#include "globals.hh"
#include "Randomize.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4FastSimulationPhysics.hh"
#include "G4UImanager.hh"

#include "KBCompiled.h"
#include "KBParameterContainer.hh"
#include "KBG4RunManager.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

#include "NewTPC.hh"
#include "NewTPCDetectorConstruction.hh"
#include "NewTPCPhysicsList.hh"
#include "NewTPCRandomPrimaryGenerate.hh"
#include "NewTPCEventAction.hh"
#include "NewTPCTrackingAction.hh"
#include "NewTPCSteppingAction.hh"

int main(int argc, char** argv)
{
  G4Random::setTheSeed(time(0));
  auto runManager = new KBG4RunManager();

  G4VModularPhysicsList* physicsList = new NewTPCPhysicsList;
  G4FastSimulationPhysics* fastsimPhysics = new G4FastSimulationPhysics();
  fastsimPhysics->ActivateFastSimulation("e-");
  physicsList->RegisterPhysics(fastsimPhysics);
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());

  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new NewTPCDetectorConstruction());
  runManager -> SetUserAction(new NewTPCEventAction());
  runManager -> SetUserAction(new NewTPCTrackingAction());
  runManager -> SetUserAction(new NewTPCSteppingAction());

  auto par = runManager -> GetParameterContainer();
  TString EventNum = par -> GetParString("Event");
  G4bool Random = par -> GetParBool("RandomEngine");

  if (Random == true) { 
    runManager -> SetUserAction(new NewTPCRandomPrimaryGenerate());
  }
  
  runManager -> Initialize();

  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  uiManager -> ApplyCommand("/run/suppressPP true");

  bool isPhysics = par -> GetParBool("Physics");
  if(isPhysics == true){
    TString physicsName = par -> GetParString("PhysicsType");
    int eventnum_i = 2*EventNum.Atoi();
    EventNum = TString::Itoa(eventnum_i, 10);
  }
  
  if(Random == true) { 
    uiManager -> ApplyCommand("/run/beamOn "+EventNum);
  }
  if(Random == false){
    uiManager -> ApplyCommand("/run/beamOnAll");
  }
  
  runManager -> Run(argc, argv);

  delete runManager;

  return 0;
}