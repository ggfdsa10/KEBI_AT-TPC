#ifndef KBPARAMETERCONTAINER
#define KBPARAMETERCONTAINER

#include "KBGlobal.hh"
#include "TObjArray.h"
#include "TObjString.h"
#include "TNamed.h"
#include "TParameter.h"
#include "KBVector3.hh"
#include <vector>

using std::vector;

/**
 * List of parameters <[parameter name], [parameter values]>
 * parameter type features Bool_t, Int_t, Double_t, TString, KBVector3::Axis.
 *
 * Structure of parameter file should be list of : [name] [type_initial] [value]
 * Each elemets are divided by space.
 * Comments are used by #.
 *
 * @param name Name of parameter file with no space
 * @param type_initial
 *   - b for Bool_t
 *   - i for Int_t
 *   - d for Double_t
 *   - s for TString
 *   - a for KBVector3::Axis
 * @param value Value of parameter. TString value do not accept space.
 *
 * ex)\n
 * \#example parameter file\n
 * worldSize    d  1000   # [mm]\n
 * nTbs         i  512\n
 * specialFile  s  /path/to/specialFile.dat  \#special\n 
 *
 * ============================================================================
 *
 * With fDebugMode true, KBParameterContainer will not terminate at attempt of
 * getting non-existing paramter, but print message and create empty parameter.
 *
*/

typedef KBVector3::Axis kbaxis;

class KBParameterContainer : public TObjArray
{
  public:
    KBParameterContainer(Bool_t debug = false);
    KBParameterContainer(const char *parName, Bool_t debug = false); ///< Constructor with input parameter file name
    virtual ~KBParameterContainer() {}

    void SaveAs(const char *filename, Option_t *option = "") const;

    void SetDebugMode(Bool_t val = true);

    virtual void Print(Option_t *option ="") const;

    /**
     * Add parameter by given fileName.
     * If fileName does not include path, file will be searched in path/to/KEBI/input.
     *
     * fileName will also be registered as parameter. 
     * If parNameForFile is not set, parameter name will be set as 
     * INPUT_PARAMETER_FILE[fNumInputFiles]
    */
    virtual Int_t AddFile(TString fileName, TString parNameForFile = "");
    virtual Int_t AddPar(KBParameterContainer *parc, TString parNameForFile = "");
    Int_t GetNumInputFiles(); ///< Get number of input parameter files

    Bool_t SetPar(TString name, Bool_t val, Bool_t overwrite = false);       ///< Set Bool_t   type parameter with given name
    Bool_t SetPar(TString name, Int_t val, Bool_t overwrite = false);        ///< Set Int_t    type parameter with given name
    Bool_t SetPar(TString name, Double_t val, Bool_t overwrite = false);     ///< Set Double_t type parameter with given name
    Bool_t SetPar(TString name, TString val, Bool_t overwrite = false);      ///< Set TString  type parameter with given name
    Bool_t SetPar(TString name, const char* val, Bool_t overwrite = false);  ///< Set TString  type parameter with given name
    Bool_t SetParColor(TString name, TString valColor, Bool_t overwrite = false);

    Int_t    GetParN     (TString name);                ///< Get number of parameters in array of given name.
    Bool_t   GetParBool  (TString name, Int_t idx=-1);  ///< Get Bool_t   type parameter by given name.
    Int_t    GetParInt   (TString name, Int_t idx=-1);  ///< Get Int_t    type parameter by given name.
    Double_t GetParDouble(TString name, Int_t idx=-1);  ///< Get Double_t type parameter by given name.
    TString  GetParString(TString name, Int_t idx=-1);  ///< Get TString  type parameter by given name.
    kbaxis   GetParAxis  (TString name, Int_t idx=-1);  ///< Get KBVector3::Axis type parameter by given name.

    vector<Bool_t>   GetParVBool  (TString name);
    vector<Int_t>    GetParVInt   (TString name);
    vector<Double_t> GetParVDouble(TString name);
    vector<TString>  GetParVString(TString name);
    vector<kbaxis>   GetParVAxis  (TString name);

    TVector3 GetParV3(TString name);

    Double_t GetParX(TString name) { return GetParDouble(name,0); }
    Double_t GetParY(TString name) { return GetParDouble(name,1); }
    Double_t GetParZ(TString name) { return GetParDouble(name,2); }

    Int_t GetParWidth(TString name)   { return GetParInt(name); }
    Int_t GetParColor(TString name)   { return GetParInt(name); }
    Double_t GetParSize(TString name) { return GetParDouble(name); }

    Bool_t CheckPar(TString name);

    void ReplaceEnvironmentVariable(TString &val);

    bool IsEmpty() const;

  private:
    Bool_t fDebugMode = false;
    Int_t fNumInputFiles = 0;

  ClassDef(KBParameterContainer, 1)
};

#endif
