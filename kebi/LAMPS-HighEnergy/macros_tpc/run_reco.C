void run_reco(TString name = "iqmd_test")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digi", name+".reco");
  run -> AddDetector(new LHTpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());
  run -> Add(psa);

  run -> Add(new LHHelixTrackFindingTask());

  run -> Add(new LHVertexFindingTask());

  run -> Init();
  run -> Run();
}
