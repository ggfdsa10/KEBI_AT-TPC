#include "LHForwardTrackerFastDigiTask.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"
#include "KBNDHit.hh"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"

#include <iostream>
using namespace std;

ClassImp(LHForwardTrackerFastDigiTask)

LHForwardTrackerFastDigiTask::LHForwardTrackerFastDigiTask()
:KBTask("LHForwardTrackerFastDigiTask","")
{
}

bool LHForwardTrackerFastDigiTask::Init()
{
  KBRun *run = KBRun::GetRun();

	KBParameterContainer *par = run -> GetParameterContainer();

	ftLayerN = par -> GetParInt("FTLayerN");
	ftLayerD = par -> GetParDouble("FTLayerD");
	ftLayerZ = par -> GetParDouble("FTLayerZ");
	ftLayerT = par -> GetParDouble("FTBoxT");

	fMCStepArray = (TClonesArray *) run -> GetBranch(Form("MCStep%d",fDetID));

	fHitArray = new TClonesArray("KBHit");
	run -> RegisterBranch(fOutputBranchName, fHitArray, fPersistency);

	for (int ii=0; ii<ftLayerN; ii++){
		h2d_pos[ii] = new TH2D(Form("h2d_pos_%02d",ii),"",1000,-500,500,1000,-500,500);
	}

  return true;
}

void LHForwardTrackerFastDigiTask::Exec(Option_t*)
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

		//cout << modID << " " << xx << " " << yy << " " << zz << endl;

		h2d_pos[modID]->Fill(xx, yy, edep);
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
	for (int il=0; il<ftLayerN; il++){

		float wz = ftLayerZ + il*ftLayerD; 

		while ( h2d_pos[il]->Integral()>0 ){

			int binx, biny, binz;
			float ww = 0, wx = 0, wy = 0;

			int maxbin = h2d_pos[il]->GetMaximumBin();
			h2d_pos[il]->GetBinXYZ(maxbin, binx, biny, binz);

			for (int ix=0; ix<3; ix++){ //+/- 1 mm
				int indx = binx - 1 + ix;
				if ( indx<=0 || indx>1000 ) continue;

				for (int iy=0; iy<3; iy++){ // +/- 1 cm
					int indy = biny - 1 + iy;
					if ( indy<=0 || indy>1000 ) continue;

					ww += (h2d_pos[il]->GetBinContent(indx, indy));
					wx += (h2d_pos[il]->GetBinContent(indx, indy))*(h2d_pos[il]->GetXaxis()->GetBinCenter(indx));
					wy += (h2d_pos[il]->GetBinContent(indx, indy))*(h2d_pos[il]->GetYaxis()->GetBinCenter(indy));

					h2d_pos[il]->SetBinContent(indx, indy, 0.0);

				}//iy
			}//ix

			wx /= ww;
			wy /= ww;

			//cout << idx << " " << il << " " << wx << " " << wy << " " << ww << endl; 

			if ( ww<eCut ) continue;

			auto hit = (KBHit *) fHitArray -> ConstructedAt(idx);
			hit -> SetHitID(idx);
			hit -> SetX(wx);
			hit -> SetY(wy);
			hit -> SetZ(wz);
			hit -> SetCharge(ww);

			idx++;

		}//while

	}//il


	for (int ii=0; ii<ftLayerN; ii++){
		h2d_pos[ii]->Reset();
	}

}

void LHForwardTrackerFastDigiTask::SetChannelPersistency(bool persistence) { fPersistency = persistence; }
