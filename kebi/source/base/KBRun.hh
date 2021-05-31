#ifndef KBRUN_HH
#define KBRUN_HH

#include "KBCompiled.h"
#include "KBTask.hh"
#include "KBParameterContainer.hh"
#include "KBDetectorSystem.hh"
#include "KBDetector.hh"
#include "KBTpc.hh"
#include "KBPadPlane.hh"
#include "KBMCTrack.hh"

#include "TDatabasePDG.h"
#include "TError.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#ifdef ACTIVATE_EVE
#include "TEveEventManager.h"
#include "TEveEventManager.h"
#endif

#include <map>
#include <vector>
#include <fstream>
#include <stdlib.h>
using namespace std;

#include "TSysEvtHandler.h"

class InterruptHandler;

class KBRun : public KBTask
{
  public:
    static KBRun* GetRun(); ///< Get KBRun static pointer.

    KBRun(); ///< Do not use this constructor. Use GetRun() only.
    virtual ~KBRun() {};

    static TString GetKEBIVersion();      ///< Compiled KEBI version
    static TString GetGETDecoderVersion();  ///< Compiled GETDecoder version
    static TString GetKEBIHostName();     ///< KEBI Host name
    static TString GetKEBIUserName();     ///< KEBI User name
    static TString GetKEBIPath();         ///< Compiled KEBI path
    static void PrintKEBI();              ///< Print compiled KEBI information

    /**
     * @param option
     *   - "all" : Print all. Same as "cpdio". (Default)
     *   - "c" : Print compiled info.
     *   - "p" : Print parameter container
     *   - "d" : Print detector
     *   - "i" : Print input
     *   - "o" : Print output
     */
    virtual void Print(Option_t *option="all") const;

    void SetRunName(TString name, Int_t id=-999);
    TString GetRunName() const;

    void SetRunID(Int_t id);
    Int_t GetRunID() const;

    void SetDataPath(TString path); ///< Set data directory path. Default directory : path/to/KEBI/data
    TString GetDataPath();

    void SetInputFile(TString fileName, TString treeName = "event"); ///< Set input file and tree name
    void AddInputFile(TString fileName, TString treeName = "event"); ///< Add file to input file
    void AddFriend(TString fileName); ///< Add file to input file
    void SetInputTreeName(TString treeName); ///< Set input tree name
    TFile *GetInputFile();
    TTree *GetInputTree() const;
    TChain *GetInputChain() const;
    TChain *GetFriendChain(Int_t iFriend) const;

    void SetOutputFile(TString name); ///< Set output file name
    TFile *GetOutputFile();
    TTree *GetOutputTree();
    void SetTag(TString tag);
    void SetSplit(Int_t split, Long64_t numSplitEntries);

    void SetIOFile(TString inputName, TString outputName, TString treeName = "event");

    bool Init(); ///< Initailize KBRun. Init() must be done before Run().

    KBParameterContainer *GetProcessTable() const;
    KBParameterContainer *GetSDTable() const;
    KBParameterContainer *GetVolumeTable() const;

    void CreateParameterFile(TString name);
    virtual void AddParameterContainer(KBParameterContainer *par);
    virtual void AddParameterContainer(TString fname);

    /**
     * Register obj as a output branch with given name.
     * obj will not be registered if same name already exist in the branch list and return fail.
     * If persistent is true, branch will write it's data to output tree.
     */
    bool RegisterBranch(TString name, TObject *obj, bool persistent);
    TObject *GetBranch(TString name); ///< Get branch by name.
    TClonesArray *GetBranchA(TString name); ///< Get TClonesArray branch by name.

    void AddDetector(KBDetector *detector); ///< Set detector
    KBDetector *GetDetector(Int_t idx=0) const;
    KBDetectorSystem *GetDetectorSystem() const;

    void SetGeoManager(TGeoManager *gm);
    TGeoManager *GetGeoManager() const;
    void SetGeoTransparency(Int_t transparency); ///< Set transparency of geometry. Will show in eve.
    void SetEntries(Long64_t num); ///< Set total number of entries. Use only input do not exist.
    void SetNumEvents(Long64_t num);
    Long64_t GetEntries() const; ///< Get total number of entries
    Long64_t GetNumEvents() const;
    Int_t GetEntry(Long64_t entry = 0, Int_t getall = 0); ///< GetEntry from input tree
    Int_t GetEvent(Long64_t entry);
    bool GetNextEvent();

    Long64_t GetStartEventID() const;   ///< Get starting eventID
    Long64_t GetEndEventID() const;     ///< Get ending eventID
    Long64_t GetCurrentEventID() const; ///< Get current eventID

    /// Get eventID count; event_count = current_eventID - start_eventID
    Long64_t GetEventCount() const;

    bool Event(Long64_t eventID);
    bool NextEvent();

    void Run(Long64_t endEventID = -1); ///< Run all events

    void RunSingle(Long64_t eventID); ///< Run single event given eventID
    void RunInRange(Long64_t startID, Long64_t endID); ///< Run in range from startID to endID
    void RunInEventRange(Long64_t startID, Long64_t endID); ///< @todo Write this method

    void SignalEndOfRun();
    void SignalInterrupt();

    /// Run eventdisplay of given eveEventID.
    /// option is used to activate following displays:
    /// - e : display 3D eventdisplay
    /// - p : display detector planes
    void RunEve(Long64_t eveEventID=0, TString option="");
    //void SelectEveBranches(TString option);
    void SetEveScale(Double_t scale);
    //Color_t GetColor();

#ifdef ACTIVATE_EVE
    void AddEveElementToEvent(KBContainer *eveObj, bool permanent = true);
    void AddEveElementToEvent(TEveElement *element, bool permanent = true);
#endif

    void WriteCvsDetectorPlanes(TString format = "pdf");

    static void ClickSelectedPadPlane();
    void DrawPadByPosition(Double_t x, Double_t y);

    void SetLogFile(TString name = "");
    TString GetLogFile();
    //std::ofstream &GetLogFileStream();

    void SetAutoTermination(Bool_t val);
    void Terminate(TObject *obj, TString message = "");

    TString GetFileVersion(TString name);
    static TString ConfigureDataPath(TString name, bool search = false, TString dataPath="", bool isRootFile=true);
    static TString ConfigureEnv(TString name);
    static bool CheckFileExistence(TString fileName);

    TDatabasePDG *GetDatabasePDG();
    TParticlePDG *GetParticle(Int_t pdgCode);
    TParticlePDG *GetParticle(const char *name);

    void PrintMCTrack(KBMCTrack *track, Option_t *);

  private:
#ifdef ACTIVATE_EVE
    void ConfigureEventDisplay();
#endif
    void DrawEve3D();
    void ConfigureDetectorPlanes();
    void DrawDetectorPlanes();
    void SetEveLineAtt(TEveElement *el, TString branchName);
    void SetEveMarkerAtt(TEveElement *el, TString branchName);
    bool SelectTrack(KBTracklet *track);
    bool SelectHit(KBHit *hit);

    void CheckIn();
    void CheckOut();

  private:
    TString fRunName = "";
    Int_t fRunID = -1;

    bool fInitialized = false;

    TString fDataPath = "";

    TString fInputVersion = "";
    TString fInputFileName = "";
    TString fInputTreeName = "";
    TFile *fInputFile = nullptr;
    TChain *fInputTree = nullptr;

    KBParameterContainer fTempPar;

    Int_t fNumFriends = 0;
    TObjArray *fFriendTrees = nullptr;

    vector<TString> fInputFileNameArray;
    vector<TString> fFriendFileNameArray;

    TString fOutputVersion = "";
    TString fOutputFileName = "";
    TString fTag = "";
    Int_t fSplit = -1;
    Long64_t fNumSplitEntries = -1;
    TFile *fOutputFile = nullptr;
    TTree *fOutputTree = nullptr;

    TObjArray *fPersistentBranchArray = nullptr;
    TObjArray *fTemporaryBranchArray = nullptr;

    Int_t fNumBranches = 0;
    TObject **fBranchPtr;
    std::vector<TString> fBranchNames;
    std::map<TString, TObject*> fBranchPtrMap;

    Long64_t fNumEntries = 0;


    Long64_t fIdxEntry = 0;
    Long64_t fStartEventID = -1;
    Long64_t fEndEventID = -1;
    Long64_t fCurrentEventID = 0;
    Long64_t fEventCount = 0;
    bool fSignalEndOfRun = false;
    bool fCheckIn = false;

    KBParameterContainer *fRunHeader = nullptr;
    KBParameterContainer *fProcessTable = nullptr;
    KBParameterContainer *fSDTable = nullptr;
    KBParameterContainer *fVolumeTable = nullptr;

    KBDetectorSystem *fDetectorSystem = nullptr;

    std::vector<TString> fListOfGitBranches;
    std::vector<int> fListOfNumTagsInGitBranches;
    std::vector<TString> fListOfGitHashTags;
    std::vector<TString> fListOfVersionMarks;

#ifdef ACTIVATE_EVE
    TObjArray *fEveEventManagerArray = nullptr;
    TEveEventManager *fEveEventManager = nullptr;
    std::vector<TEveElement *> fEveElementList;
    std::vector<TEveElement *> fPermanentEveElementList;

    Double_t fEveScale = 1;
#endif
    //TString fEveBranches;
    TString fEveOption;

    vector<Int_t> fSelTrkIDs;
    vector<Int_t> fIgnTrkIDs;
    vector<Int_t> fSelPntIDs;
    vector<Int_t> fIgnPntIDs;
    vector<Int_t> fSelPDGs;
    vector<Int_t> fIgnPDGs;
    vector<Int_t> fSelMCIDs;
    vector<Int_t> fIgnMCIDs;
    vector<Int_t> fSelHitPntIDs;
    vector<Int_t> fIgnHitPntIDs;
    vector<TString> fSelBranchNames;

    int fNumSelectedBranches;

    TObjArray *fCvsDetectorPlaneArray = nullptr;
    TCanvas *fCvsChannelBuffer = nullptr;
    TH1D *fHistChannelBuffer = nullptr;
    TGraph *fGraphChannelBoundary = nullptr;
    TGraph *fGraphChannelBoundaryNb[20] = {0};

    TString fRunLogFileName;
    std::ofstream fRunLogFileStream;

    TString fKBLogFileName;
    TString fHash;

    Bool_t fAutoTerminate = true;

    InterruptHandler *fInterruptHandler = nullptr;

  private:
    static KBRun *fInstance;


    ClassDef(KBRun, 1)
};

class InterruptHandler : public TSignalHandler
{
  public:
    InterruptHandler() : TSignalHandler(kSigInterrupt, kFALSE) { }

    virtual Bool_t Notify()
    {
      KBRun::GetRun() -> SignalInterrupt();
      return kTRUE;
    }
};

#endif
