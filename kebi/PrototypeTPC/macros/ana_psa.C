void ana_psa()
{
  auto tree = new TChain("event");
  tree -> Add("/Users/ejungwoo/kebi/data/run0098.test.0.2b74df9.root");
  TClonesArray *padArray = nullptr;
  tree -> SetBranchAddress("Pad",&padArray);
  tree -> GetEntry(0);

  auto pad = (KBPad *) padArray -> At(49);



  ///////////////////////////////////////////////////
  Double_t *bufferOut = pad -> GetBufferOut();
  ///////////////////////////////////////////////////



  auto hist = new TH1D("buffer","",512,0,512);
  hist -> SetContent(bufferOut);
  new TCanvas();
  hist -> Draw();



  // find max
  vector<KBChannelHit> hitArray1;
  auto psaFindMax = new KBPSA();
  psaFindMax -> SetPar("prototype.par");
  psaFindMax -> Init();
  psaFindMax -> AnalyzeChannel(bufferOut, &hitArray1);

  auto channelHit = hitArray1[0];
  auto m = new TMarker(channelHit.GetTDC(),channelHit.GetADC(),20);
  m -> Draw("samep");



  // find pulse
  vector<KBChannelHit> hitArray2;
  auto psaFindPulse = new KBPSAFastFit();
  psaFindPulse -> SetPar("prototype.par");
  psaFindPulse -> Init();
  psaFindPulse -> AnalyzeChannel(bufferOut, &hitArray2);

  auto numHits2 = hitArray2.size();
  for (auto iHit=0; iHit<numHits2; ++iHit) {
    auto channelHit = hitArray2[iHit];
    auto pulse = KBPulseGenerator::GetPulseGenerator() -> GetPulseFunction(Form("pulse%d",iHit));
    pulse -> SetParameters(channelHit.GetADC(),channelHit.GetTDC());
    pulse -> Draw("samel");
  }
}
