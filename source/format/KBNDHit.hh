#ifndef KBNDHIT_HH
#define KBNDHIT_HH

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

class KBNDHit : public KBHit
{
  protected:
    Int_t fLayer    = -999; //0-5 (0: veto)
    Int_t fSubLayer = -999; //0-1
    Int_t fModule   = -999; //0-19
		Float_t fTime 	= -999;

  public :
    KBNDHit() { Clear(); };
    virtual ~KBNDHit() {};

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "at") const;
    virtual void Copy (TObject &object) const;

    virtual void PrintNDHit() const;
    void CopyFrom(KBNDHit const *hit);

		//void AddHit(KBNDHit *hit);

    void SetLayer(Int_t layer);
    void SetSubLayer(Int_t sublayer);
    void SetModule(Int_t module);
		void SetTime(Float_t time);

		Int_t GetLayer() const;
		Int_t GetSubLayer() const;
		Int_t GetModule() const;
		Float_t GetTime() const;

    //void SetSortByLayer(bool sortEarlierIfSmaller);

#ifdef ACTIVATE_EVE
    virtual TEveElement *CreateEveElement();
#endif

  ClassDef(KBNDHit, 1)
};

/*
class KBHitSortSectionRow {
  public:
    KBHitSortSectionRow();
    bool operator() (KBNDHit* h1, KBNDHit* h2) {
      if (h1 -> GetSection() == h2 -> GetSection())
        return h1 -> GetRow() < h2 -> GetRow();
      return h1 -> GetSection() < h2 -> GetSection();
    }
};

class KBHitSortLayer {
  public:
    KBHitSortSectionLayer();
    bool operator() (KBNDHit* h1, KBNDHit* h2) {
      if (h1 -> GetLayer() == h2 -> GetLayer())
        return h1 -> GetSubLayer() < h2 -> GetSubLayer();
      return h1 -> GetLayer() < h2 -> GetLayer();
    }
};
*/

#endif
