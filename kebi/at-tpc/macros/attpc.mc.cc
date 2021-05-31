#include <time.h>
#include "globals.hh"
#include "Randomize.hh"
#include "G4StepLimiterPhysics.hh"

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

int main(int argc, char** argv)
{
  auto runManager = new KBG4RunManager();
  
  G4VModularPhysicsList* physicsList = new ATTPCPhysicsList;
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());

  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new ATTPCDetectorConstruction());
  runManager -> SetUserAction(new ATTPCTrackingAction());
  
  auto par = runManager -> GetParameterContainer();
  G4bool Random = par -> GetParBool("RandomEngine");
  
  if (Random == true) {
    G4Random::setTheSeed(time(0));
    runManager -> SetUserAction(new ATTPCRandomPrimaryGenerate());
  }
  
  runManager -> Initialize();
  runManager -> Run(argc, argv);

  delete runManager;

  return 0;
}
