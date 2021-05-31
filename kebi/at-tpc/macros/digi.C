void digi(TString name = "ATTPC")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".digi");
  run -> AddDetector(new ATTPC());

  auto parameter = new ATTPCSetupParameter();
  
  auto drift = new ATTPCDriftElectron();
  drift -> SetPadPersistency(true);

  auto electronics = new ATTPCElectronics(true);

  run -> Add(parameter);
  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
 
}
