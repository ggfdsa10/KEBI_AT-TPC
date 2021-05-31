#ifndef KBGEAR_HH
#define KBGEAR_HH

#include <iostream>
#include <fstream>
#include "KBGlobal.hh"
#include "KBParameterContainer.hh"

/**
 * Virtual class for stear classes of KEBI
*/

class KBGear
{
  public:
    KBGear() {};
    virtual ~KBGear() {};

    void CreateParameterContainer(bool debug=false);

    void SetParameterContainer(KBParameterContainer *par);
    void SetParameterContainer(TString fname);

    virtual void AddParameterContainer(KBParameterContainer *par);
    virtual void AddParameterContainer(TString fname);

    void SetPar(KBParameterContainer *par) { SetParameterContainer(par); }
    void AddPar(KBParameterContainer *par) { AddParameterContainer(par); }
    void SetPar(TString fname) { SetParameterContainer(fname); }
    void AddPar(TString fname) { AddParameterContainer(fname); }

    KBParameterContainer *GetParameterContainer() const;
    KBParameterContainer *GetPar() const;

    virtual void SetRank(Int_t rank);
    Int_t GetRank() const;

  protected:
    KBParameterContainer *fPar = nullptr;

    Int_t fRank = 0;

  ClassDef(KBGear, 1)
};

#endif
