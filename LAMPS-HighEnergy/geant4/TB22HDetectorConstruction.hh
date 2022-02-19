#ifndef TB22HDETECTORCONSTRUCTION_HH
#define TB22HDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4NistManager;
class G4VisAttributes;

class TB22HDetectorConstruction : public G4VUserDetectorConstruction
{
	public:

		TB22HDetectorConstruction();
		virtual ~TB22HDetectorConstruction();

		virtual G4VPhysicalVolume* Construct();

	private:

		G4NistManager* fNist;
};
#endif
