#include "TLine.h"
#include "TText.h"
#include "TF1.h"

#include "KBPad.hh"

#include <iomanip>
#include <iostream>
using namespace std;

ClassImp(KBPad)

void KBPad::Clear(Option_t *option)
{
  KBChannel::Clear(option);

  fActive = false;

  memset(fBufferIn, 0, sizeof(Double_t)*512);
  memset(fBufferRaw, 0, sizeof(Short_t)*512);
  memset(fBufferOut, 0, sizeof(Double_t)*512);

  fMCIDArray.clear();
  fMCWeightArray.clear();
  fMCTbArray.clear();

  ClearHits();

  fGrabed = false;
}

void KBPad::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (opts.Index("s")>=0) {
    kr_info(0) << "id:" << fID << " s:" << fSection << " r:" << fRow << " l:" << fLayer << endl;
    return;
  }

  kr_info(0) << "==" << endl;
  kr_info(0) << "Pad-ID(Plane-ID)      : " << fID << "(" << fPlaneID << ")";
  if (fActive) kb_out << " is Active!" << endl;
  else kb_out << " is NOT Active." << endl;

  kr_info(0) << "AsAd(1)AGET(1)CH(2)   : " << Form("%d%d%02d",fAsAdID,fAGETID,fChannelID) << endl;
  kr_info(0) << "(Section, Row, Layer) : (" << fSection << ", " << fRow << ", " << fLayer << ")" << endl;
  kr_info(0) << "Noise-Amp | BaseLine  : " << fNoiseAmp << " | " << fBaseLine << endl;
  kr_info(0) << "Position              : (" << fPosition.X() << ", " << fPosition.Y() << ", " << fPosition.Z() << ") ; " << fPosition.GetReferenceAxis() << endl;
  TString nbids; for (auto ii=0; ii<int(fNeighborPadArray.size()); ++ii) nbids += Form(" %d",fNeighborPadArray.at(ii)->GetPadID());
  kr_info(0) << "Neighbors             : " << nbids << "(" << fNeighborPadArray.size() << ")" << endl;

  if (opts.Index(">")>=0) {
    Int_t numMCID = fMCIDArray.size();
    kr_info(1) << "List of MC-IDs (co. Tb [mm]),  : ";
    for (auto iMC = 0; iMC < numMCID; ++iMC)
      kb_out << fMCIDArray.at(iMC) << "(" << fMCTbArray.at(iMC) << "), ";
    kb_out << endl;
  }
}

void KBPad::Draw(Option_t *option)
{
  kr_info(0) << "GetHist(o)->Draw(); DrawMCID(o); DrawHit(o);" << endl;

  GetHist(option) -> Draw();
  DrawMCID(option);
  DrawHit(option);
}

void KBPad::DrawMCID(Option_t *option)
{
  TString opts(option);
  opts.ToLower();

  Int_t numMCIDs = fMCIDArray.size();
  if (numMCIDs != 0 && opts.Index("mc") >= 0)
  {
    auto wgsum = 0.;
    for (auto iMC = 0; iMC < numMCIDs; ++iMC)
      wgsum += fMCWeightArray.at(iMC);
    auto line = new TLine(); line -> SetLineColor(kBlue-4);
    auto text = new TText(); text -> SetTextAlign(12); text -> SetTextFont(122);
    for (auto iMC = 0; iMC < numMCIDs; ++iMC) {
      auto id = fMCIDArray.at(iMC);
      auto tb = fMCTbArray.at(iMC);
      auto wg = fMCWeightArray.at(iMC);

      Int_t lw = 10*wg/wgsum;
      text -> DrawText(tb+1,4095*(0.9-iMC*0.1),Form("%d (%.2f | %d)",id,tb,lw));

      if (lw < 1) lw = 1;
      line -> SetLineWidth(lw);
      line -> DrawLine(tb,0,tb,4095*(0.9-iMC*0.1));
    }
  }
}

void KBPad::DrawHit(Option_t *option)
{
  TString opts(option);
  opts.ToLower();

  Int_t numHits = fHitArray.size();
  if (opts.Index("h") >= 0) {
    if (numHits == 0)
      kr_warning(0) << "No hit exist in pad." << endl;
    else {
      for (auto hit : fHitArray) {
        auto pulse = hit -> GetPulseFunction();
        pulse -> SetNpx(500);
        pulse -> Draw("samel");
      }
    }
  }
}

Bool_t KBPad::IsSortable() const { return true; }

Int_t KBPad::Compare(const TObject *obj) const
{
  /// By default, the pads should be sorted from the outer-side of the TPC to the inner-side of the TPC.
  /// This sorting is used in KBPadPlane::PullOutNextFreeHit().
  /// In track finding KBPadPlane::PullOutNextFreeHit() method is used 
  /// and the hits are pull from first index of the pad array to the last indes of the pad array
  /// assumming that the pads are sorted this way.
  /// Here we assume that the layer numbering is given from inner-side to the outer-side of the TPC
  /// in increasing order.

  auto pad2 = (KBPad *) obj;

  int sortThisPadLatterThanPad2 = 1;
  int sortThisPadEarlierThanPad2 = -1;
  int noChange = 0;

  if (fSortValue >= 0) {
         if (fSortValue < pad2 -> GetSortValue()) return sortThisPadEarlierThanPad2;
    else if (fSortValue > pad2 -> GetSortValue()) return sortThisPadLatterThanPad2;
    return noChange;
  }
       if (pad2 -> GetLayer() < fLayer) return sortThisPadEarlierThanPad2;
  else if (pad2 -> GetLayer() > fLayer) return sortThisPadLatterThanPad2;
  else //same layer
  {
         if (pad2 -> GetSection() > fSection) return sortThisPadEarlierThanPad2;
    else if (pad2 -> GetSection() < fSection) return sortThisPadLatterThanPad2;
    else // same layer, same section
    {
           if (pad2 -> GetRow() < fRow) return sortThisPadEarlierThanPad2;
      else if (pad2 -> GetRow() > fRow) return sortThisPadLatterThanPad2;
      else // same pad
        return noChange;
    }
  }
}

void KBPad::SetPad(KBPad *padRef)
{
  fID = padRef -> GetPadID();
  fPlaneID = padRef -> GetPlaneID();
  fAsAdID = padRef -> GetAsAdID();
  fAGETID = padRef -> GetAGETID();
  fChannelID = padRef -> GetChannelID();

  fPosition = padRef -> GetPosition();

  fSection = padRef -> GetSection();
  fRow = padRef -> GetRow();
  fLayer = padRef -> GetLayer();

  fBaseLine = padRef -> GetBaseLine();
  fNoiseAmp = padRef -> GetNoiseAmplitude();
}

void KBPad::CopyPadData(KBPad* padRef)
{
  SetBufferIn(padRef->GetBufferIn());
  SetBufferRaw(padRef->GetBufferRaw());
  SetBufferOut(padRef->GetBufferOut());

  auto is = padRef -> GetMCIDArray();
  auto ws = padRef -> GetMCWeightArray();
  auto ts = padRef -> GetMCTbArray();
  Int_t numMCID = is -> size();
  for (auto iMC = 0; iMC < numMCID; ++iMC) {
    fMCIDArray.push_back(is->at(iMC));
    fMCWeightArray.push_back(ws->at(iMC));
    fMCTbArray.push_back(ts->at(iMC));
  }
}

void KBPad::SetActive(bool active) { fActive = active; }
bool KBPad::IsActive() const { return fActive; }

void KBPad::SetPadID(Int_t id) { fID = id; }
Int_t KBPad::GetPadID() const { return fID; }

void KBPad::SetPlaneID(Int_t id) { fPlaneID = id; }
Int_t KBPad::GetPlaneID() const { return fPlaneID; }

void KBPad::SetAsAdID(Int_t id) { fAsAdID = id; }
Int_t KBPad::GetAsAdID() const { return fAsAdID; }

void KBPad::SetAGETID(Int_t id) { fAGETID = id; }
Int_t KBPad::GetAGETID() const { return fAGETID; }

void KBPad::SetChannelID(Int_t id) { fChannelID = id; }
Int_t KBPad::GetChannelID() const { return fChannelID; }

void KBPad::SetBaseLine(Double_t baseLine) { fBaseLine = baseLine; }
Double_t KBPad::GetBaseLine() const { return fBaseLine; }

void KBPad::SetNoiseAmplitude(Double_t gain) { fNoiseAmp = gain; }
Double_t KBPad::GetNoiseAmplitude() const { return fNoiseAmp; }

void KBPad::SetPosition(KBVector3 pos) { fPosition = pos; }

void KBPad::SetPosition(Double_t i, Double_t j)
{
  fPosition.SetI(i);
  fPosition.SetJ(j);
}

void KBPad::GetPosition(Double_t &i, Double_t &j) const
{
  i = fPosition.I();
  j = fPosition.J();
}

KBVector3 KBPad::GetPosition() const { return fPosition; }
Double_t KBPad::GetI() const { return fPosition.I(); }
Double_t KBPad::GetJ() const { return fPosition.J(); }
Double_t KBPad::GetK() const { return fPosition.K(); }
Double_t KBPad::GetX() const { return fPosition.X(); }
Double_t KBPad::GetY() const { return fPosition.Y(); }
Double_t KBPad::GetZ() const { return fPosition.Z(); }

void KBPad::AddPadCorner(Double_t i, Double_t j) { fPadCorners.push_back(TVector2(i,j)); }
vector<TVector2> *KBPad::GetPadCorners() { return &fPadCorners; }

void KBPad::SetSectionRowLayer(Int_t section, Int_t row, Int_t layer) 
{
  fSection = section;
  fRow = row;
  fLayer = layer;
}

void KBPad::GetSectionRowLayer(Int_t &section, Int_t &row, Int_t &layer) const 
{
  section = fSection;
  row = fRow;
  layer = fLayer;
}

Int_t KBPad::GetSection() const { return fSection; }
Int_t KBPad::GetRow() const { return fRow; }
Int_t KBPad::GetLayer() const { return fLayer; } 

void KBPad::FillBufferIn(Int_t tbin, Double_t val, Int_t trackID)
{
  fActive = true;
  fBufferIn[tbin] += val;
  if (trackID != -1) {
    Int_t numTracks = fMCIDArray.size();
    Int_t idxFound = -1;
    for (Int_t idx = 0; idx < numTracks; ++idx) {
      if (fMCIDArray[idx] == trackID) {
        idxFound = idx;
        break;
      }
    }
    if (idxFound == -1) {
      fMCIDArray.push_back(trackID);
      fMCWeightArray.push_back(val);
      fMCTbArray.push_back(tbin);
    }
    else {
      auto w = fMCWeightArray[idxFound];
      auto tb = fMCTbArray[idxFound];

      auto ww = w + val;
      fMCWeightArray[idxFound] = ww;

      tb = (w*tb + val*tbin)/ww;
      fMCTbArray[idxFound] = tb;
    }
  }
}

void KBPad::SetBufferIn(Double_t *buffer) { memcpy(fBufferIn, buffer, sizeof(Double_t)*512); }
Double_t *KBPad::GetBufferIn() { return fBufferIn; }

void KBPad::SetBufferRaw(Short_t *buffer) { memcpy(fBufferRaw, buffer, sizeof(Short_t)*512); }
Short_t *KBPad::GetBufferRaw() { return fBufferRaw; }

void KBPad::SetBufferOut(Double_t *buffer) { memcpy(fBufferOut, buffer, sizeof(Double_t)*512); }
Double_t *KBPad::GetBufferOut() { return fBufferOut; }

void KBPad::AddNeighborPad(KBPad *pad) { fNeighborPadArray.push_back(pad); }
vector<KBPad *> *KBPad::GetNeighborPadArray() { return &fNeighborPadArray; }

void KBPad::AddHit(KBTpcHit *hit) { fHitArray.push_back(hit); }
Int_t KBPad::GetNumHits() const { return fHitArray.size(); }
KBTpcHit *KBPad::GetHit(Int_t idx) { return fHitArray.at(idx); }
void KBPad::ClearHits() { fHitArray.clear(); }

KBTpcHit *KBPad::PullOutNextFreeHit()
{
  Int_t n = fHitArray.size();
  if (n == 0)
    return nullptr;

  for (auto i = 0; i < n; i++) {
    auto hit = fHitArray[i];
    if (hit -> GetNumTrackCands() == 0) {
      fHitArray.erase(fHitArray.begin()+i);
      return hit;
    }
  }

  return nullptr;
}

void KBPad::PullOutHits(KBHitArray *hits)
{
  Int_t n = fHitArray.size();
  if (n == 0)
    return;

  for (auto i = 0; i < n; i++)
    hits -> AddHit(fHitArray[i]);
  fHitArray.clear();
}

void KBPad::PullOutHits(vector<KBTpcHit *> *hits)
{
  Int_t n = fHitArray.size();
  if (n == 0)
    return;

  for (auto i = 0; i < n; i++)
    hits -> push_back(fHitArray[i]);
  fHitArray.clear();
}

bool KBPad::IsGrabed() const { return fGrabed; }
void KBPad::Grab() { fGrabed = true; }
void KBPad::LetGo() { fGrabed = false; }

TH1D *KBPad::GetHist(Option_t *option)
{
  TH1D *hist = new TH1D(Form("Pad%03d",fID),"",512,0,512);
  SetHist(hist, option);

  return hist;
}

void KBPad::SetHist(TH1D *hist, Option_t *option)
{
  //kr_info(0) << "option: p(ID) && a(ID2) && mc(MC) && [o(out) || r(raw) || i(input)] && h(hit)" << endl;
  hist -> Reset();

  TString opts(option);
  opts.ToLower();

  TString namePad = Form("Pad%03d",fID);
  TString nameID = Form("ID%d%d%02d",fAsAdID,fAGETID,fChannelID);

  bool firstNameOn = false;

  TString name;

  if (opts.Index("p") >= 0) {
    name = name + namePad;
    firstNameOn = true;
  }

  if (opts.Index("a") >= 0) {
    if (firstNameOn) name = name + "_";
    name = name + nameID;
    firstNameOn = true;
  }

  if (firstNameOn == false)
    name = namePad;

  hist -> SetNameTitle(name,name+";Time Bucket;ADC");

  if (opts.Index("o") >= 0) {
    for (auto tb = 0; tb < 512; tb++)
      hist -> SetBinContent(tb+1, fBufferOut[tb]);
  } else if (opts.Index("r") >= 0) {
    for (auto tb = 0; tb < 512; tb++)
      hist -> SetBinContent(tb+1, fBufferRaw[tb]);
  } else if (opts.Index("i") >= 0) {
    for (auto tb = 0; tb < 512; tb++)
      hist -> SetBinContent(tb+1, fBufferIn[tb]);
  }

  Double_t maxHeight = 4095;
  hist -> GetYaxis() -> SetRangeUser(0,maxHeight);
}
