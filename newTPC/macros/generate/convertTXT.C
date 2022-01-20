// ########################################################################
//               Txt File to Geant4 particle generate file
//
//        Txt file          ->          Generate file
//                               e
//                               TotalEventNum
//   x, y, z, vx, vy, vz         Eventnum, Tracknum, x, y, z
//                               ParticlePDG, EnergyVX, EnergyVY, EnergyVZ 
//
// #########################################################################


#include <iostream>
#include <fstream>

using namespace std;
void convertTXT(){

  int particlePDG = 13; 
  int eventnum = 0;
  int tracknum = 1;
  double Energy = 4000.; // [Mev]

  fstream filein1;
  filein1.open("./CosmicMuonBDC.txt", ios::in);

  ofstream fileout;
  fileout.open("./CosmicMuon.gen",ios::out);


  double x, y, z, vx, vy, vz;

  int linecount =0;
  while (!filein1.eof()){  
    filein1 >> x >> y >> z >> vx >> vy >> vz;
    linecount++;  
    if(linecount%100 ==0){
      cout << linecount << endl;
    }
  }
  cout << " Total line count : " << linecount << endl;
  filein1.close();



  fstream filein2;
  filein2.open("./CosmicMuonBDC.txt", ios::in);



  fileout << "e" << endl;
  fileout << linecount << endl;

  while (!filein2.eof()){

    filein2 >> x >> y >> z >> vx >> vy >> vz;
    double Ex = Energy * vx;
    double Ey = Energy * vy;
    double Ez = Energy * vz;
    
    fileout << eventnum << " " <<tracknum << " " << x << " " << y << " " << z << endl;
    fileout << particlePDG << " " << Ex << " " << Ey << " " << Ez << endl;

    eventnum++;
    // tracknum++;

    if(eventnum%100 ==0){
      cout << " event : " << eventnum << endl;
    }
  }
  cout << " event : " << eventnum << endl;

}
