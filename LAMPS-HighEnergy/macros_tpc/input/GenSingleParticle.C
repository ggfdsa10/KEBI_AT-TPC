#include <TMath.h>
#include <TRandom.h>
#include <TRandom3.h>

void GenSingleParticle(){

	gRandom = new TRandom3(0);

	const float DegToRad = TMath::Pi()/180.0;

	const int nEvent = 100;
	const int nParticlePerEvent = 10;
	const int ParticleID = 13;

	const float pMin = 100.0; //MeV
	const float pMax = 500.0; //MeV 

	const float thetaMin = 80.0*DegToRad; //rad
	const float thetaMax = 100.0*DegToRad; //rad

	float vx = 0.0; //mm
	float vy = 0.0; //mm
	float vz = 0.0; //mm

	ofstream fdata;
	fdata.open("single_particle.gen");

	fdata << "p" << endl;
	fdata << nEvent << endl;

	for (int iEvent=0; iEvent<nEvent; iEvent++){

		fdata << "0 " << nParticlePerEvent << " "  << vx << " " << vy << " " << vz << endl;

		for (int iPart=0; iPart<nParticlePerEvent; iPart++){
			float p = pMin + (pMax - pMin)*gRandom->Rndm();
			float phi = gRandom->Rndm()*TMath::TwoPi() - TMath::Pi();
			float theta = thetaMin + (thetaMax - thetaMin)*gRandom->Rndm();

			TVector3 vec;
			vec.SetMagThetaPhi(p, theta, phi);

			int pid = (gRandom->Rndm()>0.5) ? ParticleID : -1*ParticleID;

			fdata << pid << " " << vec.Px() << " " << vec.Py() << " " << vec.Pz() << endl;
		}//iPart

	}//iEvent

	fdata.close();


}
