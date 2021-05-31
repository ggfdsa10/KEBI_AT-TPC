#include "KBTrackFinder.hh"

ClassImp(KBTrackFinder)

KBTrackFinder::KBTrackFinder()
:KBTask("KBTrackFinder", "KBTrackFinder Base Class")
{
}

KBTrackFinder::KBTrackFinder(const char* name, const char *title)
:KBTask(name, title)
{
}
