#ifndef Neu_TrackingAction_hh
#define Neu_TrackingAction_hh

#include "G4UserTrackingAction.hh"

class NeuEventAction;

class NeuTrackingAction : public G4UserTrackingAction
{
public:
  explicit NeuTrackingAction(NeuEventAction*);
  ~NeuTrackingAction() override;

  void PreUserTrackingAction(const G4Track*) override;
  void PostUserTrackingAction(const G4Track*) override;

private:
  NeuEventAction* fEventAction = nullptr;
};

#endif
