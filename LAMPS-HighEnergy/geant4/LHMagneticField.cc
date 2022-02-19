#include "LHMagneticField.hh"

#include "TFile.h"
#include "TH3.h"

#include "G4SystemOfUnits.hh"

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
using namespace std;

LHMagneticField::LHMagneticField():
	Verbosity(0),
	filename("LHMagneticFieldMap.txt"),
	xMin(NAN), xMax(NAN), xOffset(0),
	yMin(NAN), yMax(NAN), yOffset(0),
	zMin(NAN), zMax(NAN), zOffset(0),
	setMonoB(false), mBx(NAN), mBy(NAN), mBz(NAN),
	unitD(mm), unitF(tesla)
{
	mapBx = new TH3D();
	mapBy = new TH3D();
	mapBz = new TH3D();
}//Constructor

LHMagneticField::~LHMagneticField()
{
	mapBx->Delete();
	mapBy->Delete();
	mapBx->Delete();
}//Destructor

//+++++++++++++++++++++++++++++++++++++++++++++++++
void LHMagneticField::MakeFieldMap(string fname)
{
	mapBx->Reset();
	mapBy->Reset();
	mapBz->Reset();

	//Warnings
	const char* CWM = "LHMagneticField::MakeFieldMap - WARNING!";
	if (xOffset!=0 || yOffset!=0 || zOffset!=0)
	{
		printf("%s Applying offsets... %f %f %f\n", CWM, xOffset, yOffset, zOffset);
	}
	if (setMonoB) printf("%s Appllying uniform B-field... %f %f %f\n", CWM, mBx, mBy, mBz);
	if (unitD != mm) printf("%s Updating unit distance... %f\n", CWM, unitD);
	if (unitF != tesla) printf("%s Updating unit field... %f\n", CWM, unitF);

	//Open file
	ifstream in(fname.c_str());
    if (!in.is_open())
	{
		G4Exception("LHMagneticField::MakeFieldMap", "FileOpenFail", FatalException, "");
		return;
    }

	//-------------------------------------------

	//1st reading to determine binning and boundary
	int Count = 0;
	set<double> xPos, yPos, zPos;
	while (in.peek() != EOF)
	{
		//Read 1st line and discard it
		if (Count == 0) { string firstLine; std::getline(in, firstLine); }

		double x,y,z, Bx=0,By=0,Bz;
		in >> x >> y >> z >> Bz;
		if (!in.good()) break;

		//Apply distance unit and Offset
		x = x * (unitD/mm) + xOffset;
		y = y * (unitD/mm) + yOffset;
		z = z * (unitD/mm) + zOffset;

		//Skip positions out of boundary
		if ( !isnan(xMin) && x<xMin ) { if (Verbosity>1) printf("%f < %f (xMin), skip...\n", x,xMin); continue; }
		if ( !isnan(xMax) && x>xMax ) { if (Verbosity>1) printf("%f > %f (xMax), skip...\n", x,xMax); continue; }
		if ( !isnan(yMin) && y<yMin ) { if (Verbosity>1) printf("%f < %f (yMin), skip...\n", y,yMin); continue; }
		if ( !isnan(yMax) && y>yMax ) { if (Verbosity>1) printf("%f > %f (yMax), skip...\n", y,yMax); continue; }
		if ( !isnan(zMin) && z<zMin ) { if (Verbosity>1) printf("%f < %f (zMin), skip...\n", z,zMin); continue; }
		if ( !isnan(zMax) && z>zMax ) { if (Verbosity>1) printf("%f > %f (zMax), skip...\n", z,zMax); continue; }
		if ( Verbosity>0 ) printf("Reading: %6d | %6.0f %6.0f %6.0f | %1.0f %1.0f %10.9f\n", Count,x,y,z,Bx,By,Bz);

		xPos.insert(x);
		yPos.insert(y);
		zPos.insert(z);
		Count++;
	}//While loop: reading file
	in.close();

	//Define TH3 maps
	const int xBin = xPos.size() - 1;
	const int yBin = yPos.size() - 1;
	const int zBin = zPos.size() - 1;
	const float x1 = (float)*xPos.begin();
	const float y1 = (float)*yPos.begin();
	const float z1 = (float)*zPos.begin();
	const float x2 = (float)*xPos.rbegin();
	const float y2 = (float)*yPos.rbegin();
	const float z2 = (float)*zPos.rbegin();
	const float xBinW = (float)mapBz->GetXaxis()->GetBinWidth(1);
	const float yBinW = (float)mapBz->GetYaxis()->GetBinWidth(1);
	const float zBinW = (float)mapBz->GetZaxis()->GetBinWidth(1);
	mapBz->SetBins(xBin+1,x1,x2+xBinW, yBin+1,y1,y2+yBinW, zBin+1,z1,z2+zBinW);
	mapBz->SetName("mapBz");
	mapBz->Sumw2();
	mapBx = (TH3D*)mapBz->Clone("mapBx");
	mapBy = (TH3D*)mapBz->Clone("mapBy");

	//-------------------------------------------

	//2nd reading for field mapping
	Count = 0;
	in.clear();
	in.open(fname.c_str());
	while (in.peek() != EOF)
	{
		//Read 1st line and discard it
		if (Count == 0) { string firstLine; std::getline(in, firstLine); Count++; }

		double x,y,z, Bx=0,By=0,Bz;
		in >> x >> y >> z >> Bz;
		if (!in.good()) break;

		//Apply distance unit and Offset
		x = x * (unitD/mm) + xOffset;
		y = y * (unitD/mm) + yOffset;
		z = z * (unitD/mm) + zOffset;

		//All maps share same structure
		const int xBinTemp = mapBz->GetXaxis()->FindBin(x + 0.01);
		const int yBinTemp = mapBz->GetYaxis()->FindBin(y + 0.01);
		const int zBinTemp = mapBz->GetZaxis()->FindBin(z + 0.01);
		if (xBinTemp<1 || xBinTemp>mapBz->GetNbinsX()) continue;
		if (yBinTemp<1 || yBinTemp>mapBz->GetNbinsY()) continue;
		if (zBinTemp<1 || zBinTemp>mapBz->GetNbinsZ()) continue;

        //Apply field unit or set it constant if the flag is on
        Bx = setMonoB?mBx:(Bx * unitF);
        By = setMonoB?mBy:(By * unitF);
        Bz = setMonoB?mBz:(Bz * unitF);

		mapBx->SetBinContent(xBinTemp, yBinTemp, zBinTemp, Bx);
		mapBy->SetBinContent(xBinTemp, yBinTemp, zBinTemp, By);
		mapBz->SetBinContent(xBinTemp, yBinTemp, zBinTemp, Bz);
	}//While loop: reading file
	in.close();

	#if 0
	TFile* mapROOT = new TFile("LHMagneticFieldMap.root", "RECREATE");
	mapBx->Write();
	mapBy->Write();
	mapBz->Write();
	mapROOT->Close();
	mapROOT->Delete();
	#endif

	//-------------------------------------------

	//Set boundaries if it's not defined already
	if (isnan(xMin)) xMin = mapBz->GetXaxis()->GetBinLowEdge(1);
	if (isnan(yMin)) yMin = mapBz->GetYaxis()->GetBinLowEdge(1);
	if (isnan(zMin)) zMin = mapBz->GetZaxis()->GetBinLowEdge(1);
	if (isnan(xMax)) xMax = mapBz->GetXaxis()->GetBinUpEdge(mapBz->GetNbinsX());
	if (isnan(yMax)) yMax = mapBz->GetYaxis()->GetBinUpEdge(mapBz->GetNbinsY());
	if (isnan(zMax)) zMax = mapBz->GetZaxis()->GetBinUpEdge(mapBz->GetNbinsZ());

	return;
}//MakeFieldMap

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LHMagneticField::GetFieldValue(const double Point[4], double *Bfield) const
{
	double x = Point[0];
	double y = Point[1];
	double z = Point[2];

	const double xBinW = mapBx->GetXaxis()->GetBinWidth(1);
	const double yBinW = mapBy->GetYaxis()->GetBinWidth(1);
	const double zBinW = mapBz->GetZaxis()->GetBinWidth(1);
	//if (x<xMin || x>xMax || y<yMin || y>yMax || z<zMin || z>zMax)
	if ( x < (xMin + xBinW) || x > (xMax - xBinW) ||
		 y < (yMin + yBinW) || y > (yMax - yBinW) ||
		 z < (zMin + zBinW) || z > (zMax - zBinW) ) //To avoid TH3 interpolation error at boundary
	{
		Bfield[0] = 0.;
		Bfield[1] = 0.;
		Bfield[2] = 0.;
	}
	else
	{
		Bfield[0] = mapBx->Interpolate(x, y, z);
		Bfield[1] = mapBy->Interpolate(x, y, z);
		Bfield[2] = mapBz->Interpolate(x, y, z);
		if (Verbosity>2) printf("%7.1f %7.1f %7.1f | %f %f %f \n", x,y,z, Bfield[0],Bfield[1],Bfield[2]);
	}

	return;
}//GetFieldValue
