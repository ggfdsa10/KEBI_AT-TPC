void run_digi(TString name = "single")
{
  auto run = KBRun::GetRun();
  run -> AddPar("prototype_g4.par");
  run -> SetIOFile(name+".mc", name+".digi");
  run -> AddDetector(new LAPTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask(true);

  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
}
