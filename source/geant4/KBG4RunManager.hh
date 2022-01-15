#ifndef KBG4RUNMANAGER_HH
#define KBG4RUNMANAGER_HH

#define g4_info KBLog("Geant4",__FUNCTION__,0,2)
#define g4_warning KBLog("Geant4",__FUNCTION__,0,3)

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "KBGear.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "TString.h"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TObjArray.h"

#include "KBMCTrack.hh"
#include "KBMCStep.hh"

class KBG4RunManager : public G4RunManager, public KBGear
{
  public:
    KBG4RunManager();
    virtual ~KBG4RunManager();

    virtual void Initialize();
    virtual void InitializeGeometry();
    virtual void InitializePhysics();
    void Run(G4int argc=0, char **argv=nullptr, const G4String &type="");

    void BeamOn(G4int numEvents, const char *macroFile=0, G4int numSelect=-1);
    void BeamOnAll();

    void SetSuppressInitMessage(bool val);

    void SetSensitiveDetector(G4VPhysicalVolume *physicalVolume, TString assemblyName="");
    void SetVolume(G4VPhysicalVolume *physicalVolume);

    KBParameterContainer *GetVolumes();
    KBParameterContainer *GetSensitiveDetectors();
    KBParameterContainer *GetProcessTable();

    void AddMCTrack(Int_t trackID, Int_t parentID, Int_t pdg,
        Double_t px, Double_t py, Double_t pz,
		    Int_t dID, Double_t vx, Double_t vy, Double_t vz, Double_t ke, Double_t edep1, Double_t edep2, Int_t pcID);

    void AddTrackVertex(Double_t px, Double_t py, Double_t pz,
        Int_t dID, Double_t vx, Double_t vy, Double_t vz);

    void AddMCStep(Int_t dID, Double_t x, Double_t y, Double_t z, Double_t t, Double_t e);

    void SetNumEvents(Int_t numEvents);

    void NextEvent();
    void WriteToFile(TObject *obj);
    void EndOfRun();

  private:
    void SetGeneratorFile(TString value);
    void SetOutputFile(TString value);

    bool fSuppressInitMessage = false;

    TFile* fFile;
    TTree* fTree;
    TClonesArray *fTrackArray;
    TObjArray *fStepArrayList;

    Double_t fEdepSumArray[200] = {0};

    std::map<Int_t, Int_t> fIdxOfCopyNo;
    Int_t fNumActiveVolumes = 0;

    bool fSetEdepSumTree = false;
    bool fStepPersistency = false;
    bool fSecondaryPersistency = false;
    bool fTrackVertexPersistency = false;

    KBParameterContainer *fVolumes;
    KBParameterContainer *fSensitiveDetectors;
    KBParameterContainer *fProcessTable;

    vector<TString> fSDAssembly;
    vector<TString> fSDNames;

    Int_t fNumSDAssemblyInTree = 0;
    map<Int_t, Int_t> fMapSDToAssembly;
    map<Int_t, Int_t> fMapSDToStep;

    Int_t fTrackID = 0;
    KBMCTrack* fCurrentTrack = nullptr;

    Int_t fNumEvents;
};

#endif
