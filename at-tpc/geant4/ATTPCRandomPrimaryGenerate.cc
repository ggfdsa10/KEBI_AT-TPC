#include "ATTPCRandomPrimaryGenerate.hh"
#include "ATTPCRectanglePad.hh"
#include "ATTPCHoneyCombPad.hh"
#include "ATTPC20RectanglePad.hh"
#include "ATTPCStripPad.hh"
#include "TVector3.h"
#include "TLorentzVector.h"

using namespace std;
using namespace TMath;

ATTPCRandomPrimaryGenerate::ATTPCRandomPrimaryGenerate() : G4VUserPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  fRunManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  fPar = fRunManager -> GetParameterContainer();
  fRandom = new TRandom3(0);

  fParticleTable = G4ParticleTable::GetParticleTable();
  fIontable = G4IonTable::GetIonTable();
}

ATTPCRandomPrimaryGenerate::~ATTPCRandomPrimaryGenerate()
{
  delete fParticleGun;
}

void ATTPCRandomPrimaryGenerate::GeneratePrimaries(G4Event* anEvent)
{   
  fRandom -> SetSeed(0);
  TString RandomParticle = fPar -> GetParString("RandomParticle");
  G4String particleName;
  if (RandomParticle == "mu-"){
    fParticle = fParticleTable -> FindParticle(particleName = "mu-");
  }

  else if (RandomParticle == "proton"){
    fParticle = fParticleTable -> FindParticle(particleName = "proton");
  }

  else if (RandomParticle == "alpha"){
    fParticle = fIontable ->FindIon(2,4,0); //helium
  }

  else if (RandomParticle == "ion"){
    Int_t IonZ = fPar -> GetParInt("IonInfo", 0);
    Int_t IonA = fPar -> GetParInt("IonInfo", 1);
    Int_t IonE = fPar -> GetParInt("IonInfo", 2);

    fParticle = fIontable ->GetIon(IonZ, IonA, IonE);
    g4_info << " Primary particle : " << fIontable ->GetIonName(IonZ, IonA, IonE) << endl;
  }

  else if (RandomParticle == "gamma"){
    fParticle = fParticleTable -> FindParticle(particleName = "gamma");
  }

  else{
    g4_warning << " Particle name is not valid. " << RandomParticle << endl;
  }
  if(RandomParticle != "ion"){
    g4_info << " Primary particle : " << RandomParticle << endl;
  }


  ExternalTriggerPMT(anEvent);

  fNumberOfPrimary = anEvent -> GetNumberOfPrimaryVertex();
}

void ATTPCRandomPrimaryGenerate::ExternalTriggerPMT(G4Event* event)
{
  G4double tpcX = fPar -> GetParDouble("tpcX");
  G4double tpcY = fPar -> GetParDouble("tpcY");
  G4double tpcZ = fPar -> GetParDouble("tpcZ");

  G4double windowX = fPar -> GetParDouble("WindowSize", 0);
  G4double windowZ = fPar -> GetParDouble("WindowSize", 1);
  G4double WindowHeight = fPar -> GetParDouble("WindowHeight");
  G4double WindowShift = fPar -> GetParDouble("WindowShift");
  G4double TriggerDistance = fPar -> GetParDouble("TriggerDistance");
  TString PadPlaneType = fPar -> GetParString("PadPlaneType");

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

  PositionX = gRandom -> Uniform(-windowX/2., windowX/2.) +tpcX-PadWidth/2-PadGap/2 +WindowShift;
  PositionY = -TriggerDistance +tpcY-PadHeight/2-PadGap/2;
  PositionZ = gRandom -> Uniform(-windowZ/2., windowZ/2.) +tpcZ +WindowHeight;

  G4double arctanX1 = ATan(PositionX / (2 * TriggerDistance));
  G4double arctanX2 = ATan((windowX - PositionX) / (2 * TriggerDistance));

  G4double arctanZ1 = ATan(PositionZ / (2 * TriggerDistance));
  G4double arctanZ2 = ATan((windowZ - PositionZ) / (2 * TriggerDistance));

  G4double ThetaAngle = gRandom -> Uniform(-arctanZ2, arctanZ1) +Pi()/2;
  G4double PaiAngle = gRandom -> Uniform(-arctanX2, arctanX1) +Pi()/2;

  DirectionX = Sin(ThetaAngle) * Cos(PaiAngle);
  DirectionY = Sin(ThetaAngle) * Sin(PaiAngle);
  DirectionZ = Cos(ThetaAngle);

  G4double ParticleEnergy = fPar -> GetParDouble("ParticleEnergy");

  fParticleGun -> SetParticleDefinition(fParticle);
  fParticleGun -> SetParticleMomentumDirection(G4ThreeVector(DirectionX, DirectionY, DirectionZ));
  fParticleGun -> SetParticleEnergy(ParticleEnergy *MeV);
  fParticleGun -> SetParticlePosition(G4ThreeVector(PositionX *mm, PositionY *mm, PositionZ *mm));
  fParticleGun -> GeneratePrimaryVertex(event);
}

void ATTPCRandomPrimaryGenerate::ProtonBeam(G4Event* event)
{
  // TString PadPlaneType = fPar -> GetParString("PadPlaneType");
  // G4double tpcX = fPar -> GetParDouble("tpcX");
  // G4double tpcY = fPar -> GetParDouble("tpcY");
  // G4double tpcZ = fPar -> GetParDouble("tpcZ");
  // G4double PadWidth;
  // G4double PadHeight;
  // G4double PadGap;

  // if(PadPlaneType == "RectanglePad"){
  //   ATTPCRectanglePad *fPadPlane = new ATTPCRectanglePad(); 
  //   PadWidth = fPadPlane ->GetPadWidth();
  //   PadHeight = fPadPlane ->GetPadHeight();
  //   PadGap = fPadPlane ->GetPadGap();
  // }
  // if(PadPlaneType == "HoneyCombPad"){
  //   ATTPCHoneyCombPad *fPadPlane = new ATTPCHoneyCombPad(); 
  //   PadWidth = fPadPlane ->GetPadWidth();
  //   PadHeight = fPadPlane ->GetPadHeight();
  //   PadGap = fPadPlane ->GetPadGap();
  // }
  // if(PadPlaneType == "20RectanglePad"){
  //   ATTPC20RectanglePad *fPadPlane = new ATTPC20RectanglePad(); 
  //   PadWidth = fPadPlane ->GetPadWidth();
  //   PadHeight = fPadPlane ->GetPadHeight();
  //   PadGap = fPadPlane ->GetPadGap();
  // }
  // if(PadPlaneType == "StripPad"){
  //   ATTPCStripPad *fPadPlane = new ATTPCStripPad(); 
  //   PadWidth = fPadPlane ->GetPadWidth();
  //   PadHeight = fPadPlane ->GetPadHeight();
  //   PadGap = fPadPlane ->GetPadGap();
  // }

  // if(trackNum==0){
  //   PositionX = 0.;
  //   PositionY = 0.;
  //   PositionZ = 0.;
  //   DirectionX = 0.;
  //   DirectionY = 1.;
  //   DirectionZ = 0.;

  //   ParticleEnergy = 4000.;
  // }

}