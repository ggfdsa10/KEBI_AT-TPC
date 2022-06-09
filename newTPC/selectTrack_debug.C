
void selectTrack_debug(TString name = "NewTPC")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digi", name+".hit");
  run -> AddDetector(new ATTPC());

  auto psa = new ATTPCPSATask();
  psa -> SetPSA(new ATTPCPSAFastFit());
  psa -> SetHitPersistency(true);
  run -> Add(psa);

  run -> Add(new LHSelectHitsBelongToMCIDTask(1));

  run -> Init();
  run -> Run();
}