#ifndef GammaNuclearPhysics_h
#define GammaNuclearPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class GammaNuclearPhysics : public G4VPhysicsConstructor
{
  public:
    GammaNuclearPhysics(const G4String& name = "gamma");
    ~GammaNuclearPhysics() override = default;

  public:
    void ConstructParticle() override {};
    void ConstructProcess() override;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

