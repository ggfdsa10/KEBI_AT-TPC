#include "KBGear.hh"

void KBGear::CreateParameterContainer(bool debug) {
  if (fPar == nullptr)
    fPar = new KBParameterContainer(debug);
}

void KBGear::SetParameterContainer(KBParameterContainer *par) { fPar = par; }

void KBGear::SetParameterContainer(TString fname)
{
  if (fPar != nullptr) {
    fPar -> Clear();
    fPar -> AddFile(fname);
  }
  else
    fPar = new KBParameterContainer(fname);
}

void KBGear::AddParameterContainer(KBParameterContainer *par) {
  if (fPar == nullptr)
    fPar = par;
  else
    fPar -> AddPar(par);
}

void KBGear::AddParameterContainer(TString fname) {
  if (fPar == nullptr)
    fPar = new KBParameterContainer();
  fPar -> AddFile(fname);
}

KBParameterContainer *KBGear::GetParameterContainer() const { return fPar; }
KBParameterContainer *KBGear::GetPar() const { return fPar; }

void KBGear::SetRank(Int_t rank) { fRank = rank; }
Int_t KBGear::GetRank() const { return fRank; }
