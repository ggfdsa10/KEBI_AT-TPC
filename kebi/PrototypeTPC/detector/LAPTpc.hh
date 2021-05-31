#ifndef LAMPSPROTOTYPEDETECTOR_HH
#define LAMPSPROTOTYPEDETECTOR_HH

#include "KBTpc.hh"

class LAPTpc : public KBTpc
{
  public:
    LAPTpc();
    virtual ~LAPTpc() {};

    virtual bool Init();

    virtual KBVector3::Axis GetEFieldAxis() { return fEFieldAxis; }
    virtual TVector3 GetEField(TVector3 pos);
    virtual KBPadPlane *GetDriftPlane(TVector3 pos);

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

    KBVector3::Axis fEFieldAxis = KBVector3::kNon;

  ClassDef(LAPTpc, 1)
};

#endif
