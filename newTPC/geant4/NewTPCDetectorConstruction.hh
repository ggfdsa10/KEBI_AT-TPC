#ifndef NEWTPCDETECTORCONSTRUCTION_HH 
#define NEWTPCDETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class NewTPCDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    NewTPCDetectorConstruction();
    virtual ~NewTPCDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
