#include "LHNDFastDigiTask.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"
#include "KBNDHit.hh"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"

#include <iostream>
using namespace std;

ClassImp(LHNDFastDigiTask)

LHNDFastDigiTask::LHNDFastDigiTask()
:KBTask("LHNDFastDigiTask","")
{
}

bool LHNDFastDigiTask::Init()
{
  KBRun *run = KBRun::GetRun();

	KBParameterContainer *par = run -> GetParameterContainer();

	ndLayerN = par -> GetParInt("NDLayerN");
	ndSlatN = par -> GetParInt("NDSlatN");

	double ndLayerZ = par -> GetParDouble("NDLayerZ"); //1st VETO's z offset
	double ndLayerD = par -> GetParDouble("NDLayerD"); //Distance btw each layer surface
	double ndSlatT = par -> GetParDouble("NDSlatT"); //Slat thickness (z)
	double ndVetoT = par -> GetParDouble("NDVetoT"); //Slat thickness (z)
	double ndTotalT = ndVetoT + (2*ndSlatT + ndLayerD) * (ndLayerN-1);

	ndTheta = par -> GetParDouble("NDTheta")/180.*TMath::Pi(); //Theta angle of entire ND

	ndOfsX = sin(ndTheta) * (ndLayerZ + ndTotalT/2);
	ndOfsZ = cos(ndTheta) * (ndLayerZ + ndTotalT/2);

	fMCStepArray = (TClonesArray *) run -> GetBranch(Form("MCStep%d",fDetID));

	fHitArray = new TClonesArray("KBNDHit");
	run -> RegisterBranch(fOutputBranchName, fHitArray, fPersistency);

	for (int ii=0; ii<ndLayerN*ndSubLayerN*ndSlatN; ii++){
		h2d_time_pos[ii] = new TH2D(Form("h2d_time_pos_%02d",ii),"",1000,0,1000,200,-1000,1000);
	}

	//x or y
	hprof_fixed_pos[0] = new TProfile("hprof_fixed_pos_0","",ndLayerN*ndSubLayerN*ndSlatN,0,ndLayerN*ndSubLayerN*ndSlatN);
	hprof_fixed_pos[0]->SetErrorOption("s");
	//z
	hprof_fixed_pos[1] = new TProfile("hprof_fixed_pos_1","",ndLayerN*ndSubLayerN*ndSlatN,0,ndLayerN*ndSubLayerN*ndSlatN);
	hprof_fixed_pos[1]->SetErrorOption("s");

  return true;
}

void LHNDFastDigiTask::Exec(Option_t*)
{

	fHitArray->Clear("C");

	Long64_t nMCSteps = fMCStepArray -> GetEntries();
	for (Long64_t iStep = 0; iStep < nMCSteps; ++iStep) {
		KBMCStep* step = (KBMCStep*) fMCStepArray -> At(iStep);

		Double_t edep = step -> GetEdep();
		Double_t time = step -> GetTime();

		Int_t modID = step -> GetModuleID();

		Double_t xx = step -> GetX();
		Double_t yy = step -> GetY();
		Double_t zz = step -> GetZ();

		int layer = modID/100;
		int mod = modID%100;

		//global to local
		xx -= ndOfsX;
		zz -= ndOfsZ;

		TVector3 vec(xx, yy, zz);
		vec.RotateY(-ndTheta);

		int ind = layer*ndSubLayerN*ndSlatN + mod;

		if ( mod<ndSlatN ){
			h2d_time_pos[ind]->Fill(time, vec.Y(), edep);
			hprof_fixed_pos[0]->Fill(ind, vec.X());
		}else{
			h2d_time_pos[ind]->Fill(time, vec.X(), edep);
			hprof_fixed_pos[0]->Fill(ind, vec.Y());
		}
		hprof_fixed_pos[1]->Fill(ind, vec.Z());

	}//

	/*
	for (int il=0; il<ndLayerN; il++){
		for (int is=0; is<ndSubLayerN; is++){
			if ( il==0 && is==0 ) continue;

			for (int im=0; im<ndSlatN; im++){

				int ind = il*ndSubLayerN*ndSlatN + is*ndSlatN + im;

				cout << il << " " << is << " " << im << " " 
					<< hprof_fixed_pos[0]->GetBinContent(ind+1) << " "
					<< hprof_fixed_pos[0]->GetBinError(ind+1) << " "
					<< hprof_fixed_pos[1]->GetBinContent(ind+1) << " "
					<< hprof_fixed_pos[1]->GetBinError(ind+1) << " "
					<< endl;
			}
		}
	}
	*/

	int idx = 0;

	for (int il=0; il<ndLayerN; il++){
		for (int is=0; is<ndSubLayerN; is++){
			if ( il==0 && is==0 ) continue;

			for (int im=0; im<ndSlatN; im++){

				int ind = il*ndSubLayerN*ndSlatN + is*ndSlatN + im;

				while ( h2d_time_pos[ind]->Integral()>0 ){

					int binx, biny, binz;
					float ww = 0, wx = 0, wy = 0;

					int maxbin = h2d_time_pos[ind]->GetMaximumBin();
					h2d_time_pos[ind]->GetBinXYZ(maxbin, binx, biny, binz);

					for (int ix=0; ix<7; ix++){ //+/- 3 ns
						int indx = binx - 3 + ix;
						if ( indx<=0 || indx>1000 ) continue;

						for (int iy=0; iy<90; iy++){ // +/- 45 cm
							int indy = biny - 45 + iy;
							if ( indy<=0 || indy>200 ) continue;

							ww += (h2d_time_pos[ind]->GetBinContent(indx, indy));
							wx += (h2d_time_pos[ind]->GetBinContent(indx, indy))*(h2d_time_pos[ind]->GetXaxis()->GetBinCenter(indx));
							wy += (h2d_time_pos[ind]->GetBinContent(indx, indy))*(h2d_time_pos[ind]->GetYaxis()->GetBinCenter(indy));

							h2d_time_pos[ind]->SetBinContent(indx, indy, 0.0);

						}//iy
					}//ix

					wx /= ww;
					wy /= ww;

					if ( ww<eCut ) continue;

					float recoLX = (is==0) ? hprof_fixed_pos[0]->GetBinContent(ind+1) : wy;
					float recoLY = (is==0) ? wy : hprof_fixed_pos[0]->GetBinContent(ind+1);
					float recoLZ = hprof_fixed_pos[1]->GetBinContent(ind+1);

					//local to global
					TVector3 vecL(recoLX, recoLY, recoLZ);
					vecL.RotateY(+ndTheta);

					TVector3 vecG(vecL.X()+ndOfsX, vecL.Y(), vecL.Z()+ndOfsZ);

					/*
					cout << "L: " << il << " " << is << " " << im << " " 
						<< vecL.X() << " "
						<< vecL.Y() << " "
						<< vecL.Z() << " "
						<< ww << " "
						<< endl;

					cout << "G: " << il << " " << is << " " << im << " " 
						<< vecG.X() << " "
						<< vecG.Y() << " "
						<< vecG.Z() << " "
						<< ww << " "
						<< endl;
					*/

					auto hit = (KBNDHit *) fHitArray -> ConstructedAt(idx);
					hit -> SetHitID(idx);
					hit -> SetX(vecG.X());
					hit -> SetY(vecG.Y());
					hit -> SetZ(vecG.Z());
					hit -> SetCharge(ww);
					hit -> SetLayer(il);
					hit -> SetSubLayer(is);
					hit -> SetModule(im);
					hit -> SetTime(wx);

					idx++;

				}//while

			}//im
		}//is
	}//il


	for (int ii=0; ii<ndLayerN*ndSubLayerN*ndSlatN; ii++){
		h2d_time_pos[ii]->Reset();
	}
	hprof_fixed_pos[0]->Reset();
	hprof_fixed_pos[1]->Reset();

}

void LHNDFastDigiTask::SetChannelPersistency(bool persistence) { fPersistency = persistence; }
