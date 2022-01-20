//#include "/Users/ejungwoo/config/ejungwoo.h"
//#include "/Users/ejungwoo/config/jw.h"

{
  SysInfo_t info;
  gSystem -> GetSysInfo(&info);
  TString osString = info.fOS;

  TString libString;
  if (osString.Index("Darwin") >= 0)
    libString = TString(gSystem -> Getenv("KEBIPATH")) + "/build/libKEBI.dylib";
  else if (osString.Index("Linux") >= 0)
    libString = TString(gSystem -> Getenv("KEBIPATH")) + "/build/libKEBI.so";

  if (gSystem -> Load(libString) != -1)
    cout << "KEBI Library Loaded" << endl;
  else
    cout << "Cannot Load KEBI" << endl;
}
