#include "KBGeo2DBox.hh"
#include <iomanip>
#include <cmath>

ClassImp(KBGeo2DBox)

KBGeo2DBox::KBGeo2DBox() {}

KBGeo2DBox::KBGeo2DBox(Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
  Set2DBox(x1,x2,y1,y2);
}

KBGeo2DBox::KBGeo2DBox(Double_t xc, Double_t yc, Double_t dx, Double_t dy, Double_t rotation)
{
  Set2DBox(xc,yc,dx,dy,rotation);
}

void KBGeo2DBox::Print(Option_t *) const
{
  kb_out << std::endl;
  kb_out << "  (3)-----(e2)-----(2) y2=" << GetY(2) << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << " (e3) [KBGeo2DBox] (e1)" << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << "  (0)-----(e0)-----(1) y1=" << GetY(1) << std::endl;
  kb_out << std::left;
  kb_out << "   x1=" << std::setw(13) << GetX(1) << " x2=" << GetX(2) << std::endl;
  kb_out << std::right;
}

TVector3 KBGeo2DBox::GetCenter() const { return TVector3(GetXCenter(),GetYCenter(),0); }

void KBGeo2DBox::Set2DBox(Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
  fX[0] = x1; fY[0] = y1;
  fX[1] = x2; fY[1] = y1;
  fX[2] = x2; fY[2] = y2;
  fX[3] = x1; fY[3] = y2;
}

void KBGeo2DBox::Set2DBox(Double_t xc, Double_t yc, Double_t dx, Double_t dy, Double_t rotation)
{
  fX[0] = xc-.5*dx; fY[0] = yc-.5*dy;
  fX[1] = xc+.5*dx; fY[1] = yc-.5*dy;
  fX[2] = xc+.5*dx; fY[2] = yc+.5*dy;
  fX[3] = xc-.5*dx; fY[3] = yc+.5*dy;

  for (auto i=0; i<4; ++i) {
    TVector3 position(fX[i]-xc,fY[i]-yc,0);
    position.RotateZ(rotation);
    fX[i] = position.X()+xc;
    fY[i] = position.Y()+yc;
  }
}

Double_t KBGeo2DBox::GetXCenter() const { return ((fX[0]+fX[1]+fX[2]+fX[3])/4.); }
Double_t KBGeo2DBox::GetdX() const { return TMath::Abs(fX[1]-fX[0]); }
Double_t KBGeo2DBox::GetX(Int_t idx) const { return fX[idx]; }

Double_t KBGeo2DBox::GetYCenter() const { return ((fY[0]+fY[1]+fY[2]+fY[3])/4.); }
Double_t KBGeo2DBox::GetdY() const { return TMath::Abs(fY[1]-fY[0]); }
Double_t KBGeo2DBox::GetY(Int_t idx) const { return fY[idx]; }

TVector3 KBGeo2DBox::GetCorner(Int_t idx) const
{
  TVector3 corner;
  if (idx == 4)
    corner = TVector3(fX[0],fY[0],0);
  else
    corner = TVector3(fX[idx],fY[idx],0);

  return corner;
}


TVector3 KBGeo2DBox::GetCorner(Int_t xpm, Int_t ypm) const
{
  Double_t x, y;
  if (xpm < 0) x = fX[0]; else x = fX[2];
  if (ypm < 0) y = fY[0]; else y = fY[2];

  TVector3 corner(x,y,0);

  return corner;
}

KBGeoLine KBGeo2DBox::GetEdge(Int_t idx) const
{
  auto c1 = GetCorner(idx);
  auto c2 = GetCorner(idx+1);

  KBGeoLine line(c1.X(),c1.Y(),0, c2.X(),c2.Y(),0);

  return line;
}

KBGeoLine KBGeo2DBox::GetEdge(Int_t xpm, Int_t ypm) const
{
  TVector3 c1;
  TVector3 c2;

  if (ypm == 0) {
    if (xpm < 0) { c1 = GetCorner(0); c2 = GetCorner(1); }
    else         { c1 = GetCorner(3); c2 = GetCorner(2); }
  } else if (xpm == 0) {
    if (ypm < 0)  { c1 = GetCorner(0); c2 = GetCorner(3); }
    else          { c1 = GetCorner(1); c2 = GetCorner(2); }
  }

  KBGeoLine line(c1.X(),c1.Y(),0, c2.X(),c2.Y(),0);

  return line;
}

TGraph *KBGeo2DBox::DrawGraph()
{
  auto graph = new TGraph();
  for (auto i : {0,1,2,3,0}) {
    TVector3 cn = GetCorner(i);
    graph -> SetPoint(graph -> GetN(),cn.X(),cn.Y());
  }

  return graph;
}

void KBGeo2DBox::Translate(Double_t x, Double_t y)
{
  for (auto i=0; i<4; ++i) {
    fX[i] = fX[i] + x;
    fY[i] = fY[i] + y;
  }
}

void KBGeo2DBox::Rotate(Double_t deg, Double_t x, Double_t y)
{
  for (auto i=0; i<4; ++i) {
    TVector3 position(fX[i]-x,fY[i]-y,0);
    position.RotateZ(deg*TMath::DegToRad());
    fX[i] = position.X()+x;
    fY[i] = position.Y()+y;
  }
}
