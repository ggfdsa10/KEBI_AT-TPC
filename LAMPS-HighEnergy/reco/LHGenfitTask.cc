#include "LHGenfitTask.hh"

#include "KBRun.hh"
//#include "KBG4RunManager.hh"
//#include "KBParameterContainer.hh"
#include "KBHitArray.hh"

#include <iostream>
using namespace std;

#include "TMatrixDSym.h"

#include "genfit2/KalmanFitterRefTrack.h"
#include "genfit2/DAF.h"
#include "genfit2/FieldManager.h"
#include "genfit2/ConstField.h"
#include "genfit2/MaterialEffects.h"
#include "genfit2/TGeoMaterialInterface.h"
#include "genfit2/TrackCand.h"

#include "genfit2/GFRaveVertex.h"

#include "TRandom.h"

ClassImp(LHGenfitTask)

LHGenfitTask::LHGenfitTask()
:KBTask("LHGenfitTask","")
{
}

bool LHGenfitTask::Init()
{
  auto run = KBRun::GetRun();
  fTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fGFTrackHitClusterArray = new TClonesArray("KBHit");
  //fGenfitTrackArray = new TClonesArray("genfit::Track");

	fTrackHits = new KBHitArray();

	fVertexArray = new TClonesArray("KBVertex");
	run -> RegisterBranch("Vertex", fVertexArray, fPersistency);

	KBParameterContainer *par = run -> GetParameterContainer();
	double bfieldx = par -> GetParDouble("bfieldx");
	double bfieldy = par -> GetParDouble("bfieldy");
	double bfieldz = par -> GetParDouble("bfieldz");

  fKalmanFitter = new genfit::KalmanFitterRefTrack();
  //fKalmanFitter = new genfit::DAF();
  fKalmanFitter -> setMinIterations(5);
  fKalmanFitter -> setMaxIterations(20);
	//fKalmanFitter -> setDebugLvl(10);

  fMeasurementProducer = new genfit::MeasurementProducer<KBHit, genfit::LHSpacepointMeasurement>(fGFTrackHitClusterArray);
  fMeasurementFactory = new genfit::MeasurementFactory<genfit::AbsMeasurement>();
  fMeasurementFactory -> addProducer(fDetectorID, fMeasurementProducer);
  genfit::FieldManager::getInstance() -> init(new genfit::ConstField(bfieldx, bfieldy, 10*bfieldz)); //T -> kGauss
	cout << "LHGenfitTask::genfit::FieldManager init " << bfieldx << ", " << bfieldy << ", " <<bfieldz << endl;

  genfit::MaterialEffects *materialEffects = genfit::MaterialEffects::getInstance();
  materialEffects -> init(new genfit::TGeoMaterialInterface());

  TVector3 posTarget(0, 0, 0); // cm
  TVector3 normalTarget(0, 0, 1);
  fTargetPlane = genfit::SharedPlanePtr(new genfit::DetPlane(posTarget, normalTarget));

	fVertexFinder = new genfit::GFRaveVertexFactory();
	fVertexFinder->setMethod("avf-smoothing:1");

  return true;
}

void LHGenfitTask::Exec(Option_t*)
{

	fVertexArray -> Clear("C");

	std::vector<genfit::GFRaveVertex*> rave_vertices;
	rave_vertices.clear();

	vector<genfit::Track*> gf_tracks;
	gf_tracks.clear();

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++)
  {
		fCurrentMomTargetPlane.SetXYZ(0, 0, 0);
		fCurrentPosTargetPlane.SetXYZ(0, 0, 0);
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
    auto genfitTrack = FitTrack(track, 2212);
		track -> SetGenfitMomentum(fCurrentMomTargetPlane);

		if ( genfitTrack )
		{
			track -> SetIsGenfitTrack();
			gf_tracks.push_back(genfitTrack);
		}
  }


	if ( gf_tracks.size()>= 2 )
	{
		fVertexFinder->findVertices(&rave_vertices, gf_tracks);
	}

	KBVertex *vertex = (KBVertex *) fVertexArray -> ConstructedAt(0);

	if ( rave_vertices.size()>0 )
	{
		genfit::GFRaveVertex* rave_vtx = rave_vertices[0];
		vertex->SetX(rave_vtx->getPos().X());
		vertex->SetY(rave_vtx->getPos().Y());
		vertex->SetZ(rave_vtx->getPos().Z());
	}else{
		vertex->SetX(-999);
		vertex->SetY(-999);
		vertex->SetZ(-999);
	}

	cout << "Vertex size: " << rave_vertices.size() << endl; 

  kb_info << "..." << endl;

  return;
}

genfit::Track* LHGenfitTask::FitTrack(KBHelixTrack *helixTrack, Int_t pdg)
{
	fGFTrackHitClusterArray -> Clear("C");
	fTrackHits -> Clear();
  genfit::TrackCand trackCand;

	KBHitArray *hitArray = helixTrack -> GetHitArray();
	hitArray->MoveHitsTo(fTrackHits);

	fTrackHits->SortByR(false);

	TIter next(fTrackHits);
	while (KBHit *hit = (KBHit *) next()) {
		auto idx = fGFTrackHitClusterArray -> GetEntriesFast();
		auto gfhit = (KBHit *) fGFTrackHitClusterArray -> ConstructedAt(idx);
    gfhit -> CopyFrom(hit);
		trackCand.addHit(fDetectorID, idx);
	}

	//auto refHit = (KBHit *) fGFTrackHitClusterArray -> At(fGFTrackHitClusterArray->GetEntriesFast()-1);
	auto refHit = (KBHit *) fGFTrackHitClusterArray -> At(0);

  TMatrixDSym covSeed(6);
	float nhits = fTrackHits->GetEntries();

	covSeed(0,0) = covSeed(1,1) = pow(1.0, 2);
	covSeed(2,2) = pow(0.05, 2);
	covSeed(3,3) = covSeed(4,4) = pow(1.0/nhits/sqrt(3), 2);
	covSeed(5,5) = pow(0.05/nhits/sqrt(3), 2);
	
  Double_t dip = helixTrack -> DipAngle();
  TVector3 momSeed = 0.001 * helixTrack -> Momentum(1.0); // MeV -> GeV
  momSeed.SetTheta(dip); /// TODO

	TVector3 posSeed = refHit -> GetPosition();
	posSeed.SetMag(posSeed.Mag()/10.); // mm -> cm

  trackCand.setCovSeed(covSeed);
  trackCand.setPosMomSeed(posSeed, momSeed, KBRun::GetRun()->GetParticle(pdg)->Charge()/3.); /// TODO

  //auto genfitTrack = (genfit::Track *) fGenfitTrackArray -> ConstructedAt(fGenfitTrackArray -> GetEntriesFast());
  //genfitTrack -> createMeasurements(trackCand, *fMeasurementFactory);
	fCurrentTrackRep = new genfit::RKTrackRep(pdg);
  //genfitTrack -> addTrackRep(fCurrentTrackRep);
	auto genfitTrack = new genfit::Track(trackCand, *fMeasurementFactory, fCurrentTrackRep);

  try { fKalmanFitter -> processTrackWithRep(genfitTrack, fCurrentTrackRep, false); }
  catch (genfit::Exception &e) { return (genfit::Track *) nullptr; } /// TODO

  try { fCurrentFitStatus = genfitTrack -> getFitStatus(fCurrentTrackRep); }
  catch (genfit::Exception &e) { return (genfit::Track *) nullptr; } /// TODO

  if (fCurrentFitStatus -> isFitted() == false || fCurrentFitStatus -> isFitConverged() == false)
    return (genfit::Track *) nullptr;

  try { fCurrentFitState = genfitTrack -> getFittedState(); }
  catch (genfit::Exception &e) { (genfit::Track *) nullptr; } /// TODO

  //TVector3 fCurrentMomTargetPlane;
  //TVector3 fCurrentPosTargetPlane;
  try { 
    fCurrentTrackRep -> extrapolateToPlane(fCurrentFitState, fTargetPlane); 
		fCurrentMomTargetPlane = fCurrentFitState.getMom() * 1000.; // GeV -> MeV
		fCurrentPosTargetPlane = fCurrentFitState.getPos() * 10.; // cm -> mm
  } catch (genfit::Exception &e) {
    fCurrentMomTargetPlane.SetXYZ(0, 0, 0);
    fCurrentPosTargetPlane.SetXYZ(0, 0, 0);
  }

  return genfitTrack;
}
