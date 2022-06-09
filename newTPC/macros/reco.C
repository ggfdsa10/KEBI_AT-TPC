
void reco(TString input = "NewTPC")
{
    auto run = new KBRun();
    run -> SetIOFile(input+".digi_two", input+".reco_two");
    run -> AddDetector(new NewTPC());
    run -> GetParameterContainer() -> SetPar("ADCThreshold", 10.);
    run -> GetParameterContainer() -> SetPar("LHTF_defaultScale", 2.5);
    
    auto psa = new NewTPCPSATask();
    psa -> SetPSA(new NewTPCPSAFastFit());

    run -> Add(psa);
    run -> Add(new NewTPCHelixTrackFindingTask());

    run -> Init();
    run -> Run();
}