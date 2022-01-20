#ifndef LAMPSPROTOTYPETPCDETECTORCONSTRUCTION_HH
#define LAMPSPROTOTYPETPCDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class LAPDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    LAPDetectorConstruction();
    virtual ~LAPDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
