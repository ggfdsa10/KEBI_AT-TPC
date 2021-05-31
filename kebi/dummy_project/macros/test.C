void test()
{
  bool drawCheck = false;

  ofstream file("dummy.gen");

  Int_t numEvents = 100;
  auto maxTracks = 5;
  auto sigmaTheta = TMath::Pi()/100.;

  Int_t pdg[] = {11,13,22,2112,2212};

  file << "p" << endl;
  file << numEvents << endl;

  if (drawCheck) {
    style::cc();
    (new TH2D("frame",";z;x(y)",10,-1.1,1.1,10,-1.1,1.1)) -> Draw();
  }

  for (auto event=0; event<numEvents; ++event)
  {
    auto numTracks = gRandom -> Integer(maxTracks)+1;
    file << event << " " << numTracks << " 0 0 0" << endl;

    for (auto track=0; track<numTracks; ++track)
    {
      TVector3 mom;
      Double_t pt = gRandom -> Gaus(300,10);
      Double_t theta = TMath::Abs(gRandom -> Gaus(0,sigmaTheta));
      Double_t phi = gRandom -> Uniform(2*TMath::Pi());

      mom.SetZ(pt);
      mom.SetTheta(theta);
      mom.SetPhi(phi);

      Int_t ipdg = gRandom -> Integer(5);
      file << " " << pdg[ipdg] << " " << mom.X() << " " << mom.Y() << " " << mom.Z() << endl;

      if (drawCheck) {
        KBGeoLine line(TVector3(), mom.Unit());
        line.DrawArrowYZ() -> Draw("same>");
        //cout << setw(14) << mom.x() << setw(14) << mom.y() << setw(14) << mom.z() << "   " << setw(14) << theta << setw(14) << phi << endl;
      }
    }
  }
}
