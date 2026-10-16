#ifndef Neu_SteppingAction_hh
#define Neu_SteppingAction_hh

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <map>

class NeuEventAction;

class NeuSteppingAction : public G4UserSteppingAction
{
public:
  explicit NeuSteppingAction(NeuEventAction*);
  ~NeuSteppingAction() override;

  void UserSteppingAction(const G4Step*) override;

private:
  NeuEventAction* fEventAction = nullptr;
  std::map<G4int, G4double> fThermalizationPathByTrack;
  G4int fThermalizationEventId = -1;
};

#endif
