#ifndef KBTPCHIT_HH
#define KBTPCHIT_HH

#include "KBHit.hh"
#include "KBContainer.hh"

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif
#include "TVector3.h"
#include "TMath.h"
#include "TF1.h"

#include <vector>
using namespace std;

class KBTpcHit : public KBHit
{
  protected:
    Int_t fPadID   = -1;
    Int_t fSection = -999;
    Int_t fRow     = -999;
    Int_t fLayer   = -999;
    Double_t fTb   = -1;

  public :
    KBTpcHit() { Clear(); };
    virtual ~KBTpcHit() {};

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "at") const;
    virtual void Copy (TObject &object) const;

    virtual void PrintTpcHit() const;
    void CopyFrom(KBTpcHit const *hit);

    void AddHit(KBTpcHit *hit);

    virtual TF1 *GetPulseFunction(Option_t *option = "");

    void SetPadID(Int_t id);
    void SetSection(Int_t section);
    void SetRow(Int_t row);
    void SetLayer(Int_t layer);
    void SetTb(Double_t tb);

    Int_t GetPadID() const;
    Int_t GetSection() const;
    Int_t GetRow() const;
    Int_t GetLayer() const;
    Double_t GetTb() const;

    void SetSortByLayer(bool sortEarlierIfSmaller);

#ifdef ACTIVATE_EVE
    virtual TEveElement *CreateEveElement();
#endif

  ClassDef(KBTpcHit, 1)
};

class KBHitSortSectionRow {
  public:
    KBHitSortSectionRow();
    bool operator() (KBTpcHit* h1, KBTpcHit* h2) {
      if (h1 -> GetSection() == h2 -> GetSection())
        return h1 -> GetRow() < h2 -> GetRow();
      return h1 -> GetSection() < h2 -> GetSection();
    }
};

class KBHitSortSectionLayer {
  public:
    KBHitSortSectionLayer();
    bool operator() (KBTpcHit* h1, KBTpcHit* h2) {
      if (h1 -> GetSection() == h2 -> GetSection())
        return h1 -> GetLayer() < h2 -> GetLayer();
      return h1 -> GetSection() < h2 -> GetSection();
    }
};

#endif
