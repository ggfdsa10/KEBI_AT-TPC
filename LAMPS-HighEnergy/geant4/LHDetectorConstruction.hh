#ifndef LHDETECTORCONSTRUCTION_HH
#define LHDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Cache.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class LHMagneticFieldSetup;

class LHDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    LHDetectorConstruction();
    virtual ~LHDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

		//BField
		G4Cache<LHMagneticFieldSetup*> fFieldCache;
};

#endif
