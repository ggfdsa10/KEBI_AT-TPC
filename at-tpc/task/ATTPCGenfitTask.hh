#ifndef ATTPCGENFITTASK_HH
#define ATTPCGENFITTASK_HH

#include "KBTask.hh"
#include "KBHelixTrack.hh"
#include "KBHit.hh"
#include "KBVertex.hh"
#include "KBHitArray.hh"

#include "TClonesArray.h"

#include "ATTPCSpacepointMeasurement.hh"
#include "genfit2/AbsKalmanFitter.h"
#include "genfit2/Track.h"
#include "genfit2/RKTrackRep.h"
#include "genfit2/MeasurementFactory.h"
#include "genfit2/MeasurementProducer.h"
#include "genfit2/GFRaveVertexFactory.h"

class ATTPCGenfitTask : public KBTask
{ 
    public:
        ATTPCGenfitTask();
        virtual ~ATTPCGenfitTask() {}

        bool Init();
        void Exec(Option_t*);

        genfit::Track* FitTrack(KBHelixTrack *helixTrack, Int_t pdg);

        void SetDetID(int detID){ fDetectorID = detID; }

    private:
        TClonesArray* fTrackArray = nullptr;
        TClonesArray* fVertexArray = nullptr;

        TClonesArray *fGFTrackHitClusterArray;
        TClonesArray *fGenfitTrackArray;

        KBHitArray *fTrackHits = nullptr;

        Int_t fDetectorID = 0;

        genfit::AbsKalmanFitter *fKalmanFitter;
        genfit::MeasurementProducer<KBHit, genfit::ATTPCSpacepointMeasurement> *fMeasurementProducer;
        genfit::MeasurementFactory<genfit::AbsMeasurement> *fMeasurementFactory;
        genfit::SharedPlanePtr fTargetPlane;

        // FitTrack output
        genfit::RKTrackRep *fCurrentTrackRep;
        genfit::FitStatus *fCurrentFitStatus;
        genfit::MeasuredStateOnPlane fCurrentFitState;
        TVector3 fCurrentMomTargetPlane;
        TVector3 fCurrentPosTargetPlane;

        genfit::GFRaveVertexFactory* fVertexFinder;

        bool fPersistency = true;

        double fBfieldX;
        double fBfieldY;
        double fBfieldZ;


    ClassDef(ATTPCGenfitTask, 1)
};

#endif