#include "PadMap.h"


void geoCheck(){

    auto poly = new TH2Poly();

    PadMap pad;
    pad.TrianglePad(poly);

    auto c1 = new TCanvas();
    poly -> Draw("colz");
    c1 -> Draw();

}