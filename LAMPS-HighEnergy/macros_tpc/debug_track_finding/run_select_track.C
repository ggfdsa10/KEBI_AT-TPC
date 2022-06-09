void run_select_track(TString name = "LHmulti")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digiSingle", name+".hit");
  run -> AddDetector(new LHTpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());
  psa -> SetHitPersistency(true);
  run -> Add(psa);

  run -> Add(new LHSelectHitsBelongToMCIDTask(1));

  run -> Init();
  run -> Run();
}
