#include "NeuPhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonPhysicsXS.hh"
#include "G4NuclideTable.hh"
#include "G4OpticalPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

NeuPhysicsList::NeuPhysicsList() : G4VModularPhysicsList()
{
  G4int verb = 1;
  SetVerboseLevel(verb);

  new G4UnitDefinition("mm2/g", "mm2/g", "Surface/Mass", mm2 / g);
  new G4UnitDefinition("um2/mg", "um2/mg", "Surface/Mass", um * um / mg);

  const G4double meanLife = 1 * nanosecond, halfLife = meanLife * std::log(2);
  G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(halfLife);

  // NOTE: thermal neutron scattering (S(alpha,beta)) is included via
  // G4HadronElasticPhysicsHP and is active because the water uses the
  // TS_H_of_Water / TS_D_of_Heavy_Water element names.
  RegisterPhysics(new G4HadronElasticPhysicsHP(verb));
  RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP(verb));
  RegisterPhysics(new G4IonElasticPhysics(verb));
  RegisterPhysics(new G4IonPhysicsXS(verb));
  RegisterPhysics(new G4StoppingPhysics(verb));
  RegisterPhysics(new G4EmExtraPhysics(verb));           // gamma-nuclear, etc.
  RegisterPhysics(new G4EmStandardPhysics_option3());
  RegisterPhysics(new G4OpticalPhysics());
  RegisterPhysics(new G4DecayPhysics());
  RegisterPhysics(new G4RadioactiveDecayPhysics());
}

NeuPhysicsList::~NeuPhysicsList() {}

void NeuPhysicsList::SetCuts()
{
  SetCutValue(0 * mm, "proton");
  SetCutValue(10 * km, "e-");
  SetCutValue(10 * km, "e+");
  SetCutValue(10 * km, "gamma");
}
