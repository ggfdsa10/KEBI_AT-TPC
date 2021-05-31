#ifndef KBGDMLDETECTOR_HH
#define KBGDMLDETECTOR_HH

#include "KBDetector.hh"

class KBGDMLDetector : public KBDetector
{
  public:
    KBGDMLDetector(TString fileName);
    virtual ~KBGDMLDetector() {};

    virtual bool Init();

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

    TString fFileName;

  ClassDef(KBGDMLDetector, 1)
};

#endif
