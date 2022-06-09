#include "PadMap.h"


void geoCheck(){

    auto poly = new TH2Poly();

    PadMap pad;
    pad.StripPad(poly);

    auto c1 = new TCanvas();
    poly -> Draw("colz");
    poly->SetStats(0);
    c1 -> Draw();

    auto ch0 = pad.GetTestPad(0);
    auto ch1 = pad.GetTestPad(1);
    auto ch2 = pad.GetTestPad(2);
    int prevPad = -1;
    for(int ch=0; ch<3; ch++){
        auto channal = ch0;
        if(ch==1){channal = ch1;}
        if(ch==2){channal = ch2;}
        
        for(int layer=0; layer < channal.size(); layer++){
            int rows = channal.at(layer).size();
            for(int row=0; row<rows; row++){

                int padIndex = get<0>(channal.at(layer).at(row));
                int padX = get<1>(channal.at(layer).at(row));
                int padY = get<2>(channal.at(layer).at(row));
                if((padIndex - prevPad) !=1){cout << "not index sort !!! " << endl;}
                prevPad = padIndex;

                // cout << padIndex << endl;
            }

        }
    }

}