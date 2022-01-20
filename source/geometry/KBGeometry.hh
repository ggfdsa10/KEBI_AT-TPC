#ifndef KBGEOMETRY_HH
#define KBGEOMETRY_HH

#include "KBContainer.hh"

class KBGeometry
{
  protected:
    Double_t fRMS = -1;

  public:
    KBGeometry() {}
    virtual ~KBGeometry() {}

    void SetRMS(Double_t val) { fRMS = val; }
    Double_t GetRMS() const { return fRMS; }

  ClassDef(KBGeometry, 1)
};

#endif
