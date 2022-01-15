#include "KBMCRecoMatching.hh"
#include <iostream>

ClassImp(KBMCRecoMatching)

KBMCRecoMatching::KBMCRecoMatching()
: fStatus(KBMCRecoMatching::kNotFound), fMCID(-1), fMCMomentum(TVector3(0,0,0)), fRecoID(-1), fRecoMomentum(TVector3(0,0,0))
{
  Clear();
}

void KBMCRecoMatching::Clear(Option_t *)
{
  fStatus = KBMCRecoMatching::kNotFound;

  fMCID = -1;
  fMCMomentum = TVector3();

  fRecoID = -1;
  fRecoMomentum = TVector3();

  fRecoIDCand.clear();
  fRecoMomentumCand.clear();
}

void KBMCRecoMatching::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (fStatus == KBMCRecoMatching::kNotFound)
    kr_info(0) << "[Not-Found] MC_ID:" << fMCID << " MC_p("
                  << fMCMomentum.X() << " "
                  << fMCMomentum.Y() << " "
                  << fMCMomentum.Z() << ")" << endl;
  else if (fStatus == KBMCRecoMatching::kFake)
    kr_info(0) << "[Fake] Reco_ID:" << fRecoID << " Reco_p("
                  << fRecoMomentum.X() << " "
                  << fRecoMomentum.Y() << " "
                  << fRecoMomentum.Z() << ")" << endl;
  else if (fStatus == KBMCRecoMatching::kMatched) {
    kr_info(0) << "[MATCH] MC_ID:" << fMCID << " MC_p("
                  << fMCMomentum.X() << " "
                  << fMCMomentum.Y() << " "
                  << fMCMomentum.Z() << ")" << endl;
    kr_info(0) << "      Reco_ID:" << fRecoID << " Reco_p("
                  << fRecoMomentum.X() << " "
                  << fRecoMomentum.Y() << " "
                  << fRecoMomentum.Z() << ")" << endl;
  }
}

void KBMCRecoMatching::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto m = (KBMCRecoMatching &) obj;

  m.Set(fMCID,fMCMomentum,fRecoID,fRecoMomentum);

       if (fStatus == KBMCRecoMatching::kNotFound) m.IsNotFound();
  else if (fStatus == KBMCRecoMatching::kFake)     m.IsFake();
  else if (fStatus == KBMCRecoMatching::kMatched)  m.IsMatched();
}

void KBMCRecoMatching::Set(Int_t mcid, TVector3 mcp, Int_t recoid, TVector3 recop)
{
  fMCID = mcid;
  fRecoID = recoid;
  fMCMomentum = mcp;
  fRecoMomentum = recop;
}

void KBMCRecoMatching::SetMC(Int_t id, TVector3 p)
{
  fMCID = id;
  fMCMomentum = p;
}

void KBMCRecoMatching::SetReco(Int_t id, TVector3 p)
{
  fRecoID = id;
  fRecoMomentum = p;
}

void KBMCRecoMatching::AddRecoCand(Int_t id, TVector3 p)
{
  fRecoIDCand.push_back(id);
  fRecoMomentumCand.push_back(p);
}

void KBMCRecoMatching::SetRecoCand(Int_t idx, Int_t id, TVector3 p)
{
  fRecoIDCand[idx] = id;
  fRecoMomentumCand[idx] = p;
}
