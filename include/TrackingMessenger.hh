#ifndef TrackingMessenger_h
#define TrackingMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class NeuTrackingAction;
class G4UIdirectory;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TrackingMessenger : public G4UImessenger
{
  public:
    TrackingMessenger(NeuTrackingAction*);
    ~TrackingMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    NeuTrackingAction* fTrackingAction = nullptr;

    G4UIdirectory* fTrackingDir = nullptr;
    G4UIcmdWithABool* fCountCmd = nullptr;
    G4UIcmdWithABool* fKillCmd = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
