void reco_single_findPar(TString input = "NewTPC"){

    auto run = new KBRun();
    run -> SetIOFile(input+".digi_single", input+".reco_single");
    // run->SetParameterContainer("$KEBIPATH/newTPC/macros/input/par_NewTPC.conf");
    run -> AddDetector(new NewTPC());


    auto psa = new NewTPCPSATask();
    psa -> SetPSA(new NewTPCPSAFastFit());
    run -> Add(psa);

    // run -> Add(new ATTPCHelixTrackFindingTask());
    run->Add( new KBSingleHelixTask());
    run -> Init();
    run -> Run();
}
