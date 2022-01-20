#include "KBMCTagged.hh"

KBMCTagged::KBMCTagged()
{
}

KBMCTagged::~KBMCTagged()
{
}

void KBMCTagged::Clear(Option_t *option)
{
  KBContainer::Clear(option);
}

void KBMCTagged::Copy(TObject &obj) const
{
  KBContainer::Copy(obj);
  auto container = (KBMCTagged &) obj;

  container.SetMCTag(fMCID, fMCError, fMCPurity);
}

void KBMCTagged::SetMCID(Int_t id) { fMCID = id; }
void KBMCTagged::SetMCError(Double_t error) { fMCError = error; }
void KBMCTagged::SetMCPurity(Double_t purity) { fMCPurity = purity; }

void KBMCTagged::SetMCTag(Int_t id, Double_t error, Double_t purity)
{
  fMCID = id;
  fMCError = error;
  fMCPurity = purity;
}
