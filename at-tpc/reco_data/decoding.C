void decoding(int eventNum = -1, TString ExpName = "MuonTest")
{
  TString fileName = "ATTPCEXP";
  auto run = new KBRun();
  run -> SetOutputFile(fileName+Form(".raw%s", ExpName.Data()));
  run -> SetTag(ExpName);
  run -> AddPar("attpc_Exp.par");
  run -> AddDetector(new ATTPC());

  auto parSetup = new ATTPCSetupParameter();

  auto decoder = new ATTPCDecoderTask();
  decoder -> SetNumEvents(eventNum);
  decoder -> SetPadPersistency(true);

  auto subtractor = new ATTPCNoiseSubtractTask();

  auto pulseFinder = new ATTPCPSATask();
  pulseFinder->SetHitPersistency(true);
  pulseFinder -> SetPSA(new ATTPCPSAFastFit());

  auto recontructer = new ATTPCHelixTrackFindingTask();

  run -> Add(parSetup);
  run -> Add(decoder);
  // run -> Add(subtractor);
  // run -> Add(pulseFinder);
  // run -> Add(recontructer);
  
  run -> Init();
  // decoder -> GetDate();
  run -> Run();
  

}
