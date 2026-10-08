#include "NeuActionInitialization.hh"
#include "NeuEventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "NeuSteppingAction.hh"
#include "NeuTrackingAction.hh"

NeuActionInitialization::NeuActionInitialization(NeuDetectorConstruction* detector)
  : fDetector(detector)
{
}

NeuActionInitialization::~NeuActionInitialization() {}

void NeuActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction(fDetector, nullptr));
}

void NeuActionInitialization::Build() const
{
  PrimaryGeneratorAction* primary = new PrimaryGeneratorAction();
  SetUserAction(primary);

  SetUserAction(new RunAction(fDetector, primary));

  NeuEventAction* event = new NeuEventAction();
  SetUserAction(event);

  SetUserAction(new NeuTrackingAction(event));

  SetUserAction(new NeuSteppingAction(event));
}
