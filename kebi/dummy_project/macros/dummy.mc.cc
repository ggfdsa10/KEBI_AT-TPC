#include "globals.hh"
#include "KBCompiled.h"
#include "KBParameterContainer.hh"
#include "QGSP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "KBG4RunManager.hh"
#include "DUMMYDetectorConstruction.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

/**
 * How to run geant4 simulation
 *
 * - The least change user should make is rewritting Construct() of DUMMYDetectorConstruction.
 *   The class source should be inside geant4/ of the project folder by default.
 * - The primary tracks are created by input generator file.
 *   The input generator file is set by G4InputFile describe below.
 *   Refere to KBMCEventGenerator for the format of generator file.
 *   (http://nuclear.korea.ac.kr/~lamps/kebi/classKBMCEventGenerator.html)
 *
 * - The physics list may be replaced by user list.
 * - The PrimaryGeneratorAction, EventAction, TrackingAction, KBSteppingAction
 *   are automatically added to run manager if KBG4RunaManager is used
 *   unless such user class is added to run manager.
 *   They may be replaced by user actions but the data handling has to be done again.
 *   This is not recommanded since it is same as writting geant4 from the scratch,
 *   but if one intend to do it, refere to KBG4RunManager
 *   (http://nuclear.korea.ac.kr/~lamps/kebi/classKBG4RunManager.html).
 *
 * - The parameter file is essential for running simulation ex) dummy.par
 * - In the parameter file, 5 parameters can be used for simulation.
 *   (Note, one should use only one of G4VisFile and G4MacroFile for the geant4 macro)
 *
 *   1) G4VisFile:         Geant4 macro file name for event display.
 *   2) G4MacroFile:       Geant4 macro file name file.
 *   3) G4InputFile:       Input generator file name for the primary tracks.
 *   4) G4OutputFile:      Output file name of the simulation
 *   5) MCStepPersistency: true/false for the write flag of step information.
 *
 *   Refere KBParameterContainer for the format of parameter file.
 *   (http://nuclear.korea.ac.kr/~lamps/kebi/classKBParameterContainer.html)
 *
 * - Run simulation: ./dummy.mc.g4sim dummy.par
 */

int main(int argc, char** argv)
{
  auto runManager = new KBG4RunManager();

  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());

  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new DUMMYDetectorConstruction());
  runManager -> Initialize();
  runManager -> Run(argc, argv);

  return 0;
}
