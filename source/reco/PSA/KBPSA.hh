#ifndef KBPSA_HH
#define KBPSA_HH

#include "KBChannelHit.hh"
#include "KBParameterContainer.hh"

#include <vector>
#include <iostream>
using namespace std;

class KBPSA
{
  public:
    KBPSA() {}
    virtual ~KBPSA() {}

    virtual void AnalyzeChannel(Double_t *buffer, vector<KBChannelHit> *hitArray);
    void SetTbRange(Int_t tbi, Int_t tbf);
    void SetThreshold(Double_t val);

    void SetParameters(KBParameterContainer *par);

  protected:
    Int_t fTbStart = 0;
    Int_t fTbEnd = 512;
    Double_t fThreshold = 100;
    Double_t fDynamicRange = 4000;

  ClassDef(KBPSA, 1)
};

#endif
