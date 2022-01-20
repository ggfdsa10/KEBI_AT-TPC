Int_t fEventID = 0;

void next(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}


void run_eve(TString name = "last")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> AddPar("prototype_eve.par");
  run -> SetTag("eve");
  run -> AddDetector(new LAPTpc());
  run -> Init();
  run -> SetGeoTransparency(80);
  run -> Print();
  run -> RunEve(fEventID);
}
