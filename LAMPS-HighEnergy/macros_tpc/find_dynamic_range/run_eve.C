Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

void run_eve(TString name = "single_proton.psa")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> SetTag("eve");
  //run -> SelectEveBranches("Tracklet:Hit");
  run -> SelectEveBranches("Hit");
  run -> AddDetector(new LHTpc());
  run -> Init();
  //run -> AddPar("kb_eve.par");
  run -> SetGeoTransparency(80);
  run -> RunEve(fEventID);
}
