void digi(TString name = "ATTPC")
{
  TString version = "_zDist";

  auto run = KBRun::GetRun();
  run -> SetIOFile(name+Form(".mc%s",version.Data()), name+Form(".digi%s",version.Data()));
  run -> AddDetector(new ATTPC());
  run -> AddPar("$KEBIPATH/at-tpc/macros/input/par_at-tpc.conf");
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
