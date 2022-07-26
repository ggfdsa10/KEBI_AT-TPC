#include <string> 

#define od NextEvent()
#define oa PreviousEvent()
#define os SavePicture()
#define ow(event) ParticluarEvent(event)
#define oto AutoEvent()

TString fileName = "ATTPCEXP";
TString ExpName = "MuonTest";
TString picturePath = "./picture";

int fNumEvents = 1;
int fEventIdx = -1;
char fKeyStats;

bool CheckNullData(){
  auto fHitArray = KBRun::GetRun() -> GetBranchA("Pad");
  if(fHitArray -> GetEntries()==0 || fHitArray==nullptr){
    if(fKeyStats=='d'){
      if(fEventIdx==fNumEvents){fEventIdx--;}
      else{fEventIdx++;}
    }
    if(fKeyStats=='a'){
      fEventIdx--;
    }
    return true;
  }
  return false;
}

bool SkimToData(){
  if(fEventIdx==0 || fEventIdx==fNumEvents){return true;}
  else{return false;}
}

void PrintStats(ATTPCDecoderTask *decoder){
  cout << endl;
  cout << "  ----------------------------------------- Event stats --------------------------------------- " << endl;
  cout << " | "; decoder -> GetDate();                                                                                   cout << "         | " << endl;
  cout << " |                                                                                             | " << endl;
  cout << " |   Current event number : " << fEventIdx << " / " << fNumEvents << " [current/total]                                          | " << endl;
  cout << " |   Current event time   : " << decoder -> GetEventTime() << " ns                                                        | " << endl;
  cout << "  --------------------------------------------------------------------------------------------- " << endl;
  cout << endl;
}

void Excute(){
  if(fEventIdx < 0){fEventIdx = 0;}
  if(fEventIdx > fNumEvents){fEventIdx = fNumEvents;}

  KBRun *run = new KBRun();
  run -> SetAutoTermination(false);
  run -> AddPar("attpc_Exp.par");
  run -> AddDetector(new ATTPC());

  auto parSetup = new ATTPCSetupParameter();

  auto decoder = new ATTPCDecoderTask();
  decoder -> ExcuteOnline(fEventIdx, SkimToData());
  decoder -> SetPadPersistency(true);

  auto subtractor = new ATTPCNoiseSubtractTask();

  auto pulseFinder = new ATTPCPSATask();
  pulseFinder->SetHitPersistency(true);
  pulseFinder -> SetPSA(new ATTPCPSAFastFit());

  auto recontructer = new ATTPCHelixTrackFindingTask();

  run -> Add(parSetup);
  run -> Add(decoder);
  // run -> Add(subtractor);
  // run -> Add(pulseFinder);
  // run -> Add(recontructer);

  run -> Init();
  run -> Run();

  if(SkimToData()){fNumEvents = decoder->GetNumEvent()-1;}
  cout << "num events : " << fNumEvents << endl;
  if(CheckNullData()){Excute();}
  run -> RunEve(0,"p");
  PrintStats(decoder);
}


void NextEvent(){
  fEventIdx++;
  fKeyStats = 'd';
  Excute();
}

void PreviousEvent(){
  fEventIdx--;
  fKeyStats = 'a';
  Excute();
}

void ParticluarEvent(int event){
  fEventIdx = event;
  fKeyStats = 'd';
  Excute();
}

void SavePicture(){
  auto canvas = KBRun::GetRun() -> GetCvsDetectorPlanes();
  canvas -> SaveAs(Form("%s/%s_%i.pdf", picturePath.Data(), ExpName.Data(), fEventIdx));
}

void online()
{
  Excute();
  cout << endl;
  cout << "                  -------------------- command macors gauid -------------------- " << endl;
  cout << "                 |   od : Next event                                            | " << endl;
  cout << "                 |   oa : Previous event                                        | " << endl;
  cout << "                 |   os : Save the event picture                                | " << endl;
  cout << "                 |   ow(int) : show the n th event. Ex) ow(150)                 | " << endl; 
  cout << "                  -------------------------------------------------------------- " << endl;
  cout << endl;
}
