#ifndef NEWTPC_HH
#define NEWTPC_HH

#include "KBTpc.hh"

class NewTPC : public KBTpc
{
  public:
    NewTPC();
    virtual ~NewTPC() {};

    virtual bool Init();

    KBVector3::Axis GetEFieldAxis();

    virtual TVector3 GetEField(TVector3 pos);
    virtual KBPadPlane *GetDriftPlane(TVector3 pos);

  protected:
    virtual bool BuildGeometry();
    virtual bool BuildDetectorPlane();

    KBVector3::Axis fEFieldAxis = KBVector3::kNon;

  ClassDef(NewTPC, 1)
};

#endif
