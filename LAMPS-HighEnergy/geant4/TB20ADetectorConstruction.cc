#include "KBParameterContainer.hh"
#include "TB20ADetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4UniformMagField.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4SubtractionSolid.hh"

TB20ADetectorConstruction::TB20ADetectorConstruction()
: G4VUserDetectorConstruction()
{
}

TB20ADetectorConstruction::~TB20ADetectorConstruction()
{
}

G4VPhysicalVolume *TB20ADetectorConstruction::Construct()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();

  auto par = runManager -> GetParameterContainer();

	G4double tpcInnerRadius = par -> GetParDouble("TPCrMin");
	G4double tpcOuterRadius = par -> GetParDouble("TPCrMax");
	G4double tpcLength = par -> GetParDouble("TPCLength");
	G4double tpcZOffset = par -> GetParDouble("TPCzOffset");

  G4NistManager *nist = G4NistManager::Instance();
  G4double STPTemperature = 273.15;
  G4double labTemperature = STPTemperature + 20.*kelvin;
  
	G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
	G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);
	G4Element *elementO = new G4Element("elementO", "O", 8., 16.000*g/mole);
	//G4Element *elementCu = new G4Element("elementCu","Cu", 29., 63.546*g/mole);
	//G4Element *elementAl = new G4Element("elementAl","Al", 13., 26.982*g/mole);

  G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);

	G4Material *matAcrylic = new G4Material("Acrylic", 1.19*g/cm3, 3);
	matAcrylic->AddElement(elementC,5);
	matAcrylic->AddElement(elementH,8);
	matAcrylic->AddElement(elementO,2);

	//G4Material *matCH2 = new G4Material("CH2", 0.91*g/cm3, 2);
	//matCH2->AddElement(elementH,2);
	//matCH2->AddElement(elementC,1);

	G4Material *matB = nist->FindOrBuildMaterial("G4_B");
	G4Material *matAl = nist->FindOrBuildMaterial("G4_Al");
	G4Material *matCu = nist->FindOrBuildMaterial("G4_Cu");
	G4Material *matFe = nist->FindOrBuildMaterial("G4_Fe");
	G4Material *matSn = nist->FindOrBuildMaterial("G4_Sn");
	G4Material *matSC = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
	//G4Material *matXYLENE = nist->FindOrBuildMaterial("G4_XYLENE");
	G4Material *matCH2 = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
	G4Material *matAir = nist->FindOrBuildMaterial("G4_AIR");
	G4Material *matVac = nist->FindOrBuildMaterial("G4_Galactic");
 
  G4double densityMethane = 0.717e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matMethaneGas = new G4Material("matMethaneGas ", densityMethane, 2, kStateGas, labTemperature);
  matMethaneGas -> AddElement(elementH, 4);
  matMethaneGas -> AddElement(elementC, 1);

	G4double densityBP = 0.3*matB->GetDensity() + 0.7*matCH2->GetDensity(); 
	G4Material *matBP = new G4Material("BoratedPolyethylene", densityBP, 2);
	matBP->AddMaterial(matB, 0.3);
	matBP->AddMaterial(matCH2, 0.7);

  TString gasPar = "p10";
  if (par -> CheckPar("TPCgasPar")) {
    gasPar = par -> GetParString("TPCgasPar");
    gasPar.ToLower();
         if (gasPar.Index("p10")>=0) gasPar = "p10";
    else if (gasPar.Index("p20")>=0) gasPar = "p20";
    else gasPar = "p10";
  }

  G4Material *matGas = nullptr;
  if (gasPar == "p10") {
    G4double densityGas = .9*densityArGas + .1*densityMethane;
    matGas = new G4Material("matP10", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.9*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.1*densityMethane/densityGas);
  }
  else if (gasPar == "p20") {
    G4double densityGas = .8*densityArGas + .2*densityMethane;
    matGas = new G4Material("matP20", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.8*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.2*densityMethane/densityGas);
  }

  G4double worlddX = par -> GetParDouble("worlddX");
  G4double worlddY = par -> GetParDouble("worlddY");
  G4double worlddZ = par -> GetParDouble("worlddZ");

	//world
  G4Box *solidWorld = new G4Box("World", worlddX, worlddY, worlddZ);
  G4LogicalVolume *logicWorld;
	if ( par -> GetParInt("worldOpt")==0 ){
		logicWorld = new G4LogicalVolume(solidWorld, matVac, "World");
	}else if ( par -> GetParInt("worldOpt")==1 ){
		logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
	}else{
		logicWorld = new G4LogicalVolume(solidWorld, matVac, "World");
	}
  G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, -1, true);


	//Acrylic Collimator
	if ( par -> GetParBool("CollimatorIn") )
	{
		G4double Collimatorx = par -> GetParDouble("Collimatorx");
		G4double Collimatory = par -> GetParDouble("Collimatory");
		G4double Collimatorz = par -> GetParDouble("Collimatorz");
		G4double CollimatorzOffset = par -> GetParDouble("CollimatorzOffset");

		G4double Holex = par -> GetParDouble("Holex");
		G4double Holey = par -> GetParDouble("Holey");

		G4Box *solidCollimator1 = new G4Box("Collimator1", Collimatorx/2.0, Collimatory/2.0, Collimatorz/2.0);
		G4Box *solidHole1 = new G4Box("Hole1", Collimatorx/2.0, Holey/2.0, Collimatorz/2.0);
		G4SubtractionSolid *solidSubC1 = new G4SubtractionSolid("SubC1", solidCollimator1, solidHole1, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubC1;
		if ( par -> GetParInt("CollimatorOpt")==0 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matAl, "SubC1");
		}else if ( par -> GetParInt("CollimatorOpt")==1 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matCu, "SubC1");
		}else if ( par -> GetParInt("CollimatorOpt")==2 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matFe, "SubC1");
		}else if ( par -> GetParInt("CollimatorOpt")==3 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matAcrylic, "SubC1");
		}else{
			logicSubC1 = new G4LogicalVolume(solidSubC1, matAl, "SubC1");
		}

		{
			G4VisAttributes * attSubC1 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubC1 -> SetVisAttributes(attSubC1);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,CollimatorzOffset+Collimatorz/2), logicSubC1, "SubC1", logicWorld, false, 0, true);

		G4Box *solidCollimator2 = new G4Box("Collimator2", Collimatorx/2.0, Collimatory/2.0, Collimatorz/2.0);
		G4Box *solidHole2 = new G4Box("Hole2", Holex/2.0, Collimatory/2.0, Collimatorz/2.0);
		G4SubtractionSolid *solidSubC2 = new G4SubtractionSolid("SubC2", solidCollimator2, solidHole2, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubC2;
		if ( par -> GetParInt("CollimatorOpt")==0 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matAl, "SubC2");
		}else if ( par -> GetParInt("CollimatorOpt")==1 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matCu, "SubC2");
		}else if ( par -> GetParInt("CollimatorOpt")==2 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matFe, "SubC2");
		}else if ( par -> GetParInt("CollimatorOpt")==3 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matAcrylic, "SubC2");
		}else{
			logicSubC2 = new G4LogicalVolume(solidSubC2, matAl, "SubC2");
		}

		{
			G4VisAttributes * attSubC2 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubC2 -> SetVisAttributes(attSubC2);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,CollimatorzOffset+3*Collimatorz/2), logicSubC2, "SubC2", logicWorld, false, 0, true);
	}

	//Boron Shield
	if ( par -> GetParBool("ShieldIn") )
	{
		G4double Shieldx = par -> GetParDouble("Shieldx");
		G4double Shieldy = par -> GetParDouble("Shieldy");
		G4double Shieldz = par -> GetParDouble("Shieldz");
		G4double ShieldzOffset = par -> GetParDouble("ShieldzOffset");

		G4double ShieldHolex = par -> GetParDouble("ShieldHolex");
		G4double ShieldHoley = par -> GetParDouble("ShieldHoley");

		G4Box *solidShield1 = new G4Box("Shield1", Shieldx/2.0, Shieldy/2.0, Shieldz/2.0);
		G4LogicalVolume *logicShield1 = nullptr;
		if ( par -> GetParInt("ShieldOpt")==0 ){
			logicShield1 = new G4LogicalVolume(solidShield1, matB, "Shield1");
		}else if ( par -> GetParInt("ShieldOpt")==1 ){
			logicShield1 = new G4LogicalVolume(solidShield1, matBP, "Shield1");
		}else{
			logicShield1 = new G4LogicalVolume(solidShield1, matB, "Shield1");
		}

		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Brown()));
			logicShield1 -> SetVisAttributes(attSubS);
		}

		G4Box *solidShield1_1 = new G4Box("Shield1_1", Shieldx/2.0, ShieldHoley/2.0, Shieldz/2.0);
		G4Box *solidHole1_1 = new G4Box("ShieldHole1_1", ShieldHolex/2.0, ShieldHoley/2.0, Shieldz/2.0);
		G4SubtractionSolid *solidSubS1_1 = new G4SubtractionSolid("SubS1_1", solidShield1_1, solidHole1_1, 0, G4ThreeVector(0,0,0));
		G4LogicalVolume *logicSubS1_1 = new G4LogicalVolume(solidSubS1_1, matAcrylic, "SubS1_1");

		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubS1_1 -> SetVisAttributes(attSubS);
		}

		new G4PVPlacement(0, G4ThreeVector(0,+(ShieldHoley+Shieldy)/2,ShieldzOffset+Shieldz/2), logicShield1, "Shiedl1", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(0,-(ShieldHoley+Shieldy)/2,ShieldzOffset+Shieldz/2), logicShield1, "Shiedl1", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(0,0,ShieldzOffset+Shieldz/2), logicSubS1_1, "SubS1_1", logicWorld, false, 0, true);

		G4Box *solidShield2 = new G4Box("Shield2", Shieldy/2.0, Shieldx/2.0, Shieldz/2.0);
		G4LogicalVolume *logicShield2 = nullptr;
		if ( par -> GetParInt("ShieldOpt")==0 ){
			logicShield2 = new G4LogicalVolume(solidShield2, matB, "Shield2");
		}else if ( par -> GetParInt("ShieldOpt")==1 ){
			logicShield2 = new G4LogicalVolume(solidShield2, matBP, "Shield2");
		}else{
			logicShield2 = new G4LogicalVolume(solidShield2, matB, "Shield2");
		}

		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Brown()));
			logicShield2 -> SetVisAttributes(attSubS);
		}

		G4Box *solidShield2_1 = new G4Box("Shield2_1", ShieldHoley/2.0, Shieldx/2.0, Shieldz/2.0);
		G4Box *solidHole2_1 = new G4Box("ShieldHole2_1", ShieldHoley/2.0, ShieldHolex/2.0, Shieldz/2.0);
		G4SubtractionSolid *solidSubS2_1 = new G4SubtractionSolid("SubS2_1", solidShield2_1, solidHole2_1, 0, G4ThreeVector(0,0,0));
		G4LogicalVolume *logicSubS2_1 = new G4LogicalVolume(solidSubS2_1, matAcrylic, "SubS2_1");

		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubS2_1 -> SetVisAttributes(attSubS);
		}

		new G4PVPlacement(0, G4ThreeVector(+(ShieldHoley+Shieldy)/2,0,ShieldzOffset+3*Shieldz/2), logicShield2, "Shiedl2", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(-(ShieldHoley+Shieldy)/2,0,ShieldzOffset+3*Shieldz/2), logicShield2, "Shiedl2", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(0,0,ShieldzOffset+3*Shieldz/2), logicSubS2_1, "SubS2_1", logicWorld, false, 0, true);
	}

	//Block 
	if ( par -> GetParBool("BlockIn") )
	{
		G4double Blockx = par -> GetParDouble("Blockx");
		G4double Blocky = par -> GetParDouble("Blocky");
		G4double Blockz = par -> GetParDouble("Blockz");
		G4double BlockzOffset = par -> GetParDouble("BlockzOffset");

		G4Box *solidBlock = new G4Box("Block", Blockx/2.0, Blocky/2.0, Blockz/2.0);
		G4Tubs *solidHole = new G4Tubs("Hole", 0, 60.0/2, Blockz/2.0, 0, 2*M_PI);
		G4SubtractionSolid *solidSubS = new G4SubtractionSolid("SubC", solidBlock, solidHole, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubS = new G4LogicalVolume(solidSubS, matAcrylic, "SubS");
		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubS -> SetVisAttributes(attSubS);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,BlockzOffset+Blockz/2), logicSubS, "Block", logicWorld, false, 0, true);
	}

	//Start counter
	if ( par -> GetParBool("StartCounterIn") )
	{
		G4double SCx = par -> GetParDouble("SCx");
		G4double SCy = par -> GetParDouble("SCy");
		G4double SCz = par -> GetParDouble("SCz");
		G4double SCzOffset = par -> GetParDouble("SCzOffset");

		G4Box *solidSC = new G4Box("SC", SCx/2.0, SCy/2.0, SCz/2.0);
		G4LogicalVolume *logicSC = new G4LogicalVolume(solidSC, matSC, "SC");
		{
			G4VisAttributes * attSC = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attSC -> SetForceWireframe(true);
			logicSC -> SetVisAttributes(attSC);
		}
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,0,SCzOffset+SCz/2), logicSC, "SC1", logicWorld, false, 1, true);
		runManager -> SetSensitiveDetector(pvp1);

		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,0,SCzOffset+SCz/2+20), logicSC, "SC2", logicWorld, false, 2, true);
		runManager -> SetSensitiveDetector(pvp2);
	}

	//Veto counter
	if ( par -> GetParBool("VetoIn") )
	{

		G4double Vetox = par -> GetParDouble("Vetox");
		G4double Vetoy = par -> GetParDouble("Vetoy");
		G4double Vetoz = par -> GetParDouble("Vetoz");
		G4double VetozOffset = par -> GetParDouble("VetozOffset");

		G4Box *solidVeto = new G4Box("Veto", Vetox/2.0, Vetoy/2.0, Vetoz/2.0);
		G4LogicalVolume *logicVeto = new G4LogicalVolume(solidVeto, matSC, "Veto");
		{
			G4VisAttributes * attVeto = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			logicVeto -> SetVisAttributes(attVeto);
		}
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(0,(Vetoy+10.0)/2,VetozOffset+Vetoz/2), logicVeto, "Veto1", logicWorld, false, 3, true);
		runManager -> SetSensitiveDetector(pvp1);

		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(0,-(Vetoy+10.0)/2,VetozOffset+Vetoz/2), logicVeto, "Veto2", logicWorld, false, 4, true);
		runManager -> SetSensitiveDetector(pvp2);
	}

	//BDC1
	if ( par -> GetParBool("BDC1In") )
	{
		G4double BDC1x = par -> GetParDouble("BDC1x");
		G4double BDC1y = par -> GetParDouble("BDC1y");
		G4double BDC1z = par -> GetParDouble("BDC1z");
		G4double BDC1zOffset = par -> GetParDouble("BDC1zOffset");

		G4Box *solidBDC1 = new G4Box("BDC1", BDC1x/2.0, BDC1y/2.0, BDC1z/2.0);
		G4LogicalVolume *logicBDC1 = new G4LogicalVolume(solidBDC1, matGas, "BDC1");
		{
			G4VisAttributes * attBDC1 = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attBDC1 -> SetForceWireframe(true);
			logicBDC1 -> SetVisAttributes(attBDC1);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,BDC1zOffset+BDC1z/2), logicBDC1, "BDC1", logicWorld, false, 5, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//BDC2
	if ( par -> GetParBool("BDC2In") )
	{
		G4double BDC2x = par -> GetParDouble("BDC2x");
		G4double BDC2y = par -> GetParDouble("BDC2y");
		G4double BDC2z = par -> GetParDouble("BDC2z");
		G4double BDC2zOffset = par -> GetParDouble("BDC2zOffset");

		G4Box *solidBDC2 = new G4Box("BDC2", BDC2x/2.0, BDC2y/2.0, BDC2z/2.0);
		G4LogicalVolume *logicBDC2 = new G4LogicalVolume(solidBDC2, matGas, "BDC2");
		{
			G4VisAttributes * attBDC2 = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attBDC2 -> SetForceWireframe(true);
			logicBDC2 -> SetVisAttributes(attBDC2);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,BDC2zOffset+BDC2z/2), logicBDC2, "BDC2", logicWorld, false, 6, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//Target1
	if ( par -> GetParBool("Target1In") )
	{
		G4double Target1x = par -> GetParDouble("Target1x");
		G4double Target1y = par -> GetParDouble("Target1y");
		G4double Target1z = par -> GetParDouble("Target1z");
		G4double Target1zOffset = par -> GetParDouble("Target1zOffset");

		G4Box *solidTarget1 = new G4Box("Target1", Target1x/2.0, Target1y/2.0, Target1z/2.0);
		G4LogicalVolume *logicTarget1 = new G4LogicalVolume(solidTarget1, matCH2, "Traget1");
		//G4LogicalVolume *logicTarget1 = new G4LogicalVolume(solidTarget1, matSn, "Traget1");
		{
			G4VisAttributes * attTarget1 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget1 -> SetVisAttributes(attTarget1);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target1zOffset+Target1z/2), logicTarget1, "Target1", logicWorld, false, 20, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//Target2
	if ( par -> GetParBool("Target2In") )
	{
		G4double Target2x = par -> GetParDouble("Target2x");
		G4double Target2y = par -> GetParDouble("Target2y");
		G4double Target2z = par -> GetParDouble("Target2z");
		G4double Target2zOffset = par -> GetParDouble("Target2zOffset");

		G4Box *solidTarget2 = new G4Box("Target2", Target2x/2.0, Target2y/2.0, Target2z/2.0);
		G4LogicalVolume *logicTarget2 = new G4LogicalVolume(solidTarget2, matCH2, "Traget2");
		{
			G4VisAttributes * attTarget2 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget2 -> SetVisAttributes(attTarget2);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target2zOffset+Target2z/2), logicTarget2, "Target2", logicWorld, false, 21, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//TPC
	if ( par -> GetParBool("TPCIn") )
	{
		G4Tubs *solidTPC = new G4Tubs("TPC", tpcInnerRadius, tpcOuterRadius, .5*tpcLength, 0., 360*deg);
		G4LogicalVolume *logicTPC = new G4LogicalVolume(solidTPC, matGas, "TPC");
		{
			G4VisAttributes * attTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attTPC -> SetForceWireframe(true);
			logicTPC -> SetVisAttributes(attTPC);
		}
		logicTPC -> SetUserLimits(new G4UserLimits(1.*mm));
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset+tpcLength/2), logicTPC, "TPC", logicWorld, false, 7, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//BTOF
	if ( par -> GetParBool("BTOFIn") )
	{

		G4double BTOFx = par -> GetParDouble("BTOFx");
		G4double BTOFy = par -> GetParDouble("BTOFy");
		G4double BTOFz = par -> GetParDouble("BTOFz");
		G4double BTOFzOffset = par -> GetParDouble("BTOFzOffset");

		G4Box *solidBTOF1 = new G4Box("BTOF1", BTOFx/2.0, BTOFy/2.0, BTOFz/2.0);
		G4LogicalVolume *logicBTOF1 = new G4LogicalVolume(solidBTOF1, matSC, "BTOF1");
		{
			G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOF -> SetForceWireframe(true);
			logicBTOF1 -> SetVisAttributes(attBTOF);
		}

		G4Box *solidBTOF2 = new G4Box("BTOF1", BTOFy/2.0, BTOFx/2.0, BTOFz/2.0);
		G4LogicalVolume *logicBTOF2 = new G4LogicalVolume(solidBTOF2, matSC, "BTOF2");
		{
			G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOF -> SetForceWireframe(true);
			logicBTOF2 -> SetVisAttributes(attBTOF);
		}

		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(+620,+BTOFy/2,BTOFzOffset+BTOFz/2), logicBTOF1, "BTOF1", logicWorld, false, 8, true);
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(+620,-BTOFy/2,BTOFzOffset+BTOFz/2), logicBTOF1, "BTOF2", logicWorld, false, 9, true);

		auto pvp3 = new G4PVPlacement(0, G4ThreeVector(+BTOFy/2,+520,BTOFzOffset+BTOFz/2), logicBTOF2, "BTOF3", logicWorld, false, 10, true);
		auto pvp4 = new G4PVPlacement(0, G4ThreeVector(-BTOFy/2,+520,BTOFzOffset+BTOFz/2), logicBTOF2, "BTOF4", logicWorld, false, 11, true);

		runManager -> SetSensitiveDetector(pvp1);
		runManager -> SetSensitiveDetector(pvp2);
		runManager -> SetSensitiveDetector(pvp3);
		runManager -> SetSensitiveDetector(pvp4);
	}

	//NeutronDetIn
	if ( par -> GetParBool("NeutronDetIn") )
	{

		G4double NeutronDetx = par -> GetParDouble("NeutronDetx");
		G4double NeutronDety = par -> GetParDouble("NeutronDety");
		G4double NeutronDetz = par -> GetParDouble("NeutronDetz");
		G4double NeutronDetzOffset = par -> GetParDouble("NeutronDetzOffset");

		G4Box *solidNeutronDet = new G4Box("NeutronDet", NeutronDetx/2.0, NeutronDety/2.0, NeutronDetz/2.0);
		G4LogicalVolume *logicNeutronDet = new G4LogicalVolume(solidNeutronDet, matSC, "NeutronDet");
		{
			G4VisAttributes * attNeutronDet = new G4VisAttributes(G4Colour(G4Colour::Green()));
			attNeutronDet -> SetForceWireframe(true);
			logicNeutronDet -> SetVisAttributes(attNeutronDet);
		}

		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(+1450,+NeutronDety/2,NeutronDetzOffset+NeutronDetz/2), logicNeutronDet, "NeutronDet", logicWorld, false, 12, true);
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(+1450,-NeutronDety/2,NeutronDetzOffset+NeutronDetz/2), logicNeutronDet, "NeutronDet", logicWorld, false, 12, true);
		runManager -> SetSensitiveDetector(pvp1);
		runManager -> SetSensitiveDetector(pvp2);

	}

	//ATTPCIn
	if ( par -> GetParBool("ATTPCIn") )
	{

		G4double ATTPCx = par -> GetParDouble("ATTPCx");
		G4double ATTPCy = par -> GetParDouble("ATTPCy");
		G4double ATTPCz = par -> GetParDouble("ATTPCz");
		G4double ATTPCzOffset = par -> GetParDouble("ATTPCzOffset");

		G4Box *solidATTPC = new G4Box("ATTPC", ATTPCx/2.0, ATTPCy/2.0, ATTPCz/2.0);
		G4LogicalVolume *logicATTPC = new G4LogicalVolume(solidATTPC, matGas, "ATTPC");

		{
			G4VisAttributes * attATTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attATTPC -> SetForceWireframe(true);
			logicATTPC -> SetVisAttributes(attATTPC);
		}

		G4RotationMatrix* Rot = new G4RotationMatrix;
		Rot->rotateY(40*deg);
		//auto pvp = new G4PVPlacement(0, G4ThreeVector(+925,+0,ATTPCzOffset+ATTPCz/2), logicATTPC, "ATTPC", logicWorld, false, 10, true);
		auto pvp = new G4PVPlacement(G4Transform3D(*Rot,G4ThreeVector(+950,+0,ATTPCzOffset+ATTPCz/2)), logicATTPC, "ATTPC", logicWorld, false, 13, true);
		runManager -> SetSensitiveDetector(pvp);

	}

	/*
  bool checkWall = par -> CheckPar("numNeutronWall");
  if (checkWall)
  {

    G4int numWall = par -> GetParInt("numNeutronWall");
    for (auto iwall = 0; iwall < numWall; ++iwall) {
      auto naStackAxis = par -> GetParAxis(Form("naStackAxis%d",iwall));
      auto naNumStack = par -> GetParInt(Form("naNumStack%d",iwall));
      auto nadX = par -> GetParDouble(Form("nadX%d",iwall));
      auto nadY = par -> GetParDouble(Form("nadY%d",iwall));
      auto nadZ = par -> GetParDouble(Form("nadZ%d",iwall));
      auto naXOffset = par -> GetParDouble(Form("naXOffset%d",iwall));
      auto naYOffset = par -> GetParDouble(Form("naYOffset%d",iwall));
      auto naZOffset = par -> GetParDouble(Form("naZOffset%d",iwall));

      G4Box* solidScint = new G4Box(Form("Scintillator_%d",iwall), 0.5*nadX, 0.5*nadY, 0.5*nadZ);
      G4LogicalVolume* logicScint = new G4LogicalVolume(solidScint, scint_mat, Form("Scintillator_%d",iwall));

      KBGeoBoxStack boxStack(naXOffset,naYOffset,naZOffset,nadX,nadY,nadZ,naNumStack,naStackAxis,KBVector3::kZ);

      for (auto copy = 0; copy < naNumStack; ++copy) {
        Int_t id = 10000+copy+iwall*100;
        G4String name = Form("Scintillator_%d_%d",iwall,copy);
        auto box = boxStack.GetBox(copy);
        auto pos = box.GetCenter();
        G4ThreeVector gpos(pos.X(),pos.Y(),pos.Z());
        auto cpvp = new G4PVPlacement(0, gpos, logicScint, name, logicWorld, false, id, true);
        runManager -> SetSensitiveDetector(cpvp);
      }
    }
  }
	*/

  //G4double bfieldx = par -> GetParDouble("bfieldx");
  //G4double bfieldy = par -> GetParDouble("bfieldy");
  //G4double bfieldz = par -> GetParDouble("bfieldz");
  //new G4GlobalMagFieldMessenger(G4ThreeVector(bfieldx*tesla, bfieldy*tesla, bfieldz*tesla));

  return physWorld;
}
