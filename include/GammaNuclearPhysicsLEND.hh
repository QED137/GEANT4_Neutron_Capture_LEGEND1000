
#ifndef GammaNuclearPhysicsLEND_h
#define GammaNuclearPhysicsLEND_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class GammaNuclearPhysicsLEND : public G4VPhysicsConstructor
{
  public:
    GammaNuclearPhysicsLEND(const G4String& name = "gamma");
    ~GammaNuclearPhysicsLEND() override = default;

  public:
    void ConstructParticle() override {};
    void ConstructProcess() override;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

