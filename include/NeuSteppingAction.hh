#ifndef Neu_SteppingAction_hh
#define Neu_SteppingAction_hh

#include "G4UserSteppingAction.hh"

class NeuEventAction;

class NeuSteppingAction : public G4UserSteppingAction
{
public:
  explicit NeuSteppingAction(NeuEventAction*);
  ~NeuSteppingAction() override;

  void UserSteppingAction(const G4Step*) override;

private:
  NeuEventAction* fEventAction = nullptr;
};

#endif
