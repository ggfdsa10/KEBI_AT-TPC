#ifndef ATTPC_HH
#define ATTPC_HH

#include "KBTpc.hh"

class ATTPC : public KBTpc
{
  public:
    ATTPC();
    virtual ~ATTPC() {};

    virtual bool Init();

    KBVector3::Axis GetEFieldAxis();

    virtual TVector3 GetEField(TVector3 pos);
    virtual KBPadPlane *GetDriftPlane(TVector3 pos);

  protected:
    virtual bool BuildGeometry();
    virtual bool BuildDetectorPlane();

    KBVector3::Axis fEFieldAxis = KBVector3::kNon;

  ClassDef(ATTPC, 1)
};

#endif
