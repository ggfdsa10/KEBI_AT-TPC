#ifndef TB20ADETECTORCONSTRUCTION_HH
#define TB20ADETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class TB20ADetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    TB20ADetectorConstruction();
    virtual ~TB20ADetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
