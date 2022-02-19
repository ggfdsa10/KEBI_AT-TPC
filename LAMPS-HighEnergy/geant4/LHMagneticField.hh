#ifndef LHMAGNETICFIELD_HH
#define LHMAGNETICFIELD_HH

#include "G4MagneticField.hh"
#include "globals.hh"

class TH3D;

class LHMagneticField : public G4MagneticField
{
	public:

		LHMagneticField();
		virtual ~LHMagneticField();
		void SetVerbosity(int lv) { Verbosity = lv; }

		//Setter functions: must be invoked before MakeFieldMap
		void SetFieldBoundX(double min, double max) { xMin = min; xMax = max; }
		void SetFieldBoundY(double min, double max) { yMin = min; yMax = max; }
		void SetFieldBoundZ(double min, double max) { zMin = min; zMax = max; }
		void SetFieldOffset(double x, double y, double z) { xOffset = x, yOffset = y, zOffset = z; } 
		void SetUniformField(double Bx, double By, double Bz) { setMonoB = true; mBx = Bx; mBy = By; mBz = Bz; }
		void SetUnitDistance(double unit) { unitD = unit; } //Use CLHEP units: ex. mm or meter
		void SetUnitField   (double unit) { unitF = unit; } //Use CLHEP units: ex. gauss, kiligauss, or tesla

		//Main
		void MakeFieldMap(std::string fname); //Read text input
		void GetFieldValue(const double Point[4], double *Bfield) const;

	protected:

		int Verbosity;
		std::string filename;

		double xMin, xMax, xOffset;
		double yMin, yMax, yOffset;
		double zMin, zMax, zOffset;
		double setMonoB, mBx, mBy, mBz;
		double unitD, unitF; //Distance unit (default mm) and Field unit (default tesla)

		TH3D* mapBx;
		TH3D* mapBy;
		TH3D* mapBz;
};

#endif //LHMAGNETICFIELD_HH
