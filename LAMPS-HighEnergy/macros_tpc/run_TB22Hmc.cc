#include "globals.hh"
#include "G4PhysListFactory.hh"
#include "G4StepLimiterPhysics.hh"

#include "KBCompiled.h"
#include "KBEventAction.hh"
#include "KBG4RunManager.hh"
#include "KBParameterContainer.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "KBSteppingAction.hh"
#include "KBTrackingAction.hh"

#include "TB22HDetectorConstruction.hh"

int main(int argc, char** argv)
{
	std::string physListStr;
	if      (argc == 2) physListStr = "QGSP_BERT";
	else if (argc == 3) physListStr = argv[2];
	else
	{
		cout <<"Usage: ./run_Tb22Hmc.g4sim blah.conf G4PhysList\n";
		assert(false);
	}

	G4PhysListFactory* physListFac = new G4PhysListFactory();
	G4VModularPhysicsList* physList = physListFac->GetReferencePhysList(physListStr.c_str());
	physList->RegisterPhysics(new G4StepLimiterPhysics());

	auto runManager = new KBG4RunManager();
	runManager->SetParameterContainer(argv[1]);
	runManager->SetUserInitialization(physList);
	runManager->SetUserInitialization(new TB22HDetectorConstruction());
	runManager->Initialize();
	runManager->Run(argc, argv);
	delete runManager;
	return 0;
}//Main
