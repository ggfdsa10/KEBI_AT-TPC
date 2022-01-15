void make_single_gen()
{
  int numEvents = 200;
  int numTracks = 1;
  int pdg = 2212;
  double pVal = 400;
  double vx = 0;
  double vy = -200;
  double vz = -400;

  ofstream file("single.gen");
  file << "p" << endl;
  file << numEvents << endl;

  for (auto event=0; event<numEvents; ++event)
  {
    double px = gRandom -> Gaus(0,   0.02*pVal);
    double py = gRandom -> Gaus(0,   0.02*pVal);
    double pz = gRandom -> Gaus(pVal,0.02*pVal);

    file << event << " " << numTracks << " " << vx << " " << vy << " " << vz << endl;
    file << pdg << " " << px << " " << py << " " << pz << endl;
  }
}
