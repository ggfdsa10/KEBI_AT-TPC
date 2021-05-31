void check_padplane()
{
  auto tpc = new LHTpc();
  tpc -> AddPar("input/kbpar_tpc.conf");
  tpc -> Init();

  auto padplane = (LHPadPlaneRPad *) tpc -> GetPadPlane();
  padplane -> Print();
  padplane -> Clear();

  auto hist_padplane = padplane -> GetHist("0");
  auto numPads = padplane -> GetNumPads();
  //auto hist_padplane = padplane -> GetHist();
  /*
  for (auto iPad=0; iPad<numPads; ++iPad) {
    auto pad = padplane -> GetPad(iPad);
    auto bin = hist_padplane -> FindBin(pad -> GetI(), pad -> GetJ());
    hist_padplane -> SetBinContent(bin,iPad);
  }
  */

  auto cvs = padplane -> GetCanvas();
  //hist_padplane -> SetName("PadPlane");
  //hist_padplane -> SetTitle("PadPlane");

  gStyle -> SetPalette(kBird);
  hist_padplane -> Draw("col");
  //hist_padplane -> Draw("text");
  padplane -> DrawFrame();

  padplane -> PadPositionChecker();
  padplane -> PadNeighborChecker();

  cout << "PadGap          " << padplane -> GetPadGap()           << endl;
  cout << "PadWid          " << padplane -> GetPadWid()           << endl;
  cout << "PadHei          " << padplane -> GetPadHei()           << endl;
  cout << "YPPMin          " << padplane -> GetYPPMin()           << endl;
  cout << "YPPMax          " << padplane -> GetYPPMax()           << endl;
  cout << "WPPBot          " << padplane -> GetWPPBot()           << endl;
  cout << "PadAreaLL       " << padplane -> GetPadAreaLL()        << endl;
  cout << "RMin            " << padplane -> GetRMin()             << endl;
  cout << "RMax            " << padplane -> GetRMax()             << endl;
  cout << "LayerMax        " << padplane -> GetLayerMax()         << endl;
  cout << "XSpacing        " << padplane -> GetXSpacing()         << endl;
  cout << "YSpacing        " << padplane -> GetYSpacing()         << endl;
  cout << "RemoveCuttedPad " << padplane -> GetRemoveCuttedPad()  << endl;
}
