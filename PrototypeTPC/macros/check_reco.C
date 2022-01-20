void check_reco()
{
  auto tree = new TChain("event");
  tree -> Add("/Users/ejungwoo/kebi/data/run0098.test.0.2b74df9.root");

  TCut cut0("Beam[0].fRMS>0.&&Beam[0].fRMS<2.");

  new TCanvas(); tree -> Draw("Beam[0].fRMS>>hist0(100,-3,3)");
  new TCanvas(); tree -> Draw("HitCluster.fW>>hist1(100,0,5000)",cut0); // charge
  new TCanvas(); tree -> Draw("HitCluster.GetDPosition().Mag()>>hist2(100,0,10)",cut0);
  new TCanvas(); tree -> Draw("HitCluster.fTb>>hist3(512,0,512)",cut0);
  new TCanvas(); tree -> Draw("Hit.fTb>>hist4(512,0,512)",cut0);


  //


  TClonesArray *padArray = nullptr;
  TClonesArray *hitArray = nullptr;
  TClonesArray *hitClusterArray = nullptr;
  TClonesArray *beamArray = nullptr;
  tree -> SetBranchAddress("Pad",&padArray);
  tree -> SetBranchAddress("Hit",&hitArray);
  tree -> SetBranchAddress("HitCluster",&hitClusterArray);
  tree -> SetBranchAddress("Beam",&beamArray);

  auto numEvents = tree -> GetEntries();
  //for (auto iEvent = 0; iEvent < numEvents; ++iEvent)
  for (auto iEvent : {0})
  {
    tree -> GetEntry(iEvent);

    // beam track
    auto beam = (KBLinearTrack *) beamArray -> At(0);
    if (beam == nullptr)
      continue;

    auto beamRMS = beam -> GetRMS();
    cout << "event: " << iEvent << ", beam_rms=" << beamRMS << endl;

    auto numHits = hitArray -> GetEntriesFast();
    for (auto iHit = 0; iHit < numHits; ++iHit)
    {
      // hit
      auto hit = (KBTpcHit *) hitArray -> At(iHit);
      auto charge = hit -> GetCharge();
      auto x = hit -> GetX();
      auto y = hit -> GetY();
      auto z = hit -> GetZ();
      auto t = hit -> GetTb();

      if (padArray != nullptr)
      {
        for (auto i=0; i<padArray->GetEntriesFast(); ++i) {
          auto pad = (KBPad *) padArray -> At(i);
          if (pad && hit->GetPadID()==pad->GetPadID()) {
            pad -> AddHit(hit);
            break;
          }
        }
      }
    }

    if (padArray != nullptr)
    {
      auto numPads = padArray -> GetEntriesFast();
      for (auto iPad = 0; iPad < numPads; ++iPad) {
        auto pad = (KBPad *) padArray -> At(iPad);
        if (pad -> GetNumHits() > 0) {
          new TCanvas();
          pad -> Draw();
          break;
        }
      }
    }
  }
}
