#ifndef ATTPCDETECTORCONSTRUCTION_HH 
#define ATTPCDETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class ATTPCDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    ATTPCDetectorConstruction();
    virtual ~ATTPCDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
