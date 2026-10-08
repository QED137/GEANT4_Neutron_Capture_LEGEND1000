#ifndef Neu_ActionInitialization_hh
#define Neu_ActionInitialization_hh

#include "G4VUserActionInitialization.hh"

class NeuDetectorConstruction;

class NeuActionInitialization : public G4VUserActionInitialization
{
public:
  explicit NeuActionInitialization(NeuDetectorConstruction*);
  ~NeuActionInitialization() override;

  void BuildForMaster() const override;
  void Build() const override;

private:
  NeuDetectorConstruction* fDetector = nullptr;
};

#endif
