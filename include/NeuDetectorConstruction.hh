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
  void SetMaterial(G4String);
  void SetGdFraction(G4double);

  G4double GetRadius() const { return fRadius; }
  G4Material* GetMaterial() const { return fMaterial; }

  void PrintParameters();

private:
  void DefineMaterials();
  G4VPhysicalVolume* ConstructVolumes();
  void DefineGdMaterial(G4double);

  G4double fRadius = 30. * cm;              // <-- your geometry: 30 cm sphere
  G4Material* fMaterial = nullptr;
  G4LogicalVolume* fLAbsor = nullptr;
  G4double fWorldSize = 1.1 * fRadius;      // <-- world = 1.1 x radius
  G4Material* fWorldMat = nullptr;
  G4Element* fHydrogen = nullptr;
  G4Element* fOxygen = nullptr;
  G4VPhysicalVolume* fPWorld = nullptr;
  DetectorMessenger* fDetectorMessenger = nullptr;
};

#endif
