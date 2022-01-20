void ana_dedx(TString name = "single_proton.psa")
{
  auto run = KBRun::GetRun();
  auto tpc = new LHTpc();
  run -> AddDetector(tpc);
  run -> SetInputFile(name);

  run -> Init();
  auto hitArray = run -> GetBranchA("Hit");

  Double_t mean_dedx_all = 0;
  while (run->NextEvent()) {
    //cout << run -> GetCurrentEventID() << " " << hitArray -> GetEntries() << endl;
    double chargeInLayer[38] = {0};
    auto next = TIter(hitArray);
    KBTpcHit *hit;
    while ((hit = (KBTpcHit *) next())) {
      chargeInLayer[hit->GetLayer()] += hit -> GetCharge();
    }

    Double_t mean_dedx = 0;
    for (auto layer=0; layer<38; ++layer) {
      auto dedx = chargeInLayer[layer]/10;
      mean_dedx += dedx;
      //cout << layer << " " << dedx << endl;
    }
    mean_dedx = mean_dedx / 38;
    //cout << mean_dedx << endl;

    mean_dedx_all += mean_dedx;
  }

  mean_dedx_all = mean_dedx_all / run -> GetNumEvents();

  cout << "eV to ADC ratio should be " << mean_dedx_all/500 << endl;
}
