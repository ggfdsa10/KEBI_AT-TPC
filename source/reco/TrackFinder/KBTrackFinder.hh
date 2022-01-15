#ifndef KBTRACKFINDER_HH
#define KBTRACKFINDER_HH

#include "KBTask.hh"

class KBTrackFinder : public KBTask
{
  public:
    KBTrackFinder();
    KBTrackFinder(const char* name, const char *title);
    virtual ~KBTrackFinder() {}

    /**
     * @param in   Array of KBHit
     * @param out  Array of KBTracklet
     */
    virtual void FindTrack(TClonesArray *in, TClonesArray *out) = 0;

  ClassDef(KBTrackFinder, 1)
};

#endif
