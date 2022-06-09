#ifndef ATTPCSPACEPOINTMEASUREMENT_HH
#define ATTPCSPACEPOINTMEASUREMENT_HH

#include "genfit2/AbsMeasurement.h"
#include "genfit2/SpacepointMeasurement.h"
#include "genfit2/TrackCandHit.h"

#include "KBHit.hh"

namespace genfit {
  //////////////////////////////////////////////////////////////////////////////////

    class ATTPCSpacepointMeasurement : public SpacepointMeasurement
    {
        public:
            ATTPCSpacepointMeasurement();
            ATTPCSpacepointMeasurement(const KBHit* detHit, const TrackCandHit* hit);

            virtual ATTPCSpacepointMeasurement* clone() const { return new ATTPCSpacepointMeasurement(*this); }

        ClassDef(ATTPCSpacepointMeasurement, 1)
    };

  //////////////////////////////////////////////////////////////////////////////////
}
#endif