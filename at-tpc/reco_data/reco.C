void reco(int eventNum = -1, TString ExpName = "Muon")
{
  TString fileName = "ATTPCEXP";
  auto run = new KBRun();
  run -> SetInputFile(fileName+Form(".raw%s", ExpName.Data()));
  run -> SetOutputFile(fileName+Form(".raw%s_reco", ExpName.Data()));
  run -> SetTag(ExpName);
  run -> AddDetector(new ATTPC());

  auto recontructer = new ATTPCHelixTrackFindingTask();

  run -> Add(recontructer);

  run -> Init();
  run -> Run();
  

}
