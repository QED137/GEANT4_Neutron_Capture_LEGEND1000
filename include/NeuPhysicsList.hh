#ifndef Neu_PhysicsList_hh
#define Neu_PhysicsList_hh

#include "G4VModularPhysicsList.hh"

class NeuPhysicsList : public G4VModularPhysicsList
{
public:
  NeuPhysicsList();
  ~NeuPhysicsList() override;

  void SetCuts() override;
};

#endif
