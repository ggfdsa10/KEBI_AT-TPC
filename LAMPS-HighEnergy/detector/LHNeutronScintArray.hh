#ifndef LHNEUTRONSCINTARRAY_HH
#define LHNEUTRONSCINTARRAY_HH

#include "KBDetector.hh"

class LHNeutronScintArray : public KBDetector
{
  public:
    LHNeutronScintArray();
    virtual ~LHNeutronScintArray() {};

    virtual bool Init();

  protected:
    virtual bool BuildGeometry();
    virtual bool BuildDetectorPlane();

  ClassDef(LHNeutronScintArray, 1)
};

#endif
