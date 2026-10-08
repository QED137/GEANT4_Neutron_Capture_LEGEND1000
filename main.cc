// main.cc -- neutron capture in Gd-doped water (LEGEND-1000 style)
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

#include "NeuDetectorConstruction.hh"
#include "NeuPhysicsList.hh"
#include "NeuActionInitialization.hh"

int main(int argc, char** argv)
{
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

  auto* det = new NeuDetectorConstruction();
  runManager->SetUserInitialization(det);
  runManager->SetUserInitialization(new NeuPhysicsList());
  runManager->SetUserInitialization(new NeuActionInitialization(det));

  runManager->Initialize();

  G4VisExecutive* visManager = nullptr;
  if (argc == 1) {
    visManager = new G4VisExecutive();
    visManager->Initialize();
  }

  auto* UImanager = G4UImanager::GetUIpointer();

  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute macros/vis.mac");
    ui->SessionStart();
    delete ui;
  }
  else {
    UImanager->ApplyCommand("/control/execute " + G4String(argv[1]));
  }

  delete visManager;
  delete runManager;
  return 0;
}
