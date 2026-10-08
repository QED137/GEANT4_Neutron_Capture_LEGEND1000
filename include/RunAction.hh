#ifndef RunAction_hh
#define RunAction_hh

#include "G4UserRunAction.hh"

class NeuDetectorConstruction;
class PrimaryGeneratorAction;
class HistoManager;
class Run;

class RunAction : public G4UserRunAction
{
public:
  RunAction(NeuDetectorConstruction*, PrimaryGeneratorAction*);
  ~RunAction() override;

  G4Run* GenerateRun() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

private:
  NeuDetectorConstruction* fDetector = nullptr;
  PrimaryGeneratorAction* fPrimary = nullptr;
  HistoManager* fHistoManager = nullptr;
  Run* fRun = nullptr;
};

#endif
