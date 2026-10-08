#include "NeuEventAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

NeuEventAction::NeuEventAction() {}
NeuEventAction::~NeuEventAction() {}

void NeuEventAction::BeginOfEventAction(const G4Event*)
{
  fTotalEnergyDeposit = 0.;
  fTotalEnergyFlow = 0.;
}

void NeuEventAction::AddEdep(G4double Edep) { fTotalEnergyDeposit += Edep; }
void NeuEventAction::AddEflow(G4double Eflow) { fTotalEnergyFlow += Eflow; }

void NeuEventAction::EndOfEventAction(const G4Event*)
{
  Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  G4double totalEnergy = fTotalEnergyDeposit + fTotalEnergyFlow;
  run->SumEnergies(fTotalEnergyDeposit, fTotalEnergyFlow, totalEnergy);

  G4AnalysisManager::Instance()->FillH1(1, fTotalEnergyDeposit);
  G4AnalysisManager::Instance()->FillH1(3, fTotalEnergyFlow);
  G4AnalysisManager::Instance()->FillH1(0, totalEnergy);
}
