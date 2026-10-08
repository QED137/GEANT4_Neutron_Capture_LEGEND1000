#include "NeuTrackingAction.hh"
#include "NeuEventAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4AnalysisManager.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4StepStatus.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"

NeuTrackingAction::NeuTrackingAction(NeuEventAction* event) : fEventAction(event) {}
NeuTrackingAction::~NeuTrackingAction() {}

void NeuTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // count secondary particles at creation
  if (track->GetTrackID() == 1) return;

  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  const G4ParticleDefinition* particle = track->GetParticleDefinition();
  G4String name = particle->GetParticleName();
  G4double meanLife = particle->GetPDGLifeTime();
  G4double energy = track->GetKineticEnergy();
  run->ParticleCount(name, energy, meanLife);

  // histograms: energy at creation
  G4AnalysisManager* analysis = G4AnalysisManager::Instance();

  G4int ih = 0;
  G4String type = particle->GetParticleType();
  G4double charge = particle->GetPDGCharge();
  if (charge > 3.)
    ih = 10;
  else if (particle == G4Gamma::Gamma())
    ih = 4;
  else if (particle == G4Electron::Electron())
    ih = 5;
  else if (particle == G4Positron::Positron())
    ih = 5;
  else if (particle == G4Neutron::Neutron())
    ih = 6;
  else if (particle == G4Proton::Proton())
    ih = 7;
  else if (particle == G4Deuteron::Deuteron())
    ih = 8;
  else if (particle == G4Alpha::Alpha())
    ih = 9;
  else if (type == "nucleus")
    ih = 10;
  else if (type == "baryon")
    ih = 11;
  else if (type == "meson")
    ih = 12;
  else if (type == "lepton")
    ih = 13;
  if (ih > 0) analysis->FillH1(ih, energy);
}

void NeuTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // keep only particles emerging from the absorber
  const G4Step* step = track->GetStep();
  if (!step) return;  // robustness: track killed before its first step

  G4StepStatus status = step->GetPostStepPoint()->GetStepStatus();
  if (status != fWorldBoundary) return;

  const G4ParticleDefinition* particle = track->GetParticleDefinition();
  G4String name = particle->GetParticleName();
  G4double energy = track->GetKineticEnergy();

  fEventAction->AddEflow(energy);

  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->ParticleFlux(name, energy);

  // histograms: energy at exit
  G4AnalysisManager* analysis = G4AnalysisManager::Instance();

  G4int ih = 0;
  G4String type = particle->GetParticleType();
  G4double charge = particle->GetPDGCharge();
  if (charge > 3.)
    ih = 20;
  else if (particle == G4Gamma::Gamma())
    ih = 14;
  else if (particle == G4Electron::Electron())
    ih = 15;
  else if (particle == G4Positron::Positron())
    ih = 15;
  else if (particle == G4Neutron::Neutron())
    ih = 16;
  else if (particle == G4Proton::Proton())
    ih = 17;
  else if (particle == G4Deuteron::Deuteron())
    ih = 18;
  else if (particle == G4Alpha::Alpha())
    ih = 19;
  else if (type == "nucleus")
    ih = 20;
  else if (type == "baryon")
    ih = 21;
  else if (type == "meson")
    ih = 22;
  else if (type == "lepton")
    ih = 23;
  if (ih > 0) analysis->FillH1(ih, energy);
}
