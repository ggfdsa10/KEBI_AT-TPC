Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

void write()
{
  KBRun::GetRun() -> WriteCvsDetectorPlanes("pdf");
}

void run_eve(TString name = "iqmd_test")
{
  auto run = new KBRun();
  run -> SetInputFile(name+".reco");
  //run -> AddFriend(name+".digi");
  //run -> SetInputFile(name+".mc");
  //run -> AddFriend(name+".mc");

  run -> AddDetector(new LHTpc());
  run -> SetTag("eve");
  run -> Init();
  run -> AddPar("kbpar_eve.conf");
  run -> SetGeoTransparency(80);
  run -> Print();
  run -> RunEve(fEventID);
}
