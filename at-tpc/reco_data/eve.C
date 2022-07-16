Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
    if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
    else KBRun::GetRun() -> RunEve(fEventID = eventID);
    
    auto fHitArray = KBRun::GetRun() -> GetBranchA("Pad");
    cout << "fHitArray.size() " << fHitArray -> GetEntries() << endl;

    if(fHitArray -> GetEntries()==0 || fHitArray==nullptr){
        cout << fEventID << " event is null data, so skip." << endl;
        KBRun::GetRun() -> RunEve(++fEventID);
    }

    cout << "This event " << fEventID << endl;
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
    run -> AddDetector(new ATTPC());
    run -> SetTag("eve");
    run -> Init();
    run -> SetGeoTransparency(80);
    run -> Print();
    run -> RunEve(fEventID);

}
