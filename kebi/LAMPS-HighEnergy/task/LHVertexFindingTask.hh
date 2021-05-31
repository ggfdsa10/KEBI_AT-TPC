#ifndef LHVERTEXFINDINGTASK_HH
#define LHVERTEXFINDINGTASK_HH

#include "KBTask.hh"
#include "KBVertex.hh"
#include "KBVector3.hh"

#include "TClonesArray.h"

class LHVertexFindingTask : public KBTask
{ 
  public:
    LHVertexFindingTask();
    virtual ~LHVertexFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    Double_t TestVertexAtK(KBVertex *vertex, KBVector3 testPosition, bool last = false);

    void SetVertexPersistency(bool val);

    void NewTrackWithHitClsuters(KBHit *vertex);

  private:
    TClonesArray* fTrackArray = nullptr;
    TClonesArray* fVertexArray = nullptr;

    TClonesArray *fTrackArray2 = nullptr;
    TClonesArray *fClusterArray = nullptr;
    TClonesArray* fVertexArray2 = nullptr;

    bool fPersistency = true;

    KBVector3::Axis fReferenceAxis = KBVector3::kZ;

  ClassDef(LHVertexFindingTask, 1)
};

#endif
