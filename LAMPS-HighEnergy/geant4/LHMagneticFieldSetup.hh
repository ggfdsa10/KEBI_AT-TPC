#ifndef LHMAGNETICFIELDSETUP_HH
#define LHMAGNETICFIELDSETUP_HH

class G4ChordFinder;
class G4FieldManager;
class G4MagneticField;
class G4MagIntegratorStepper;
class G4Mag_UsualEqRhs;

class LHMagneticFieldSetup
{
	public:

		LHMagneticFieldSetup();
		virtual ~LHMagneticFieldSetup();

		//Setters
		void SetDeltaChord(double val) { fDeltaChord = val; } //Default: 0.25 (delta_chord = miss distance)
		void SetDeltaIntersection(double val) { fDeltaIntersection = val; } //Default: 1.e-3 
		void SetDeltaOneStep(double val) { fDeltaOneStep = val; } //Default: 1.e-2
		void SetEpsilonMax(double max) { fEpsilonMax = max; } //Default: 1.e-3
		void SetEpsilonMin(double min) { fEpsilonMin = min; } //Default: 5.e-5

		void SetStepperType(int type) { fStepperType = type; } //Default: ClassicalRK4 (index 1)
		void SetStepMin(double step) { fStepMin = step; } //Use CLHEP units: ex. 10 * mm

		//Setter/Getter for field manager
		void SetFieldManager(G4FieldManager* fieldManager) { fFieldManager = fieldManager; }
		G4FieldManager* GetFieldManager() { return fFieldManager; }

		//Main
		void MakeStepper(int type); 
		void MakeSetup(G4MagneticField* fField);

	protected:

		G4ChordFinder*          fChordFinder;
		G4FieldManager*         fFieldManager;
		G4MagIntegratorStepper* fStepper;
		G4Mag_UsualEqRhs*       fStepEq;

		int    fStepperType;
		double fDeltaChord;
		double fDeltaIntersection;
		double fDeltaOneStep;
		double fEpsilonMax;
		double fEpsilonMin;
		double fStepMin;
};

#endif //LHMAGNETICFIELDSETUP_HH
