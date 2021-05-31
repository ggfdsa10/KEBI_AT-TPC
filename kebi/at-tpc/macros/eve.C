Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

// the last output from KBRun is used if input is "last".
void eve(TString input = "ATTPC")
{
  auto run = new KBRun();
  run -> SetInputFile(input+".digi");
  run -> AddFriend(input+".mc");
  //run -> AddFriend(name+".digi");

  run -> AddDetector(new ATTPC());
  run -> SetTag("eve");
  run -> Init();
  run -> AddPar("input/par_at-tpc_eve.conf");
  run -> SetGeoTransparency(80);
  run -> Print();
  run -> RunEve(fEventID);
}
