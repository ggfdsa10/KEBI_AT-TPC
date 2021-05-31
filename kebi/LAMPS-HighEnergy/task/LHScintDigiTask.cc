#include "LHScintDigiTask.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"

#include <iostream>
using namespace std;

ClassImp(LHScintDigiTask)

LHScintDigiTask::LHScintDigiTask()
:KBTask("LHScintDigiTask","")
{
}

bool LHScintDigiTask::Init()
{
  return true;
}

void LHScintDigiTask::Exec(Option_t*)
{
}

void LHScintDigiTask::SetChannelPersistency(bool persistence) { fPersistency = persistence; }
