#ifndef KBDETECTORSYSTEM_HH
#define KBDETECTORSYSTEM_HH

#include "KBGear.hh"
#include "KBDetectorPlane.hh"
#include "KBDetector.hh"
#include "KBTpc.hh"

#include "TObjArray.h"
#include "TGeoManager.h"

#include "TObjArray.h"

class KBDetector;
class KBTpc;

class KBDetectorSystem : public TObjArray, public KBGear
{
  public:
    KBDetectorSystem();
    KBDetectorSystem(const char *name);
    virtual ~KBDetectorSystem() {}

    virtual void SetParameterContainer(KBParameterContainer *par);
    virtual void SetParameterContainer(TString file);
    virtual void AddParameterContainer(KBParameterContainer *par);
    virtual void AddParameterContainer(TString file);

    virtual void Print(Option_t *option="") const;
    virtual bool Init();

    TGeoManager *GetGeoManager() const;
    TGeoVolume *GetGeoTopVolume() const;
    void SetGeoManager(TGeoManager *);
    void SetTransparency(Int_t transparency);

    void AddDetector(KBDetector *detector);

    Int_t GetNumDetectors() const;
    KBDetector *GetDetector(Int_t idx = 0) const;

    KBTpc *GetTpc() const;

    Int_t GetNumPlanes() const;
    KBDetectorPlane *GetDetectorPlane(Int_t idx = 0) const;

  protected:
    TGeoManager *fGeoManager = nullptr;

    void SetDetector(KBDetector *detector);
    void SetDetectorPar(); 

  ClassDef(KBDetectorSystem, 1)
};

#endif
