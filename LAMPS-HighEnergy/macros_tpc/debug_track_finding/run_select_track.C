void run_select_track(TString name = "iqmd_test")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digi", name+".hit");
  run -> AddDetector(new LHTpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());
  psa -> SetHitPersistency(false);
  run -> Add(psa);

  run -> Add(new LHSelectHitsBelongToMCIDTask(116));

  run -> Init();
  run -> Run();
}
