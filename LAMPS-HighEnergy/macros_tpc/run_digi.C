void run_digi(TString name = "LHmulti")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mcSingle", name+".digiSingle");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask(true);

  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
}
