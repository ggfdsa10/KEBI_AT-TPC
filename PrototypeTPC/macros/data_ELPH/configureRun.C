void configureRun (
    Int_t fRunID = 98,
    TString fPathToInputData = "/Users/ejungwoo/kebi/PrototypeTPC/macros/data_ELPH",
    bool fTestRun = false,
    bool fPrintAll = false,
    bool fPrintBlock = true
    )
{
  Int_t numWordsInLine = 8;
  Long64_t stepBack = 80;
  Long64_t stepForward = 400;
  Int_t count_ffff_limit = 100;

  vector<TString> fileList;
  TSystemDirectory inputDir("inputDir",fPathToInputData);
  TIter nextFile(inputDir.GetListOfFiles());
  while (auto file0 = (TSystemFile *) nextFile())
  {
    if (file0 -> IsDirectory())
      continue;
    TString fileName = file0 -> GetName();
    if (fileName.Index(Form("run_%04d.",fRunID)) == 0) {
      fileList.push_back(fPathToInputData+"/"+fileName);
      if (fTestRun) cout << fileName << endl;
    }
  }
  if(fileList.size() == 0) {
    cout << Form("No matching run_%04d",fRunID) << endl;
    return;
  }

  sort(fileList.begin(), fileList.end(), less<TString>());

  Char_t buffer[2];

  for (auto iFile = 0; iFile < fileList.size(); ++iFile)
  {
    Long64_t position = -1;
    Long64_t position_4000 = -1;
    Long64_t position_1st0800 = -1;
    Int_t count_ffff = 0;

    TString inputName = fileList[iFile];
    bool splitFile = false;
    if (iFile != 0)
      splitFile = true;

    std::ifstream in(inputName.Data(), std::ifstream::in|std::ifstream::binary);
    cout << std::hex << std::setfill('0');
    cout << "INPUT: " << inputName.Data() << endl;

    if (in.is_open()) {
      while(true)
      {
        bool found_ffff = false;

        for (auto iword = 0; iword < numWordsInLine; ++iword) {
          position = in.tellg();
          if (!in.read(buffer, 2*sizeof(Char_t)))
          {
            cout << "End of file at " << std::dec << position << "++"<< endl;
            return;
          }
          if (fPrintAll)
            cout << std::hex << setw(2) << (unsigned short) (unsigned char) buffer[0]
              << std::hex << setw(2) << (unsigned short) (unsigned char) buffer[1] 
              << std::dec << "(" << setw(7) << position << ") ";

          if ((unsigned short) (unsigned char) buffer[0] == 0xff && (unsigned short) (unsigned char) buffer[1] == 0xff) {
            found_ffff = true;
            break;
          }
        }
        if (fPrintAll)
          cout << endl;

        if (found_ffff) 
        {
          if (position - stepBack < 0)
            in.seekg(0);
          else
            in.seekg(position - stepBack);

          if (fPrintBlock)
            cout << "==== count_ffff: " << std::dec << count_ffff << endl;

          for (auto iline2 = 0; iline2 < stepBack/16 + 1; ++iline2) 
          {
            bool found_1st0800 = false;
            bool found_4000 = false;
            if (splitFile)
              found_4000 = true;

            for (auto iword = 0; iword < numWordsInLine; ++iword) {
              position = in.tellg();
              in.read(buffer, 2*sizeof(Char_t));
              if (fPrintBlock)
                cout << std::hex << setw(2) << (unsigned short) (unsigned char) buffer[0]
                  << std::hex << setw(2) << (unsigned short) (unsigned char) buffer[1] 
                  << std::dec << "(" << setw(7) << position << ") ";

              if (position_1st0800 == -1 && (unsigned short) (unsigned char) buffer[0] == 0x08 && (unsigned short) (unsigned char) buffer[1] == 0x00) {
                found_1st0800 = true;
                position_1st0800 = position;
              }

              if ((unsigned short) (unsigned char) buffer[0] == 0x40 && (unsigned short) (unsigned char) buffer[1] == 0x00) {
                found_4000 = true;
                position_4000 = position;
              }
            }

            if (found_4000) {
              if (fPrintBlock && !splitFile)
                cout << " <-- 4000! at " << std::dec << position_4000;
              count_ffff = count_ffff_limit+1;
            }
            if (found_1st0800)
              if (fPrintBlock)
                cout << " <-- 1st 0800! at " << std::dec << position_1st0800;
            if (fPrintBlock)
              cout << endl;
          }

          in.seekg(position + stepForward);
          ++count_ffff;
        }

        if (count_ffff > count_ffff_limit)
          break;
      }
    }

    if (!splitFile && position_4000 == -1) {
      cout << "Cannot find topology frame! Increase count_ffff_limit(" << count_ffff_limit << ") or check file." << endl;
      return;
    }

    if (splitFile) {
      if (position_1st0800 != 0) {
        cout << "File_Header exist! Removing File_Header..." << endl;

        TString moveOrigin = "mv " + inputName + " " + "original." + inputName;
        TString rmFileHeader = Form("tail -c+%lld ",position_1st0800+1) + TString("original.") + inputName + " > " + inputName;

        if (fTestRun) {
          TString checkNew = Form("tail -c+%lld ",position_1st0800+1) + inputName + "| xxd | less";
          cout << checkNew << endl;

          cout << "On Real Run -> Execute commands: " << endl;
          cout <<  "  " << moveOrigin << endl;
          cout <<  "  " << rmFileHeader << endl;
        } else {
          cout << moveOrigin << endl; gSystem -> Exec(moveOrigin.Data());
          cout << rmFileHeader << endl; gSystem -> Exec(rmFileHeader.Data());
        }
      }
    } else {
      if (position_4000 > position_1st0800) {
        cout << "Topology frame comes after the first basic frame. Switching order..." << endl;

        TString createHead = Form("tail -c+%lld ",position_4000+1) + inputName + " > head." + inputName;
        TString createBody = Form("head -c%lld ", position_4000) + inputName + Form("| tail -c+%lld ", position_1st0800+1) + " > body." + inputName;
        TString moveOrigin = "mv " + inputName + " " + "original." + inputName;
        TString mergeFile = "cat head." + inputName + " body." + inputName + " > " + inputName;
        TString rmHeadBody = TString("rm ") + " head." + inputName + " body." + inputName;

        if (fTestRun) {
          TString checkHead = Form("tail -c+%lld ",position_4000+1) + inputName + "| xxd | less";
          TString checkBody = Form("head -c%lld ", position_4000) + inputName + Form("| tail -c+%lld | xxd | less", position_1st0800+1);
          cout << checkHead << endl;
          cout << checkBody << endl;

          cout << "On Real Run -> Execute commands: " << endl;
          cout << "  " << createHead << endl;
          cout << "  " << createBody << endl;
          cout << "  " << moveOrigin << endl;
          cout << "  " << mergeFile << endl;
          cout << "  " << rmHeadBody << endl;
        } else {
          cout << createHead << endl; gSystem -> Exec(createHead.Data());
          cout << createBody << endl; gSystem -> Exec(createBody.Data());
          cout << moveOrigin << endl; gSystem -> Exec(moveOrigin.Data());
          cout << mergeFile << endl; gSystem -> Exec(mergeFile.Data());
          cout << rmHeadBody << endl; gSystem -> Exec(rmHeadBody.Data());
        }
      }
      else if (position_4000 != 0) {
        cout << "File_Header exist! Removing File_Header..." << endl;

        TString moveOrigin = "mv " + inputName + " " + "original." + inputName;
        TString rmFileHeader = Form("tail -c+%lld ",position_4000+1) + TString("original.") + inputName + " > " + inputName;

        if (fTestRun) {
          TString checkNew = Form("tail -c+%lld ",position_4000+1) + inputName + "| xxd | less";
          cout << checkNew << endl;

          cout << "File will not be configured on test run. The configure commands on real run will be:" << endl;
          cout << " " << moveOrigin << endl;
          cout << " " << rmFileHeader << endl;
        } else {
          cout << moveOrigin << endl; gSystem -> Exec(moveOrigin.Data());
          cout << rmFileHeader << endl; gSystem -> Exec(rmFileHeader.Data());
        }
      } 
    }
  }

  if (fTestRun) {
    cout << "Meta data will not be created on test run. Exit." << endl;
    return;
  }

  cout << "Creating meta data..." << endl;

  auto decoder = new GETDecoder();
  for (auto fileName : fileList) decoder -> AddData(fileName.Data());
  decoder -> SetData(0);
  decoder -> GoToEnd();
  decoder -> SaveMetaData(fRunID);
}
