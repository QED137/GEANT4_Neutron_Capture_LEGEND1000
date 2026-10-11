#ifndef Neu_TrackingAction_hh
#define Neu_TrackingAction_hh

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class NeuEventAction;
class TrackingMessenger;

class NeuTrackingAction : public G4UserTrackingAction
{
public:
  explicit NeuTrackingAction(NeuEventAction*);
  ~NeuTrackingAction() override;

  void PreUserTrackingAction(const G4Track*) override;
  void PostUserTrackingAction(const G4Track*) override;

  void SetParticleCount(G4bool flag) { fParticleCount = flag; }
  void SetKillNeutrons(G4bool flag) { fKillNeutron = flag; }

private:
  NeuEventAction* fEventAction = nullptr;
  TrackingMessenger* fTrackMessenger = nullptr;
  G4bool fParticleCount = true;
  G4bool fKillNeutron = false;
};

#endif
