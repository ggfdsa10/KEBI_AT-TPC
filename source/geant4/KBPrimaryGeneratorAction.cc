#include "KBPrimaryGeneratorAction.hh"
#include "KBG4RunManager.hh"

#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <G4strstreambuf.hh>

#include <cmath>
#include <random>

KBPrimaryGeneratorAction::KBPrimaryGeneratorAction()
{
	fParticleGun = new G4ParticleGun();
}

KBPrimaryGeneratorAction::KBPrimaryGeneratorAction(const char *fileName)
{
	fParticleGun = new G4ParticleGun();
	auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
	auto par = runManager -> GetParameterContainer();
	if ( par->GetParInt("G4InputMode")==1 )
	{
		fEventGenerator = new KBMCEventGenerator(fileName);
		fReadMomentumOrEnergy = fEventGenerator -> ReadMomentumOrEnergy();
	}
}

KBPrimaryGeneratorAction::~KBPrimaryGeneratorAction()
{
	delete fParticleGun;
}

void KBPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
	auto par = runManager -> GetParameterContainer();
	if ( par->GetParInt("G4InputMode")==0 )
	{
		GeneratePrimariesMode0(anEvent);
	}
	else if ( par->GetParInt("G4InputMode")==1 )
	{
		GeneratePrimariesMode1(anEvent);
	}

	return;
}//GeneratePrimaries

void KBPrimaryGeneratorAction::GeneratePrimariesMode0(G4Event* anEvent)
{
	auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
	auto par = runManager -> GetParameterContainer();

	//Random numner, ckim
	std::random_device RD;
	std::mt19937_64 RDGen(RD()); //Mersenne Twister 19937 generator (64 bit)

	//Beam energy and unceretainty (ckim)
	G4double energy = par->GetParDouble("G4InputEnergy");
	G4double energyErr = 0.;
	if (par->CheckPar("G4InputEnergyError") == true)
	{
		G4double energyErrFac = par->GetParDouble("G4InputEnergyError");
		std::uniform_real_distribution<> RDdistEnergyErr(-energy * energyErrFac, energy * energyErrFac);
		energyErr = RDdistEnergyErr(RDGen);
	}
	G4double charge = par->GetParDouble("G4InputCharge");

	G4strstreambuf* oldBuffer = dynamic_cast<G4strstreambuf*>(G4cout.rdbuf(0));
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));
	G4cout.rdbuf(oldBuffer);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	TString particleName = par->GetParString("G4InputParticle");
	if ( particleName=="ion" )
	{
		G4ParticleDefinition* particle = G4IonTable::GetIonTable()->GetIon(par->GetParInt("G4InputIonId"));
		fParticleGun->SetParticleDefinition(particle);
		fParticleGun->SetParticleEnergy((energy + energyErr) * MeV);
		fParticleGun->SetParticleCharge(charge);
	}
	else
	{
		G4ParticleDefinition* particle = particleTable->FindParticle(particleName.Data());
		fParticleGun->SetParticleDefinition(particle);
		fParticleGun->SetParticleEnergy((energy + energyErr) * MeV);
	}

	G4int NperEvent = par->GetParInt("G4InputNumberPerEvent"); 
	G4double vx = 999., vy = 999., vz = 999.;
	vz = -1.0 * par->GetParDouble("worlddZ"); //Beam insertion point, default

	//-------------------------------------------

	//Beam insertion point (user defined), ckim
	if (par->CheckPar("G4InputPosZ") == true)
	{
		G4double vzUser = par->GetParDouble("G4InputPosZ");
		if (fabs(vzUser) <= par->GetParDouble("worlddZ")) vz = vzUser;
		else cout <<"WARNING: beam insertion point located out of the world! Set default (-worlddZ\n";
	}

	//Beam shape, ckim
	for (G4int ip=0; ip<NperEvent; ip++)
	{
		if ( par->CheckPar("G4InputCircular") && par->GetParBool("G4InputCircular")==true ) //Circular
		{
			G4double beam_dr_max = par->GetParDouble("G4InputRadius");

			std::uniform_real_distribution<> RDdistR(-beam_dr_max, beam_dr_max);
			G4double vr = 999.;
			while (vr > beam_dr_max)
			{
				vx = RDdistR(RDGen);
				vy = RDdistR(RDGen);
				vr = std::sqrt( vx*vx + vy*vy );
			}
		}
		else //Square, default
		{
			G4double beamdx = par->GetParDouble("G4InputWidthX");
			G4double beamdy = par->GetParDouble("G4InputWidthY");

			std::uniform_real_distribution<> RDdistX(-beamdx, beamdx); //ckim
			std::uniform_real_distribution<> RDdistY(-beamdy, beamdy);
			vx = RDdistX(RDGen); //(G4UniformRand()-0.5) * beamdx;
			vy = RDdistY(RDGen); //(G4UniformRand()-0.5) * beamdy;
		}

		fParticleGun -> SetParticlePosition(G4ThreeVector(vx,vy,vz));
		fParticleGun->GeneratePrimaryVertex(anEvent);
	}//ip

	return;
}//GeneratePrimariesMode0

void KBPrimaryGeneratorAction::GeneratePrimariesMode1(G4Event* anEvent)
{
	G4int pdg;
	G4double vx, vy, vz, px, py, pz;

	fEventGenerator -> ReadNextEvent(vx, vy, vz);

	fParticleGun -> SetParticlePosition(G4ThreeVector(vx,vy,vz));

	while (fEventGenerator -> ReadNextTrack(pdg, px, py, pz))
	{
		if ( pdg>1000000000 )
		{
			G4ParticleDefinition* particle = G4IonTable::GetIonTable()->GetIon(pdg);
			fParticleGun->SetParticleDefinition(particle);
		}
		else
		{
			G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable() -> FindParticle(pdg);
			fParticleGun -> SetParticleDefinition(particle);
		}

		G4ThreeVector momentum(px,py,pz);
		fParticleGun -> SetParticleMomentumDirection(momentum.unit());

		G4strstreambuf* oldBuffer = dynamic_cast<G4strstreambuf*>(G4cout.rdbuf(0));
		// Removing print outs in between here ------------->
		if (fReadMomentumOrEnergy) fParticleGun -> SetParticleMomentum(momentum.mag()*MeV);
		else                       fParticleGun -> SetParticleEnergy(momentum.mag()*MeV);
		// <------------- to here
		G4cout.rdbuf(oldBuffer);

		fParticleGun -> GeneratePrimaryVertex(anEvent);
	}

	return;
}

void KBPrimaryGeneratorAction::SetEventGenerator(const char *fileName)
{
	fEventGenerator = new KBMCEventGenerator(fileName);
	fReadMomentumOrEnergy = fEventGenerator -> ReadMomentumOrEnergy();
	((KBG4RunManager *) KBG4RunManager::GetRunManager()) -> SetNumEvents(fEventGenerator -> GetNumEvents());

	return;
}