void example_read_track()
{
  auto file = new TFile("/home/lamps/kebi/data/iqmd_test.reco.master.250.0713284.root");
  auto tree = (TTree *) file -> Get("event");

  TClonesArray *trackArray = nullptr;
  tree -> SetBranchAddress("Tracklet", &trackArray);

  Int_t numEvents = tree -> GetEntries();
  for (Int_t event=0; event<numEvents; ++event)
  {
    tree -> GetEntry(event);

    cout << "Event " << event << endl;

    Int_t numTracks = trackArray -> GetEntries();
    for (Int_t iTrack=0; iTrack<numTracks; ++iTrack)
    {
      auto track = (KBHelixTrack *) trackArray -> At(iTrack);
      auto momentum = track -> Momentum();
      cout << iTrack << " " << momentum.x() << " " << momentum.x() << " " << momentum.x() << endl;
    }
  }
}
