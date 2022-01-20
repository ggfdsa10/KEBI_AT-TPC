<<<<<<< HEAD
void run_digi(TString name = "iqmd_test")
=======
void run_digi(TString name = "single_proton")
>>>>>>> 2079b1e984042cd756ba71efe7768120e39f4343
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".digi");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask(true);

  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
}
