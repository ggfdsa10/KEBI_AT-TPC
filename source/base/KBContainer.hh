#ifndef KBCONTAINER_HH
#define KBCONTAINER_HH

#include "TObject.h"
#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

#include "KBGlobal.hh"

/**
 * Print(Option_t *option), option convension
 * - a (aligned) : Aligned print format. [Default]
 * - s (simple) : Simplest print format. Suppress all aligned spacings if possible.
 * - t (title) : Print titles of contents. [Default]
 * - > (print lower lvl) : Print lower level object as reference.
 */

class KBContainer : public TObject
{
  public:
    KBContainer();
    virtual ~KBContainer();

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault()              { return false; } ///< return true if to be displayed on eve.
    virtual bool IsEveSet()                   { return false; } ///< Check if this element should be a "set" of TEveElements (e.g. TEvePointSet, TEveStraightLineSet)
    virtual TEveElement *CreateEveElement()   { return nullptr; } ///< Create TEveElement
    virtual void SetEveElement(TEveElement *, Double_t) {} ///< Set TEveElement. For when IsEveSet() is false.
    virtual void   AddToEveSet(TEveElement *, Double_t) {} ///< Add TEveElement to this eve-set
#endif

  ClassDef(KBContainer, 4)
};

#endif
