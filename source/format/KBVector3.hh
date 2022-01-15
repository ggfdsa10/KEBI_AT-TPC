#ifndef KBVECTOR3_HH
#define KBVECTOR3_HH

#include "TVector3.h"
#include "TArrow.h"
#include <map>

/**
 * Utility class to handel global axis (x,y,z) and local axis (i,j,k) togeter.
 * Local axis (i,j,k) is definded to have reference axis same as k-axis
 * and i,j-axis defined by right-handed orientation in i,j,k order.
 */

class KBVector3 : public TVector3
{
  public:
    enum Axis : int {
      kNon = 0,
      kX   = 1,  kMX  = 2,
      kY   = 3,  kMY  = 4,
      kZ   = 5,  kMZ  = 6,
      kI   = 7,  kMI  = 8,
      kJ   = 9,  kMJ  = 10,
      kK   = 11, kMK  = 12
    };

    friend std::ostream& operator<<(std::ostream& out, const Axis axisIn) {
      static std::map<Axis, std::string> axisNames;
      if (axisNames.size() == 0) {
        axisNames[kNon] = "not-defined";
        axisNames[kX]   = "x";
        axisNames[kY]   = "y";
        axisNames[kZ]   = "z";
        axisNames[kMX]  = "-x";
        axisNames[kMY]  = "-y";
        axisNames[kMZ]  = "-z";
        axisNames[kI]   = "i";
        axisNames[kJ]   = "j";
        axisNames[kK]   = "k";
        axisNames[kMI]  = "-i";
        axisNames[kMJ]  = "-j";
        axisNames[kMK]  = "-k";
      }
      return out << axisNames[axisIn];
    }

    static Axis GetAxis(TString name) {
      name.ToLower();
           if (name == "x")  return kX;
      else if (name == "y")  return kY;
      else if (name == "z")  return kZ;
      else if (name == "-x") return kMX;
      else if (name == "-y") return kMY;
      else if (name == "-z") return kMZ;
      else if (name == "i")  return kI;
      else if (name == "j")  return kJ;
      else if (name == "k")  return kK;
      else if (name == "-i") return kMI;
      else if (name == "-j") return kMJ;
      else if (name == "-k") return kMK;
      else                   return kNon;
    }

    static TString AxisName(Axis axisIn) {
      static std::map<Axis, std::string> axisNames;
      if (axisNames.size() == 0) {
        axisNames[kNon] = "not-defined";
        axisNames[kX]   = "x";
        axisNames[kY]   = "y";
        axisNames[kZ]   = "z";
        axisNames[kMX]  = "-x";
        axisNames[kMY]  = "-y";
        axisNames[kMZ]  = "-z";
        axisNames[kI]   = "i";
        axisNames[kJ]   = "j";
        axisNames[kK]   = "k";
        axisNames[kMI]  = "-i";
        axisNames[kMJ]  = "-j";
        axisNames[kMK]  = "-k";
      }
      return TString(axisNames[axisIn].c_str());
    }

    static bool IsGlobalAxis(Axis axisIn) {
      if (int(axisIn)>0 && int(axisIn)<7)
        return true;
      return false;
    }

    static bool IsLocalAxis(Axis axisIn) {
      if (int(axisIn)>6)
        return true;
      return false;
    }

    static bool IsPositive(Axis axisIn) {
      if (axisIn%2==1)
        return true;
      return false;
    }

    static bool IsNegative(Axis axisIn) {
      if (axisIn%2==0)
        return true;
      return false;
    }


    KBVector3()
    :TVector3() { Clear(); }

    KBVector3(Axis referenceAxis)
    :TVector3(), fReferenceAxis(referenceAxis) {}

    KBVector3(const KBVector3 &p)
    :TVector3(p), fReferenceAxis(p.fReferenceAxis) {}

    KBVector3(const TVector3 &p, Axis referenceAxis = kNon)
    :TVector3(p), fReferenceAxis(referenceAxis) {}

    KBVector3(Double_t x, Double_t y, Double_t z, Axis referenceAxis = kNon)
    :TVector3(x,y,z), fReferenceAxis(referenceAxis) {}

    KBVector3(Axis referenceAxis, Double_t i, Double_t j, Double_t k)
    :fReferenceAxis(referenceAxis) { SetIJK(i,j,k); }

    KBVector3(Axis referenceAxis, TVector3 ijk)
    :fReferenceAxis(referenceAxis) { SetIJK(ijk.X(),ijk.Y(),ijk.Z()); }

    virtual ~KBVector3() {}

    inline KBVector3 operator - () const {
      return KBVector3(-X(), -Y(), -Z(), fReferenceAxis);
    }

    inline KBVector3 & operator = (const KBVector3 & p) {
      SetX(p.X());
      SetY(p.Y());
      SetZ(p.Z());
      fReferenceAxis = p.fReferenceAxis;
      return *this;
    }

    virtual void Print(Option_t *option = "at") const;
    virtual void Clear(Option_t *option = "");

    void SetReferenceAxis(Axis referenceAxis);
    Axis GetReferenceAxis() const;
    Axis GetGlobalAxis(Axis ka) const;

    Double_t At(Axis ka) const;
    void AddAt(Double_t value, Axis ka, bool ignoreNegative = false);
    void SetAt(Double_t value, Axis ka, bool ignoreNegative = false);

    void SetIJKR(Double_t i, Double_t j, Double_t k, Axis referenceAxis);
    void SetIJK(Double_t i, Double_t j, Double_t k);
    void SetI(Double_t i);
    void SetJ(Double_t j);
    void SetK(Double_t k);

    Double_t I() const;
    Double_t J() const;
    Double_t K() const;

    TVector3 GetXYZ() const;
    TVector3 GetIJK() const;

    TArrow *ArrowXY();
    TArrow *ArrowYZ();
    TArrow *ArrowZX();

    void Rotate(Double_t angle, Axis ka=kNon);

    //virtual Bool_t IsSortable() const { return true; }
    //Int_t Compare(const TObject *obj) const;

    //void SetSortBy(Double_t sortby) { fSortBy = sortby; }
    //Double_t SortBy() { return fSortBy; }

    //Double_t Angle2(const KBVector3 &q, TVector3 ref) const;

  private:
    Axis fReferenceAxis = kNon;
    //Double_t fSortBy = 0;

  ClassDef(KBVector3, 1)
};

KBVector3 operator + (const KBVector3 &a, const KBVector3 &b);
KBVector3 operator - (const KBVector3 &a, const KBVector3 &b);

KBVector3 operator * (Double_t a, const KBVector3 &p);
KBVector3 operator * (const KBVector3 &p, Double_t a);
KBVector3 operator * (Int_t a, const KBVector3 &p);
KBVector3 operator * (const KBVector3 &p, Int_t a);

KBVector3::Axis operator % (const KBVector3::Axis &a1, const KBVector3::Axis &a2);
KBVector3::Axis operator -- (const KBVector3::Axis &a);
KBVector3::Axis operator ++ (const KBVector3::Axis &a);

#endif
