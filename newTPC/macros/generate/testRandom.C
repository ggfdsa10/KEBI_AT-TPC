#include <iostream>
#include <fstream>

using namespace std;
using namespace TMath;
void testRandom(){

    int particlePDG = 13; 
    int totalevent = 1600;

    int tracknum = 1;
    double Energy = 4000.; // [Mev]
    
    double x, y, z, vx, vy, vz;
    double zrange[16] ={5,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150};

    double windowSize  = 155; 
    double windowHeight = 24.7;
    double windowShift = 31.125; 
    double TriggerDistance = 450.;

    ofstream fileout;
    fileout.open("./CosmicMuon.gen",ios::out);
    fileout << "e" << endl;
    fileout << totalevent << endl;

    int index =0;
    for(int event = 0; event < totalevent; event++){
        if(event%100 ==0){
            cout << " event : " << event << endl;
            z = zrange[index];
            index++;

        }
        if(z < 3.) {z = 160;}
        TRandom3* fRandom = new TRandom3(0);
  
        x = fRandom -> Gaus(windowSize/2, 22);

        double arctanX1 = ATan(x / (2 * TriggerDistance));
        double arctanX2 = ATan((windowSize - x) / (2 * TriggerDistance));

        double PaiAngle = fRandom -> Uniform(-arctanX2, arctanX1) +Pi()/2;

        x = x -windowShift;
        y = -400.;

        vx =  Cos(PaiAngle);
        vy =  Sin(PaiAngle);
        vz = 0.;

        double Ex = Energy * vx;
        double Ey = Energy * vy;
        double Ez = Energy * vz;
        
        fileout << event << " " <<tracknum << " " << x << " " << y << " " << z << endl;
        fileout << particlePDG << " " << Ex << " " << Ey << " " << Ez << endl;


    }   
    cout << " event : " << totalevent << endl;

}