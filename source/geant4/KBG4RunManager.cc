#include "KBG4RunManager.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "globals.hh"
#include "G4ProcessTable.hh"
#include "G4GDMLParser.hh"
#include "G4strstreambuf.hh"

#include "TSystem.h"

#include "KBG4RunMessenger.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

KBG4RunManager::KBG4RunManager()
:G4RunManager()
{
  new KBG4RunMessenger(this);

  fGeom = new KBParameterContainer();
  fGeom -> SetName("Geom");

  fVolumes = new KBParameterContainer();
  fVolumes -> SetName("Volumes");

  fSensitiveDetectors = new KBParameterContainer();
  fSensitiveDetectors -> SetName("SensitiveDetectors");

  fProcessTable = new KBParameterContainer();
  fProcessTable -> SetName("ProcessTable");
}

KBG4RunManager::~KBG4RunManager()
{
}

void KBG4RunManager::Initialize()
{
  if (GetUserPrimaryGeneratorAction() == nullptr) SetUserAction(new KBPrimaryGeneratorAction());
  if (GetUserEventAction() == nullptr)    SetUserAction(new KBEventAction(this));
  if (GetUserTrackingAction() == nullptr) SetUserAction(new KBTrackingAction(this));
  if (GetUserSteppingAction() == nullptr) SetUserAction(new KBSteppingAction(this));

  if (fPar -> CheckPar("SensitiveDetectors")) {
    auto sdNames = fPar -> GetParVString("SensitiveDetectors");
    for (auto sdName : sdNames)
    {
      if (sdName.Index("!")!=0)
      {
        sdName.ReplaceAll("!","");
        fSDAssembly.push_back(sdName);
      }
      else
        fSDNames.push_back(sdName);
    }
  }

  G4RunManager::Initialize();

  SetOutputFile(fPar->GetParString("G4OutputFile").Data());
	if ( fPar->GetParInt("G4InputMode")==1 )
	{
		SetGeneratorFile(fPar->GetParString("G4InputFile").Data());
	}

  auto procNames = G4ProcessTable::GetProcessTable() -> GetNameList();
  Int_t idx = 0;
  fProcessTable -> SetPar("Primary", idx++);
  for (auto name : *procNames){
    if(fProcessTable -> CheckPar(name)){continue;}
		fProcessTable -> SetPar(name, idx++);
	}

  if (fPar->CheckPar("G4ExportGDML"))
  {
    TString fileName = fPar -> GetParString("G4ExportGDML");
    TString name = gSystem -> Which(".", fileName.Data());

    if (name.IsNull()) {
      g4_info << "Exporting geometry in GMDL format: " << fileName << endl;
      G4GDMLParser parser;
      auto world = G4RunManagerKernel::GetRunManagerKernel() -> GetCurrentWorld();
      parser.Write(fileName.Data(),world);
    }
    else {
      g4_warning << "The file " << fileName << " exist already." << endl;
      g4_warning << "Stopped exporting geomtry" << endl;
    }
  }
}

void KBG4RunManager::InitializeGeometry()
{
  g4_info << "InitializeGeometry" << endl;
  if (fSuppressInitMessage) {
    G4strstreambuf* suppressMessage = dynamic_cast<G4strstreambuf*>(G4cout.rdbuf(0));
    // Suppress print outs in between here ------------->
    G4RunManager::InitializeGeometry();
    // <------------- to here
    G4cout.rdbuf(suppressMessage);
  }
  else
    G4RunManager::InitializeGeometry();
  g4_info << "InitializeGeometry" << endl;
}

void KBG4RunManager::InitializePhysics()
{
  g4_info << "InitializePhysics" << endl;
  if (fSuppressInitMessage) {
    G4strstreambuf* suppressMessage = dynamic_cast<G4strstreambuf*>(G4cout.rdbuf(0));
    // Suppress print outs in between here ------------->
    G4RunManager::InitializePhysics();
    // <------------- to here
    G4cout.rdbuf(suppressMessage);
  }
  else
    G4RunManager::InitializePhysics();
  g4_info << "InitializePhysics" << endl;
}

void KBG4RunManager::Run(G4int argc, char **argv, const G4String &type)
{
  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  TString command("/control/execute ");

  if (fPar->CheckPar("G4VisFile")) {
    auto fileName = fPar -> GetParString("G4VisFile");

    G4VisManager* visManager = new G4VisExecutive;
    visManager -> Initialize();

    G4UIExecutive* uiExecutive = new G4UIExecutive(argc,argv,type);
    g4_info << "Initializing Geant4 run with viewer macro " << fileName << endl;
    uiManager -> ApplyCommand(command+fileName);
    uiExecutive -> SessionStart();

    delete uiExecutive;
    delete visManager;
  }
  else if (fPar->CheckPar("G4MacroFile")) {
    auto fileName = fPar -> GetParString("G4MacroFile");
    g4_info << "Initializing Geant4 run with macro " << fileName << endl;
    uiManager -> ApplyCommand(command+fileName);
  }


  WriteToFile(fProcessTable);
  WriteToFile(fSensitiveDetectors);
  WriteToFile(fVolumes);
  WriteToFile(fGeom);
  EndOfRun();
}

void KBG4RunManager::BeamOnAll()
{
  BeamOn(fNumEvents);
}

void KBG4RunManager::BeamOn(G4int numEvents, const char *macroFile, G4int numSelect)
{
  if(numEvents<=0) { fakeRun = true; }
  else { fakeRun = false; }
  G4bool cond = ConfirmBeamOnCondition();
  if(cond)
  {
    numberOfEventToBeProcessed = numEvents;
    numberOfEventProcessed = 0;
    ConstructScoringWorlds();

    if (fSuppressInitMessage) {
      G4strstreambuf* suppressMessage = dynamic_cast<G4strstreambuf*>(G4cout.rdbuf(0));
      // Suppress print outs in between here ------------->
      RunInitialization();
      // <------------- to here
      G4cout.rdbuf(suppressMessage);
    }
    else
      RunInitialization();

    DoEventLoop(numEvents,macroFile,numSelect);
    RunTermination();
  }
  fakeRun = false;
}

void KBG4RunManager::SetSuppressInitMessage(bool val) { fSuppressInitMessage = val; }

void KBG4RunManager::SetGeneratorFile(TString value)
{
  auto pga = (KBPrimaryGeneratorAction *) userPrimaryGeneratorAction;
  fPar -> ReplaceEnvironmentVariable(value);
  pga -> SetEventGenerator(value.Data());
  //fNumEvents set from pga
}

void KBG4RunManager::SetOutputFile(TString name)
{
  fPar -> ReplaceEnvironmentVariable(name);

  fMCTrack 				        = fPar->GetParBool("MCTrack");
  fMCPostTrack		        = fPar->GetParBool("MCPostTrack");
  fSetEdepSumTree         = fPar->GetParBool("MCSetEdepSumTree");
  fStepPersistency        = fPar->GetParBool("MCStepPersistency");
  fSecondaryPersistency   = fPar->GetParBool("MCSecondaryPersistency");
  fTrackVertexPersistency = fPar->GetParBool("MCTrackVertexPersistency");

  fFile = new TFile(name,"recreate");
  fTree = new TTree("event", name);

	fTrackArray = new TClonesArray("KBMCTrack", 100);
	fPostTrackArray = new TClonesArray("KBMCTrack", 100);
  fTree -> Branch("MCTrack", &fTrackArray);
  fTree -> Branch("MCPostTrack", &fPostTrackArray);

  fStepArrayList = new TObjArray();

  if (fStepPersistency)
  {
    TIter itDetectors(fSensitiveDetectors);
    TParameter<Int_t> *det;

    while ((det = dynamic_cast<TParameter<Int_t>*>(itDetectors())))
    {
      TString detName = det -> GetName();
      Int_t copyNo = det -> GetVal();

      if (detName.Index("_PARTOFASSEMBLY")>0)
        continue;

      TString branchHeader = "MCStep";
      if (detName.Index("_ASSEMBLY")>0)
        branchHeader = "MCStepAssembly";

      g4_info << "Set " << detName << " " << copyNo << endl;
      auto stepArray = new TClonesArray("KBMCStep", 10000);
      stepArray -> SetName(branchHeader+Form("%d", copyNo));

      fTree -> Branch(stepArray -> GetName(), &stepArray);
      fStepArrayList -> Add(stepArray);

      TString edepSumName = Form("EdepSum%d", copyNo);
      fIdxOfCopyNo[copyNo] = fNumActiveVolumes;
      fTree -> Branch(edepSumName, &fEdepSumArray[fNumActiveVolumes]);
      ++fNumActiveVolumes;
    }
  }
}

void KBG4RunManager::SetVolume(G4VPhysicalVolume *physicalVolume)
{
  TString name = physicalVolume -> GetName().data();

  for (auto assembly : fSDAssembly) {
    assembly.ReplaceAll("!","");
    TObjArray *sdAssemblyNames = assembly.Tokenize("+");
    Int_t numsda = sdAssemblyNames -> GetEntries();
    if (numsda<2)
      continue;

    TString assemblyName = ((TObjString *) sdAssemblyNames  -> At(0)) -> GetString();
    for (auto isda=1; isda<numsda; ++isda) {
      TString partName = ((TObjString *) sdAssemblyNames  -> At(isda)) -> GetString();

      if (partName.Index("*")>=1) {
        partName.ReplaceAll("*","");
        if (name.Index(partName)>=0) {
          SetSensitiveDetector(physicalVolume, assemblyName);
          return;
        }
      }
      else {
        if (name == partName) {
          SetSensitiveDetector(physicalVolume, assemblyName);
          return;
        }
      }

    }
  }

  for (auto sdName : fSDNames) {
    if (sdName.Index("*")>=1) {
      sdName.ReplaceAll("*","");
      if (name.Index(sdName)>=0) {
        SetSensitiveDetector(physicalVolume);
        return;
      }
    }
    else {
      if (name == sdName) {
        SetSensitiveDetector(physicalVolume);
        return;
      }
    }
  }

  Int_t copyNo = physicalVolume -> GetCopyNo();

  fVolumes -> SetPar(name, copyNo);
}

void KBG4RunManager::SetSensitiveDetector(G4VPhysicalVolume *physicalVolume, TString assemblyName)
{
  TString name = physicalVolume -> GetName().data();
  Int_t copyNo = physicalVolume -> GetCopyNo();

  if (assemblyName.IsNull())
    fSensitiveDetectors -> SetPar(name, copyNo);
  else {
    Int_t assemblyID;
    if (fSensitiveDetectors -> CheckPar(assemblyName+"_ASSEMBLY")) {
      assemblyID = fSensitiveDetectors -> GetParInt(assemblyName+"_ASSEMBLY");
    }
    else {
      assemblyID = fNumSDAssemblyInTree;
      fSensitiveDetectors -> SetPar(assemblyName+"_ASSEMBLY",assemblyID);
      ++fNumSDAssemblyInTree;
    }
    fMapSDToAssembly[copyNo] = assemblyID;
    fSensitiveDetectors -> SetPar(name+"_PARTOFASSEMBLY",assemblyID);
  }
}

KBParameterContainer *KBG4RunManager::GetGeom() { return fGeom; }
KBParameterContainer *KBG4RunManager::GetVolumes() { return fVolumes; }
KBParameterContainer *KBG4RunManager::GetSensitiveDetectors() { return fSensitiveDetectors; }
KBParameterContainer *KBG4RunManager::GetProcessTable()       { return fProcessTable; }



// void KBG4RunManager::AddMCTrack(Int_t opt, Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz, Int_t processID)
// {
//   if (opt==0 && parentID != 0 && !fSecondaryPersistency) {
//     fCurrentTrack = nullptr;
//     return;
//   }

// 	if (opt==0 && !fMCTrack) {
// 		fCurrentTrack = nullptr;
// 		return;
// 	}else if (opt==1 && !fMCPostTrack) {
// 		fCurrentTrack = nullptr;
// 		return;
// 	}

//   fTrackID = trackID;
// 	if ( opt==0 ){
// 		fCurrentTrack = (KBMCTrack *) fTrackArray -> ConstructedAt(fTrackArray -> GetEntriesFast());
// 	}else if ( opt==1 ){
// 		fCurrentTrack = (KBMCTrack *) fPostTrackArray -> ConstructedAt(fPostTrackArray -> GetEntriesFast());
// 	}

// 	fCurrentTrack -> SetMCTrack(trackID, parentID, pdg, px, py, pz, detectorID, vx, vy, vz, processID);
// }


void KBG4RunManager::AddMCTrack(Int_t opt, Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz, Double_t ke, Double_t edep1, Double_t edep2, Int_t processID)
{
  if (opt==0 && parentID != 0 && !fSecondaryPersistency) {
    fCurrentTrack = nullptr;
    return;
  }

	if (opt==0 && !fMCTrack) {
		fCurrentTrack = nullptr;
		return;
	}else if (opt==1 && !fMCPostTrack) {
		fCurrentTrack = nullptr;
		return;
	}

  fTrackID = trackID;
	if ( opt==0 ){
		fCurrentTrack = (KBMCTrack *) fTrackArray -> ConstructedAt(fTrackArray -> GetEntriesFast());
	}else if ( opt==1 ){
		fCurrentTrack = (KBMCTrack *) fPostTrackArray -> ConstructedAt(fPostTrackArray -> GetEntriesFast());
	}

	fCurrentTrack -> SetMCTrack(trackID, parentID, pdg, px, py, pz, detectorID, vx, vy, vz, ke, edep1, edep2, processID);
}

void KBG4RunManager::AddTrackVertex(Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz)
{
  if (fCurrentTrack == nullptr || !fTrackVertexPersistency)
    return;

  fCurrentTrack -> AddVertex(px, py, pz, detectorID, vx, vy, vz);
}

void KBG4RunManager::AddMCStep(Int_t detectorID, Double_t x, Double_t y, Double_t z, Double_t t, Double_t e)
{
	Int_t motherID = (detectorID/1000)*10;
	Int_t moduleID = detectorID>1000 ? detectorID%1000 : 0;
	auto idx = fIdxOfCopyNo[motherID];

  if (fSetEdepSumTree)
    fEdepSumArray[idx] = fEdepSumArray[idx] + e;

  if (fStepPersistency)
  {
    //auto stepArray = (TClonesArray *) fStepArrayList -> FindObject(Form("MCStep%d", detectorID));
    auto stepArray = (TClonesArray *) fStepArrayList -> At(idx);
    if (stepArray == nullptr)
      return;

    KBMCStep *step = (KBMCStep *) stepArray -> ConstructedAt(stepArray -> GetEntriesFast());
    step -> SetMCStep(fTrackID, moduleID, x, y, z, t, e);
  }
}

void KBG4RunManager::SetNumEvents(Int_t numEvents)
{
  fNumEvents = numEvents;
}

void KBG4RunManager::NextEvent()
{

    g4_info << "End of Event " << fTree -> GetEntries() << endl;

  fTree -> Fill();

  fTrackArray -> Clear("C");
  fPostTrackArray -> Clear("C");
  TIter it(fStepArrayList);

  if (fStepPersistency) {
    while (auto stepArray = (TClonesArray *) it.Next())
      stepArray -> Clear("C");
  }

  memset(fEdepSumArray, 0, sizeof(Double_t)*fNumActiveVolumes);
}

void KBG4RunManager::WriteToFile(TObject *obj)
{
  fFile -> cd();
  g4_info << "Writing " << obj -> GetName() << " to output file" << endl;
  obj -> Write(obj->GetName(),TObject::kSingleKey);
}

void KBG4RunManager::EndOfRun()
{
  fFile -> cd();
  g4_info << "Writing " << fTree -> GetName() << " to output file" << endl;
  fTree -> Write();
  g4_info << "Writing " << fPar -> GetName() << " to output file" << endl;
  fPar -> Write(fPar->GetName(),TObject::kSingleKey);
  fFile -> Close();
  g4_info << "Output: " << fFile -> GetName() << endl;
}