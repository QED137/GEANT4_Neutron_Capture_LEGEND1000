#include "TrackingMessenger.hh"

#include "NeuTrackingAction.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingMessenger::TrackingMessenger(NeuTrackingAction* trackA) : fTrackingAction(trackA)
{
  fTrackingDir = new G4UIdirectory("/testhadr/tracking/");
  fTrackingDir->SetGuidance("tracking commands");

  fCountCmd = new G4UIcmdWithABool("/testhadr/tracking/countParticles", this);
  fCountCmd->SetGuidance("count created particles");
  fCountCmd->SetParameterName("flag", false);
  fCountCmd->SetDefaultValue(true);
  fCountCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fKillCmd = new G4UIcmdWithABool("/testhadr/tracking/killNeutrons", this);
  fKillCmd->SetGuidance("kill secondaries neutrons");
  fKillCmd->SetParameterName("flag", false);
  fKillCmd->SetDefaultValue(false);
  fKillCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingMessenger::~TrackingMessenger()
{
  delete fCountCmd;
  delete fKillCmd;
  delete fTrackingDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fCountCmd) {
    fTrackingAction->SetParticleCount(fCountCmd->GetNewBoolValue(newValue));
  }

  if (command == fKillCmd) {
    fTrackingAction->SetKillNeutrons(fKillCmd->GetNewBoolValue(newValue));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
