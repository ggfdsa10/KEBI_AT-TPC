#ifndef LAMPSTPC_HH
#define LAMPSTPC_HH

#include "KBTpc.hh"

class LHTpc : public KBTpc
{
  public:
    LHTpc();
    virtual ~LHTpc() {};

    virtual bool Init();

    KBVector3::Axis GetEFieldAxis();

    virtual TVector3 GetEField(TVector3 pos);
    virtual KBPadPlane *GetDriftPlane(TVector3 pos);

  protected:
    virtual bool BuildGeometry();
    virtual bool BuildDetectorPlane();

    KBVector3::Axis fEFieldAxis = KBVector3::kNon;

  ClassDef(LHTpc, 1)
};

#endif
