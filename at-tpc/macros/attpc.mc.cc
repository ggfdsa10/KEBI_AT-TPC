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

#include "ATTPC.hh"
#include "ATTPCDetectorConstruction.hh"
#include "ATTPCPhysicsList.hh"
#include "ATTPCRandomPrimaryGenerate.hh"
#include "ATTPCTrackingAction.hh"
#include "ATTPCSteppingAction.hh"

int main(int argc, char** argv)
{
  G4Random::setTheSeed(time(0));
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  auto runManager = new KBG4RunManager();

  G4VModularPhysicsList* physicsList = new ATTPCPhysicsList;
  G4FastSimulationPhysics* fastsimPhysics = new G4FastSimulationPhysics();
  fastsimPhysics->ActivateFastSimulation("e-");
  physicsList->RegisterPhysics(fastsimPhysics);
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());

  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new ATTPCDetectorConstruction());
  runManager -> SetUserAction(new ATTPCTrackingAction());
  runManager -> SetUserAction(new ATTPCSteppingAction());
  
  auto par = runManager -> GetParameterContainer();
  TString EventNum = par -> GetParString("Event");
  G4bool Random = par -> GetParBool("RandomEngine");

  if (Random == true) { 
    runManager -> SetUserAction(new ATTPCRandomPrimaryGenerate());
  }
  
  runManager -> Initialize();

  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  uiManager -> ApplyCommand("/run/suppressPP true");

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