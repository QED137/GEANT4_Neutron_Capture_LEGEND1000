#include "DetectorMessenger.hh"
#include "NeuDetectorConstruction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"

DetectorMessenger::DetectorMessenger(NeuDetectorConstruction* Det) : fDetector(Det)
{
  fTestemDir = new G4UIdirectory("/testhadr/");
  fTestemDir->SetGuidance("commands specific to this example");

  G4bool broadcast = false;
  fDetDir = new G4UIdirectory("/testhadr/det/", broadcast);
  fDetDir->SetGuidance("detector construction commands");

  fMaterCmd = new G4UIcmdWithAString("/testhadr/det/setMat", this);
  fMaterCmd->SetGuidance("Select material of the absorber.");
  fMaterCmd->SetParameterName("choice", false);
  fMaterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fSizeCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setRadius", this);
  fSizeCmd->SetGuidance("Set radius of the sphere");
  fSizeCmd->SetParameterName("Size", false);
  fSizeCmd->SetRange("Size>0.");
  fSizeCmd->SetUnitCategory("Length");
  fSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fTankRadiusCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setTankRadius", this);
  fTankRadiusCmd->SetGuidance("Set cylindrical water-tank radius.");
  fTankRadiusCmd->SetParameterName("radius", false);
  fTankRadiusCmd->SetRange("radius>0.");
  fTankRadiusCmd->SetUnitCategory("Length");
  fTankRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fTankHeightCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setTankHeight", this);
  fTankHeightCmd->SetGuidance("Set cylindrical water-tank height.");
  fTankHeightCmd->SetParameterName("height", false);
  fTankHeightCmd->SetRange("height>0.");
  fTankHeightCmd->SetUnitCategory("Length");
  fTankHeightCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fCryostatRadiusCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setCryostatRadius", this);
  fCryostatRadiusCmd->SetGuidance("Set central cryostat outer radius.");
  fCryostatRadiusCmd->SetParameterName("radius", false);
  fCryostatRadiusCmd->SetRange("radius>0.");
  fCryostatRadiusCmd->SetUnitCategory("Length");
  fCryostatRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fCryostatHeightCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setCryostatHeight", this);
  fCryostatHeightCmd->SetGuidance("Set central cryostat height.");
  fCryostatHeightCmd->SetParameterName("height", false);
  fCryostatHeightCmd->SetRange("height>0.");
  fCryostatHeightCmd->SetUnitCategory("Length");
  fCryostatHeightCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fPlateMaterialCmd = new G4UIcmdWithAString("/testhadr/det/setPlateMaterial", this);
  fPlateMaterialCmd->SetGuidance("Set plate material: Gd, Water_ts, G4_STAINLESS-STEEL, or G4_Pb.");
  fPlateMaterialCmd->SetParameterName("material", false);
  fPlateMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fPlateDistanceCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setPlateDistance", this);
  fPlateDistanceCmd->SetGuidance("Set plate distance from the cryostat outer surface.");
  fPlateDistanceCmd->SetParameterName("distance", false);
  fPlateDistanceCmd->SetRange("distance>=0.");
  fPlateDistanceCmd->SetUnitCategory("Length");
  fPlateDistanceCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fPlateWidthCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setPlateWidth", this);
  fPlateWidthCmd->SetGuidance("Set rectangular plate width.");
  fPlateWidthCmd->SetParameterName("width", false);
  fPlateWidthCmd->SetRange("width>0.");
  fPlateWidthCmd->SetUnitCategory("Length");
  fPlateWidthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fPlateHeightCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setPlateHeight", this);
  fPlateHeightCmd->SetGuidance("Set rectangular plate height.");
  fPlateHeightCmd->SetParameterName("height", false);
  fPlateHeightCmd->SetRange("height>0.");
  fPlateHeightCmd->SetUnitCategory("Length");
  fPlateHeightCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fPlateThicknessCmd = new G4UIcmdWithADoubleAndUnit("/testhadr/det/setPlateThickness", this);
  fPlateThicknessCmd->SetGuidance("Set rectangular plate thickness.");
  fPlateThicknessCmd->SetParameterName("thickness", false);
  fPlateThicknessCmd->SetRange("thickness>0.");
  fPlateThicknessCmd->SetUnitCategory("Length");
  fPlateThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fPlateEnabledCmd = new G4UIcmdWithAString("/testhadr/det/setPlateEnabled", this);
  fPlateEnabledCmd->SetGuidance("Enable or disable the rectangular plate: true or false.");
  fPlateEnabledCmd->SetParameterName("enabled", false);
  fPlateEnabledCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fGdFractionCmd = new G4UIcmdWithADouble("/testhadr/det/setGdFraction", this);
  fGdFractionCmd->SetGuidance("Set natural Gd mass fraction in water (0 to 1).");
  fGdFractionCmd->SetParameterName("fraction", false);
  fGdFractionCmd->SetRange("fraction>=0. && fraction<=1.");
  fGdFractionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fIsotopeCmd = new G4UIcommand("/testhadr/det/setIsotopeMat", this);
  fIsotopeCmd->SetGuidance("Build and select a material with single isotope");
  fIsotopeCmd->SetGuidance("  symbol of isotope, Z, A, density of material");

  G4UIparameter* symbPrm = new G4UIparameter("isotope", 's', false);
  symbPrm->SetGuidance("isotope symbol");
  fIsotopeCmd->SetParameter(symbPrm);

  G4UIparameter* ZPrm = new G4UIparameter("Z", 'i', false);
  ZPrm->SetGuidance("Z");
  ZPrm->SetParameterRange("Z>0");
  fIsotopeCmd->SetParameter(ZPrm);

  G4UIparameter* APrm = new G4UIparameter("A", 'i', false);
  APrm->SetGuidance("A");
  APrm->SetParameterRange("A>0");
  fIsotopeCmd->SetParameter(APrm);

  G4UIparameter* densityPrm = new G4UIparameter("density", 'd', false);
  densityPrm->SetGuidance("density of material");
  densityPrm->SetParameterRange("density>0.");
  fIsotopeCmd->SetParameter(densityPrm);

  G4UIparameter* unitPrm = new G4UIparameter("unit", 's', false);
  unitPrm->SetGuidance("unit of density");
  G4String unitList = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("g/cm3"));
  unitPrm->SetParameterCandidates(unitList);
  fIsotopeCmd->SetParameter(unitPrm);

  fIsotopeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete fMaterCmd;
  delete fSizeCmd;
  delete fTankRadiusCmd;
  delete fTankHeightCmd;
  delete fCryostatRadiusCmd;
  delete fCryostatHeightCmd;
  delete fPlateMaterialCmd;
  delete fPlateDistanceCmd;
  delete fPlateWidthCmd;
  delete fPlateHeightCmd;
  delete fPlateThicknessCmd;
  delete fPlateEnabledCmd;
  delete fGdFractionCmd;
  delete fIsotopeCmd;
  delete fDetDir;
  delete fTestemDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fMaterCmd) {
    fDetector->SetMaterial(newValue);
  }
  if (command == fSizeCmd) {
    fDetector->SetRadius(fSizeCmd->GetNewDoubleValue(newValue));
  }
  if (command == fTankRadiusCmd) {
    fDetector->SetTankRadius(fTankRadiusCmd->GetNewDoubleValue(newValue));
  }
  if (command == fTankHeightCmd) {
    fDetector->SetTankHeight(fTankHeightCmd->GetNewDoubleValue(newValue));
  }
  if (command == fCryostatRadiusCmd) {
    fDetector->SetCryostatRadius(fCryostatRadiusCmd->GetNewDoubleValue(newValue));
  }
  if (command == fCryostatHeightCmd) {
    fDetector->SetCryostatHeight(fCryostatHeightCmd->GetNewDoubleValue(newValue));
  }
  if (command == fPlateMaterialCmd) fDetector->SetPlateMaterial(newValue);
  if (command == fPlateDistanceCmd)
    fDetector->SetPlateDistance(fPlateDistanceCmd->GetNewDoubleValue(newValue));
  if (command == fPlateWidthCmd)
    fDetector->SetPlateWidth(fPlateWidthCmd->GetNewDoubleValue(newValue));
  if (command == fPlateHeightCmd)
    fDetector->SetPlateHeight(fPlateHeightCmd->GetNewDoubleValue(newValue));
  if (command == fPlateThicknessCmd)
    fDetector->SetPlateThickness(fPlateThicknessCmd->GetNewDoubleValue(newValue));
  if (command == fPlateEnabledCmd)
    fDetector->SetPlateEnabled(newValue == "true" || newValue == "1");
  if (command == fGdFractionCmd) {
    fDetector->SetGdFraction(fGdFractionCmd->GetNewDoubleValue(newValue));
  }
  if (command == fIsotopeCmd) {
    G4int Z, A;
    G4double dens;
    G4String name, unt;
    std::istringstream is(newValue);
    is >> name >> Z >> A >> dens >> unt;
    dens *= G4UIcommand::ValueOf(unt);
    fDetector->MaterialWithSingleIsotope(name, name, dens, Z, A);
    fDetector->SetMaterial(name);
  }
}
