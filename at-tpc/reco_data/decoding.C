void decoding(Int_t eventNum = 100, TString ExpName = "MuonTest")
{
  TString fileName = "ATTPCEXP";
  auto run = new KBRun();
  run -> SetOutputFile(fileName+Form(".raw%s", ExpName.Data()));
  run -> SetTag(ExpName);
  run -> AddPar("attpc_Exp.par");
  run -> AddPar("$KEBIPATH/at-tpc/reco_data/input/par_Exp_at-tpc.conf");
  run -> AddDetector(new ATTPC());

  auto decoder = new ATTPCDecoderTask();
  decoder -> SetNumEvents(eventNum);
  decoder -> SetPadPersistency(true);
  run -> Add(decoder);

  run -> Init();

  decoder -> GetDate();
  run -> Run();


}
