#include "GammaNuclearPhysicsLEND.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

// Processes

#include "G4HadronInelasticProcess.hh"
#include "G4LENDCombinedCrossSection.hh"
#include "G4LENDorBERTModel.hh"
#include "G4PhotoNuclearCrossSection.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GammaNuclearPhysicsLEND::GammaNuclearPhysicsLEND(const G4String& name) : G4VPhysicsConstructor(name)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GammaNuclearPhysicsLEND::ConstructProcess()
{
  G4ProcessManager* pManager = G4Gamma::Gamma()->GetProcessManager();
  //
  G4HadronInelasticProcess* process =
    new G4HadronInelasticProcess("photonNuclear", G4Gamma::Definition());
  process->AddDataSet(new G4PhotoNuclearCrossSection);
  //
  G4LENDorBERTModel* lend = new G4LENDorBERTModel(G4Gamma::Gamma());
  lend->SetMaxEnergy(20 * MeV);
  process->RegisterMe(lend);
  //
  G4LENDCombinedCrossSection* lendXS = new G4LENDCombinedCrossSection(G4Gamma::Gamma());
  process->AddDataSet(lendXS);
  //
  pManager->AddDiscreteProcess(process);
}

