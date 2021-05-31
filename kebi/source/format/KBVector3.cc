#include "KBGlobal.hh"
#include "KBVector3.hh"
#include "KBGeoLine.hh"
#include <iostream>

using namespace std;

ClassImp(KBVector3)

void KBVector3::Print(Option_t *option) const
{
  TString opts = TString(option);

  Int_t rank = 0;
  if (opts.Index("r1")>=0) { rank = 1; opts.ReplaceAll("r1",""); }
  if (opts.Index("r2")>=0) { rank = 2; opts.ReplaceAll("r2",""); }

  if (opts.Index("s")>=0) {
    if (fReferenceAxis != KBVector3::kNon)
      kr_info(rank) << "ref:" << fReferenceAxis
        << ", (x,y,z)=("<<X()<<","<<Y()<<","<<Z()<<")"
        << ", (i,j,k)=("<<I()<<","<<J()<<","<<K()<<")" << endl;
    else
      kr_info(rank) << "ref:" << fReferenceAxis << ", (x,y,z)=("<<X()<<","<<Y()<<","<<Z()<<")" << endl;
  }
  else {
    kr_info(rank) << "Reference axis : " << fReferenceAxis << endl;
    kr_info(rank) << "(x,y,z) = ("<<X()<<","<<Y()<<","<<Z()<<")" << endl;
    if (fReferenceAxis != KBVector3::kNon)
      kr_info(rank) << "(i,j,k) = ("<<I()<<","<<J()<<","<<K()<<")" << endl;
  }
}

void KBVector3::Clear(Option_t *)
{
  SetX(0);
  SetY(0);
  SetZ(0);
  fReferenceAxis = KBVector3::kNon;
}

void KBVector3::SetReferenceAxis(KBVector3::Axis referenceAxis)
{
  if (!IsGlobalAxis(referenceAxis)) {
    kr_error(0) << "Reference axis should be one of: kX(1), kY(2), kZ(3), kMX(4), kMY(5), kMZ(6)" << endl;
    return;
  }
  fReferenceAxis = referenceAxis;
}

KBVector3::Axis KBVector3::GetReferenceAxis() const { return fReferenceAxis; }

KBVector3::Axis KBVector3::GetGlobalAxis(Axis axisIn) const
{
  if (IsGlobalAxis(axisIn))
    return axisIn;

  Axis axisOut = kNon;
  Axis axisRef = fReferenceAxis;
  if (axisRef==kNon)
    return kNon;

  bool negativeCorrection = 0;
  if (int(axisIn)%2==0) {
    negativeCorrection = 1;
    axisIn = KBVector3::Axis(int(axisIn)-1);
  }
  if (int(axisRef)%2==0) {
    negativeCorrection = !negativeCorrection;
    axisRef = KBVector3::Axis(int(axisRef)-1);
  }

       if (axisRef==kZ)  { axisOut = (  (axisIn==kI) ?kX :((axisIn==kJ) ?kY :kZ)  ); }
  else if (axisRef==kY)  { axisOut = (  (axisIn==kI) ?kZ :((axisIn==kJ) ?kX :kY)  ); }
  else if (axisRef==kX)  { axisOut = (  (axisIn==kI) ?kY :((axisIn==kJ) ?kZ :kX)  ); }
  else {}

  axisOut = Axis(int(axisOut)+int(negativeCorrection));
  return axisOut;
}

Double_t KBVector3::At(KBVector3::Axis ka) const
{
       if (ka == KBVector3::kX)  return  X();
  else if (ka == KBVector3::kY)  return  Y();
  else if (ka == KBVector3::kZ)  return  Z();
  else if (ka == KBVector3::kMX) return -X();
  else if (ka == KBVector3::kMY) return -Y();
  else if (ka == KBVector3::kMZ) return -Z();
  else if (ka == KBVector3::kI)  return  I();
  else if (ka == KBVector3::kJ)  return  J();
  else if (ka == KBVector3::kK)  return  K();
  else if (ka == KBVector3::kMI) return -I();
  else if (ka == KBVector3::kMJ) return -J();
  else if (ka == KBVector3::kMK) return -K();
  else
    kr_error(0) << "Cannot use method At() for axis kNon" << endl;

  return -999;
}

void KBVector3::AddAt(Double_t value, Axis ka, bool ignoreNegative)
{
       if (ka == KBVector3::kX)  SetX(X()+value);
  else if (ka == KBVector3::kY)  SetY(Y()+value);
  else if (ka == KBVector3::kZ)  SetZ(Z()+value);
  else if (ka == KBVector3::kMX) { if (ignoreNegative) SetX(X()+value); else SetX(X()-value); }
  else if (ka == KBVector3::kMY) { if (ignoreNegative) SetY(Y()+value); else SetY(Y()-value); }
  else if (ka == KBVector3::kMZ) { if (ignoreNegative) SetZ(Z()+value); else SetZ(Z()-value); }
  else if (ka == KBVector3::kI)  SetI(I()+value);
  else if (ka == KBVector3::kJ)  SetJ(J()+value);
  else if (ka == KBVector3::kK)  SetK(K()+value);
  else if (ka == KBVector3::kMI) { if (ignoreNegative) SetI(I()+value); else SetI(I()-value); }
  else if (ka == KBVector3::kMJ) { if (ignoreNegative) SetJ(J()+value); else SetJ(J()-value); }
  else if (ka == KBVector3::kMK) { if (ignoreNegative) SetK(K()+value); else SetK(K()-value); }
  else
    kr_error(0) << "Cannot use method AddAt() for axis kNon" << endl;
}

void KBVector3::SetAt(Double_t value, Axis ka, bool ignoreNegative)
{
       if (ka == KBVector3::kX)  SetX(value);
  else if (ka == KBVector3::kY)  SetY(value);
  else if (ka == KBVector3::kZ)  SetZ(value);
  else if (ka == KBVector3::kMX) { if (ignoreNegative) SetX(value); else SetX(-value); }
  else if (ka == KBVector3::kMY) { if (ignoreNegative) SetY(value); else SetY(-value); }
  else if (ka == KBVector3::kMZ) { if (ignoreNegative) SetZ(value); else SetZ(-value); }
  else if (ka == KBVector3::kI)  SetI(value);
  else if (ka == KBVector3::kJ)  SetJ(value);
  else if (ka == KBVector3::kK)  SetK(value);
  else if (ka == KBVector3::kMI) { if (ignoreNegative) SetI(value); else SetI(-value); }
  else if (ka == KBVector3::kMJ) { if (ignoreNegative) SetJ(value); else SetJ(-value); }
  else if (ka == KBVector3::kMK) { if (ignoreNegative) SetK(value); else SetK(-value); }
  else
    kr_error(0) << "Cannot use method AddAt() for axis kNon" << endl;
}

void KBVector3::SetIJKR(Double_t i, Double_t j, Double_t k, KBVector3::Axis referenceAxis)
{
  if (!IsGlobalAxis(referenceAxis)) {
    kr_error(0) << "Reference axis should be one of; kX(1), kMX(2), kY(3), kMY(4), kZ(5), kMZ(6)" << endl;
    return;
  }

  fReferenceAxis = referenceAxis;
  SetIJK(i,j,k);
}

void KBVector3::SetIJK(Double_t i, Double_t j, Double_t k)
{
  SetAt(i, GetGlobalAxis(kI));
  SetAt(j, GetGlobalAxis(kJ));
  SetAt(k, GetGlobalAxis(kK));
}

void KBVector3::SetI(Double_t i) { SetAt(i, GetGlobalAxis(kI)); }
void KBVector3::SetJ(Double_t j) { SetAt(j, GetGlobalAxis(kJ)); }
void KBVector3::SetK(Double_t k) { SetAt(k, GetGlobalAxis(kK)); }

Double_t KBVector3::I() const { return At(GetGlobalAxis(kI)); }
Double_t KBVector3::J() const { return At(GetGlobalAxis(kJ)); }
Double_t KBVector3::K() const { return At(GetGlobalAxis(kK)); }

TVector3 KBVector3::GetXYZ() const { return TVector3(X(), Y(), Z()); }
TVector3 KBVector3::GetIJK() const { return TVector3(I(), J(), K()); }

TArrow *KBVector3::ArrowXY() { return KBGeoLine(TVector3(),GetXYZ()).DrawArrowXY(); }
TArrow *KBVector3::ArrowYZ() { return KBGeoLine(TVector3(),GetXYZ()).DrawArrowYZ(); }
TArrow *KBVector3::ArrowZX() { return KBGeoLine(TVector3(),GetXYZ()).DrawArrowZX(); }

void KBVector3::Rotate(Double_t angle, Axis ka)
{
  if (ka==kNon) ka = fReferenceAxis;
  KBVector3 rotationVector(fReferenceAxis);
  rotationVector.SetAt(1,GetGlobalAxis(ka));
  TVector3::Rotate(angle, rotationVector.GetXYZ());
}

KBVector3 operator - (const KBVector3 &a, const KBVector3 &b) {
  if (a.GetReferenceAxis() != b.GetReferenceAxis()) {
    Error("operator -", "operation - between KBVector3s with different reference axis is not allowed");
    return KBVector3(0,0,0,KBVector3::kNon);
  }
  return KBVector3(a.X()-b.X(), a.Y()-b.Y(), a.Z()-b.Z(), a.GetReferenceAxis());
}

KBVector3 operator + (const KBVector3 &a, const KBVector3 &b) {
  if (a.GetReferenceAxis() != b.GetReferenceAxis()) {
    Error("operator +", "operation + between KBVector3s with different reference axis is not allowed");
    return KBVector3(KBVector3::kNon);
  }
  return KBVector3(a.X()+b.X(), a.Y()+b.Y(), a.Z()+b.Z(), a.GetReferenceAxis());
}

KBVector3 operator * (Double_t a, const KBVector3 &p) {
  return KBVector3(a*p.X(), a*p.Y(), a*p.Z(), p.GetReferenceAxis());
}

KBVector3 operator * (const KBVector3 &p, Double_t a) {
  return KBVector3(a*p.X(), a*p.Y(), a*p.Z(), p.GetReferenceAxis());
}

KBVector3 operator * (Int_t a, const KBVector3 &p) {
  return KBVector3(a*p.X(), a*p.Y(), a*p.Z(), p.GetReferenceAxis());
}

KBVector3 operator * (const KBVector3 &p, Int_t a) {
  return KBVector3(a*p.X(), a*p.Y(), a*p.Z(), p.GetReferenceAxis());
}

KBVector3::Axis operator % (const KBVector3::Axis &a1, const KBVector3::Axis &a2)
{
  if (a1==KBVector3::kX)
  {
         if (a2==KBVector3::kX)  return KBVector3::kNon;
    else if (a2==KBVector3::kY)  return KBVector3::kZ;
    else if (a2==KBVector3::kZ)  return KBVector3::kMY;
    else if (a2==KBVector3::kMX) return KBVector3::kNon;
    else if (a2==KBVector3::kMY) return KBVector3::kMZ;
    else if (a2==KBVector3::kMZ) return KBVector3::kY;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kY)
  {
         if (a2==KBVector3::kX)  return KBVector3::kMZ;
    else if (a2==KBVector3::kY)  return KBVector3::kNon;
    else if (a2==KBVector3::kZ)  return KBVector3::kX;
    else if (a2==KBVector3::kMX) return KBVector3::kZ;
    else if (a2==KBVector3::kMY) return KBVector3::kNon;
    else if (a2==KBVector3::kMZ) return KBVector3::kMX;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kZ)
  {
         if (a2==KBVector3::kX)  return KBVector3::kY;
    else if (a2==KBVector3::kY)  return KBVector3::kMX;
    else if (a2==KBVector3::kZ)  return KBVector3::kNon;
    else if (a2==KBVector3::kMX) return KBVector3::kMY;
    else if (a2==KBVector3::kMY) return KBVector3::kX;
    else if (a2==KBVector3::kMZ) return KBVector3::kNon;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kMX)
  {
         if (a2==KBVector3::kX)  return KBVector3::kNon;
    else if (a2==KBVector3::kY)  return KBVector3::kMZ;
    else if (a2==KBVector3::kZ)  return KBVector3::kY;
    else if (a2==KBVector3::kMX) return KBVector3::kNon;
    else if (a2==KBVector3::kMY) return KBVector3::kZ;
    else if (a2==KBVector3::kMZ) return KBVector3::kMY;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kMY)
  {
         if (a2==KBVector3::kX)  return KBVector3::kZ;
    else if (a2==KBVector3::kY)  return KBVector3::kNon;
    else if (a2==KBVector3::kZ)  return KBVector3::kMX;
    else if (a2==KBVector3::kMX) return KBVector3::kMZ;
    else if (a2==KBVector3::kMY) return KBVector3::kNon;
    else if (a2==KBVector3::kMZ) return KBVector3::kX;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kMZ)
  {
         if (a2==KBVector3::kX)  return KBVector3::kMY;
    else if (a2==KBVector3::kY)  return KBVector3::kX;
    else if (a2==KBVector3::kZ)  return KBVector3::kNon;
    else if (a2==KBVector3::kMX) return KBVector3::kY;
    else if (a2==KBVector3::kMY) return KBVector3::kMX;
    else if (a2==KBVector3::kMZ) return KBVector3::kNon;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kI)
  {
         if (a2==KBVector3::kI)  return KBVector3::kNon;
    else if (a2==KBVector3::kJ)  return KBVector3::kK;
    else if (a2==KBVector3::kK)  return KBVector3::kMJ;
    else if (a2==KBVector3::kMI) return KBVector3::kNon;
    else if (a2==KBVector3::kMJ) return KBVector3::kMK;
    else if (a2==KBVector3::kMK) return KBVector3::kJ;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kJ)
  {
         if (a2==KBVector3::kI)  return KBVector3::kMK;
    else if (a2==KBVector3::kJ)  return KBVector3::kNon;
    else if (a2==KBVector3::kK)  return KBVector3::kI;
    else if (a2==KBVector3::kMI) return KBVector3::kK;
    else if (a2==KBVector3::kMJ) return KBVector3::kNon;
    else if (a2==KBVector3::kMK) return KBVector3::kMI;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kK)
  {
         if (a2==KBVector3::kI)  return KBVector3::kJ;
    else if (a2==KBVector3::kJ)  return KBVector3::kMI;
    else if (a2==KBVector3::kK)  return KBVector3::kNon;
    else if (a2==KBVector3::kMI) return KBVector3::kMJ;
    else if (a2==KBVector3::kMJ) return KBVector3::kI;
    else if (a2==KBVector3::kMK) return KBVector3::kNon;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kMI)
  {
         if (a2==KBVector3::kI)  return KBVector3::kNon;
    else if (a2==KBVector3::kJ)  return KBVector3::kMK;
    else if (a2==KBVector3::kK)  return KBVector3::kJ;
    else if (a2==KBVector3::kMI) return KBVector3::kNon;
    else if (a2==KBVector3::kMJ) return KBVector3::kK;
    else if (a2==KBVector3::kMK) return KBVector3::kMJ;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kMJ)
  {
         if (a2==KBVector3::kI)  return KBVector3::kK;
    else if (a2==KBVector3::kJ)  return KBVector3::kNon;
    else if (a2==KBVector3::kK)  return KBVector3::kMI;
    else if (a2==KBVector3::kMI) return KBVector3::kMK;
    else if (a2==KBVector3::kMJ) return KBVector3::kNon;
    else if (a2==KBVector3::kMK) return KBVector3::kI;
    else                         return KBVector3::kNon;
  }
  else if (a1==KBVector3::kMK)
  {
         if (a2==KBVector3::kI)  return KBVector3::kMJ;
    else if (a2==KBVector3::kJ)  return KBVector3::kI;
    else if (a2==KBVector3::kK)  return KBVector3::kNon;
    else if (a2==KBVector3::kMI) return KBVector3::kJ;
    else if (a2==KBVector3::kMJ) return KBVector3::kMI;
    else if (a2==KBVector3::kMK) return KBVector3::kNon;
    else                         return KBVector3::kNon;
  }

  return KBVector3::kNon;
}

KBVector3::Axis operator -- (const KBVector3::Axis &a)
{
       if (a==KBVector3::kX)  return KBVector3::kMX;
  else if (a==KBVector3::kY)  return KBVector3::kMY;
  else if (a==KBVector3::kZ)  return KBVector3::kMZ;
  else if (a==KBVector3::kMX) return KBVector3::kMX;
  else if (a==KBVector3::kMY) return KBVector3::kMY;
  else if (a==KBVector3::kMZ) return KBVector3::kMZ;

  return KBVector3::kNon;
}

KBVector3::Axis operator ++ (const KBVector3::Axis &a)
{
       if (a==KBVector3::kX)  return KBVector3::kX;
  else if (a==KBVector3::kY)  return KBVector3::kY;
  else if (a==KBVector3::kZ)  return KBVector3::kZ;
  else if (a==KBVector3::kMX) return KBVector3::kX;
  else if (a==KBVector3::kMY) return KBVector3::kY;
  else if (a==KBVector3::kMZ) return KBVector3::kZ;

  return KBVector3::kNon;
}

/*
Int_t KBVector3::Compare(const TObject *obj) const
{
  auto compare = ((KBVector3 *) obj) -> SortBy();

       if (fSortBy > compare) return 1;
  else if (fSortBy < compare) return -1;
  else                        return 0;
}
*/

/*
Double_t KBVector3::Angle2(const KBVector3 &q, TVector3 ref) const
{
  Double_t ptot2 = Mag2()*q.Mag2();
  if (ptot2 <= 0) {
    return 0.0;
  } else {
    Double_t arg = Dot(q)/TMath::Sqrt(ptot2);
    if (arg >  1.0) arg =  1.0;
    if (arg < -1.0) arg = -1.0;
    if (Cross(q).Dot(ref) > 0)
      return TMath::ACos(arg);
    else
      return -TMath::ACos(arg);
  }
}
*/
