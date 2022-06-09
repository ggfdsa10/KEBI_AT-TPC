Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
    if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
    else KBRun::GetRun() -> RunEve(fEventID = eventID);
    cout << "Event " << fEventID << endl;
}

void AllEventSave(){  
    Long64_t eventNum = KBRun::GetRun() -> GetNumEvents();

    for(int event =0; event <eventNum; event++){
        KBRun::GetRun() -> RunEve(event);
        KBRun::GetRun() -> WriteCvsDetectorPlanes("png");
    }
}

void eve(TString input = "ATTPCEXP")
{
    TString version = "MuonTest";

    auto run = new KBRun();
    run -> SetInputFile(input+Form(".raw%s", version.Data()));
    run -> AddPar("$KEBIPATH/at-tpc/reco_data/input/par_Exp_at-tpc.conf");
    run -> AddDetector(new ATTPC());
    run -> SetTag("eve");
    run -> Init();
    run -> AddPar("$KEBIPATH/at-tpc/macros/input/par_at-tpc_eve.conf");
    run -> SetGeoTransparency(80);
    run -> Print();
    run -> RunEve(fEventID);
}
