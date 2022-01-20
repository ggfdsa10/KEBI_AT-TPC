#ifndef LHSCINTDIGITIZER_HH
#define LHSCINTDIGITIZER_HH

#include "KBTask.hh"
#include "LHNeutronScintArray.hh"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHScintDigiTask : public KBTask
{ 
  public:
    LHScintDigiTask();
    virtual ~LHScintDigiTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetChannelPersistency(bool persistence);

  private:
    bool fPersistency = false;

  ClassDef(LHScintDigiTask, 1)
};

#endif
