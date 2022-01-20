#include "ATTPCRandomPrimaryGenerate.hh"
#include "ATTPCRectanglePad.hh"
#include "ATTPCHoneyCombPad.hh"

using namespace std;
using namespace TMath;

ATTPCRandomPrimaryGenerate::ATTPCRandomPrimaryGenerate() : G4VUserPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);
}

ATTPCRandomPrimaryGenerate::~ATTPCRandomPrimaryGenerate()
{
  delete fParticleGun;
}

void ATTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{   
  gRandom -> SetSeed(0);
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* iontable = G4IonTable::GetIonTable();
  G4String particleName;
  G4ParticleDefinition* particle = nullptr;

  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString RandomParticle = par -> GetParString("RandomParticle");
  ParticleEnergy = par -> GetParDouble("ParticleEnergy");

  if (RandomParticle == "mu-"){
    particle = particleTable -> FindParticle(particleName = "mu-");
  }

  else if (RandomParticle == "proton"){
    particle = particleTable -> FindParticle(particleName = "proton");
  }

  else if (RandomParticle == "alpha"){
    particle = iontable ->FindIon(2,4,0); //helium
  }

  else if (RandomParticle == "ion"){
    Int_t IonZ = par -> GetParInt("IonInfo", 0);
    Int_t IonA = par -> GetParInt("IonInfo", 1);
    Int_t IonE = par -> GetParInt("IonInfo", 2);

    particle = iontable ->GetIon(IonZ, IonA, IonE);
    g4_info << " Primary particle : " << iontable ->GetIonName(IonZ, IonA, IonE) << endl;
  }

  else if (RandomParticle == "gamma"){
    particle = particleTable -> FindParticle(particleName = "gamma");
  }

  else{
    g4_warning << " Particle name is not valid. " << RandomParticle << endl;
  }
  if(RandomParticle != "ion")
    g4_info << " Primary particle : " << RandomParticle << endl;


  // SingleProtonBeam();


  // auto gRandom = new TRandom3(0);
  // PositionX = gRandom -> Uniform(0,40); //gRandom -> Uniform(18.75, 21.875);
  // PositionY = -50;
  // PositionZ = gRandom -> Uniform(50,150);

  // double angle = gRandom -> Uniform(-5,5);
  // DirectionX = Sin(angle*Pi()/180.);
  // DirectionY = Cos(angle*Pi()/180.);
  // ParticleEnergy = gRandom -> Uniform(500, 100000000);

  int event = gRandom -> Uniform(5,10);
  for(int i=0; i<1; i++){
    TriggerFunction();
    fParticleGun -> SetParticleDefinition(particle);
    fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
    fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
    fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
    fParticleGun -> GeneratePrimaryVertex(anEvent);
  }


  fNumberOfPrimary = anEvent -> GetNumberOfPrimaryVertex();
}

void ATTPCRandomPrimaryGenerate::TriggerFunction()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");

  G4double windowX = par -> GetParDouble("WindowSize", 0);
  G4double windowZ = par -> GetParDouble("WindowSize", 1);
  G4double WindowHeight = par -> GetParDouble("WindowHeight");
  G4double WindowShift = par -> GetParDouble("WindowShift");
  G4double TriggerDistance = par -> GetParDouble("TriggerDistance");
  TString PadPlaneType = par -> GetParString("PadPlaneType");

  G4double PadWidth;
  G4double PadHeight;
  G4double PadGap;

  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  else if(PadPlaneType == "HoneycombPad"){
    ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  
  PositionX = gRandom -> Gaus(windowX/2, 5); //22
  PositionZ = gRandom -> Gaus(windowZ/2, 5); // 23

  G4double arctanX1 = ATan(PositionX / (2 * TriggerDistance));
  G4double arctanX2 = ATan((windowX - PositionX) / (2 * TriggerDistance));

  G4double arctanZ1 = ATan(PositionZ / (2 * TriggerDistance));
  G4double arctanZ2 = ATan((windowZ - PositionZ) / (2 * TriggerDistance));

  G4double ThetaAngle = gRandom -> Uniform(-arctanZ2, arctanZ1) +Pi()/2;
  G4double PaiAngle = gRandom -> Uniform(-arctanX2, arctanX1) +Pi()/2;

  PositionX = PositionX -WindowShift;
  PositionY = -400.;
  PositionZ = PositionZ +WindowHeight;

  DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  DirectionZ = Cos(ThetaAngle);

}
  
void ATTPCRandomPrimaryGenerate::SingleProtonBeam()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  auto par = runManager -> GetParameterContainer();

  TString PadPlaneType = par -> GetParString("PadPlaneType");
  G4double tpcX = par -> GetParDouble("tpcX");
  G4double tpcY = par -> GetParDouble("tpcY");
  G4double tpcZ = par -> GetParDouble("tpcZ");
  G4double PadWidth;
  G4double PadHeight;
  G4double PadGap;

  if(PadPlaneType == "RectanglePad"){
    ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }

  if(PadPlaneType == "HoneyCombPad"){
    ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
    PadWidth = fPadPlane ->GetPadWidth();
    PadHeight = fPadPlane ->GetPadHeight();
    PadGap = fPadPlane ->GetPadGap();
  }
  
  PositionX = gRandom -> Gaus(50-PadWidth/2., 3);
  PositionZ = gRandom -> Gaus(75, 3);
  PositionY = -PadHeight/2.;

  G4double ThetaAngle = gRandom -> Uniform(-5., 5.)*Pi()/180 +Pi()/2;
  G4double PaiAngle = gRandom -> Uniform(-5., 5.)*Pi()/180 +Pi()/2;
  
  DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  DirectionZ = Cos(ThetaAngle);

  ParticleEnergy = gRandom -> Uniform(10, 100.);
}