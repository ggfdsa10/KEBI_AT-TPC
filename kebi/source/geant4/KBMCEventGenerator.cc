#include "KBMCEventGenerator.hh"
#include "KBG4RunManager.hh"
#include "KBMCTrack.hh"
#include "TSystem.h"

KBMCEventGenerator::KBMCEventGenerator()
{
}

KBMCEventGenerator::KBMCEventGenerator(TString fileName)
{
  fInputFile.open(fileName.Data());

  TString me;
  fInputFile >> me;
  me.ToLower();

       if (me == "p") fReadMomentumOrEnergy = true;
  else if (me == "e") fReadMomentumOrEnergy = false;

  fInputFile >> fNumEvents;

  g4_info << fileName << " containing " << fNumEvents << " events, initialized with " << me << endl;
}

KBMCEventGenerator::~KBMCEventGenerator()
{
  if(fInputFile.is_open()) fInputFile.close();
}

bool KBMCEventGenerator::ReadNextEvent(Double_t &vx, Double_t &vy, Double_t &vz)
{
  Int_t eventID;
  if (!(fInputFile >> eventID >> fNumTracks >> vx >> vy >> vz))
    return false;

  fCurrentTrackID = 0;
  return true;
}

bool KBMCEventGenerator::ReadNextTrack(Int_t &pdg, Double_t &px, Double_t &py, Double_t &pz)
{
  if (fCurrentTrackID >= fNumTracks)
    return false;

  fInputFile >> pdg >> px >> py >> pz;
  fCurrentTrackID++;

  return true;
}
