#include <algorithm>

Double_t PadHeight, PadWidth, PadGap;
Double_t DVelocity;
Double_t TimeBucket;

void RectanglePadAnalysis(){
    double Threshole = 1.;
    int RowSelect = 5;
    
    auto tree = new TChain("event");
    tree -> Add("$KEBIPATH/data/ATTPC.digi.2e5c877.root");
    TClonesArray *padArray = nullptr;
    tree -> SetBranchAddress("Pad",&padArray);

    TString DataPath = "$KEBIPATH/data/ATTPC.mc.root";
    auto tracktree = new TChain("event");
    tracktree -> Add(DataPath);
    TClonesArray *TrackArray = nullptr;
    tracktree -> SetBranchAddress("MCTrack",&TrackArray);
    
    TFile* FileOut = new TFile("../output_data/SimPadData.root","recreate");
    
    TTree* PadTree = new TTree("data","MC data");
    TTree* unitPadTree = new TTree("ADC_UnitPad","ADC Sum Unit Pad");
    TTree* SumPadTree = new TTree("ADC_SumAll","ADC Sum pad");

    ATTPCRectanglePad *PadPlane = new ATTPCRectanglePad();
    PadHeight = PadPlane -> GetPadHeight();
    PadWidth = PadPlane -> GetPadWidth();
    PadGap = PadPlane -> GetPadGap();

    cout << PadHeight << "  " << PadWidth << "   " << PadGap << endl;
    
    Double_t HitPad[8][32];
    Double_t TimePad[8][32]; // Time bucket 
    Double_t PositionPad[8][32][2];
    Double_t ADCPad;
    Double_t ADCSumAll;
    Double_t ADCSumRow[8];
    Int_t ADCSumRowFire[8];

    Double_t XfromY0 = 0.;
    Double_t XfromY100 = 0.;
    Double_t ZfromY0 = 0.;
    Double_t ZfromY100 = 0.;

    Double_t XYgradient = 0.;
    Double_t XYconstant = 0.;
    Double_t YZgradient = 0.;
    Double_t YZconstant = 0.;

    PadTree -> Branch("HitPad", &HitPad, "HitPad[8][32]/D");
    PadTree -> Branch("TimePad", &TimePad, "TimePad[8][32]/D");
    PadTree -> Branch("PositionPad", &PositionPad, "PositionPad[8][32][2]/D");

    // X,Z position at y0, y100 for Machine leaning 
    PadTree -> Branch("XfromY0",&XfromY0, "XfromY0/D");
    PadTree -> Branch("XfromY100",&XfromY100, "XfromY100/D");
    PadTree -> Branch("ZfromY0",&ZfromY0, "ZfromY0/D");
    PadTree -> Branch("ZfromY100",&ZfromY100, "ZfromY100/D");

    // PadTree -> Branch("XYgradient",&XYgradient, "XYgradient/D");
    // PadTree -> Branch("XYconstant",&XYconstant, "XYconstant/D");
    // PadTree -> Branch("YZgradient",&YZgradient, "YZgradient/D");
    // PadTree -> Branch("YZconstant",&YZconstant, "YZconstant/D");

    unitPadTree -> Branch("SumUnitPad",&ADCPad,"ADCPad/D");
    SumPadTree -> Branch("SumAllPad",&ADCSumAll,"ADCSumAll/D");
    SumPadTree -> Branch("SumRowPad",&ADCSumRow,"ADCSumRow[8]/D");
    SumPadTree -> Branch("SumFiredRowPad",&ADCSumRowFire,"ADCSumRowFire[8]/I");

    // macros information 
    cout << "=======================================" << endl;
    cout << " KEBI data analysis and converting     " << endl;
    cout << " Pad type : Rectangle Pad              " << endl;
    cout << " Total Event : " << tree -> GetEntries() << endl;
    cout << "=======================================" << endl;

    for(int event = 0; event < tree->GetEntries(); event++){
        if(event%100 ==0)
            cout << "Event Number : " << event << endl;

        tree -> GetEntry(event);
        tracktree -> GetEntry(event);

        ADCSumAll = 0.;
        fill_n(ADCSumRow, 8, 0.);
        fill_n(ADCSumRowFire, 8, 0);
        std::fill(&HitPad[0][0], &HitPad[7][32], 0.);
        std::fill(&TimePad[0][0], &TimePad[7][32], 0.);
        std::fill(&PositionPad[0][0][0], &PositionPad[7][32][2], 0.);

        for ( int ihit = 0 ; ihit <256 ; ihit++)  {

            auto pad = (KBPad *) padArray -> At(ihit);

            Double_t* bufferOut = pad -> GetBufferOut();
            auto tbArray = pad -> GetMCTbArray();

            sort(bufferOut, bufferOut+512);

            if(!(bufferOut[511]<=0.)){
                ADCSumAll +=bufferOut[511];
                ADCPad = bufferOut[511];
                unitPadTree -> Fill();
            }  

            if(bufferOut[511] <= Threshole){
                bufferOut[511] = 0.;
            }
            HitPad[pad -> GetLayer()][pad ->GetRow()] = bufferOut[511]; 
            TVector3 Position = pad -> GetPosition();

            PositionPad[pad -> GetLayer()][pad -> GetRow()][0] = Position.X();
            PositionPad[pad -> GetLayer()][pad -> GetRow()][1] = Position.Y();

            for(int NumTb =0; NumTb < tbArray -> size(); NumTb++){
                auto tb1 = tbArray -> at(NumTb);
                TimePad[pad -> GetLayer()][pad ->GetRow()] = tb1;
            }
        }



        for(int i =0; i < 1; i++){
            auto Track = (KBMCTrack *) TrackArray -> At(i);
            if(Track -> GetPDG() == 11) continue;
            TVector3 Momentum = Track -> GetMomentum();

            TVector3 Correction;
            TVector3 TrackHead;
            TVector3 TrackTail;

            Correction.SetXYZ(-(PadWidth/2 +PadGap/2)/(PadHeight/2 +PadGap/2), 0, -DVelocity/2 *TimeBucket);
            TrackHead = Track -> GetPrimaryPosition() +Correction;
            TrackTail.SetXYZ(TrackHead.X() + 100./TMath::Tan(Momentum.Phi()), 100.+TrackHead.Y(), TrackHead.Z() + 100./TMath::Tan(Momentum.Theta()));

            XYgradient = (TrackTail.Y() - TrackHead.Y()) / (TrackTail.X() - TrackHead.X());
            XYconstant = -1. * (TrackHead.X() * XYgradient) + TrackHead.Y();

            YZgradient = (TrackTail.Z() - TrackHead.Z()) / (TrackTail.Y() - TrackHead.Y());
            YZconstant = -1. * (TrackHead.Y() * YZgradient) + TrackHead.Z();

            // XfromY0 = -XYconstant/XYgradient;
            // XfromY100 = (100 -XYconstant)/XYgradient;
            // ZfromY0 = YZgradient*0 +YZconstant;
            // ZfromY100 = YZgradient*100 +YZconstant;

            XfromY0 = TrackHead.X() + 10./TMath::Tan(Momentum.Phi());
            XfromY100 = TrackHead.X() + 110./TMath::Tan(Momentum.Phi());
            ZfromY0 = TrackHead.Z() + 10./TMath::Tan(Momentum.Theta());
            ZfromY100 = TrackHead.Z() + 110./TMath::Tan(Momentum.Theta());
        }

        PadTree -> Fill();

        for(int i=0; i<8; i++){
            double max =0.;
            int max_index =0;

            for(int j=0; j<32; j++){
                if (HitPad[i][j] > max){
                    max = HitPad[i][j]; 
                    max_index = j;
                }
            }
            
            for(int index =0; index < 32; index++){
                if(HitPad[i][index] > Threshole){
                    ADCSumRow[i] += HitPad[i][index];
                    ADCSumRowFire[i]+=1;
                }
            }           
        }
        if(!(ADCSumAll ==0.)){
            SumPadTree -> Fill();
        }
    }
    cout << "=======================================" << endl;
    cout << "Total event " << tree -> GetEntries() << " end" << endl;
    cout << "=======================================" << endl;
    FileOut  -> cd();
    PadTree -> Write();
    // unitPadTree -> Write();
    // SumPadTree -> Write();
    
    FileOut -> Close();

} 


void HoneyCombPadAnalysis()
{
    double Threshole = 1.;
    int RowSelect = 5;

    auto tree = new TChain("event");
    tree -> Add("$KEBIPATH/data/ATTPC.digi.2e5c877.root");
    TClonesArray *padArray = nullptr;
    tree -> SetBranchAddress("Pad",&padArray);
    
    TFile* FileOut = new TFile("../output_data/SimPadData.root","recreate");
    
    TTree* PadTree = new TTree("Pad","Pad MC data");
    TTree* unitPadTree = new TTree("ADC_UnitPad","ADC Sum Unit Pad");
    TTree* SumPadTree = new TTree("ADC_SumAll","ADC Sum pad");
    
    Double_t HitPad[16][16];
    Double_t TimePad[16][16]; // Time bucket 
    Double_t PositionPad[16][16][2];
    Double_t ADCPad;
    Double_t ADCSumAll;
    Double_t ADCSumRow[16];
    Int_t ADCSumRowFire[16];

    PadTree -> Branch("HitPad", &HitPad, "HitPad[16][16]/D");
    PadTree -> Branch("TimePad", &TimePad, "TimePad[16][16]/D");
    PadTree -> Branch("PositionPad", &PositionPad, "PositionPad[16][16][2]/D");
    unitPadTree -> Branch("SumUnitPad",&ADCPad,"ADCPad/D");
    SumPadTree -> Branch("SumAllPad",&ADCSumAll,"ADCSumAll/D");
    SumPadTree -> Branch("SumRowPad",&ADCSumRow,"ADCSumRow[16]/D");
    SumPadTree -> Branch("SumFiredRowPad",&ADCSumRowFire,"ADCSumRowFire[16]/I");

    // macros information 
    cout << "=======================================" << endl;
    cout << " KEBI data analysis and converting     " << endl;
    cout << " Pad type : HoneyComb Pad              " << endl;
    cout << " Total Event : " << tree -> GetEntries() << endl;
    cout << "=======================================" << endl;

    for(int event = 0; event < tree -> GetEntries(); event++){
        if(event%100 ==0)
            cout << "Event Number : " << event << endl;

        tree -> GetEntry(event);


        ADCSumAll = 0.;
        fill_n(ADCSumRow, 8, 0.);
        fill_n(ADCSumRowFire, 8, 0);
        std::fill(&HitPad[0][0], &HitPad[15][16], 0.);
        std::fill(&TimePad[0][0], &TimePad[15][16], 0.);
        std::fill(&PositionPad[0][0][0], &PositionPad[15][16][2], 0.);

        for ( int ihit = 0 ; ihit <256 ; ihit++)  {

            auto pad = (KBPad *) padArray -> At(ihit);

            Double_t* bufferOut = pad -> GetBufferOut();
            auto tbArray = pad -> GetMCTbArray();

            sort(bufferOut, bufferOut+512);

            if(!(bufferOut[511]<=0.)){
                ADCSumAll +=bufferOut[511];
                ADCPad = bufferOut[511];
                unitPadTree -> Fill();
            }  

            if(bufferOut[511] <= Threshole){
                bufferOut[511] = 0.;
            }
            HitPad[pad -> GetLayer()][pad ->GetRow()] = bufferOut[511]; 
            TVector3 Position = pad -> GetPosition();

            PositionPad[pad -> GetLayer()][pad -> GetRow()][0] = Position.X();
            PositionPad[pad -> GetLayer()][pad -> GetRow()][1] = Position.Y();

            for(int NumTb =0; NumTb < tbArray -> size(); NumTb++){
                auto tb1 = tbArray -> at(NumTb);
                TimePad[pad -> GetLayer()][pad ->GetRow()] = tb1;
            }
        }

        PadTree -> Fill();

        for(int i=0; i<16; i++){
            double max =0.;
            int max_index =0;

            for(int j=0; j<16; j++){
                if (HitPad[i][j] > max){
                    max = HitPad[i][j]; 
                    max_index = j;
                }
            }
            
            for(int index =0; index < 16; index++){
                if(HitPad[i][index] > Threshole){
                    ADCSumRow[i] += HitPad[i][index];
                    ADCSumRowFire[i]+=1;
                }
            }           
        }
        if(!(ADCSumAll ==0.)){
            // SumPadTree -> Fill();
        }
    }

    cout << "=======================================" << endl;
    cout << "Total event " << tree -> GetEntries() << " end" << endl;
    cout << "=======================================" << endl;

    FileOut  -> cd();
    PadTree -> Write();
    // unitPadTree -> Write();
    // SumPadTree -> Write();
    
    FileOut -> Close();

} 

void ADC_analysis(TString input = "ATTPC"){

    auto run = KBRun::GetRun();
    run -> SetInputFile(input+".digi.2e5c877");
    // run -> AddFriend();
    run -> Init();
    auto par = run -> GetPar();

    DVelocity = par -> GetParDouble("VelocityE");
    TimeBucket = par -> GetParDouble("TBtime");

    TString PadPlaneType = par -> GetParString("PadPlaneType");

    if(PadPlaneType == "RectanglePad"){

        RectanglePadAnalysis();

    }
    else if(PadPlaneType == "HoneyCombPad"){
        HoneyCombPadAnalysis();
    }
}

