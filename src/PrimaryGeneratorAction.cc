#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  fParticleGun = new G4ParticleGun(1);

  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));
  fParticleGun->SetParticleEnergy(14 * MeV);
  fParticleGun->SetParticlePosition(G4ThreeVector(0. * cm, 0. * cm, 0. * cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // distribution uniform in solid angle
  G4double cosTheta = 2 * G4UniformRand() - 1., phi = twopi * G4UniformRand();
  G4double sinTheta = std::sqrt(1. - cosTheta * cosTheta);
  G4double ux = sinTheta * std::cos(phi), uy = sinTheta * std::sin(phi), uz = cosTheta;

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
