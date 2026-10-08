#ifndef Neu_EventAction_hh
#define Neu_EventAction_hh

#include "G4UserEventAction.hh"
#include "globals.hh"

class NeuEventAction : public G4UserEventAction
{
public:
  NeuEventAction();
  ~NeuEventAction() override;

  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;

  void AddEdep(G4double);
  void AddEflow(G4double);

private:
  G4double fTotalEnergyDeposit = 0.;
  G4double fTotalEnergyFlow = 0.;
};

#endif
