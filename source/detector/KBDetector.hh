#ifndef KBDETECTOR_HH
#define KBDETECTOR_HH

#include "KBGear.hh"
#include "KBDetectorPlane.hh"

#include "TNamed.h"
#include "TGeoManager.h"

#include "TObjArray.h"

class KBDetectorSystem;

class KBDetector : public TNamed, public KBGear
{
  public:
    KBDetector();
    KBDetector(const char *name, const char *title);
    virtual ~KBDetector() {}

    virtual void Print(Option_t *option="") const;
    virtual bool Init() = 0;

    TGeoVolume *CreateGeoTop(TString name = "TOP");
    void FinishGeometry();

    TGeoManager *GetGeoManager();
    void SetGeoManager(TGeoManager *);
    void SetTransparency(Int_t transparency);

    void AddPlane(KBDetectorPlane *plane);
    Int_t GetNumPlanes();
    KBDetectorPlane *GetDetectorPlane(Int_t idx = 0);

    KBDetectorSystem *GetParent();
    void SetParent(KBDetectorSystem *system);

  protected:
    virtual bool BuildGeometry() = 0;
    virtual bool BuildDetectorPlane() = 0;

    TGeoManager *fGeoManager = nullptr;

    Int_t fNumPlanes = 0;
    TObjArray *fDetectorPlaneArray;

    KBDetectorSystem *fParent = nullptr;

  ClassDef(KBDetector, 1)
};

#endif
