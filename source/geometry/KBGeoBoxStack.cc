#include "KBGeoBoxStack.hh"
#include <cmath>

ClassImp(KBGeoBoxStack)

KBGeoBoxStack::KBGeoBoxStack()
{
}

KBGeoBoxStack::KBGeoBoxStack(Double_t x,  Double_t y,  Double_t z,
                             Double_t dx, Double_t dy, Double_t dz,
                             Int_t n, kbaxis_t aStack, kbaxis_t aFace)
{
  SetBoxStack(x,y,z,dx,dy,dz,n,aStack,aFace);
}

void KBGeoBoxStack::Print(Option_t *) const
{
  kb_out << "[KBGeoBoxStack]" << std::endl;
  kb_out << "  Center      : " << fX << " " << fY << " " << fZ << std::endl;
  kb_out << "  Displacement: " << fdX << " " << fdY << " " << fdZ << std::endl;
  kb_out << "  Stack Axis  : " << fStackAxis << std::endl;
  kb_out << "  Face Axis   : " << fFaceAxis << std::endl;
}

TVector3 KBGeoBoxStack::GetCenter() const { return TVector3(fX, fY, fZ); }

void KBGeoBoxStack::SetBoxStack(Double_t x,  Double_t y,  Double_t z,
                                Double_t dx, Double_t dy, Double_t dz,
                                Int_t n, kbaxis_t aStack, kbaxis_t aFace)
{
  fX = x;
  fY = y;
  fZ = z;
  fdX = dx;
  fdY = dy;
  fdZ = dz;
  fNumStacks = n;
  fStackAxis = aStack;
  fFaceAxis = aFace;
}

kbaxis_t KBGeoBoxStack::GetStackAxis() const { return fStackAxis; }
kbaxis_t KBGeoBoxStack::GetFaceAxis()  const { return fFaceAxis; }

Double_t KBGeoBoxStack::GetStackAxisMax() const { return GetStackAxisCenter() + .5*fNumStacks*GetStackAxisDisplacement(); }
Double_t KBGeoBoxStack::GetFaceAxisMax()  const { return GetFaceAxisCenter()  + .5*GetFaceAxisDisplacement(); }
Double_t KBGeoBoxStack::GetLongAxisMax()  const { return GetLongAxisCenter()  + .5*GetLongAxisDisplacement(); }

Double_t KBGeoBoxStack::GetStackAxisMin() const { return GetStackAxisCenter() - .5*fNumStacks*GetStackAxisDisplacement(); }
Double_t KBGeoBoxStack::GetFaceAxisMin()  const { return GetFaceAxisCenter()  - .5*GetFaceAxisDisplacement(); }
Double_t KBGeoBoxStack::GetLongAxisMin()  const { return GetLongAxisCenter()  - .5*GetLongAxisDisplacement(); }

Double_t KBGeoBoxStack::GetStackAxisCenter() const { return KBVector3(fX,fY,fZ).At(fStackAxis); }
Double_t KBGeoBoxStack::GetFaceAxisCenter()  const { return KBVector3(fX,fY,fZ).At(fFaceAxis); }
Double_t KBGeoBoxStack::GetLongAxisCenter()  const { return KBVector3(fX,fY,fZ).At(++(fStackAxis%fFaceAxis)); }

Double_t KBGeoBoxStack::GetStackAxisDisplacement() const { return KBVector3(fdX,fdY,fdZ).At(fStackAxis); }
Double_t KBGeoBoxStack::GetFaceAxisDisplacement()  const { return KBVector3(fdX,fdY,fdZ).At(fFaceAxis); }
Double_t KBGeoBoxStack::GetLongAxisDisplacement()  const { return KBVector3(fdX,fdY,fdZ).At(++(fStackAxis%fFaceAxis)); }

KBGeoBox KBGeoBoxStack::GetBox(Int_t idx) const
{
  KBVector3 pos(fX, fY, fZ);

  pos.AddAt(GetStackAxisDisplacement()*(-.5*(fNumStacks-1)+idx),fStackAxis);

  return KBGeoBox(pos, fdX, fdY, fdZ);
}

TMultiGraph *KBGeoBoxStack::DrawStackGraph(kbaxis_t a1, kbaxis_t a2)
{
  if (a1 == KBVector3::kNon || a2 == KBVector3::kNon) {
    a1 = fFaceAxis%fStackAxis;
    a1 = ++a1;
    a2 = fStackAxis;
    if (KBVector3::IsNegative(a1%a2)) { auto ar = a1; a1 = a2; a2 = ar; }
  }

  auto mgraph = new TMultiGraph();
  for (auto idx = 0; idx < fNumStacks; ++idx) {
    auto box = GetBox(idx).Draw2DBox(a1,a2);
    if (idx%2!=0)
      box -> SetFillColor(kGray);
    mgraph -> Add(box,"lf");
  }

  return mgraph;
}

TH2D *KBGeoBoxStack::DrawStackHist(TString name, TString title, kbaxis_t a1, kbaxis_t a2)
{
  if (a1 == KBVector3::kNon || a2 == KBVector3::kNon) {
    a1 = fFaceAxis%fStackAxis;
    a1 = ++a1;
    a2 = fStackAxis;
    if (KBVector3::IsNegative(a1%a2)) { auto ar = a1; a1 = a2; a2 = ar; }
  }

  KBVector3 pos(fX, fY, fZ);
  KBVector3 dis(fdX, fdY, fdZ);

  Int_t nx = 1;
  Double_t x1=pos.At(a1)-.5*dis.At(a1);
  Double_t x2=pos.At(a1)+.5*dis.At(a1);

  Int_t ny = 1;
  Double_t y1=pos.At(a2)-.5*dis.At(a2);
  Double_t y2=pos.At(a2)+.5*dis.At(a2);

  Double_t max = GetStackAxisMax();
  Double_t min = GetStackAxisMin();

  if (a1 == fStackAxis) {
    nx = fNumStacks;
    x1 = min;
    x2 = max;
  }
  else if (a2 == fStackAxis) {
    ny = fNumStacks;
    y1 = min;
    y2 = max;
  }

  if (name.IsNull())
    name = "BoxStack";

  if (title.Index(";")<0)
    title = title+";"+KBVector3::AxisName(a1)+";"+KBVector3::AxisName(a2);

  auto hist = new TH2D(name,title,nx,x1,x2,ny,y1,y2);

  return hist;
}

TH2Poly *KBGeoBoxStack::DrawStackHistPoly(TString name, TString title, kbaxis_t a1, kbaxis_t a2)
{
  if (a1 == KBVector3::kNon || a2 == KBVector3::kNon) {
    a1 = fFaceAxis%fStackAxis;
    a1 = ++a1;
    a2 = fStackAxis;
    if (KBVector3::IsNegative(a1%a2)) { auto ar = a1; a1 = a2; a2 = ar; }
  }
  //kbaxis_t a3 = ++(a1%a2);

  auto hist = new TH2Poly();
  for (auto idx = 0; idx < fNumStacks; ++idx) {
    auto box2D = GetBox(idx).GetFace(a1,a2);
    auto cnn = KBVector3(box2D.GetCorner(-1,-1));
    auto cnp = KBVector3(box2D.GetCorner(1,1));
    hist -> AddBin(cnn.At(a1),cnn.At(a2),cnp.At(a1),cnp.At(a2));
  }

  if (name.IsNull())
    name = "BoxStack";

  if (title.Index(";")<0)
    title = title+";"+KBVector3::AxisName(a1)+";"+KBVector3::AxisName(a2);

  hist -> SetNameTitle(name,title);

  return hist;
}

Int_t KBGeoBoxStack::FindBoxIndex(TVector3 pos) const
{
  return FindBoxIndex(pos.X(),pos.Y(),pos.Z());
}

Int_t KBGeoBoxStack::FindBoxIndex(Double_t x, Double_t y, Double_t z) const
{
  for (auto idx = 0; idx < fNumStacks; ++idx) {
    if (GetBox(idx).IsInside(x,y,z))
      return idx;
  }
  return -1;
}
