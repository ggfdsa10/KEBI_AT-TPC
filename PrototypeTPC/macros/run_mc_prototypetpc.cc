//#include "globals.hh"
#include "KBCompiled.h"

#include "KBG4RunManager.hh"
#include "G4StepLimiterPhysics.hh"

#include "KBParameterContainer.hh"

#include "QGSP_BERT.hh"
#include "LAPDetectorConstruction.hh"
#include "KBPrimaryGeneratorAction.hh"

#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

int main(int argc, char** argv)
{
  auto runManager = new KBG4RunManager();

  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());
  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new LAPDetectorConstruction());
  runManager -> Initialize();
  runManager -> Run(argc, argv);

  delete runManager;

  return 0;
}
