#include "LHMagneticFieldSetup.hh"

#include "G4AutoDelete.hh"
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"

//Stepper methods in alphabetical order
#include "G4CashKarpRKF45.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4ImplicitEuler.hh"
#include "G4RKG3_Stepper.hh"
#include "G4SimpleRunge.hh"
#include "G4SimpleHeum.hh"

#include <cmath>
#include <iostream>
using namespace std;

LHMagneticFieldSetup::LHMagneticFieldSetup():
	fChordFinder(0),
	fFieldManager(0),
	fStepper(0),
	fStepEq(0),
	fStepperType(-1),
	fDeltaChord(NAN),
	fDeltaIntersection(NAN),
	fDeltaOneStep(NAN),
	fEpsilonMax(NAN),
	fEpsilonMin(NAN),
	fStepMin(NAN)
{
	fFieldManager = new G4FieldManager();
}

LHMagneticFieldSetup::~LHMagneticFieldSetup()
{
	G4AutoDelete::Register(fChordFinder);
	G4AutoDelete::Register(fFieldManager);
	G4AutoDelete::Register(fStepper);
	G4AutoDelete::Register(fStepEq);
}

//-------------------------------------------------
void LHMagneticFieldSetup::MakeStepper(int type)
{
	if      (type==0) { fStepper = new G4CashKarpRKF45(fStepEq); cout <<"Stepper: G4CashKarpRKF45\n"; }
	else if (type==1) { fStepper = new G4ClassicalRK4(fStepEq); cout <<"Stepper: G4ClassicalRK4\n"; }
	else if (type==2) { fStepper = new G4ExplicitEuler(fStepEq); cout <<"Stepper: G4ExplicitEuler\n"; }
	else if (type==3) { fStepper = new G4HelixExplicitEuler(fStepEq); cout <<"Stepper: G4HelixExplicitEuler\n"; }
	else if (type==4) {	fStepper = new G4HelixImplicitEuler(fStepEq); cout <<"Stepper: G4HelixImplicitEuler\n"; }
	else if (type==5) { fStepper = new G4HelixSimpleRunge(fStepEq); cout <<"Stepper: G4HelixSimpleRunge\n"; }
	else if (type==6) {	fStepper = new G4ImplicitEuler(fStepEq); cout <<"Stepper: G4ImplicitEuler\n"; }
	else if (type==7) { fStepper = new G4RKG3_Stepper(fStepEq); cout <<"Stepper:G4RKG3_Stepper\n"; }
	else if (type==8) { fStepper = new G4SimpleRunge(fStepEq); cout <<"Stepper:G4SimpleRunge\n"; }
	else if (type==9) { fStepper = new G4SimpleHeum(fStepEq); cout <<"Stepper: G4SimpleHeum\n"; }
	else cout <<"Unknown stepper type invoked!" <<endl;

	return;
}//MakeStepper

//--------------------------------------------------------------
void LHMagneticFieldSetup::MakeSetup(G4MagneticField* fField)
{
	if ( !isnan(fStepMin) ) cout <<"LHMagneticFieldSetup::MakeSetup - min step: " <<fStepMin <<endl;
	else
	{
		cout <<"LHMagneticFieldSetup::MakeSetup - setting default min step (1 mm)... \n";
		fStepMin = 1 * mm;
	}
	if ( fStepperType<0 )
	{
		cout <<"LHMagneticFieldSetup::MakeSetup - setting default stepper... ";
		fStepperType = 1;
	}

	fStepEq = new G4Mag_UsualEqRhs(fField);
	MakeStepper(fStepperType);

	fChordFinder = new G4ChordFinder(fField, fStepMin, fStepper);
	if ( !isnan(fDeltaChord) ) fChordFinder->SetDeltaChord(fDeltaChord);

	fFieldManager->SetChordFinder(fChordFinder);
	if ( !isnan(fDeltaIntersection) ) fFieldManager->SetDeltaIntersection(fDeltaIntersection);
	if ( !isnan(fDeltaOneStep) )      fFieldManager->SetDeltaOneStep(fDeltaOneStep);
	if ( !isnan(fEpsilonMax) )        fFieldManager->SetMaximumEpsilonStep(fEpsilonMax);
	if ( !isnan(fEpsilonMin) )        fFieldManager->SetMinimumEpsilonStep(fEpsilonMin);
	fFieldManager->SetDetectorField(fField);

	/*
	cout <<fChordFinder->GetDeltaChord() <<endl;
	cout <<fFieldManager->GetDeltaIntersection() <<endl;
	cout <<fFieldManager->GetDeltaOneStep() <<endl;
	cout <<fFieldManager->GetMaximumEpsilonStep() <<endl;
	cout <<fFieldManager->GetMinimumEpsilonStep() <<endl;
	*/

	return;
}//MakeSetup
