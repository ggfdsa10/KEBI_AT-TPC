#include "PadMap.h"
#include <math.h>


PadMap::PadMap() {
    std::vector<int> IDs;
    std::vector<int> position;
    int agetID, channelID;
    int colN, rowN;
    for (agetID = 0; agetID < 4; agetID++) {
        for (channelID = 0; channelID < 68; channelID++) {
            if (channelID == 11 || channelID == 22 || channelID == 45 || channelID == 56) continue;
            TransformChannelToPad(agetID, channelID, colN, rowN);
            IDs.clear();
            IDs.push_back(agetID);
            IDs.push_back(channelID);
            position.clear();
            position.push_back(colN);
            position.push_back(rowN);
            this->mapToID_[std::make_pair(colN, rowN)] = IDs;
            this->mapToPosition_[std::make_pair(agetID, channelID)] = position;
        }
    }
}
int PadMap::GetAgetID(int colN, int rowN) {
    return mapToID_[std::make_pair(colN, rowN)].at(0);
}
int PadMap::GetChannelID(int colN, int rowN) {
    return mapToID_[std::make_pair(colN, rowN)].at(1);
}
int PadMap::GetColN(int agetID, int channelID) {
    return mapToPosition_[std::make_pair(agetID, channelID)].at(0);
}
int PadMap::GetRowN(int agetID, int channelID) {
    return mapToPosition_[std::make_pair(agetID, channelID)].at(1);
}
float PadMap::GetX(int colN) {
    return 6.2 * colN;
}
float PadMap::GetY(int colN, int rowN) {
    return colN % 2 ? 6.0 * rowN - 3.0 : 6.0 * rowN;
}

//   X-axis  long honeycomb pad
void PadMap::BuildHoneyCombPadXLong(TH2Poly *poly) {
    // Add the bins
    double x[6], y[6];
    // Set hexagon pattern parameters
    double par[3], delX[2];
    par[0] = 6.2;
    par[1] = 6.;
    par[2] = 0.5;  // x-spacing, y-spacing, gap spacing

    delX[0] = 0.5 * (1 - par[2] / par[1]) * (pow(par[0], 2) + pow(0.5 * par[1], 2)) / par[0];
    delX[1] = sqrt(pow(delX[0], 2) - pow(0.5 * par[1] - par[2], 2));

    double xCenter = 0, yCenter = 0;
    for (int nRow = 0; nRow < 16; nRow++) {
        //  xtemp = xloop; // Resets the temp variable
        for (int nColumn = 0; nColumn < 16; nColumn++) {
            // Go around the hexagon
            x[0] = xCenter - delX[1];
            y[0] = yCenter - 0.5 * (par[1] - par[2]);
            x[1] = xCenter - delX[0];
            y[1] = yCenter;
            x[2] = x[0];
            y[2] = yCenter + 0.5 * (par[1] - par[2]);
            x[3] = xCenter + delX[1];
            y[3] = y[2];
            x[4] = xCenter + delX[0];
            y[4] = y[1];
            x[5] = x[3];
            y[5] = y[0];

            poly->AddBin(6, x, y);

            // Go up
            yCenter += par[1];
        }
        // Increment the starting position
        xCenter += par[0];
        if (nRow % 2 == 0)
            yCenter = -0.5 * par[1];
        else
            yCenter = 0;
    }
}

//   Y-axis  long honeycomb pad
void PadMap::BuildHoneyCombPadYLong(TH2Poly *poly) {


    double fPadWidth = 6.;
    double fPadHeight = 6.2;
    double fPadGap = 0.5;

    double x[6], y[6];
    double delY[2];

    delY[0] = 0.5 * (1 - fPadGap / fPadHeight) * (pow(fPadWidth, 2) + pow(0.5 * fPadHeight, 2)) / fPadWidth;
    delY[1] = sqrt(pow(delY[0], 2) - pow(0.5 * fPadHeight - fPadGap, 2));

    cout << delY[0] << endl;
    cout << delY[1] << endl;

    double xCenter = 0, yCenter = 0;
    double boundaryX = fPadGap/2;
    double boundaryY1 = 2.2*(sqrt(3)*fPadGap/2)/5;
    double boundaryY2 = 2.2*(sqrt(3)*fPadGap/2)/5;

    for (int nRow = 0; nRow < 16; nRow++) {
        
        for (int nColumn = 0; nColumn < 16; nColumn++) {
            
            x[0] = xCenter - 0.5 * (fPadWidth - fPadGap);
            y[0] = yCenter - delY[1];
            x[1] = xCenter;
            y[1] = yCenter - delY[0];
            x[2] = xCenter + 0.5 * (fPadWidth - fPadGap);
            y[2] = y[0];
            x[3] = x[2];
            y[3] = yCenter + delY[1];
            x[4] = x[1];
            y[4] = yCenter + delY[0];
            x[5] = x[0];
            y[5] = y[3];


            poly->AddBin(6, x, y);

    
            xCenter += fPadWidth;
        }
    
        yCenter += fPadHeight;
        if (nRow % 2 == 0)
            xCenter = +0.5 * fPadWidth;
        else
            xCenter = 0;
    }


}

void PadMap::CylindarPad(TH2Poly *pad){

    double fPadWidth = 6.;
    double fPadHeight = 6.;
    double fPadGap = 0.5;
    int layerNum =65;
    int rowNum[65];
    double fTpcR1 =194.;
    double x[6], y[6];
    double delY[2];

    for(Int_t iLayer = 0; iLayer < layerNum; iLayer++) {
        Double_t layerPosY = (layerNum/2-iLayer)*6;
        Double_t layerPosX = int(sqrt(pow(fTpcR1,2) - pow(layerPosY,2))/6)*6;
        rowNum[iLayer] = layerPosX*2/6+1;
        cout << rowNum[iLayer] << endl;
    }


  delY[0] = 0.5 * (1 - fPadGap / fPadHeight) * (pow(fPadWidth, 2) + pow(0.5 * fPadHeight, 2)) / fPadWidth;
  delY[1] = sqrt(pow(delY[0], 2) - pow(0.5 * fPadHeight - fPadGap, 2));

  double boundaryX = fPadGap/2;
  double boundaryY1 = 2.*(sqrt(3)*fPadGap/2)/5;
  double boundaryY2 = 2.*(sqrt(3)*fPadGap/2)/5;

  for (int layer = 0; layer < layerNum; layer++) {
    Double_t layerYLength = (layerNum/2-layer)*6;
    Double_t layerXLength = int(sqrt(pow(fTpcR1,2) - pow(layerYLength,2))/6)*6;

    Double_t xCenter =0.;
    Double_t yCenter = layerYLength;

    if(layer%2 ==0){
       xCenter = -layerXLength +fPadWidth/2;
    }
    else{
        xCenter = -layerXLength;
    }

    for (int row = 0; row < rowNum[layer]; row++) {
        
      x[0] = xCenter - 0.5 * (fPadWidth - fPadGap) -boundaryX;
      y[0] = yCenter - delY[1] -boundaryY2;
      x[1] = xCenter;
      y[1] = yCenter - delY[0] -boundaryY2;
      x[2] = xCenter + 0.5 * (fPadWidth - fPadGap) +boundaryX;
      y[2] = y[0];
      x[3] = x[2];
      y[3] = yCenter + delY[1] +boundaryY1;
      x[4] = x[1];
      y[4] = yCenter + delY[0] +boundaryY1;
      x[5] = x[0];
      y[5] = y[3];

      pad->AddBin(6, x, y);

      xCenter += fPadWidth;
    }
  }  
    
}

void PadMap::TrianglePad(TH2Poly *poly) {
    // Add the bins
    double x[3], y[3];
    // Set hexagon pattern parameters
    double par[2], delL[3];
    par[0] = 2.565;
    par[1] = 0.5;  // x-spacing, y-spacing, gap spacing

    delL[0] = par[0]/2;
    delL[1] = sqrt(pow(par[0],2) - pow(delL[0], 2))/3;
    delL[2] = 0.5/(TMath::Cos(TMath::Pi()/6));


    double xCenter = 0, yCenter = 0;
    for (int nRow = 0; nRow < 16; nRow++) {

        for (int nColumn = 0; nColumn < 16; nColumn++) {
            // Go around the hexagon

            // if(nRow%2==0){
            //     if(nColumn%2 ==0){
            //         x[0] = xCenter - delL[0];
            //         y[0] = yCenter - delL[1];
            //         x[1] = xCenter + delL[0];
            //         y[1] = yCenter - delL[1];
            //         x[2] = xCenter;
            //         y[2] = yCenter + 2*delL[1];
            //     }
            //     if(nColumn%2 ==1){
            //         x[0] = xCenter - delL[0];
            //         y[0] = yCenter + 2*delL[1];
            //         x[1] = xCenter + delL[0];
            //         y[1] = yCenter + 2*delL[1];
            //         x[2] = xCenter;
            //         y[2] = yCenter - delL[1];
            //     }
            // }
            // if(nRow%2==1){
            //     if(nColumn%2 ==0 || nColumn==0){
            //         x[0] = xCenter - delL[0];
            //         y[0] = yCenter + 2*delL[1];
            //         x[1] = xCenter + delL[0];
            //         y[1] = yCenter + 2*delL[1];
            //         x[2] = xCenter;
            //         y[2] = yCenter - delL[1];
            //     }
            //     if(nColumn%2 ==1){
            //         x[0] = xCenter - delL[0];
            //         y[0] = yCenter - delL[1];
            //         x[1] = xCenter + delL[0];
            //         y[1] = yCenter - delL[1];
            //         x[2] = xCenter;
            //         y[2] = yCenter + 2*delL[1];
            //     }
            // }

            

            poly->AddBin(3, x, y);

            xCenter += 2*(delL[0]+delL[2]);
        }
        xCenter = 0.;
    }
}

void PadMap::TransformChannelToPad(int agetID, int channelID, int &colN, int &rowN) {
    if (agetID == 0) {
        switch (channelID) {
            case 0:
                colN = 8;
                rowN = 7;
                break;
            case 1:
                colN = 9;
                rowN = 7;
                break;
            case 2:
                colN = 10;
                rowN = 7;
                break;
            case 3:
                colN = 11;
                rowN = 7;
                break;
            case 4:
                colN = 12;
                rowN = 7;
                break;
            case 5:
                colN = 13;
                rowN = 7;
                break;
            case 6:
                colN = 14;
                rowN = 7;
                break;
            case 7:
                colN = 15;
                rowN = 7;
                break;
            case 8:
                colN = 8;
                rowN = 6;
                break;
            case 9:
                colN = 9;
                rowN = 6;
                break;
            case 10:
                colN = 10;
                rowN = 6;
                break;
            case 12:
                colN = 11;
                rowN = 6;
                break;
            case 13:
                colN = 12;
                rowN = 6;
                break;
            case 14:
                colN = 13;
                rowN = 6;
                break;
            case 15:
                colN = 14;
                rowN = 6;
                break;
            case 16:
                colN = 15;
                rowN = 6;
                break;
            case 17:
                colN = 8;
                rowN = 5;
                break;
            case 18:
                colN = 9;
                rowN = 5;
                break;
            case 19:
                colN = 10;
                rowN = 5;
                break;
            case 20:
                colN = 11;
                rowN = 5;
                break;
            case 21:
                colN = 12;
                rowN = 5;
                break;
            case 23:
                colN = 13;
                rowN = 5;
                break;
            case 24:
                colN = 14;
                rowN = 5;
                break;
            case 25:
                colN = 15;
                rowN = 5;
                break;
            case 26:
                colN = 8;
                rowN = 4;
                break;
            case 27:
                colN = 9;
                rowN = 4;
                break;
            case 28:
                colN = 10;
                rowN = 4;
                break;
            case 29:
                colN = 11;
                rowN = 4;
                break;
            case 30:
                colN = 12;
                rowN = 4;
                break;
            case 31:
                colN = 13;
                rowN = 4;
                break;
            case 32:
                colN = 14;
                rowN = 4;
                break;
            case 33:
                colN = 15;
                rowN = 4;
                break;
            case 34:
                colN = 8;
                rowN = 3;
                break;
            case 35:
                colN = 9;
                rowN = 3;
                break;
            case 36:
                colN = 10;
                rowN = 3;
                break;
            case 37:
                colN = 11;
                rowN = 3;
                break;
            case 38:
                colN = 12;
                rowN = 3;
                break;
            case 39:
                colN = 13;
                rowN = 3;
                break;
            case 40:
                colN = 14;
                rowN = 3;
                break;
            case 41:
                colN = 15;
                rowN = 3;
                break;
            case 42:
                colN = 8;
                rowN = 2;
                break;
            case 43:
                colN = 9;
                rowN = 2;
                break;
            case 44:
                colN = 10;
                rowN = 2;
                break;
            case 46:
                colN = 11;
                rowN = 2;
                break;
            case 47:
                colN = 12;
                rowN = 2;
                break;
            case 48:
                colN = 13;
                rowN = 2;
                break;
            case 49:
                colN = 14;
                rowN = 2;
                break;
            case 50:
                colN = 15;
                rowN = 2;
                break;
            case 51:
                colN = 8;
                rowN = 1;
                break;
            case 52:
                colN = 9;
                rowN = 1;
                break;
            case 53:
                colN = 10;
                rowN = 1;
                break;
            case 54:
                colN = 11;
                rowN = 1;
                break;
            case 55:
                colN = 12;
                rowN = 1;
                break;
            case 57:
                colN = 13;
                rowN = 1;
                break;
            case 58:
                colN = 14;
                rowN = 1;
                break;
            case 59:
                colN = 15;
                rowN = 1;
                break;
            case 60:
                colN = 8;
                rowN = 0;
                break;
            case 61:
                colN = 9;
                rowN = 0;
                break;
            case 62:
                colN = 10;
                rowN = 0;
                break;
            case 63:
                colN = 11;
                rowN = 0;
                break;
            case 64:
                colN = 12;
                rowN = 0;
                break;
            case 65:
                colN = 13;
                rowN = 0;
                break;
            case 66:
                colN = 14;
                rowN = 0;
                break;
            case 67:
                colN = 15;
                rowN = 0;
                break;

            default:
                colN = -99;
                rowN = -99;
                break;
        }
        return;
    } else if (agetID == 1) {
        switch (channelID) {
            case 0:
                colN = 8;
                rowN = 15;
                break;
            case 1:
                colN = 9;
                rowN = 15;
                break;
            case 2:
                colN = 10;
                rowN = 15;
                break;
            case 3:
                colN = 11;
                rowN = 15;
                break;
            case 4:
                colN = 12;
                rowN = 15;
                break;
            case 5:
                colN = 13;
                rowN = 15;
                break;
            case 6:
                colN = 14;
                rowN = 15;
                break;
            case 7:
                colN = 15;
                rowN = 15;
                break;
            case 8:
                colN = 8;
                rowN = 14;
                break;
            case 9:
                colN = 9;
                rowN = 14;
                break;
            case 10:
                colN = 10;
                rowN = 14;
                break;
            case 12:
                colN = 11;
                rowN = 14;
                break;
            case 13:
                colN = 12;
                rowN = 14;
                break;
            case 14:
                colN = 13;
                rowN = 14;
                break;
            case 15:
                colN = 14;
                rowN = 14;
                break;
            case 16:
                colN = 15;
                rowN = 14;
                break;
            case 17:
                colN = 8;
                rowN = 13;
                break;
            case 18:
                colN = 9;
                rowN = 13;
                break;
            case 19:
                colN = 10;
                rowN = 13;
                break;
            case 20:
                colN = 11;
                rowN = 13;
                break;
            case 21:
                colN = 12;
                rowN = 13;
                break;
            case 23:
                colN = 13;
                rowN = 13;
                break;
            case 24:
                colN = 14;
                rowN = 13;
                break;
            case 25:
                colN = 15;
                rowN = 13;
                break;
            case 26:
                colN = 8;
                rowN = 12;
                break;
            case 27:
                colN = 9;
                rowN = 12;
                break;
            case 28:
                colN = 10;
                rowN = 12;
                break;
            case 29:
                colN = 11;
                rowN = 12;
                break;
            case 30:
                colN = 12;
                rowN = 12;
                break;
            case 31:
                colN = 13;
                rowN = 12;
                break;
            case 32:
                colN = 14;
                rowN = 12;
                break;
            case 33:
                colN = 15;
                rowN = 12;
                break;
            case 34:
                colN = 8;
                rowN = 11;
                break;
            case 35:
                colN = 9;
                rowN = 11;
                break;
            case 36:
                colN = 10;
                rowN = 11;
                break;
            case 37:
                colN = 11;
                rowN = 11;
                break;
            case 38:
                colN = 12;
                rowN = 11;
                break;
            case 39:
                colN = 13;
                rowN = 11;
                break;
            case 40:
                colN = 14;
                rowN = 11;
                break;
            case 41:
                colN = 15;
                rowN = 11;
                break;
            case 42:
                colN = 8;
                rowN = 10;
                break;
            case 43:
                colN = 9;
                rowN = 10;
                break;
            case 44:
                colN = 10;
                rowN = 10;
                break;
            case 46:
                colN = 11;
                rowN = 10;
                break;
            case 47:
                colN = 12;
                rowN = 10;
                break;
            case 48:
                colN = 13;
                rowN = 10;
                break;
            case 49:
                colN = 14;
                rowN = 10;
                break;
            case 50:
                colN = 15;
                rowN = 10;
                break;
            case 51:
                colN = 8;
                rowN = 9;
                break;
            case 52:
                colN = 9;
                rowN = 9;
                break;
            case 53:
                colN = 10;
                rowN = 9;
                break;
            case 54:
                colN = 11;
                rowN = 9;
                break;
            case 55:
                colN = 12;
                rowN = 9;
                break;
            case 57:
                colN = 13;
                rowN = 9;
                break;
            case 58:
                colN = 14;
                rowN = 9;
                break;
            case 59:
                colN = 15;
                rowN = 9;
                break;
            case 60:
                colN = 8;
                rowN = 8;
                break;
            case 61:
                colN = 9;
                rowN = 8;
                break;
            case 62:
                colN = 10;
                rowN = 8;
                break;
            case 63:
                colN = 11;
                rowN = 8;
                break;
            case 64:
                colN = 12;
                rowN = 8;
                break;
            case 65:
                colN = 13;
                rowN = 8;
                break;
            case 66:
                colN = 14;
                rowN = 8;
                break;
            case 67:
                colN = 15;
                rowN = 8;
                break;

            default:
                colN = -99;
                rowN = -99;
                break;
        }
        return;
    } else if (agetID == 2) {
        switch (channelID) {
            case 0:
                colN = 4;
                rowN = 15;
                break;
            case 1:
                colN = 4;
                rowN = 14;
                break;
            case 2:
                colN = 4;
                rowN = 13;
                break;
            case 3:
                colN = 4;
                rowN = 12;
                break;
            case 4:
                colN = 4;
                rowN = 11;
                break;
            case 5:
                colN = 4;
                rowN = 10;
                break;
            case 6:
                colN = 4;
                rowN = 9;
                break;
            case 7:
                colN = 4;
                rowN = 8;
                break;
            case 8:
                colN = 4;
                rowN = 7;
                break;
            case 9:
                colN = 4;
                rowN = 6;
                break;
            case 10:
                colN = 4;
                rowN = 5;
                break;
            case 12:
                colN = 4;
                rowN = 4;
                break;
            case 13:
                colN = 4;
                rowN = 3;
                break;
            case 14:
                colN = 4;
                rowN = 2;
                break;
            case 15:
                colN = 4;
                rowN = 1;
                break;
            case 16:
                colN = 4;
                rowN = 0;
                break;
            case 17:
                colN = 5;
                rowN = 15;
                break;
            case 18:
                colN = 5;
                rowN = 14;
                break;
            case 19:
                colN = 5;
                rowN = 13;
                break;
            case 20:
                colN = 5;
                rowN = 12;
                break;
            case 21:
                colN = 5;
                rowN = 11;
                break;
            case 23:
                colN = 5;
                rowN = 10;
                break;
            case 24:
                colN = 5;
                rowN = 9;
                break;
            case 25:
                colN = 5;
                rowN = 8;
                break;
            case 26:
                colN = 5;
                rowN = 7;
                break;
            case 27:
                colN = 5;
                rowN = 6;
                break;
            case 28:
                colN = 5;
                rowN = 5;
                break;
            case 29:
                colN = 5;
                rowN = 4;
                break;
            case 30:
                colN = 5;
                rowN = 3;
                break;
            case 31:
                colN = 5;
                rowN = 2;
                break;
            case 32:
                colN = 5;
                rowN = 1;
                break;
            case 33:
                colN = 5;
                rowN = 0;
                break;
            case 34:
                colN = 6;
                rowN = 15;
                break;
            case 35:
                colN = 6;
                rowN = 14;
                break;
            case 36:
                colN = 6;
                rowN = 13;
                break;
            case 37:
                colN = 6;
                rowN = 12;
                break;
            case 38:
                colN = 6;
                rowN = 11;
                break;
            case 39:
                colN = 6;
                rowN = 10;
                break;
            case 40:
                colN = 6;
                rowN = 9;
                break;
            case 41:
                colN = 6;
                rowN = 8;
                break;
            case 42:
                colN = 6;
                rowN = 7;
                break;
            case 43:
                colN = 6;
                rowN = 6;
                break;
            case 44:
                colN = 6;
                rowN = 5;
                break;
            case 46:
                colN = 6;
                rowN = 4;
                break;
            case 47:
                colN = 6;
                rowN = 3;
                break;
            case 48:
                colN = 6;
                rowN = 2;
                break;
            case 49:
                colN = 6;
                rowN = 1;
                break;
            case 50:
                colN = 6;
                rowN = 0;
                break;
            case 51:
                colN = 7;
                rowN = 15;
                break;
            case 52:
                colN = 7;
                rowN = 14;
                break;
            case 53:
                colN = 7;
                rowN = 13;
                break;
            case 54:
                colN = 7;
                rowN = 12;
                break;
            case 55:
                colN = 7;
                rowN = 11;
                break;
            case 57:
                colN = 7;
                rowN = 10;
                break;
            case 58:
                colN = 7;
                rowN = 9;
                break;
            case 59:
                colN = 7;
                rowN = 8;
                break;
            case 60:
                colN = 7;
                rowN = 7;
                break;
            case 61:
                colN = 7;
                rowN = 6;
                break;
            case 62:
                colN = 7;
                rowN = 5;
                break;
            case 63:
                colN = 7;
                rowN = 4;
                break;
            case 64:
                colN = 7;
                rowN = 3;
                break;
            case 65:
                colN = 7;
                rowN = 2;
                break;
            case 66:
                colN = 7;
                rowN = 1;
                break;
            case 67:
                colN = 7;
                rowN = 0;
                break;

            default:
                colN = -99;
                rowN = -99;
                break;
        }
        return;
    } else if (agetID == 3) {
        switch (channelID) {
            case 0:
                colN = 0;
                rowN = 15;
                break;
            case 1:
                colN = 0;
                rowN = 14;
                break;
            case 2:
                colN = 0;
                rowN = 13;
                break;
            case 3:
                colN = 0;
                rowN = 12;
                break;
            case 4:
                colN = 0;
                rowN = 11;
                break;
            case 5:
                colN = 0;
                rowN = 10;
                break;
            case 6:
                colN = 0;
                rowN = 9;
                break;
            case 7:
                colN = 0;
                rowN = 8;
                break;
            case 8:
                colN = 0;
                rowN = 7;
                break;
            case 9:
                colN = 0;
                rowN = 6;
                break;
            case 10:
                colN = 0;
                rowN = 5;
                break;
            case 12:
                colN = 0;
                rowN = 4;
                break;
            case 13:
                colN = 0;
                rowN = 3;
                break;
            case 14:
                colN = 0;
                rowN = 2;
                break;
            case 15:
                colN = 0;
                rowN = 1;
                break;
            case 16:
                colN = 0;
                rowN = 0;
                break;
            case 17:
                colN = 1;
                rowN = 15;
                break;
            case 18:
                colN = 1;
                rowN = 14;
                break;
            case 19:
                colN = 1;
                rowN = 13;
                break;
            case 20:
                colN = 1;
                rowN = 12;
                break;
            case 21:
                colN = 1;
                rowN = 11;
                break;
            case 23:
                colN = 1;
                rowN = 10;
                break;
            case 24:
                colN = 1;
                rowN = 9;
                break;
            case 25:
                colN = 1;
                rowN = 8;
                break;
            case 26:
                colN = 1;
                rowN = 7;
                break;
            case 27:
                colN = 1;
                rowN = 6;
                break;
            case 28:
                colN = 1;
                rowN = 5;
                break;
            case 29:
                colN = 1;
                rowN = 4;
                break;
            case 30:
                colN = 1;
                rowN = 3;
                break;
            case 31:
                colN = 1;
                rowN = 2;
                break;
            case 32:
                colN = 1;
                rowN = 1;
                break;
            case 33:
                colN = 1;
                rowN = 0;
                break;
            case 34:
                colN = 2;
                rowN = 15;
                break;
            case 35:
                colN = 2;
                rowN = 14;
                break;
            case 36:
                colN = 2;
                rowN = 13;
                break;
            case 37:
                colN = 2;
                rowN = 12;
                break;
            case 38:
                colN = 2;
                rowN = 11;
                break;
            case 39:
                colN = 2;
                rowN = 10;
                break;
            case 40:
                colN = 2;
                rowN = 9;
                break;
            case 41:
                colN = 2;
                rowN = 8;
                break;
            case 42:
                colN = 2;
                rowN = 7;
                break;
            case 43:
                colN = 2;
                rowN = 6;
                break;
            case 44:
                colN = 2;
                rowN = 5;
                break;
            case 46:
                colN = 2;
                rowN = 4;
                break;
            case 47:
                colN = 2;
                rowN = 3;
                break;
            case 48:
                colN = 2;
                rowN = 2;
                break;
            case 49:
                colN = 2;
                rowN = 1;
                break;
            case 50:
                colN = 2;
                rowN = 0;
                break;
            case 51:
                colN = 3;
                rowN = 15;
                break;
            case 52:
                colN = 3;
                rowN = 14;
                break;
            case 53:
                colN = 3;
                rowN = 13;
                break;
            case 54:
                colN = 3;
                rowN = 12;
                break;
            case 55:
                colN = 3;
                rowN = 11;
                break;
            case 57:
                colN = 3;
                rowN = 10;
                break;
            case 58:
                colN = 3;
                rowN = 9;
                break;
            case 59:
                colN = 3;
                rowN = 8;
                break;
            case 60:
                colN = 3;
                rowN = 7;
                break;
            case 61:
                colN = 3;
                rowN = 6;
                break;
            case 62:
                colN = 3;
                rowN = 5;
                break;
            case 63:
                colN = 3;
                rowN = 4;
                break;
            case 64:
                colN = 3;
                rowN = 3;
                break;
            case 65:
                colN = 3;
                rowN = 2;
                break;
            case 66:
                colN = 3;
                rowN = 1;
                break;
            case 67:
                colN = 3;
                rowN = 0;
                break;

            default:
                colN = -99;
                rowN = -99;
                break;
        }
        return;
    }
}
