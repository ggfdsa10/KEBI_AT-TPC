#ifndef ATTPCPSA_HH
#define ATTPCPSA_HH

#include "KBChannelHit.hh"
#include "KBParameterContainer.hh"

#include <vector>
#include <iostream>
using namespace std;

class ATTPCPSA
{
  public:
    ATTPCPSA() {}
    virtual ~ATTPCPSA() {}

    virtual void AnalyzeChannel(Double_t *buffer, vector<KBChannelHit> *hitArray, vector<vector<pair<double, double>>> *TBArray);
    void SetTbRange(Int_t tbi, Int_t tbf);
    void SetThreshold(Double_t val);

    void SetParameters(KBParameterContainer *par);

  protected:
    Int_t fTbStart = 0;
    Int_t fTbEnd = 512;
    Double_t fThreshold = 20;
    Double_t fDynamicRange = 4096;

  ClassDef(ATTPCPSA, 1)
};

#endif