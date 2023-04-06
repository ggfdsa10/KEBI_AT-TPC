void digi(TString name = "ATTPC")
{
  TString version = "_PointZ";

  auto run = KBRun::GetRun();
  run -> SetIOFile(name+Form(".mc%s",version.Data()), name+Form(".digi%s",version.Data()));
  run -> AddDetector(new ATTPC());
  
  auto parameter = new ATTPCSetupParameter();
  
  auto drift = new ATTPCDriftElectron();
  drift -> SetPadPersistency(true);

  auto electronics = new ATTPCElectronics();
  auto subtractor = new ATTPCNoiseSubtractTask();

  auto psa = new ATTPCPSATask();
  psa->SetHitPersistency(true);
  psa -> SetPSA(new ATTPCPSAFastFit());

  run -> Add(parameter);
  run -> Add(drift);
  run -> Add(electronics);
  run -> Add(subtractor);
  run -> Add(psa);
  
  run -> Init();
  run -> Run();
 
}
