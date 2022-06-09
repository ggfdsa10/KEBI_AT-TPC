#ifndef LHDETECTORCONSTRUCTION_HH
#define LHDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class LHDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    LHDetectorConstruction();
    virtual ~LHDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
