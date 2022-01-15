#ifndef DUMMYDETECTOR_HH
#define DUMMYDETECTOR_HH

//#include "KBTpc.hh"
#include "KBDetector.hh"

//class DUMMYDetector : public KBTpc
class DUMMYDetector : public KBDetector
{
  public:
    DUMMYDetector();
    virtual ~DUMMYDetector() {};

    virtual bool Init();

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

  ClassDef(DUMMYDetector, 1)
};

#endif
