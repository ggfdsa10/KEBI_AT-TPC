#ifndef NEWTPCSETUPPARAMETER_HH
#define NEWTPCSETUPPARAMETER_HH

#include "KBTask.hh"
#include "KBRun.hh"
#include "KBParameterContainer.hh"

class NewTPCSetupParameter : public KBTask
{
  public:
    NewTPCSetupParameter();
    virtual ~NewTPCSetupParameter() {}


    bool Init();
    void Exec(Option_t*);

    void SetChannelPersistency(bool persistence);
  

  
  private:
    KBRun *run = KBRun::GetRun();
    KBParameterContainer *par = run -> GetParameterContainer();
  
    void GetGasParameters();
    void GetHe4_iC4H10Parameters();
    int GetNearestIndex();
    bool fPersistency = false;

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
    Double_t fElectronNumRef = 0;

    Double_t fiC4H10Ratio = 0.;
    Double_t fpressure = 0.;
    Double_t bfieldx = 0.;
    Double_t bfieldy = 0.;
    Double_t bfieldz = 0.;

    double fVEarray[8][7][7];
    double fTDarray[8][7][6][8];
    double fLDarray[8][7][8];
    int fIndexRatio;
    int fIndexPressure;
    int fIndexMagnet;
  

  ClassDef(NewTPCSetupParameter, 1)

};

#endif
