#include "HadronElasticPhysicsHP.hh"

#include "G4GenericMessenger.hh"
#include "G4HadronicProcess.hh"
#include "G4ParticleHPElastic.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPThermalScattering.hh"
#include "G4ParticleHPThermalScatteringData.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HadronElasticPhysicsHP::HadronElasticPhysicsHP(G4int ver) : G4HadronElasticPhysics(ver)
{
  // define commands for this class
  DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HadronElasticPhysicsHP::~HadronElasticPhysicsHP()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HadronElasticPhysicsHP::ConstructProcess()
{
  G4HadronElasticPhysics::ConstructProcess();
  GetNeutronModel()->SetMinEnergy(19.5 * MeV);

  G4HadronicProcess* process = GetNeutronProcess();
  G4ParticleHPElastic* model1 = new G4ParticleHPElastic();
  process->RegisterMe(model1);
  process->AddDataSet(new G4ParticleHPElasticData());

  if (fThermal) {
    model1->SetMinEnergy(4 * eV);
    G4ParticleHPThermalScattering* model2 = new G4ParticleHPThermalScattering();
    process->RegisterMe(model2);
    process->AddDataSet(new G4ParticleHPThermalScatteringData());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HadronElasticPhysicsHP::DefineCommands()
{
  // Define /testhadr/phys command directory using generic messenger class
  fMessenger = new G4GenericMessenger(this, "/testhadr/phys/", "physics list commands");

  // thermal scattering command
  auto& thermalCmd = fMessenger->DeclareProperty("thermalScattering", fThermal);

  thermalCmd.SetGuidance("set thermal scattering model");
  thermalCmd.SetParameterName("thermal", false);
  thermalCmd.SetDefaultValue("false");
  thermalCmd.SetStates(G4State_PreInit);
}

