#ifndef ATTPCSETUPPARAMETER_HH
#define ATTPCSETUPPARAMETER_HH

#include "KBTask.hh"
#include "KBRun.hh"
#include "KBParameterContainer.hh"

class ATTPCSetupParameter : public KBTask
{
  public:
    ATTPCSetupParameter();
    virtual ~ATTPCSetupParameter() {}


    bool Init();
    void Exec(Option_t*);

    void SetChannelPersistency(bool persistence);
  

  
  private:
    KBRun *run = KBRun::GetRun();
    KBParameterContainer *par = run -> GetParameterContainer();
  
    void GetGasParameters();
    void GEMGain();

    bool fPersistency = false;
    std::vector<double> GasPar[11];

    Double_t fEfield =0;
    Double_t fBfield = 0;
    Double_t fAngle = 0;
    Double_t fVelocityE = 0;
    Double_t fVelocityB = 0;
    Double_t fVelocityExB = 0;
    Double_t fLDiff = 0;
    Double_t fTDiff = 0;
    Double_t fBAt0Coef2 =0;
    Double_t fTownsend = 0;
    Double_t fAttachment = 0;
    Double_t fLorentzAngle = 0;
    Double_t fGasWvalue = 0;
    Double_t fFanoFactor =0;
  

  ClassDef(ATTPCSetupParameter, 1)

};

#endif
