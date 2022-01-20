<<<<<<< HEAD
void run_reco(TString name = "iqmd_test")
=======
void run_reco(TString name = "single_proton")
>>>>>>> 2079b1e984042cd756ba71efe7768120e39f4343
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digi", name+".reco");
  run -> AddDetector(new LHTpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());
  run -> Add(psa);

  run -> Add(new LHHelixTrackFindingTask());

  run -> Add(new LHVertexFindingTask());
  run -> Add(new LHGenfitTask());

  run -> Init();
  run -> Run();
}
