#ifndef DUMMYDETECTORCONSTRUCTION_HH 
#define DUMMYDETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class DUMMYDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DUMMYDetectorConstruction();
    virtual ~DUMMYDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
