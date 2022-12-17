void digi(TString name = "NewTPC")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".digi");
  run -> AddDetector(new NewTPC());

  auto parameter = new NewTPCSetupParameter();
  
  auto drift = new NewTPCDriftElectron();
  drift -> SetPadPersistency(true);

  auto electronics = new NewTPCElectronics(true);

  run -> Add(parameter);
  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
 
}
