Double_t errorfunc(Double_t *x, Double_t *par){
    Double_t error = par[0]*ROOT::Math::gaussian_cdf_c(x[0],par[1],par[2]);
    return error;
}

Double_t gammafunc(Double_t *x, Double_t *par){
    Double_t gamma = par[0]*ROOT::Math::gamma_pdf(x[0],par[1],par[2],par[3]);
    return gamma;
}


void parDisplay(){

    auto trackByHits = new TChain("trackByHits");
    trackByHits -> Add("$KEBIPATH/data/NewTPC.reco_single.2e5c877.root");
    Double_t rmsW1, rmsW2, rmsH;

    trackByHits -> SetBranchAddress("rmsW1",&rmsW1);
    trackByHits -> SetBranchAddress("rmsW2",&rmsW2);
    trackByHits -> SetBranchAddress("rmsH",&rmsH);

    auto track = new TChain("track");
    track -> Add("$KEBIPATH/data/NewTPC.reco_single.2e5c877.root");
    Double_t l, n, rms, rmst, rmsr;
    track -> SetBranchAddress("l",&l);
    track -> SetBranchAddress("n",&n);
    track -> SetBranchAddress("rms",&rms);
    track -> SetBranchAddress("rmst",&rmst);
    track -> SetBranchAddress("rmsr",&rmsr);

    auto HitArray = new TChain("event");
    HitArray -> Add("$KEBIPATH/data/NewTPC.reco_single.2e5c877.root");
    TClonesArray *array = nullptr;
    HitArray -> SetBranchAddress("Hit",&array);

    auto w1 = new TH1D("","",600, -20,20);
    auto w2 = new TH1D("","",1000, -20,20);
    auto h1 = new TH1D("","",1000, -0,500);

    auto rmsH1 = new TH1D("","",100, 0,10);
    auto rmstH1 = new TH1D("","",100, 0,10);
    auto rmsrH1 = new TH1D("","",100, 0,7);

    auto hitbyLayer = new TH1I("","",10,0,50);

    int eventHitsNum = trackByHits->GetEntries();
    cout << eventHitsNum << endl;
    for(int event=0; event<eventHitsNum; event++){
        trackByHits->GetEntry(event);

        w1->Fill(rmsW1);
        w2->Fill(rmsW2);
        h1->Fill(rmsH);
    }

    int eventNum = track->GetEntries();
    for(int event=0; event<eventNum; event++){
        if(event%100 ==0){cout << "event : " << event << endl;}

        track->GetEntry(event);
        HitArray->GetEntry(event);

        rmsH1->Fill(rms);
        rmstH1->Fill(rmst);
        rmsrH1->Fill(rmsr);


        int HitNums = array -> GetEntriesFast();
        // cout << HitNums << endl;
        int layers =0;
        int hitNumByLayer = 0;
        for(int iHit = 0; iHit<HitNums; iHit++){
            KBTpcHit *padArray = (KBTpcHit *) array -> ConstructedAt(iHit);
            padArray -> GetX();
            int thisPadLayer = padArray -> GetLayer();

            if(padArray->GetCharge() <= 10.){continue;
            }
            if(layers == thisPadLayer){
                hitNumByLayer++;
            }
            else{
                hitbyLayer->Fill(hitNumByLayer);
                hitNumByLayer = 1;
                layers++;
            }
        }

    }

    auto c1 = new TCanvas("","",1800,600);
    c1->Divide(3,1);
    c1->cd(1);
    w1->SetTitle("Displacement (HelixX-PadPosX);X [mm]; Entry");
    
    TF1 *f1 = new TF1("f1",errorfunc,3.9, 9, 3);
    f1->SetParLimits(0, 6500, 15000);
    f1->SetParLimits(1, 0., 1.5);
    f1->SetParLimits(2, 3.8,4.5);

    // w1->Fit(f1,"R");
    w1->Draw();
    gStyle->SetOptFit(0011);

    c1->cd(2);
    w2->SetTitle("Displacement (HelixZ-PadPosZ);Z [mm]; Entry");
    TF1 *f2 = new TF1("f2","expo",0., 2);
    // w2->Fit(f2, "R");
    w2->Draw();
    gStyle->SetOptFit(0011);    

    c1->cd(3);
    h1->Draw();

    c1->Draw();


    auto c2 = new TCanvas("","",1800,600);
    c2->Divide(3,1);
    c2->cd(1);
    
    rmsH1->Draw();
    c2->cd(2);
    rmstH1->SetTitle("RMS T; [mm];");
    auto fGaus = new TF1("fgaus","gaus",0,0.2);
    auto fLandau = new TF1("flandau","landau",0.13,0.8);

    // rmstH1->Fit(fGaus,"R");
    // rmstH1->Fit(fLandau,"R+");
    Double_t par[6];
    fGaus->GetParameters(&par[0]);
    fLandau->GetParameters(&par[3]);

    auto fnew = new TF1("","gaus(0)*landau(3)",0,0.8);
    fnew->SetParameters(par);
    // rmstH1->Fit(fnew,"R");
    rmstH1->Draw();
    Double_t fNewMean = fnew->GetMaximumX();
    Double_t fNewPeak = fnew->GetMaximum(0,0.8);
    Double_t fNewFHWMLeft = fnew->GetX(fNewPeak/2., 0, fNewMean);
    double_t fNewFHWMRight = fnew->GetX(fNewPeak/2., fNewMean, 0.8);
    
    cout << " fitting " << fNewMean << "   " << fNewPeak << "|| " << fNewFHWMRight-fNewFHWMLeft << endl;


    c2->cd(3);
    rmsrH1->SetTitle("RMS R; [mm];");
    // rmsrH1->Fit("gaus");
    rmsrH1->Draw();
    
    // c2->Draw();

    auto c3 = new TCanvas();
    hitbyLayer->SetTitle("Distribution of hit pad by layer; Count;Entry");
    hitbyLayer -> Draw();
    hitbyLayer->SetStats(0);

    // auto func = fnew;
    // func->SetNormalized(true);
    // cout << func->Integral(0.,0.44) << endl;

    // func -> Draw();
    c3->Draw();

}