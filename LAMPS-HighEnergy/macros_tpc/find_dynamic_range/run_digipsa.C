void run_digipsa(TString name = "single_proton")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".psa");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask(false);

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSA());

  run -> Add(drift);
  run -> Add(electronics);
  run -> Add(psa);

  run -> Init();
  run -> Run();
}
