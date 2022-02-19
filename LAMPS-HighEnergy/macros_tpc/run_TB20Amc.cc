#include "globals.hh"
#include "KBCompiled.h"

#include "KBG4RunManager.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4PhysListFactory.hh"

#include "KBParameterContainer.hh"

#include "TB20ADetectorConstruction.hh"
#include "KBPrimaryGeneratorAction.hh"

#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

int main(int argc, char** argv)
{
  auto runManager = new KBG4RunManager();

	std::string _PhysList;
	if ( argc==2 ){
		_PhysList = "QGSP_BERT";
		cout << "No PhysList is specfied. Set " << _PhysList << endl;
	}else if ( argc==3 ){
		_PhysList = argv[2];
		cout << "Set " << _PhysList << endl;
	}else{
		cout << "Usage: ./run_TB20Amc.g4sim file.conf PhysList" << endl;
		return 0;
	}


	G4PhysListFactory *physListFactory = new G4PhysListFactory();
	G4VModularPhysicsList* physicsList = physListFactory->GetReferencePhysList(_PhysList.c_str());
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());
  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new TB20ADetectorConstruction());
  runManager -> Initialize();
  runManager -> Run(argc, argv);

  delete runManager;

  return 0;
}
