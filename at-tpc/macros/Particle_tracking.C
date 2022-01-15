#include <vector>
#include <algorithm>

using namespace std;

Double_t PadHeight, PadWidth, PadGap;
Double_t DVelocity;
Double_t TimeBucket;
Double_t Bfield;
TString PadPlaneType;
TString DataPath = "$KEBIPATH/data/ATTPC.mc.root";

void TrackDataTree(){
    TVector3 TrackHead;
    TVector3 Momentum;
    TVector3 TrackTail;

    TFile* fileout = new TFile("../output_data/SimTrackData.root", "recreate");
    TTree* fileTree = new TTree("Track","Track MC data");

    Int_t eventNum =0;
    Int_t particlePDG = 0;
    Int_t particleNum = 0;

    Double_t XYgradient = 0.;
    Double_t XYconstant = 0.;
    Double_t YZgradient = 0.;
    Double_t YZconstant = 0.;

    Double_t XfromY0 = 0.;
    Double_t XfromY100 = 0.;
    Double_t ZfromY0 = 0.;
    Double_t ZfromY100 = 0.;

    Double_t MomentumX =0.;
    Double_t MomentumY =0.;
    Double_t MomentumZ =0.;

    Double_t edep1 = 0.;
    Double_t edep2 = 0.;

    // track index
    fileTree -> Branch("eventNum",&eventNum, "eventNum/I");
    fileTree -> Branch("PDG", &particlePDG, "particlePDG/I");
    fileTree -> Branch("particleNum",&particleNum, "particleNum/I");

    if(Bfield == 0.){
        // Particle track equation
        fileTree -> Branch("XYgradient",&XYgradient, "XYgradient/D");
        fileTree -> Branch("XYconstant",&XYconstant, "XYconstant/D");
        fileTree -> Branch("YZgradient",&YZgradient, "YZgradient/D");
        fileTree -> Branch("YZconstant",&YZconstant, "YZconstant/D");

        // X,Z position at y0, y100 for Machine leaning 
        fileTree -> Branch("XfromY0",&XfromY0, "XfromY0/D");
        fileTree -> Branch("XfromY100",&XfromY100, "XfromY100/D");
        fileTree -> Branch("ZfromY0",&ZfromY0, "ZfromY0/D");
        fileTree -> Branch("ZfromY100",&ZfromY100, "ZfromY100/D");
    }

    // particle initial momentum
    fileTree -> Branch("Px",&MomentumX, "MomentumX/D");
    fileTree -> Branch("Py",&MomentumY, "MomentumY/D");
    fileTree -> Branch("Pz",&MomentumZ, "MomentumZ/D");

    // particle delta energy deposit at start, end
    fileTree -> Branch("Edep1",&edep1, "edep1/D");
    fileTree -> Branch("Edep2",&edep2, "edep2/D");

    auto tree = new TChain("event");
    tree -> Add(DataPath);

    TClonesArray *TrackArray = nullptr;
    TClonesArray *StepArray = nullptr;
    tree -> SetBranchAddress("MCTrack",&TrackArray);
    tree -> SetBranchAddress("MCStep1",&StepArray);

    // macros information 
    cout << " Total Event : " << tree -> GetEntries() << endl;
    cout << "=======================================" << endl;

    for(int event = 0; event < tree -> GetEntries(); event++){
        if(event%100 ==0)
            cout << "Event Number : " << event << endl;

        tree -> GetEntry(event);

        int particleCount =0;
        for(int i =0; i < TrackArray->GetEntries(); i++){
            auto Track = (KBMCTrack *) TrackArray -> At(i);

            if(Track -> GetPDG() == 11) continue;

            Momentum = Track -> GetMomentum();
            
            if(Bfield == 0.){
                TVector3 Correction;
                Correction.SetXYZ(-(PadWidth/2 +PadGap/2)/(PadHeight/2 +PadGap/2), 0, -DVelocity/2 *TimeBucket);
                TrackHead = Track -> GetPrimaryPosition() +Correction;
                TrackTail.SetXYZ(TrackHead.X() + 100./TMath::Tan(Momentum.Phi()), 100.+TrackHead.Y(), TrackHead.Z() + 100./TMath::Tan(Momentum.Theta()));

                XYgradient = (TrackTail.Y() - TrackHead.Y()) / (TrackTail.X() - TrackHead.X());
                XYconstant = -1. * (TrackHead.X() * XYgradient) + TrackHead.Y();

                YZgradient = (TrackTail.Z() - TrackHead.Z()) / (TrackTail.Y() - TrackHead.Y());
                YZconstant = -1. * (TrackHead.Y() * YZgradient) + TrackHead.Z();

                XfromY0 = -XYconstant/XYgradient;
                XfromY100 = (100 -XYconstant)/XYgradient;
                ZfromY0 = YZgradient*0 +YZconstant;
                ZfromY100 = YZgradient*100 +YZconstant;

                XfromY0 = TrackHead.X() + 10./TMath::Tan(Momentum.Phi());
                XfromY100 = TrackHead.X() + 110./TMath::Tan(Momentum.Phi());
                ZfromY0 = TrackHead.Z() + 10./TMath::Tan(Momentum.Theta());
                ZfromY100 = TrackHead.Z() + 110./TMath::Tan(Momentum.Theta());
            }

            MomentumX = Momentum.X();
            MomentumY = Momentum.Y();
            MomentumZ = Momentum.Z();

            eventNum = event;
            particlePDG = Track -> GetPDG();
            particleNum = particleCount;

            edep1 = Track -> GetEdep1();
            edep2 = Track -> GetEdep2();

            fileTree -> Fill();
            particleCount++;

        }
    }

    cout << "=======================================" << endl;
    cout << " Event " << tree -> GetEntries() << " end" << endl;
    cout << "=======================================" << endl;

    fileTree -> Write();
    fileout -> Close();
    
}
void Particle_tracking(TString input = "ATTPC"){
    auto run = KBRun::GetRun();
    run -> SetInputFile(input+".digi.2e5c877");
    run -> Init();
    auto par = run -> GetPar();

    DVelocity = par -> GetParDouble("VelocityE");
    TimeBucket = par -> GetParDouble("TBtime");
    PadPlaneType = par -> GetParString("PadPlaneType");
    Bfield = par -> GetParDouble("bfieldZ");

    cout << "=======================================" << endl;
    cout << " KEBI Particle track data converting   " << endl;
    cout << " Pad type : " << PadPlaneType            << endl;

    if(PadPlaneType == "RectanglePad"){
        ATTPCRectnglePad *PadPlane = new ATTPCRectnglePad();
        PadHeight = PadPlane -> GetPadHeight();
        PadWidth = PadPlane -> GetPadWidth();
        PadGap = PadPlane -> GetPadGap();
    }
    else if(PadPlaneType == "HoneyCombPad"){
        ATTPCHoneyCombPad *PadPlane = new ATTPCHoneyCombPad();
        PadHeight = PadPlane -> GetPadHeight();
        PadWidth = PadPlane -> GetPadWidth();
        PadGap = PadPlane -> GetPadGap();
    }

    TrackDataTree();
}