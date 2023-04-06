void test()
{
    TString fileName = "ATTPCEXP";
    TString ExpName = "MuonTest";
    auto rawData = new TChain("event");
    rawData -> Add(Form("$KEBIPATH/data/ATTPCEXP.raw%s.4efa23c.root",ExpName.Data()));
    TClonesArray *rawArray = nullptr;
    rawData -> SetBranchAddress("Pad",&rawArray);
    
    auto h1 = new TH1D("","",512,0,512);

    auto file = new TFile("./noise.root","recreate");
    auto tree = new TTree("noise","noise");
    Double_t noiseArray[4][512];
    tree -> Branch("noiseArray", noiseArray, "noise[4][512]/D");

    int eventNum = rawData->GetEntries();
    for(int event = 0; event < eventNum; event++){  
        rawData -> GetEntry(event);

        int idx = 0;
        for(int i=0; i<256; i++){
            auto pad = (KBPad *) rawArray -> ConstructedAt(i);
            auto noise = pad->GetBufferIn();

            Double_t sum = 0.;
            for(int tb=0; tb<512; tb++){
                sum += noise[tb];
            }
            if(sum > 0.1){
                for(int tb=0; tb<512; tb++){
                    noiseArray[idx][tb] = noise[tb];
                }
                idx++;
            }
        }

        tree -> Fill();
    }

    tree -> Write();
    file -> Close();

}

