
TCanvas *cvsCurrentTrack = nullptr;
TGraphErrors *graphHits = nullptr;
ATTPCHelixTrackFindingTask *trackFindingTask;
int fEventID = 0;


void step()
{
  trackFindingTask -> ExecStep();
  KBRun::GetRun() -> RunEve(fEventID);

  auto currentTrack = trackFindingTask -> GetCurrentTrack();
  if (currentTrack == nullptr) return;

  cvsCurrentTrack -> cd();
  graphHits -> Set(0);
  auto numHits = currentTrack -> GetNumHits();
  for (auto iHit=0; iHit<numHits; ++iHit) {
    auto hit = currentTrack -> GetHit(iHit);
    graphHits -> SetPoint(graphHits->GetN(), hit->GetX(), hit->GetY());
  }
  if (currentTrack -> GetNumHits() > 3) {
    graphHits -> Draw("ap");
    auto graphHitTrack = currentTrack -> TrajectoryOnPlane(KBVector3::kX,KBVector3::kY);
    graphHitTrack -> Draw("samel");
  }
}

void nextEvent(int event = 0){
  if(event >0){fEventID = event;}
  else{  fEventID++;}
  cout  << " current event : " << fEventID << endl;
}

void run_tf_debug(TString name = "ATTPC")
{
  trackFindingTask = new ATTPCHelixTrackFindingTask();
  trackFindingTask -> SetHitBranchName("Hit");

  auto run = new KBRun();
  run -> AddDetector(new ATTPC());
  run -> SetInputFile(name+".hit");
  run -> SetOutputFile(name+".test");
  run -> Add(trackFindingTask);
  run -> SetTag("eve_tf");
  run -> AddPar("kbpar_eve2.conf");
  run -> Init();

  trackFindingTask -> SetActive(false);
  run -> RunEve(fEventID,"e");

  cvsCurrentTrack = new TCanvas("cvsEventTrack","",700,700);
  graphHits = new TGraphErrors();
  graphHits -> SetMarkerStyle(20);

  //for (auto i=0; i<1310; ++i) trackFindingTask -> ExecStep();
  step();
}
