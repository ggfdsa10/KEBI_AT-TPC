
void reco(TString input = "ATTPC")
{

    TString version = "_zDist";

    auto run = new KBRun();
    run -> SetIOFile(input+Form(".digi%s", version.Data()), input+Form(".reco%s", version.Data()));
    run -> AddDetector(new ATTPC());

    auto psa = new ATTPCPSATask();
    psa->SetHitPersistency(true);
    psa -> SetPSA(new ATTPCPSAFastFit());
    run -> Add(psa);

    run -> Add(new ATTPCHelixTrackFindingTask());
    // // run -> Add(new ATTPCVertexFindingTask());
    // run -> Add(new ATTPCGenfitTask());

    run -> Init();
    run -> Run();
}