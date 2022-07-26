#ifndef __PadMap_h
#define __PadMap_h

#include <map>
#include <vector>
#include <cmath>
#include <tuple>

#include "TH2Poly.h"

class PadMap {
private:
    std::map<std::pair<int, int>, std::vector<int>> mapToID_;
    std::map<std::pair<int, int>, std::vector<int>> mapToPosition_;
    std::vector<std::vector<std::tuple<Int_t, Double_t, Double_t>>> fPadChArray[3]; 
public:
    PadMap();
    void TransformChannelToPad(int agetID, int channelID, int &colN, int &rowN);
    int GetAgetID(int colN, int rowN);
    int GetChannelID(int colN, int rowN);
    // int GetFPNChannelID(int channelID);
    int GetColN(int agetID, int channelID);
    int GetRowN(int agetID, int channelID);
    float GetX(int colN);
    float GetY(int colN, int rowN);
    void BuildHoneyCombPadXLong(TH2Poly *poly);
    void BuildHoneyCombPadYLong(TH2Poly *poly);
    void CylindarPad(TH2Poly *poly);
    void TrianglePad(TH2Poly *poly);
    void StripPad(TH2Poly *poly);
    std::vector<std::vector<std::tuple<Int_t, Double_t, Double_t>>> GetTestPad(int chan){return fPadChArray[chan];}
};

#endif
