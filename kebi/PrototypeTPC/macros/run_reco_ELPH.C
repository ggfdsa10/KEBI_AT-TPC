void run_reco_ELPH(Int_t runID = 98, Int_t split = 0, Int_t numEventsPerSplit=20)
{
  auto run = new KBRun();
  run -> SetRunID(runID);
  run -> SetTag("test");
  run -> SetSplit(split, numEventsPerSplit);
  run -> AddPar("prototype_ELPH.par");
  run -> AddDetector(new LAPTpc());

  auto decoder = new LAPDecoderTask();
  decoder -> SetPadPersistency(true);
  run -> Add(decoder);

  auto noiseSubtraction = new LAPNoiseSubtractionTask();
  run -> Add(noiseSubtraction);

  auto psa = new KBPSATask();
  psa -> SetHitPersistency(true);
  psa -> SetPSA(new KBPSAFastFit);
  run -> Add(psa);

  run -> Add(new LAPBeamTrackingTask);

  run -> Init();
  run -> Run();
}
