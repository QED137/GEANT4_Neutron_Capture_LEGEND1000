#include "NeuSteppingAction.hh"
#include "NeuEventAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"

NeuSteppingAction::NeuSteppingAction(NeuEventAction* event) : fEventAction(event) {}
NeuSteppingAction::~NeuSteppingAction() {}

void NeuSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  // count processes
  const G4StepPoint* endPoint = aStep->GetPostStepPoint();
  const G4VProcess* process = endPoint->GetProcessDefinedStep();
  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->CountProcesses(process);
  const auto* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
  if (event && event->GetEventID() != fThermalizationEventId) {
    fThermalizationPathByTrack.clear();
    fThermalizationEventId = event->GetEventID();
  }
  const auto* track = aStep->GetTrack();
  if (track->GetDefinition()->GetParticleName() == "neutron" &&
      track->GetKineticEnergy() <= 1. * eV &&
      fThermalizationPathByTrack.find(track->GetTrackID()) == fThermalizationPathByTrack.end()) {
    fThermalizationPathByTrack[track->GetTrackID()] = track->GetTrackLength();
  }

  // count neutron captures by target element:
  // the residual nucleus (Z=64 excited Gd, Z=1 deuteron, ...) is a secondary of the step
  if (process && process->GetProcessName() == "nCapture") {
    const auto& touchable = aStep->GetPreStepPoint()->GetTouchableHandle();
    const auto* volume = touchable ? touchable->GetVolume() : nullptr;
    run->RecordCaptureVolume(volume ? volume->GetName() : "Unknown");

    const G4double captureLength = aStep->GetTrack()->GetTrackLength();
    run->RecordCaptureLength(captureLength);
    run->RecordCaptureTime(aStep->GetTrack()->GetGlobalTime());
    run->RecordCaptureRadius(aStep->GetPostStepPoint()->GetPosition().mag());
    G4AnalysisManager::Instance()->FillH1(24, captureLength);
    G4AnalysisManager::Instance()->FillH1(25, aStep->GetTrack()->GetGlobalTime());
    G4AnalysisManager::Instance()->FillH1(26, aStep->GetPostStepPoint()->GetPosition().mag());
    G4int Z = 0;
    G4int A = 0;
    G4double gammaEnergy = 0.;
    G4int gammaMultiplicity = 0;
    if (const auto* secs = aStep->GetSecondary()) {
      for (const auto* sec : *secs) {
        const auto* definition = sec->GetDefinition();
        if (definition->GetParticleType() == "nucleus") {
          Z = sec->GetDefinition()->GetAtomicNumber();
          A = sec->GetDefinition()->GetAtomicMass();
        }
        if (definition->GetParticleName() == "gamma") {
          gammaEnergy += sec->GetKineticEnergy();
          ++gammaMultiplicity;
        }
      }
    }
    const G4ThreeVector position = aStep->GetPostStepPoint()->GetPosition();
    const auto* logicalVolume = volume ? volume->GetLogicalVolume() : nullptr;
    const G4String materialName =
      logicalVolume && logicalVolume->GetMaterial()
        ? logicalVolume->GetMaterial()->GetName()
        : "Unknown";
    const auto thermalization = fThermalizationPathByTrack.find(track->GetTrackID());
    const G4double thermalizationPath =
      thermalization == fThermalizationPathByTrack.end()
        ? -1.
        : thermalization->second;
    auto* analysis = G4AnalysisManager::Instance();
    if (analysis->GetNofNtuples() == 0) {
      G4Exception("NeuSteppingAction::UserSteppingAction", "MissingCaptureNtuple",
                  FatalException, "The capture ntuple is not available in this worker.");
    }
    analysis->FillNtupleIColumn(0, 0, event ? event->GetEventID() : -1);
    analysis->FillNtupleSColumn(0, 1, volume ? volume->GetName() : "Unknown");
    analysis->FillNtupleSColumn(0, 2, materialName);
    analysis->FillNtupleIColumn(0, 3, Z);
    analysis->FillNtupleIColumn(0, 4, A);
    analysis->FillNtupleDColumn(0, 5, position.x() / cm);
    analysis->FillNtupleDColumn(0, 6, position.y() / cm);
    analysis->FillNtupleDColumn(0, 7, position.z() / cm);
    analysis->FillNtupleDColumn(0, 8, position.mag() / cm);
    analysis->FillNtupleDColumn(0, 9, aStep->GetTrack()->GetGlobalTime() / microsecond);
    analysis->FillNtupleDColumn(0, 10, captureLength / cm);
    analysis->FillNtupleDColumn(0, 11, gammaEnergy / MeV);
    analysis->FillNtupleIColumn(0, 12, gammaMultiplicity);
    analysis->FillNtupleDColumn(0, 13, thermalizationPath / cm);
    analysis->AddNtupleRow(0);
    const G4String isotope = "Z" + std::to_string(Z) + "A" + std::to_string(A);
    run->CountCapture(Z, A);
    run->RecordCaptureLengthByIsotope(isotope, captureLength);
    run->RecordCaptureGamma(gammaEnergy, gammaMultiplicity);
    if (gammaMultiplicity > 0) {
      G4AnalysisManager::Instance()->FillH1(27, gammaEnergy);
      G4AnalysisManager::Instance()->FillH1(28, gammaMultiplicity);
    }
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
