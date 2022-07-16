 #include <string> 

int fNumEvents = 0;
int fEventIdx = 0;
bool fEndMacros = 0;
char key[10];

void InputKey(){
  cout << " Input the key : ";

  cin.getline(key, 50);

  if(key[0]=='d'){fEventIdx++;}
  else if(key[0]=='a'){fEventIdx--;}
  else if(key[0]=='q'){fEndMacros=true;}
  else if(key[0]=='w'){
    string keyString = "";
    for(int i=1; i<10; i++){
      if(key[i] >= '0' && key[i] <= '9'){keyString += key[i];}
    }
    if(keyString==""){InputKey();}
    fEventIdx = stoi(keyString);
  }

  else{InputKey();}
}

    
bool CheckNullData(){
  auto fHitArray = KBRun::GetRun() -> GetBranchA("Pad");
  cout <<"********** "  << fHitArray -> GetEntries() << endl;
  if(fHitArray -> GetEntries()==0 || fHitArray==nullptr){
    cout << "******************* event is null data, so skip. ******************** " << endl;
    if(key[0]=='d'){
      fEventIdx++;
    }
    if(key[0]=='a'){
      fEventIdx--;
    }
    return true;
  }
  return false;
}

// void EventSave(){  
//   KBRun::GetRun() -> WriteCvsDetectorPlanes("png");
// }

void PrintStats(ATTPCDecoderTask* decoder){
  fNumEvents = decoder->GetNumEvent();
  decoder -> GetDate();
  cout << " event index : " << fEventIdx << " / " << fNumEvents << " (current/total) " << endl;
  cout << " event time : " << decoder -> GetEventTime() << " ns " << endl;
}

void Excute(){
  
}

void online(TString ExpName = "MuonTest")
{
  cout << "=========== key macors gauid ===========" << endl;
  cout << " d : Next event" << endl;
  cout << " a : Previous event" << endl;
  cout << " s : Save the event picture " << endl;
  cout << " q : quit this macors " << endl;
  cout << " w (int) : show the n th event. Ex) w 150" << endl; 
  cout << "========================================" << endl;

  TString fileName = "ATTPCEXP";


  while(fEndMacros==false){
    if(fEventIdx < 0){fEventIdx=0;}
    auto run = new KBRun();
    run -> SetOutputFile(fileName+Form(".Online%s", ExpName.Data()));
    run -> SetAutoTermination(false);
    run -> AddPar("attpc_Exp.par");
    run -> AddDetector(new ATTPC());
    auto parSetup = new ATTPCSetupParameter();

    auto decoder = new ATTPCDecoderTask();
    decoder -> ExcuteOnline(fEventIdx);
    decoder -> SetPadPersistency(true);

    auto subtractor = new ATTPCNoiseSubtractTask();

    auto pulseFinder = new ATTPCPSATask();
    pulseFinder->SetHitPersistency(true);
    pulseFinder -> SetPSA(new ATTPCPSAFastFit());

    auto recontructer = new ATTPCHelixTrackFindingTask();

    run -> Add(parSetup);
    run -> Add(decoder);
    run -> Add(subtractor);
    run -> Add(pulseFinder);
    run -> Add(recontructer);

    run -> Init();

    run -> Run();

    if(CheckNullData()){continue;}
    PrintStats(decoder);
    auto runEve = new KBRun();
    runEve -> SetInputFile(fileName+Form(".Online%s", ExpName.Data()));
    runEve -> AddDetector(new ATTPC());
    runEve -> SetTag("eve");
    runEve -> Init();
    runEve -> SetGeoTransparency(80);
    runEve -> RunEve(1);
    InputKey();
    return;
  }






  // gApplication -> Terminate();
}
