#ifndef KBVERTEX_HH
#define KBVERTEX_HH

#include "TVector3.h"

#include "KBHit.hh"
#include "KBTracklet.hh"

#include <vector>
#include <iostream>
using namespace std;

class KBVertex : public KBHit
{
  public:
    KBVertex();
    virtual ~KBVertex() { Clear(); }

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "at") const;
    virtual void Copy (TObject &object) const;

    void AddTrack(KBTracklet* track);

    vector<Int_t>       *GetTrackIDArray() { return &fTrackIDArray; }
    vector<KBTracklet*> *GetTrackArray()   { return &fTrackArray; }

    Int_t GetNumTracks() const { return fTrackIDArray.size(); }

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void AddToEveSet(TEveElement *eveSet, Double_t scale=1);
#endif

  private:
    vector<Int_t>       fTrackIDArray;
    vector<KBTracklet*> fTrackArray; //!

  ClassDef(KBVertex, 1)
};

#endif
