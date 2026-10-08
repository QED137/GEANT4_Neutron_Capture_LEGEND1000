#include "NeuDetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Element.hh"
#include "G4GeometryManager.hh"
#include "G4Isotope.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SolidStore.hh"
#include "G4Sphere.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"

NeuDetectorConstruction::NeuDetectorConstruction()
{
  DefineMaterials();
  SetMaterial("Water_ts");
  fDetectorMessenger = new DetectorMessenger(this);
}

NeuDetectorConstruction::~NeuDetectorConstruction()
{
  delete fDetectorMessenger;
}

G4VPhysicalVolume* NeuDetectorConstruction::Construct()
{
  return ConstructVolumes();
}

void NeuDetectorConstruction::DefineMaterials()
{
  G4int ncomponents, natoms;

  // pressurized water, with thermal-scattering hydrogen (neutronHP)
  G4Element* H = new G4Element("TS_H_of_Water", "H", 1., 1.0079 * g / mole);
  G4Element* O = new G4Element("Oxygen", "O", 8., 16.00 * g / mole);
  fHydrogen = H;
  fOxygen = O;
  G4Material* H2O = new G4Material("Water_ts", 1.000 * g / cm3, ncomponents = 2,
                                   kStateLiquid, 593 * kelvin, 150 * bar);
  H2O->AddElement(H, natoms = 2);
  H2O->AddElement(O, natoms = 1);
  H2O->GetIonisation()->SetMeanExcitationEnergy(78.0 * eV);

  DefineGdMaterial(0.001);

  // heavy water
  G4Isotope* H2 = new G4Isotope("H2", 1, 2);
  G4Element* D = new G4Element("TS_D_of_Heavy_Water", "D", 1);
  D->AddIsotope(H2, 100 * perCent);
  G4Material* D2O = new G4Material("HeavyWater", 1.11 * g / cm3, ncomponents = 2,
                                   kStateLiquid, 293.15 * kelvin, 1 * atmosphere);
  D2O->AddElement(D, natoms = 2);
  D2O->AddElement(O, natoms = 1);

  // graphite
  G4Isotope* C12 = new G4Isotope("C12", 6, 12);
  G4Element* C = new G4Element("TS_C_of_Graphite", "C", ncomponents = 1);
  C->AddIsotope(C12, 100. * perCent);
  G4Material* graphite = new G4Material("graphite", 2.27 * g / cm3, ncomponents = 1,
                                        kStateSolid, 293 * kelvin, 1 * atmosphere);
  graphite->AddElement(C, natoms = 1);

  // NE213
  G4Material* ne213 = new G4Material("NE213", 0.874 * g / cm3, ncomponents = 2);
  ne213->AddElement(H, 9.2 * perCent);
  ne213->AddElement(C, 90.8 * perCent);

  // vacuum world
  fWorldMat = new G4Material("Galactic", 1, 1.01 * g / mole, universe_mean_density,
                             kStateGas, 2.73 * kelvin, 3.e-18 * pascal);
}

void NeuDetectorConstruction::DefineGdMaterial(G4double gdFraction)
{
  G4Element* Gd = new G4Element("Gadolinium_scan", "Gd", 64., 157.25 * g / mole);
  const G4String name = gdFraction == 0.001
                          ? "WaterGd_ts"
                          : "WaterGd_scan_" + G4UIcommand::ConvertToString(gdFraction);
  G4Material* material = new G4Material(name, (1. + gdFraction) * g / cm3, 3,
                                         kStateLiquid, 593 * kelvin, 150 * bar);
  material->AddElement(fHydrogen, 0.1119 * (1. - gdFraction));
  material->AddElement(fOxygen, 0.8881 * (1. - gdFraction));
  material->AddElement(Gd, gdFraction);
}

G4Material* NeuDetectorConstruction::MaterialWithSingleIsotope(G4String name, G4String symbol,
                                                               G4double density, G4int Z, G4int A)
{
  G4int ncomponents;
  G4Isotope* isotope = new G4Isotope(symbol, Z, A);
  G4Element* element = new G4Element(name, symbol, ncomponents = 1);
  element->AddIsotope(isotope, 100. * perCent);
  G4Material* material = new G4Material(name, density, ncomponents = 1);
  material->AddElement(element, 100. * perCent);
  return material;
}

G4VPhysicalVolume* NeuDetectorConstruction::ConstructVolumes()
{
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // world
  G4Box* sWorld = new G4Box("World", fWorldSize, fWorldSize, fWorldSize);
  G4LogicalVolume* lWorld = new G4LogicalVolume(sWorld, fWorldMat, "World");
  fPWorld = new G4PVPlacement(0, G4ThreeVector(), lWorld, "World", 0, false, 0);

  // absorber: full sphere of fRadius (default 30 cm) of the selected material
  G4Sphere* sAbsor = new G4Sphere("Absorber", 0., fRadius, 0., twopi, 0., pi);
  fLAbsor = new G4LogicalVolume(sAbsor, fMaterial, fMaterial->GetName());
  new G4PVPlacement(0, G4ThreeVector(), fLAbsor, fMaterial->GetName(), lWorld, false, 0);

  PrintParameters();
  return fPWorld;
}

void NeuDetectorConstruction::PrintParameters()
{
  G4cout << "\n The Absorber is " << G4BestUnit(fRadius, "Length") << " of "
         << fMaterial->GetName() << "\n \n" << fMaterial << G4endl;
}

void NeuDetectorConstruction::SetMaterial(G4String materialChoice)
{
  // checks the material table first, so hand-built materials (Water_ts,
  // WaterGd_ts, Li7, ...) are found as well as NIST ones
  G4Material* pttoMaterial = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial) {
    fMaterial = pttoMaterial;
    if (fLAbsor) fLAbsor->SetMaterial(fMaterial);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  }
  else {
    G4cout << "\n--> warning from DetectorConstruction::SetMaterial : " << materialChoice
           << " not found" << G4endl;
  }
}

void NeuDetectorConstruction::SetGdFraction(G4double fraction)
{
  if (fraction == 0.) {
    SetMaterial("Water_ts");
    return;
  }
  DefineGdMaterial(fraction);
  const G4String name = fraction == 0.001
                          ? "WaterGd_ts"
                          : "WaterGd_scan_" + G4UIcommand::ConvertToString(fraction);
  SetMaterial(name);
}

void NeuDetectorConstruction::SetRadius(G4double value)
{
  fRadius = value;
  fWorldSize = 1.1 * fRadius;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
