void run_reco(TString name = "LHmulti")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digiSingle", name+".recoSingle");
  run -> AddDetector(new LHTpc());

    auto psa = new KBPSATask();
    // psa -> SetInputBranchName("TPCPad");
    // psa -> SetOutputBranchName("TPCHit");
    psa -> SetPSA(new KBPSAFastFit());
    run->Add(psa);

    auto htfTask = new LHHelixTrackFindingTask();
    // htfTask -> SetHitBranchName("TPCHit");
    // htfTask -> SetTrackletBranchName("Tracklet");
    run->Add(htfTask);

    // auto gfTask = new LHGenfitTask();
    // gfTask->SetDetID(10); //TPC
    // run->Add(gfTask);

    run->Init();
    run->Run();
}
