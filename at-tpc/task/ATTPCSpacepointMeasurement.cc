#include "ATTPCSpacepointMeasurement.hh"
#include "TVector3.h"

using namespace std;

ClassImp(genfit::ATTPCSpacepointMeasurement)

namespace genfit {
    //////////////////////////////////////////////////////////////////////////////////

    ATTPCSpacepointMeasurement::ATTPCSpacepointMeasurement()
    : SpacepointMeasurement()
    {
    }

    ATTPCSpacepointMeasurement::ATTPCSpacepointMeasurement(const KBHit* detHit, const TrackCandHit* hit)
    : SpacepointMeasurement()
    {
        TVector3 pos = detHit -> GetPosition();

        /// all genfit units in cm

        rawHitCoords_(0) = pos.X()/10.;
        rawHitCoords_(1) = pos.Y()/10.;
        rawHitCoords_(2) = pos.Z()/10.;

        //rawHitCov_(0,0) = 0.01;
        //rawHitCov_(1,1) = 0.01;
        //rawHitCov_(2,2) = 0.01;
        rawHitCov_(0,0) = pow(1.0, 2);
        rawHitCov_(1,1) = pow(1.0, 2);
        rawHitCov_(2,2) = pow(0.05, 2);

        detId_ = hit->getDetId();
        hitId_ = hit->getHitId();

        this -> initG();
    }

    //////////////////////////////////////////////////////////////////////////////////
}