#include "NeuSteppingAction.hh"
#include "NeuEventAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

NeuSteppingAction::NeuSteppingAction(NeuEventAction* event) : fEventAction(event) {}
NeuSteppingAction::~NeuSteppingAction() {}

void NeuSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  // count processes
  const G4StepPoint* endPoint = aStep->GetPostStepPoint();
  const G4VProcess* process = endPoint->GetProcessDefinedStep();
  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->CountProcesses(process);

  // count neutron captures by target element:
  // the residual nucleus (Z=64 excited Gd, Z=1 deuteron, ...) is a secondary of the step
  if (process && process->GetProcessName() == "nCapture") {
    const G4double captureLength = aStep->GetTrack()->GetTrackLength();
    run->RecordCaptureLength(captureLength);
    run->RecordCaptureTime(aStep->GetTrack()->GetGlobalTime());
    run->RecordCaptureRadius(aStep->GetPostStepPoint()->GetPosition().mag());
    G4AnalysisManager::Instance()->FillH1(24, captureLength);
    G4AnalysisManager::Instance()->FillH1(25, aStep->GetTrack()->GetGlobalTime());
    G4AnalysisManager::Instance()->FillH1(26, aStep->GetPostStepPoint()->GetPosition().mag());
    G4int Z = 0;
    if (const auto* secs = aStep->GetSecondary()) {
      for (const auto* sec : *secs) {
        if (sec->GetDefinition()->GetParticleType() == "nucleus")
          Z = sec->GetDefinition()->GetAtomicNumber();
      }
    }
    run->CountCapture(Z);
  }

  // energy deposit
  G4double edepStep = aStep->GetTotalEnergyDeposit();
  if (edepStep <= 0.) return;
  fEventAction->AddEdep(edepStep);

  // longitudinal profile of deposited energy
  G4ThreeVector prePoint = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector postPoint = aStep->GetPostStepPoint()->GetPosition();
  G4ThreeVector point = prePoint + G4UniformRand() * (postPoint - prePoint);
  G4double r = point.mag();
  G4AnalysisManager::Instance()->FillH1(2, r, edepStep);
}
