#ifndef KBMCSTEP_HH
#define KBMCSTEP_HH

#include "TObject.h"
#include "KBContainer.hh"

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

class KBMCStep : public KBContainer
{
  public:
    KBMCStep();
    virtual ~KBMCStep();

    virtual void Print(Option_t *option = "at") const;
    virtual void Clear(Option_t *option = "");

    void SetTrackID(Int_t val);
    void SetX(Double_t val);
    void SetY(Double_t val);
    void SetZ(Double_t val);
    void SetTime(Double_t val);
    void SetEdep(Double_t val);

    void SetMCStep(Int_t trackID, Double_t x, Double_t y, Double_t z, Double_t time, Double_t edep);

    Int_t GetTrackID()  const;
    Double_t GetX()     const;
    Double_t GetY()     const;
    Double_t GetZ()     const;
    Double_t GetTime()  const;
    Double_t GetEdep()  const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *, Double_t scale=1);
    virtual void AddToEveSet(TEveElement *eveSet, Double_t scale=1);
#endif

  private:
    Int_t fTrackID;
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fTime;
    Double_t fEdep;

  ClassDef(KBMCStep, 3)
};

#endif
