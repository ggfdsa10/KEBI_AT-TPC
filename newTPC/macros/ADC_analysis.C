#include <algorithm>

void RectanglePadAnalysis(){
    double Threshole = 1.;
    int RowSelect = 5;
    
    auto tree = new TChain("event");
    tree -> Add("$KEBIPATH/data/NewTPC.digi.2e5c877.root");
    TClonesArray *padArray = nullptr;
    tree -> SetBranchAddress("Pad",&padArray);
    
    TFile* FileOut = new TFile("../output_data/SimPadData.root","recreate");
    
    TTree* PadTree = new TTree("Pad","Pad MC data");
    TTree* unitPadTree = new TTree("ADC_UnitPad","ADC Sum Unit Pad");
    TTree* SumPadTree = new TTree("ADC_SumAll","ADC Sum pad");
    
    Double_t HitPad[8][32];
    Double_t TimePad[8][32]; // Time bucket 
    Double_t PositionPad[8][32][2];
    Double_t ADCPad;
    Double_t ADCSumAll;
    Double_t ADCSumRow[8];
    Int_t ADCSumRowFire[8];

    PadTree -> Branch("HitPad", &HitPad, "HitPad[8][32]/D");
    PadTree -> Branch("TimePad", &TimePad, "TimePad[8][32]/D");
    PadTree -> Branch("PositionPad", &PositionPad, "PositionPad[8][32][2]/D");
    unitPadTree -> Branch("SumUnitPad",&ADCPad,"ADCPad/D");
    SumPadTree -> Branch("SumAllPad",&ADCSumAll,"ADCSumAll/D");
    SumPadTree -> Branch("SumRowPad",&ADCSumRow,"ADCSumRow[8]/D");
    SumPadTree -> Branch("SumFiredRowPad",&ADCSumRowFire,"ADCSumRowFire[8]/I");

    for(int event = 0; event < tree -> GetEntries(); event++){
        if(event%100 ==0)
            cout << "Event Number : " << event << endl;

        tree -> GetEntry(event);


        ADCSumAll = 0.;
        fill_n(ADCSumRow, 8, 0.);
        fill_n(ADCSumRowFire, 8, 0);
        std::fill(&HitPad[0][0], &HitPad[7][32], 0.);
        std::fill(&TimePad[0][0], &TimePad[7][32], 0.);
        std::fill(&PositionPad[0][0][0], &PositionPad[7][32][2], 0.);

        for ( int ihit = 0 ; ihit <256 ; ihit++)  {

            auto pad = (KBPad *) padArray -> At(ihit);
            if (pad == nullptr){
                continue;
            }

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
    cout << " Event " << tree -> GetEntries() << " end" << endl;

    FileOut  -> cd();
    PadTree -> Write();
    unitPadTree -> Write();
    SumPadTree -> Write();
    
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
            if (pad == nullptr){
                continue;
            }

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
            SumPadTree -> Fill();
        }
    }
    cout << " Event " << tree -> GetEntries() << " end" << endl;

    FileOut  -> cd();
    PadTree -> Write();
    unitPadTree -> Write();
    SumPadTree -> Write();
    
    FileOut -> Close();

} 

void ADC_analysis(TString input = "ATTPC"){

    auto run = KBRun::GetRun();
    run -> SetInputFile(input+".mc");
    run -> Init();
    auto par = run -> GetPar();

    TString PadPlaneType = par -> GetParString("PadPlaneType");

    if(PadPlaneType == "RectanglePad"){
        RectanglePadAnalysis();
    }
    else if(PadPlaneType == "HoneyCombPad"){
        HoneyCombPadAnalysis();
    }
}

