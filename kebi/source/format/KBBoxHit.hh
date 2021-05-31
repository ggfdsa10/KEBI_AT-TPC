#ifndef KBBOXHIT_HH
#define KBBOXHIT_HH

#include "KBBoxHit.hh"
#include "KBContainer.hh"
#include "KBHit.hh"

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

#include "TVector3.h"
#include "TMath.h"
#include "TF1.h"

#include <vector>
using namespace std;

class KBBoxHit : public KBHit
{
  protected:
    Int_t fdX;
    Int_t fdY;
    Int_t fdZ;

  public :
    KBBoxHit() { Clear(); }
    KBBoxHit(Double_t x, Double_t y, Double_t z,
        Double_t dx, Double_t dy, Double_t dz,
        Double_t q)
    {
      Clear();
      Set(x,y,z,dx,dy,dz,q);
    }
    virtual ~KBBoxHit() {}

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    void Set(Double_t x, Double_t y, Double_t z,
        Double_t dx, Double_t dy, Double_t dz,
        Double_t q);

    void SetdX(Double_t dx);
    void SetdY(Double_t dy);
    void SetdZ(Double_t dz);

    Double_t GetdX() const;
    Double_t GetdY() const;
    Double_t GetdZ() const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *, Double_t scale=1);
    virtual void AddToEveSet(TEveElement *eveSet, Double_t scale=1);
#endif

  ClassDef(KBBoxHit, 1)
};

#endif
