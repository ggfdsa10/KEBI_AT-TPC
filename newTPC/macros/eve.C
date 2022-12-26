Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

void AllEventSave(){  
  Long64_t eventNum = KBRun::GetRun() -> GetNumEvents();

  for(int event =0; event <eventNum; event++){
    KBRun::GetRun() -> RunEve(event);
    KBRun::GetRun() -> WriteCvsDetectorPlanes("pdf");
  }
}

void eve(TString input = "NewTPC")
{
  auto run = new KBRun();
  run -> SetInputFile(input+".mc");
  run -> AddFriend(input+".digi");

  run -> AddDetector(new NewTPC());
  run -> SetTag("eve");
  run -> Init();
  run -> AddPar("input/par_NewTPC_eve.conf");
  run -> SetGeoTransparency(80);
  run -> Print();
  run -> RunEve(fEventID);
}
