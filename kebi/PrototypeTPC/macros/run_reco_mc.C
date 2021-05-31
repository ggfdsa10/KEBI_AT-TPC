void run_reco_mc(TString name = "single")
{
  auto run = new KBRun();
  run -> SetIOFile(name+".digi", name+".recomc");
  run -> AddDetector(new LAPTpc());

  auto psa = new KBPSATask();
  psa -> SetHitPersistency(true);
  psa -> SetPSA(new KBPSAFastFit);
  run -> Add(psa);

  run -> Add(new LAPBeamTrackingTask);

  run -> Init();
  run -> Run();
}
