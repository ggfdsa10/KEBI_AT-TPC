#include <algorithm>

void ADC_analysis(TString input = "ATTPC")
{
    int RowNum = 8;
    int ColumnNum = 32;
    double FriedThreshole = 10.;

    auto tree = new TChain("event");
    tree -> Add("$KEBIPATH/data/ATTPC.digi.2e5c877.root");
    TClonesArray *padArray = nullptr;
    tree -> SetBranchAddress("Pad",&padArray);

    TFile* file = new TFile("../output_data/ADC_Analysis.root","recreate");
    
    TTree* AllPad = new TTree("ADC_All","ADC Sum All Pad");
    TTree* RowPad = new TTree("ADC_Row","ADC Sum Row Pad");
    TTree* FiredPad = new TTree("ADC_FiredRow","ADC Sum Row Fired Pad");

    Double_t ADCSumAll;
    Double_t ADCSumRow;
    Int_t ADCRowFireNum;

    AllPad -> Branch("SumAllPad",&ADCSumAll,"ADCSumAll/D");
    RowPad -> Branch("SumRowPad",&ADCSumRow,"ADCSumRow/D");
    FiredPad -> Branch("SumFiredRowPad",&ADCRowFireNum,"ADCRowFireNum/I");

    for(int event = 0; event < tree -> GetEntries(); event++){
        cout << "Event Number : " << event+1 << endl;
        tree -> GetEntry(event);

        ADCSumAll =0.;
        int id =0;

        for(int row = 0; row < RowNum; row++){
            ADCSumRow = 0.;
            ADCRowFireNum = 0;

            for(int column = 0; column < ColumnNum; column++){
                auto pad = (KBPad *) padArray -> At(id);
                Double_t* bufferOut = pad -> GetBufferOut();

                sort(bufferOut, bufferOut+512);

                if(!(bufferOut[511]<=0.)){
                    ADCSumAll +=bufferOut[511];
                    ADCSumRow +=bufferOut[511];
                }
                
                if(bufferOut[511] > FriedThreshole){
                    ADCRowFireNum++;
                } 
                id++;
            }
            RowPad -> Fill();
            FiredPad -> Fill();
        }
        AllPad -> Fill();

    }
    AllPad -> Write();
    RowPad -> Write();
    FiredPad -> Write();
    file -> Close();
}