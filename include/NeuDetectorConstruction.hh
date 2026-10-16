#ifndef Neu_DetectorConstruction_hh
#define Neu_DetectorConstruction_hh

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

class G4Material;
class G4Element;
class G4LogicalVolume;
class G4VPhysicalVolume;
class DetectorMessenger;

class NeuDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  NeuDetectorConstruction();
  ~NeuDetectorConstruction() override;

  G4VPhysicalVolume* Construct() override;

  G4Material* MaterialWithSingleIsotope(G4String, G4String, G4double, G4int, G4int);
  void SetRadius(G4double);
  void SetTankRadius(G4double);
  void SetTankHeight(G4double);
  void SetCryostatRadius(G4double);
  void SetCryostatHeight(G4double);
  void SetMaterial(G4String);
  void SetGdFraction(G4double);

  G4double GetRadius() const { return fRadius; }
  G4Material* GetMaterial() const { return fMaterial; }

  void PrintParameters();

private:
  void DefineMaterials();
  G4VPhysicalVolume* ConstructVolumes();
  void DefineGdMaterial(G4double);

  // Simplified GERDA-like dimensions: 10 m diameter and 9 m water height.
  G4double fTankRadius = 5. * m;
  G4double fTankHeight = 9. * m;
  G4double fCryostatRadius = 1.5 * m;
  G4double fCryostatHeight = 3. * m;
  G4double fRadius = fTankRadius;            // legacy spherical-geometry alias
  G4Material* fMaterial = nullptr;
  G4LogicalVolume* fLAbsor = nullptr;
  G4double fWorldSize = 1.1 * fTankRadius;
  G4Material* fWorldMat = nullptr;
  G4Material* fSteel = nullptr;
  G4Material* fLiquidArgon = nullptr;
  G4Element* fHydrogen = nullptr;
  G4Element* fOxygen = nullptr;
  G4VPhysicalVolume* fPWorld = nullptr;
  DetectorMessenger* fDetectorMessenger = nullptr;
};

#endif
