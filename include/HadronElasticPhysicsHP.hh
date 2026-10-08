#ifndef HadronElasticPhysicsHP_h
#define HadronElasticPhysicsHP_h 1

#include "G4HadronElasticPhysics.hh"
#include "globals.hh"

class G4GenericMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HadronElasticPhysicsHP : public G4HadronElasticPhysics
{
  public:
    HadronElasticPhysicsHP(G4int ver = 1);
    ~HadronElasticPhysicsHP() override;

  public:
    void ConstructProcess() override;

  private:
    void DefineCommands();

    G4GenericMessenger* fMessenger = nullptr;
    G4bool fThermal = false;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

