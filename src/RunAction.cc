#include "RunAction.hh"

#include "NeuDetectorConstruction.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "Run.hh"

#include "G4AnalysisManager.hh"
#include "G4ParticleGun.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

RunAction::RunAction(NeuDetectorConstruction* det, PrimaryGeneratorAction* prim)
  : fDetector(det), fPrimary(prim)
{
  fHistoManager = new HistoManager();
}

RunAction::~RunAction()
{
  delete fHistoManager;
}

G4Run* RunAction::GenerateRun()
{
  fRun = new Run(fDetector);
  return fRun;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  if (isMaster) G4Random::showEngineStatus();

  if (fPrimary) {
    G4ParticleDefinition* particle = fPrimary->GetParticleGun()->GetParticleDefinition();
    G4double energy = fPrimary->GetParticleGun()->GetParticleEnergy();
    fRun->SetPrimary(particle, energy);
  }

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->IsActive()) {
    analysisManager->OpenFile();
  }
}

void RunAction::EndOfRunAction(const G4Run*)
{
  if (isMaster) fRun->EndOfRun();

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->IsActive()) {
    analysisManager->Write();
    analysisManager->CloseFile();
  }

  if (isMaster) G4Random::showEngineStatus();
}
