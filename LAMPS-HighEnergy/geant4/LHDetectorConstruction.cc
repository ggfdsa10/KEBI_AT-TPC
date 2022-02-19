#include "LHDetectorConstruction.hh"
#include "LHMagneticField.hh"
#include "LHMagneticFieldSetup.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "KBParameterContainer.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4RunManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

LHDetectorConstruction::LHDetectorConstruction()
: G4VUserDetectorConstruction()
{
}

LHDetectorConstruction::~LHDetectorConstruction()
{
}

G4VPhysicalVolume *LHDetectorConstruction::Construct()
{
	auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
	auto par = runManager -> GetParameterContainer();

	//Materials, Temperature
	//-------------------------------------------

	G4NistManager *nist = G4NistManager::Instance();
	G4double STPTemperature = 273.15;
	G4double labTemperature = STPTemperature + 20.*kelvin;

	G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
	G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);

	G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
	G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);

	G4double densityMethane = 0.717e-3*g/cm3*STPTemperature/labTemperature;
	G4Material *matMethaneGas = new G4Material("matMethaneGas ", densityMethane, 2, kStateGas, labTemperature);
	matMethaneGas -> AddElement(elementH, 4);
	matMethaneGas -> AddElement(elementC, 1);

	TString gasPar = "p10";
	if (par -> CheckPar("TPCgasPar"))
	{
		gasPar = par -> GetParString("TPCgasPar");
		gasPar.ToLower();
		if      (gasPar.Index("p10")>=0) gasPar = "p10";
		else if (gasPar.Index("p20")>=0) gasPar = "p20";
		else gasPar = "p10";
	}

	G4Material *matGas = nullptr;
	if (gasPar == "p10")
	{
		G4double densityGas = .9*densityArGas + .1*densityMethane;
		matGas = new G4Material("matP10", densityGas, 2, kStateGas, labTemperature);
		matGas -> AddMaterial(matArGas, 0.9*densityArGas/densityGas);
		matGas -> AddMaterial(matMethaneGas, 0.1*densityMethane/densityGas);
	}
	else if (gasPar == "p20")
	{
		G4double densityGas = .8*densityArGas + .2*densityMethane;
		matGas = new G4Material("matP20", densityGas, 2, kStateGas, labTemperature);
		matGas -> AddMaterial(matArGas, 0.8*densityArGas/densityGas);
		matGas -> AddMaterial(matMethaneGas, 0.2*densityMethane/densityGas);
	}

	G4Material *matAir = nist -> FindOrBuildMaterial("G4_AIR");
	G4Material *matVac = nist -> FindOrBuildMaterial("G4_Galactic");
	G4Material *matSC  = nist -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

	//World
	//-------------------------------------------

	G4double worlddX = par -> GetParDouble("worlddX");
	G4double worlddY = par -> GetParDouble("worlddY");
	G4double worlddZ = par -> GetParDouble("worlddZ");
	bool SetWorldVac = (par->GetParInt("worldOpt") == 0)?true:false;

	G4Box *solidWorld = new G4Box("World", worlddX, worlddY, worlddZ);
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, SetWorldVac?matVac:matAir, "World");
	G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(0,0,0), logicWorld, "World", 0, false, -1, true);
	//G4VisAttributes * visWorld = new G4VisAttributes(G4Colour(G4Colour::White()));
	//visWorld->SetForceWireframe(true);
	//logicWorld->SetVisAttributes(visWorld);

	//Magnetic field
	//-------------------------------------------

	if ( par -> GetParBool("bfieldmap") )
	{
		TString name = par -> GetParString("bfieldmapfile");
		G4cout << "Use bfield map, name: " << name << G4endl;

		LHMagneticField* bField = new LHMagneticField();
		//bField->SetVerbosity(3);
		//bField->SetFieldBoundX(-worlddX*2, worlddX*2);
		//bField->SetFieldBoundY(-worlddY*2, worlddY*2);
		//bField->SetFieldBoundZ(-worlddZ*2, worlddZ*2);
		//bField->SetFieldOffset(1, 1, 1);
		//bField->SetUniformField(0, 0, 1.);
		//bField->SetUnitDistance(cm);
		//bField->SetUnitField(kilogauss);
		//bField->SetUnitField(1.e-3 * tesla);
		bField->MakeFieldMap(name.Data());

		LHMagneticFieldSetup* bFieldSetup = new LHMagneticFieldSetup();
		//bFieldSetup->SetStepperType(2);
		//bFieldSetup->SetStepMin(1 * mm);
		//bFieldSetup->SetDeltaChord(0.1 * mm);
		//bFieldSetup->SetDeltaIntersection(1.e-4);
		//bFieldSetup->SetDeltaOneStep(1.e-3);
		//bFieldSetup->SetEpsilonMax(1.e-4);
		//bFieldSetup->SetEpsilonMin(1.e-6);
		//bFieldSetup->SetFieldManager(G4TransportationManager::GetTransportationManager()->GetFieldManager());
		bFieldSetup->MakeSetup(bField);
		fFieldCache.Put(bFieldSetup);
		logicWorld->SetFieldManager(fFieldCache.Get()->GetFieldManager(), true);
	}
	else
	{
		G4double bfieldx = par -> GetParDouble("bfieldx");
		G4double bfieldy = par -> GetParDouble("bfieldy");
		G4double bfieldz = par -> GetParDouble("bfieldz");
		new G4GlobalMagFieldMessenger(G4ThreeVector(bfieldx*tesla, bfieldy*tesla, bfieldz*tesla));
	}

	//Solenoid frame
	//-------------------------------------------

	if ( par -> GetParBool("MagFrameIn") )
	{
		//Materials
		//+++++++++++++++++++++++++++++++++++++++

		/*
		SS304 (steel)
		Density: 8,000 Kg/m^3

		Carbon (C)		: 0.07 (%)
		Chromium (Cr)	: 17.50 - 19.50
		Manganese (Mn)	: 2.00
		Nitrogen (N)	: 0.10
		Nickel (Ni)		: 8.00 - 10.50
		Phosphorous (P)	: 0.045
		Sulphur (S)		: 0.015
		Silicon (Si)	: 1.00
		Iron (Fe) 		: Balance (ckim: assume all remaining percentage)
		*/

		const int	SS304_nComp   = 9;
		const float SS304_density = 8000 * 0.001 * g/cm3;
		const float SS304f_C  = 0.07; //f stands for fraction, all fraction from here are %
		const float SS304f_Cr = (17.50 + 19.50)/2;
		const float SS304f_Mn = 2.00;
		const float SS304f_N  = 0.10;
		const float SS304f_Ni = (8.00 + 10.50)/2;
		const float SS304f_P  = 0.045;
		const float SS304f_S  = 0.015;
		const float SS304f_Si = 1.00;
		const float SS304f_Fe = 100 - (SS304f_C+SS304f_Cr+SS304f_Mn+SS304f_N+SS304f_Ni+SS304f_P+SS304f_S+SS304f_Si);

		G4Material* SS304 = new G4Material("SS304", SS304_density, SS304_nComp);
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_C"),  perCent * SS304f_C );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_Cr"), perCent * SS304f_Cr );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_Mn"), perCent * SS304f_Mn );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_N"),  perCent * SS304f_N );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_Ni"), perCent * SS304f_Ni );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_P"),  perCent * SS304f_P );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_S"),  perCent * SS304f_S );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_Si"), perCent * SS304f_Si );
		SS304->AddMaterial( nist->FindOrBuildMaterial("G4_Fe"), perCent * SS304f_Fe );

		//Aluminium: pure Al or Al oxide?
		//G4Material* MagFAl = nist->FindOrBuildMaterial("G4_Al"); //Pure Al, unlikely
		G4Material* MagFAl = nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"); //Al oxide

		//Structure parameters: no possibility of change
		//++++++++++++++++++++++++++++++++++++++++++++++

		//OVC: SS304 (* OVC: OVerall Construction)
		const float OVC_bt_ir = 1610./2; //bt: bore tube, ir: inner radius
		const float OVC_bt_or = 1622./2;
		const float OVC_bt_length = 3000.;

		const float OVC_ot_ir = 2144./2; //ot: outer tube
		const float OVC_ot_or = 2160./2;
		const float OVC_ot_length = OVC_bt_length;

		const float OVC_ec_ir = OVC_bt_ir; //ec: endcap
		const float OVC_ec_or = OVC_ot_or;
		const float OVC_ec_length = 20.; //Thickness

		//RS (Radiation shield): Aluminium
		const float RS_bt_ir = 1650./2;
		const float RS_bt_or = 1662./2;
		const float RS_bt_length = 2788.;

		const float RS_ot_ir = 2034./2;
		const float RS_ot_or = 2058./2;
		const float RS_ot_length = RS_bt_length;

		const float RS_ec_ir = RS_bt_ir;
		const float RS_ec_or = RS_ot_or;
		const float RS_ec_length = 6.;

		//Former (Cold-mass) : Aluminimum
		const float Former_ir = 1703.2/2;
		const float Former_or = 1815.0/2;
		const float Former_length = 2600.;

		//Implementation
		//+++++++++++++++++++++++++++++++++++++++

		G4ThreeVector ZeroP(0, 0, 0);
		G4VisAttributes* visMagF = new G4VisAttributes(G4Colour(G4Colour::White()));
		visMagF->SetForceWireframe(true);

		//OVC
		G4Tubs *OVC_solid_bt = new G4Tubs("OVC_bt_solid", OVC_bt_ir, OVC_bt_or, OVC_bt_length/2, 0., 360*deg);
		G4Tubs *OVC_solid_ot = new G4Tubs("OVC_ot_solid", OVC_ot_ir, OVC_ot_or, OVC_ot_length/2, 0., 360*deg);
		G4Tubs *OVC_solid_ec = new G4Tubs("OVC_ec_solid", OVC_ec_ir, OVC_ec_or, OVC_ec_length/2, 0., 360*deg);
		G4LogicalVolume* OVC_logic_bt = new G4LogicalVolume(OVC_solid_bt, SS304, "OVC_bt_logic");
		G4LogicalVolume* OVC_logic_ot = new G4LogicalVolume(OVC_solid_ot, SS304, "OVC_ot_logic");
		G4LogicalVolume* OVC_logic_ec = new G4LogicalVolume(OVC_solid_ec, SS304, "OVC_ec_logic");
		OVC_logic_bt->SetVisAttributes(visMagF);
		OVC_logic_ot->SetVisAttributes(visMagF);
		OVC_logic_ec->SetVisAttributes(visMagF);
		G4ThreeVector OVCec1P(0, 0, -OVC_bt_length/2 - OVC_ec_length/2);
		G4ThreeVector OVCec2P(0, 0, +OVC_bt_length/2 + OVC_ec_length/2);
		auto OVC_bt  = new G4PVPlacement(0, ZeroP, OVC_logic_bt, "MagF_OVC_bt", logicWorld, false, -1, true);
		auto OVC_ot  = new G4PVPlacement(0, ZeroP, OVC_logic_ot, "MagF_OVC_ot", logicWorld, false, -1, true);
		auto OVC_ec1 = new G4PVPlacement(0, OVCec1P, OVC_logic_ec, "MagF_OVC_ec1", logicWorld, false, -1, true);
		auto OVC_ec2 = new G4PVPlacement(0, OVCec2P, OVC_logic_ec, "MagF_OVC_ec2", logicWorld, false, -1, true);

		//RS
		G4Tubs *RS_solid_bt = new G4Tubs("RS_bt_solid", RS_bt_ir, RS_bt_or, RS_bt_length/2, 0., 360*deg);
		G4Tubs *RS_solid_ot = new G4Tubs("RS_ot_solid", RS_ot_ir, RS_ot_or, RS_ot_length/2, 0., 360*deg);
		G4Tubs *RS_solid_ec = new G4Tubs("RS_ec_solid", RS_ec_ir, RS_ec_or, RS_ec_length/2, 0., 360*deg);
		G4LogicalVolume* RS_logic_bt = new G4LogicalVolume(RS_solid_bt, MagFAl, "RS_bt_logic");
		G4LogicalVolume* RS_logic_ot = new G4LogicalVolume(RS_solid_ot, MagFAl, "RS_ot_logic");
		G4LogicalVolume* RS_logic_ec = new G4LogicalVolume(RS_solid_ec, MagFAl, "RS_ec_logic");
		RS_logic_bt->SetVisAttributes(G4VisAttributes::GetInvisible());//SetVisAttributes(visMagF);
		RS_logic_ot->SetVisAttributes(G4VisAttributes::GetInvisible());//SetVisAttributes(visMagF);
		RS_logic_ec->SetVisAttributes(G4VisAttributes::GetInvisible());//SetVisAttributes(visMagF);
		G4ThreeVector RSec1P(0, 0, -RS_bt_length/2 - RS_ec_length/2);
		G4ThreeVector RSec2P(0, 0, +RS_bt_length/2 + RS_ec_length/2);
		auto RS_bt  = new G4PVPlacement(0, ZeroP, RS_logic_bt, "MagF_RS_bt", logicWorld, false, -1, true);
		auto RS_ot  = new G4PVPlacement(0, ZeroP, RS_logic_ot, "MagF_RS_ot", logicWorld, false, -1, true);
		auto RS_ec1 = new G4PVPlacement(0, RSec1P, RS_logic_ec, "MagF_RS_ec1", logicWorld, false, -1, true);
		auto RS_ec2 = new G4PVPlacement(0, RSec2P, RS_logic_ec, "MagF_RS_ec2", logicWorld, false, -1, true);

		//Former
		G4Tubs *Former_solid = new G4Tubs("Former_solid", Former_ir, Former_or, Former_length/2, 0., 360*deg);
		G4LogicalVolume* Former_logic = new G4LogicalVolume(Former_solid, MagFAl, "Former_logic");
		Former_logic->SetVisAttributes(G4VisAttributes::GetInvisible());//SetVisAttributes(visMagF);
		auto Former = new G4PVPlacement(0, ZeroP, Former_logic, "MagF_Former", logicWorld, false, -1, true);
	}//Solenoid frame

	//TPC
	//-------------------------------------------

	G4double tpcZOffset = par -> GetParDouble("zOffset");

	if ( par -> GetParBool("TPCIn") )
	{
		G4double tpcInnerRadius = par -> GetParDouble("TPCrMin");
		G4double tpcOuterRadius = par -> GetParDouble("TPCrMax");
		G4double tpcLength      = par -> GetParDouble("TPCLength");

		G4Tubs *solidTPC = new G4Tubs("TPCSolid", tpcInnerRadius, tpcOuterRadius, .5*tpcLength, 0., 360*deg);
		G4LogicalVolume *logicTPC = new G4LogicalVolume(solidTPC, matGas, "TPCLogic");
		logicTPC -> SetUserLimits(new G4UserLimits(1. * mm));

		G4VisAttributes * visTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
		visTPC->SetForceWireframe(true);
		logicTPC->SetVisAttributes(visTPC);

		G4ThreeVector TPCoffset(0, 0, tpcZOffset);
		auto pvp = new G4PVPlacement(0, TPCoffset, logicTPC, "TPC", logicWorld, false, 10, true);
		runManager -> SetSensitiveDetector(pvp);
	}//TPC

	//B-TOF
	//-------------------------------------------

	G4int btofNum = par -> GetParInt("BTOFnum");
	G4int btofOpt = par -> GetParInt("BTOFopt");

	if ( par -> GetParBool("BTOFIn") )
	{
		G4double btofX = par -> GetParDouble("BTOFlength");
		G4double btofY = par -> GetParDouble("BTOFx");
		G4double btofZ = par -> GetParDouble("BTOFy");

		G4double dphi = 2*M_PI/btofNum, half_dphi = 0.5*dphi;
		G4double cosdphi = cos(half_dphi);
		G4double tandphi = tan(half_dphi);

		G4double radiusIn = 0.5*btofY/tandphi;
		G4double radiusOut = (radiusIn + btofZ)/cosdphi;
		G4cout << "B-TOF Radius In: " << radiusIn << ", RadiusOut: " << radiusOut << G4endl;
		G4double dz = btofX;

		G4Tubs *solidBTOF = new G4Tubs("BTOFSolid", radiusIn, radiusOut, 0.5*dz, 0., 360*deg);
		G4LogicalVolume *logicBTOF = new G4LogicalVolume(solidBTOF, matVac, "BTOFLogic");
		logicBTOF -> SetVisAttributes (G4VisAttributes::GetInvisible());
		//G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
		//attBTOF -> SetForceWireframe(true);
		//logicBTOF -> SetVisAttributes(attBTOF);

		G4Box *solidBTOFScint = new G4Box("BTOFScintillatorSolid", 0.5*btofX, 0.5*btofY, 0.5*btofZ);
		G4LogicalVolume *logicBTOFScint = new G4LogicalVolume(solidBTOFScint, matSC, "BTOFScintillatorLogic");

		G4VisAttributes* attBTOFScint = new G4VisAttributes(G4Colour(G4Colour::Brown()));
		attBTOFScint->SetForceWireframe(true);
		logicBTOFScint->SetVisAttributes(attBTOFScint);

		for (int ii=0; ii<btofNum; ii++)
		{
			if ( btofOpt>0 && abs(ii-36)==btofOpt ) continue;

			G4double phi = ii*2*M_PI/btofNum;
			G4RotationMatrix rotm  = G4RotationMatrix();
			rotm.rotateY(90*deg);
			rotm.rotateZ(phi);

			G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);
			G4ThreeVector position = (radiusIn + 0.5*btofZ)*uz;
			G4Transform3D transform = G4Transform3D(rotm, position);

			new G4PVPlacement(
					transform,      //rotation,position
					logicBTOFScint, //its logical volume
					Form("BTOFScintillator_%02d",ii), //its name
					logicBTOF, //its mother  volume
					false,     //no boolean operation
					2000+ii,   //copy number
					false);    //checking overlaps
		}

		G4ThreeVector BTOFoffset(0, 0, tpcZOffset);
		auto pvp = new G4PVPlacement(0, BTOFoffset, logicBTOF, "BTOF", logicWorld, false, 20, true);
		runManager -> SetSensitiveDetector(pvp);
	}//BTOF

	//F-TOF
	//-------------------------------------------

	G4double ftofX1 = par -> GetParDouble("FTOFx1");
	G4double ftofX2 = par -> GetParDouble("FTOFx2");
	G4double ftofLength = par -> GetParDouble("FTOFlength");
	G4double ftofThickness = par -> GetParDouble("FTOFthickness");
	G4double ftofzOffset = par -> GetParDouble("FTOFzOffset");

	if ( par -> GetParBool("FTOFIn") )
	{
		G4double dphi = 2*M_PI/btofNum, half_dphi = 0.5*dphi;
		//G4double cosdphi = cos(half_dphi);
		G4double tandphi = tan(half_dphi);

		G4double radiusIn = 0.5*ftofX2/tandphi;
		G4double radiusOut = 0.5*ftofX1/tandphi;
		ftofLength = radiusOut - radiusIn;
		G4cout << "F-TOF Radius In: " << radiusIn << ", RadiusOut: " << radiusOut << G4endl;

		G4Tubs *solidFTOF = new G4Tubs("FTOF", radiusIn, radiusOut, 0.5*5, 0., 360*deg);
		G4LogicalVolume *logicFTOF = new G4LogicalVolume(solidFTOF, matVac, "FTOF");
		logicFTOF -> SetVisAttributes (G4VisAttributes::GetInvisible());
		//G4VisAttributes * attFTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
		//attFTOF -> SetForceWireframe(true);
		//logicFTOF -> SetVisAttributes(attFTOF);

		G4Trap *solidFTOFScint = new G4Trap("FTOFScintillator",
				0.5*ftofX1, 0.5*ftofX2, 0.5*ftofThickness, 0.5*ftofThickness, 0.5*ftofLength);
		G4LogicalVolume *logicFTOFScint = new G4LogicalVolume(solidFTOFScint, matSC, "FTOFScintillator");
		G4VisAttributes * attFTOFScint = new G4VisAttributes(G4Colour(G4Colour::Brown()));
		attFTOFScint -> SetForceWireframe(true);
		logicFTOFScint -> SetVisAttributes(attFTOFScint);

		for (int ii=0; ii<btofNum; ii++)
		{
			if ( btofOpt>0 && abs(ii-36)==btofOpt ) continue;

			G4double phi = ii*2*M_PI/btofNum;
			G4RotationMatrix rotm  = G4RotationMatrix();
			rotm.rotateX(90*deg);
			rotm.rotateZ(-90*deg + phi);

			G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);
			G4ThreeVector position = (radiusOut - 0.5*ftofLength)*uz;
			G4Transform3D transform = G4Transform3D(rotm, position);

			new G4PVPlacement(
					transform,      //rotation,position
					logicFTOFScint, //its logical volume
					Form("FTOFScintillator_%02d",ii), //its name
					logicFTOF, //its mother  volume
					false,     //no boolean operation
					3000+ii,   //copy number
					false);    //checking overlaps
		}

		G4ThreeVector FTOFoffset(0, 0, tpcZOffset+ftofzOffset);
		auto pvp = new G4PVPlacement(0, FTOFoffset, logicFTOF, "FTOF", logicWorld, false, 30, true);
		runManager -> SetSensitiveDetector(pvp);
	}//FTOF

	#if 1
	//FT, rectangle w/ hole (simiar to PHENIX MuID)
	//---------------------------------------------

	if ( par -> GetParBool("FTIn") )
	{
		G4int    ftID     = (par->CheckPar("FTID"))?par->GetParInt("FTID"):40;
		G4int    ftLayerN = par->GetParInt("FTLayerN");    //Total # of layers
		G4double ftLayerD = par->GetParDouble("FTLayerD"); //Distance btw each layer
		G4double ftLayerZ = par->GetParDouble("FTLayerZ"); //1st layer's z offset

		G4double ftBoxW  = par->GetParDouble("FTBoxW"); //Width (x)
		G4double ftBoxH  = par->GetParDouble("FTBoxH"); //Height (y)
		G4double ftBoxT  = par->GetParDouble("FTBoxT"); //Thickness (z)
		G4double ftHoleW = par->GetParDouble("FTHoleW");
		G4double ftHoleH = par->GetParDouble("FTHoleH");

		G4ThreeVector ZERO(0, 0, 0);
		G4double totalT = (ftLayerN-1) * ftLayerD + ftBoxT;

		//Volume, mother
		G4Box* FTSolidBox = new G4Box("FTSolidBox", ftBoxW/2., ftBoxH/2., totalT/2.);
		G4Box* FTSolidHole = new G4Box("FTSolidHole", ftHoleW/2., ftHoleH/2., totalT/2.);
		G4SubtractionSolid* FTSolid = new G4SubtractionSolid("FTSolid", FTSolidBox, FTSolidHole, 0, ZERO);
		G4LogicalVolume* FTLogic = new G4LogicalVolume(FTSolid, matVac, "FTLogic");
		FTLogic->SetVisAttributes(G4VisAttributes::GetInvisible());

		//Volume, each unit
		G4Box* FTSolUBox = new G4Box("FTSolidUnitBox", ftBoxW/2., ftBoxH/2., ftBoxT/2.);
		G4Box* FTSolUHole = new G4Box("FTSolidUnitHole", ftHoleW/2., ftHoleH/2., ftBoxT/2.);
		G4SubtractionSolid* FTSolidUnit = new G4SubtractionSolid("FTSolidUnit", FTSolUBox, FTSolUHole, 0, ZERO);
		G4LogicalVolume* FTLogicUnit = new G4LogicalVolume(FTSolidUnit, matSC, "FTLogicUnit");

		G4VisAttributes * FTvis = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		FTvis->SetForceWireframe(true);
		FTLogicUnit->SetVisAttributes(FTvis);

		//Implementation, each unit
		for (int a=0; a<ftLayerN; a++)
		{
			G4ThreeVector ftUnitOfs(0, 0, ftLayerD * a - totalT/2.);
			new G4PVPlacement(0, ftUnitOfs, FTLogicUnit, Form("FTracker_L%d", a), FTLogic, false, 100*ftID+a, false);
		}//a

		//Implementation, mother (whole)
		G4ThreeVector FTOfs(0, 0, ftLayerZ + totalT/2.);
		auto FT = new G4PVPlacement(0, FTOfs, FTLogic, "FTracker", logicWorld, false, ftID, true);
		runManager->SetSensitiveDetector(FT);
	}//FT, rectangle
	#endif

	#if 0
	//FT, octants - DEBUG LATER!
	//-------------------------------------------

	if ( par -> GetParBool("FTIn") )
	{
		//Read parameters: note that all octant geometry decribed WRT an octant in 12 o'clock position
		G4int ftID        = (par->CheckPar("FTID"))?par->GetParInt("FTID"):40;
		G4int ftLayerN    = par->GetParInt("FTLayerN");    //Total # of layers
		G4int ftLayerNSub = par->GetParInt("FTLayerNSub"); //# of sublayer per layer
		G4double ftLayerD = par->GetParDouble("FTLayerD"); //Distance btw each layer
		G4double ftLayerZ = par->GetParDouble("FTLayerZ"); //1st layer's z offset
		G4double ftOctDS  = par->GetParDouble("FTOctDS");  //Dead space at octant's left-right boundary
		G4double ftOctVL  = par->GetParDouble("FTOctVL");  //Vertical length of an octant
		G4double ftOctVC  = par->GetParDouble("FTOctVC");  //Vertical center of an octant
		G4double ftOctThk = par->GetParDouble("FTOctThk"); //Octant's thickness

		//Implementation
		G4double tOctHL = (tan(M_PI/8) * (ftOctVC - ftOctVL/2)) - ftOctDS; //Octant's lower horizontal half length
		G4double tOctHU = (tan(M_PI/8) * (ftOctVC + ftOctVL/2)) - ftOctDS; //Octant's upper horizontal half length

		G4Trap* FTOctSolid = new G4Trap("FTOctSolid");
		FTOctSolid->SetAllParameters(ftOctThk/2, 0, 0, ftOctVL/2,tOctHL,tOctHU, 0, ftOctVL/2,tOctHL,tOctHU, 0);
		G4LogicalVolume* FTOctLogic = new G4LogicalVolume(FTOctSolid, matSC, "FTOctLogic");

		for (int a=0; a<ftLayerN;    a++) //Layers
		for (int b=0; b<8;           b++) //Octants
		for (int c=0; c<ftLayerNSub; c++) //Octant units: for the case of a layer is composed of multiple octants
		{
			char tOctName[100];
			sprintf(tOctName, "FTracker_l%do%du%d", a, b, c);
			//const int tOctId = 1000 + 100 * a + 10 * b + c;

			const double tPhi = M_PI/2 - (M_PI/4) * b; //Start from x axis
			const double tPosX = ftOctVC * sin(tPhi);
			const double tPosY = ftOctVC * cos(tPhi);
			const double tPosZ = ftLayerZ + a*ftLayerD + c*ftOctThk;
			//cout <<tOctName <<" " <<tOctId <<" " <<tPosX <<" " <<tPosY <<" " <<tPosZ <<endl;

			G4ThreeVector tOctPos(tPosX, tPosY, tPosZ);
			G4RotationMatrix* rotm = new G4RotationMatrix(); //Possible memory leak... damnit
			rotm->rotateZ(tPhi);

			auto ftOct = new G4PVPlacement(rotm, tOctPos, FTOctLogic, tOctName, logicWorld, false, ftID, true);
			//runManager->SetSensitiveDetector(ftOct);
		}//a, b, c

		G4VisAttributes * FTvis = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		FTvis->SetForceWireframe(true);
		FTOctLogic->SetVisAttributes(FTvis);
	}//FT
	#endif

	//Neutron
	//-------------------------------------------

	if ( par -> GetParBool("NDIn") )
	{
        G4int    ndID     = (par->CheckPar("NDID"))?par->GetParInt("NDID"):50;
        G4int    ndLayerN = par->GetParInt("NDLayerN");    //Total # of layers
        G4double ndLayerD = par->GetParDouble("NDLayerD"); //Distance btw each layer surface
        G4double ndLayerZ = par->GetParDouble("NDLayerZ"); //1st VETO's z offset

        G4int    ndSlatN = par->GetParInt("NDSlatN");    //# of slats per layer
        G4double ndSlatW = par->GetParDouble("NDSlatW"); //Slat width (x)
        G4double ndSlatH = par->GetParDouble("NDSlatH"); //Slat height (y)
        G4double ndSlatT = par->GetParDouble("NDSlatT"); //Slat thickness (z)
        G4double ndVetoT = par->GetParDouble("NDVetoT"); //Slat thickness (z)

        G4double ndTheta = par->GetParDouble("NDTheta"); //Theta angle of entire ND

        G4ThreeVector ZERO(0, 0, 0);
        G4RotationMatrix* RotZ90 = new G4RotationMatrix();
        RotZ90->rotateZ(-90 * deg);
        G4double ndTotalW = ndSlatN * ndSlatH;
        G4double ndTotalT = ndVetoT + (2*ndSlatT + ndLayerD) * (ndLayerN-1);

        //Volume, mother
        G4Box*           ndSolid = new G4Box("FTSolidBox", ndTotalW/2., ndTotalW/2., ndTotalT/2.);
        G4LogicalVolume* ndLogic = new G4LogicalVolume(ndSolid, matVac, "ndLogic");
        ndLogic->SetVisAttributes(G4VisAttributes::GetInvisible());

        //Volume, units
        G4Box* ndVetoSolid = new G4Box("NDVetoSolid", ndSlatW/2., ndSlatH/2., ndVetoT/2.);
        G4Box* ndSlatSolid = new G4Box("NDSlatSolid", ndSlatW/2., ndSlatH/2., ndSlatT/2.);
        G4LogicalVolume* ndVetoLogic = new G4LogicalVolume(ndVetoSolid, matSC, "NDVetoLogic");
        G4LogicalVolume* ndSlatLogic = new G4LogicalVolume(ndSlatSolid, matSC, "NDSlatLogic");
        G4VisAttributes* ndVis = new G4VisAttributes();
        ndVis->SetColor(G4Color::Magenta());
        ndVis->SetForceWireframe(true);
        ndVetoLogic->SetVisAttributes(ndVis);
        ndSlatLogic->SetVisAttributes(ndVis);

        for (int a=0; a<ndLayerN; a++) //Veto is always layer 0
        for (int b=0; b<2;        b++) //Sublayer
        for (int c=0; c<ndSlatN;  c++)
        {
            if (a==0 && b>0) continue; //No pair sublayer for Veto
            string ndSlatName = Form("ND_l%im%is%i", a, b, c);
            const int slatId = (ndID+a)*100 + ndSlatN*b + c;

            const bool vL = (b==0)?true:false; //Vertical (x sensitive) layer or not
            const float slatCenter = ndSlatH * (0.5 -ndSlatN/2 + c);
            const float ofsX = vL?slatCenter:0;
            const float ofsY = vL?0:slatCenter;
            const float ofsZ = (a==0)?(-ndTotalT/2):(-ndTotalT/2+ndVetoT+ndSlatT/2+ndLayerD*a+ndSlatT*(2*(a-1)+b));
            //cout <<Form("L%i M%i S%2i : %4.0f, %4.0f, %4.0f\n", a, b, c, ofsX, ofsY, ndLayerZ+ndTotalT/2+ofsZ);

            G4ThreeVector ofsSlat(ofsX, ofsY, ofsZ);
            new G4PVPlacement(vL?RotZ90:0, ofsSlat, (a==0)?ndVetoLogic:ndSlatLogic,
                    ndSlatName, ndLogic, false, slatId, false);
        }

				/*
        G4RotationMatrix* RotY = new G4RotationMatrix();
        RotY->rotateY(-ndTheta * deg);
        const float ndOfsX = sin(ndTheta * deg) * (ndLayerZ + ndTotalT/2);
        const float ndOfsZ = cos(ndTheta * deg) * (ndLayerZ + ndTotalT/2);
        G4ThreeVector ndOfs(ndOfsX, 0, ndOfsZ);
        cout <<Form("x: %5.1f, z: %5.1f, zOrig: %5.1f\n",
                ndOfsX, ndOfsZ, sqrt(pow(ndOfsX, 2) + pow(ndOfsZ, 2)) - ndTotalT/2);

        auto ND = new G4PVPlacement(RotY, ndOfs, ndLogic, "ND", logicWorld, false, ndID, true);
				*/
        const float ndOfsX = sin(ndTheta * deg) * (ndLayerZ + ndTotalT/2);
        const float ndOfsZ = cos(ndTheta * deg) * (ndLayerZ + ndTotalT/2);

				G4RotationMatrix rotm  = G4RotationMatrix();
				rotm.rotateY(ndTheta*deg);
				G4Transform3D transform = G4Transform3D(rotm, G4ThreeVector(ndOfsX, 0, ndOfsZ));
				auto ND = new G4PVPlacement(transform, ndLogic, "ND", logicWorld, false, ndID, true);
        runManager->SetSensitiveDetector(ND);

	}//ND

	return physWorld;
}//LHDetectorConstruction::Construct()
