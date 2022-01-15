Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

// the last output from KBRun is used if input is "last".
void eve(TString input = "dummy.mc")
{
  auto run = new KBRun();
  run -> SetInputFile(input);
  run -> SetTag("eve");
  run -> AddDetector(new DUMMYDetector());
  run -> Init();
  run -> SetGeoTransparency(80);
  run -> RunEve(fEventID);
}
