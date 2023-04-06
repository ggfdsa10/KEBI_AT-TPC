void decoding(int eventNum = 100, TString ExpName = "HIMAC")
{
  TString fileName = "ATTPCEXP";
  auto run = new KBRun();
  run -> SetOutputFile(fileName+Form(".%s", ExpName.Data()));
  run -> SetTag(ExpName);
  run -> AddPar("attpc_Exp.par");
  run -> AddDetector(new ATTPC());

  auto parSetup = new ATTPCSetupParameter();

  // auto decoder = new ATTPCDecoderTask();
  // decoder -> SetNumEvents(eventNum);
  // decoder -> SetPadPersistency(true);

  auto decoder = new ATTPCRootDecoder();
  decoder -> SetNumEvents(eventNum);
  decoder -> SetPadPersistency(true);

  auto subtractor = new ATTPCNoiseSubtractTask();

  auto pulseFinder = new ATTPCPSATask();
  pulseFinder->SetHitPersistency(true);
  pulseFinder -> SetPSA(new ATTPCPSAFastFit());

  run -> Add(parSetup);
  run -> Add(decoder);
  // run -> Add(subtractor);
  // run -> Add(pulseFinder);

  run -> Init();
  run -> Run();
  

}
