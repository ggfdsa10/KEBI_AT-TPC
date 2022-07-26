void check_pad()
{
  auto tpc = new ATTPC();
  tpc -> AddPar("../input/par_at-tpc.conf");
  tpc -> Init();

  auto padplane = (ATTPCStripPad *) tpc -> GetPadPlane();
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
  padplane -> FindPadID(0,0);
  gStyle -> SetPalette(kBird);
  hist_padplane -> Draw("colz");
  //hist_padplane -> Draw("text");
  padplane -> DrawFrame();
  
  padplane -> PadPositionChecker();
  padplane -> PadNeighborChecker();
  // auto ch0 = padplane->GetPadByChan(0);
  // auto ch1 = padplane->GetPadByChan(1);
  // auto ch2 = padplane->GetPadByChan(2);

  // int prevPad = -1;
  // for(int ch=0; ch<3; ch++){
  //   auto channal = ch0;
  //   if(ch==1){channal = ch1;}
  //   if(ch==2){channal = ch2;}
    
  //   for(int layer=0; layer < channal.size(); layer++){
  //     int rows = channal.at(layer).size();
  //     for(int row=0; row<rows; row++){

  //         int padIndex = get<0>(channal.at(layer).at(row));
  //         int padX = get<1>(channal.at(layer).at(row));
  //         int padY = get<2>(channal.at(layer).at(row));
  //         if((padIndex - prevPad) !=1){cout << "not index sort !!! " << "  ch: " << ch << "  layer: " << layer << "  row: " << row << "  padIndex: " << padIndex <<  endl;}
  //         prevPad = padIndex;

  //         // cout << padIndex << endl;
  //     }
  //   }
  // }

}
