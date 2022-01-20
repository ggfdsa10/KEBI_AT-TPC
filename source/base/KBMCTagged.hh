#ifndef KBMCTAGGED_HH
#define KBMCTAGGED_HH

#include "KBContainer.hh"

class KBMCTagged : public KBContainer
{
  protected:
    Int_t fMCID = -1;
    Double_t fMCError = -1; //!
    Double_t fMCPurity = -1; //!

  public:
    KBMCTagged();
    virtual ~KBMCTagged();

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    void SetMCID(Int_t id);
    void SetMCError(Double_t error);
    void SetMCPurity(Double_t purity);

    void SetMCTag(Int_t id, Double_t error, Double_t purity);

    Int_t GetMCID() const { return fMCID; }
    Double_t GetMCError() const { return fMCError; } /// mm
    Double_t GetMCPurity() const { return fMCPurity; } /// mm

    virtual void PropagateMC() {};

  ClassDef(KBMCTagged, 1)
};

#endif
