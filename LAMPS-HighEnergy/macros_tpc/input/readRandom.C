#include <iostream>
#include <fstream>

using namespace std;

void readRandom(){
    int particlePDG = 2212; 
    int event = 10000;

    auto mom = new TH1D("","",60,0.,3000);
    auto vX = new TH1D("","",60,-1,1);
    auto vY = new TH1D("","",60,-1,1);
    auto vZ = new TH1D("","",60,-1,1);
    auto theta = new TH1D("","",70,0,1.5);
    fstream filein1;
    filein1.open("./iqmd_test.gen", ios::in);
    double vx, vy, vz;
    int pdg;
    
    while (!filein1.eof()){
        filein1 >> pdg >> vx >> vy >> vz;
        TVector3 momvec = TVector3(vx, vy, vz);
        double momValue = momvec.Mag();
        auto unit = momvec.Unit();
        mom->Fill(momValue);

        if(unit.Theta() > 0.16){
            vX -> Fill(unit.X());
            vY -> Fill(unit.Y());
            vZ -> Fill(unit.Z());
            theta->Fill(unit.Theta());
        }

    }

    filein1.close();

    auto c1 = new TCanvas("","",1600,1600);
    c1->Divide(2,2);
    c1->cd(1);
    mom->Draw();
    c1->cd(2);
    vX->Draw();
    c1->cd(3);
    vY->Draw();
    c1->cd(4);
    theta->Draw();
    c1->Draw();



    fstream file;
    file.open("./test.gen", ios::out);

    file<< "p" << endl;
    file<< event << endl;

    for(int i=0; i<event; i++){
        gRandom->SetSeed(0);
        // auto track = gRandom->Uniform(100,150);
        int track = 1;
        file<< i << " " << int(track) << " 0 0 0" << endl;

        for(int j=0; j<int(track); j++){
            double momentum = mom->GetRandom();
            TVector3 refVer;
            refVer.SetXYZ(vX->GetRandom(), vY->GetRandom(), vZ->GetRandom());

            TVector3 ver;
            ver.SetXYZ(1,1,1);
            ver.SetPhi(gRandom->Uniform(-TMath::Pi()/12., TMath::Pi()/12.));
            ver.SetTheta(refVer.Theta());
            ver = ver.Unit();
            file << particlePDG << " " << momentum*ver.X() << " " << momentum*ver.Y() << " " << momentum*ver.Z() << endl;
        }
        
    }
    file.close();



}
