#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  void GeneratePrimaries(G4Event*) override;

private:
  G4ParticleGun* fParticleGun = nullptr;
};

#endif
