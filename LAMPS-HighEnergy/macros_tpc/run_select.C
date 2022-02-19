void run_select(TString name = "iqmd_test", int mcid = 175)
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".select");
  run -> AddDetector(new LHTpc());

  auto select = new LHSelectHitsBelongToMCIDTask(mcid);

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask(true);

  run -> Add(select);
  //run -> Add(drift);
  //run -> Add(electronics);

  run -> Init();
  run -> Run();
}
