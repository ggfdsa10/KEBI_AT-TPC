int fEventID = 0;
bool fRunFullEvent = false;
bool fRunStep = true;
bool fRunToNumTrack = true;

LHHelixTrackFindingTask *fTrackFinding;

TCanvas *cvs = nullptr;
TGraphErrors *graph = nullptr;

KBHelixTrack *track() {
  return fTrackFinding -> GetCurrentTrack();
}

void draw()
{
  auto currentTrack = fTrackFinding -> GetCurrentTrack();

  if (currentTrack == nullptr)
    return;

  if (cvs==nullptr) {
    cvs = new TCanvas("currentTrack","",700,700);
    graph = new TGraphErrors();
    graph -> SetMarkerStyle(20);
  }
  cvs -> cd();
  graph -> Set(0);

  auto numHits = currentTrack -> GetNumHits();
  for (auto iHit=0; iHit<numHits; ++iHit) {
    auto hit = currentTrack -> GetHit(iHit);
    graph -> SetPoint(graph -> GetN(), hit -> GetX(), hit -> GetY());
  }

  if (currentTrack -> GetNumHits() > 3) {
    graph -> Draw("ap");
    auto graphTrack = currentTrack -> TrajectoryOnPlane(KBVector3::kX,KBVector3::kY);
    graphTrack -> Draw("samel");
  }
}

void step(bool dodraw = true) {
  fTrackFinding -> ExecStep();
  if (dodraw) {
    KBRun::GetRun() -> RunEve(fEventID,"e");
    draw();
  }
}

void write() {
  auto currentTrack = fTrackFinding -> GetCurrentTrack();
  if (currentTrack == nullptr)
    return;

  TFile *file = new TFile("current_track.root","recreate");
  currentTrack -> Write("helix");
  currentTrack -> GetHitArray() -> Write("hitList",TObject::kSingleKey);
  file -> ls();
}

void run_tf_debug(TString name = "iqmd_test")
{
  auto run = new KBRun();
  run -> AddDetector(new LHTpc());

  //run -> SetInputFile(name+".psa");
  //run -> SelectEveBranches("CandHit:GoodHit:BadHit:TrackHit:Tracklet:Hit");

  //run -> SetInputFile(name+".reco");
  run -> SetInputFile(name+".hit");
  //run -> AddFriend(name+".reco");
  //run -> AddFriend(name+".mc");
  //run -> SelectEveBranches("CandHit:GoodHit:BadHit:TrackHit:Tracklet:Hit_MC116");
  //run -> SelectEveBranches("CandHit:GoodHit:BadHit:Tracklet:Hit");

  fTrackFinding = new LHHelixTrackFindingTask();
  //fTrackFinding -> SetHitBranchName("Hit_MC166");
  //fTrackFinding -> SetHitBranchName("Hit_MC52");
  //fTrackFinding -> SetHitBranchName("Hit_MC37");
  fTrackFinding -> SetHitBranchName("Hit_MC116");
  run -> Add(fTrackFinding);

  run -> SetTag("eve_tf");
  run -> Init();
  //run -> Print(); return;
  run -> AddPar("kbpar_eve.conf");


  if (fRunFullEvent) {
    run -> Event(fEventID);
    run -> RunEve(fEventID);
  }
  else if (fRunStep) {
    run -> RunEve(fEventID);
    //for (auto i=0; i<27; ++i) step(false);
    for (auto i=0; i<3; ++i) step(false);
    step();
  }
  else if (fRunToNumTrack) {
    run -> GetEntry(fEventID);
    for (auto i=0; i<100; ++i) {
      cout << "numTracks: " << i << endl;
      if (fTrackFinding->ExecStepUptoTrackNum(i)==false) {
        break;
      }
    }
  }
}
