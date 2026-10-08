#ifndef DetectorMessenger_hh
#define DetectorMessenger_hh

#include "G4UImessenger.hh"

class NeuDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcommand;

class DetectorMessenger : public G4UImessenger
{
public:
  explicit DetectorMessenger(NeuDetectorConstruction*);
  ~DetectorMessenger() override;

  void SetNewValue(G4UIcommand*, G4String) override;

private:
  NeuDetectorConstruction* fDetector = nullptr;
  G4UIdirectory* fTestemDir = nullptr;
  G4UIdirectory* fDetDir = nullptr;
  G4UIcmdWithAString* fMaterCmd = nullptr;
  G4UIcmdWithADoubleAndUnit* fSizeCmd = nullptr;
  G4UIcmdWithADouble* fGdFractionCmd = nullptr;
  G4UIcommand* fIsotopeCmd = nullptr;
};

#endif
