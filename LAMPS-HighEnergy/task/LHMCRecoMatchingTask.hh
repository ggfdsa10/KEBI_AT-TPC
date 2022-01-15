#ifndef LHMCRECOMATCHINGTASK_HH
#define LHMCRECOMATCHINGTASK_HH

#include "KBTask.hh"
#include "KBMCTrack.hh"
#include "KBHelixTrack.hh"

#include "TClonesArray.h"

class LHMCRecoMatchingTask : public KBTask
{ 
  public:
    LHMCRecoMatchingTask();
    virtual ~LHMCRecoMatchingTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetPersistency(bool val) { fPersistency = val; }

  private:
    TClonesArray* fMCTrackArray;
    TClonesArray* fRecoTrackArray;

    TClonesArray* fMCArray;
    TClonesArray* fRecoArray;
    TClonesArray* fMatchingArray;

    bool fPersistency = true;

  ClassDef(LHMCRecoMatchingTask, 1)
};

#endif
