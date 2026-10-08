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
