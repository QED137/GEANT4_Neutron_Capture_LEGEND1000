#include "HistoManager.hh"
#include "G4UnitsTable.hh"

HistoManager::HistoManager() : fFileName("neutron_capture")
{
  Book();
}

void HistoManager::Book()
{
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetFileName(fFileName);
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetActivation(true);

  const G4int kMaxHisto = 31;
  const G4String id[] = { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
                          "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
                          "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30" };
  const G4String title[] = {
    "dummy",                                                          // 0
    "total energy deposit",                                           // 1
    "Edep (MeV/mm) along absorber",                                   // 2
    "total kinetic energy flow",                                      // 3
    "energy spectrum of gamma at creation",                           // 4
    "energy spectrum of e+- at creation",                             // 5
    "energy spectrum of neutrons at creation",                        // 6
    "energy spectrum of protons at creation",                         // 7
    "energy spectrum of deuterons at creation",                       // 8
    "energy spectrum of alphas at creation",                          // 9
    "energy spectrum of all others ions at creation (excited Gd !)",  // 10
    "energy spectrum of all others baryons at creation",              // 11
    "energy spectrum of all others mesons at creation",               // 12
    "energy spectrum of all others leptons (neutrinos) at creation",  // 13
    "energy spectrum of emerging gamma",                              // 14
    "energy spectrum of emerging e+-",                                // 15
    "energy spectrum of emerging neutrons",                           // 16
    "energy spectrum of emerging protons",                            // 17
    "energy spectrum of emerging deuterons",                          // 18
    "energy spectrum of emerging alphas",                             // 19
    "energy spectrum of all others emerging ions",                    // 20
    "energy spectrum of all others emerging baryons",                 // 21
    "energy spectrum of all others emerging mesons",                  // 22
    "energy spectrum of all others emerging leptons (neutrinos)",     // 23
    "neutron capture path length",                                    // 24
    "neutron capture time",                                           // 25
    "neutron capture radius",                                         // 26
    "capture gamma energy",                                            // 27
    "capture gamma multiplicity",                                      // 28
    "event deposited energy",                                          // 29
    "capture energy containment fraction"                              // 30
  };

  G4int nbins = 100;
  G4double vmin = 0.;
  G4double vmax = 100.;

  for (G4int k = 0; k < kMaxHisto; k++) {
    G4int ih = analysisManager->CreateH1(id[k], title[k], nbins, vmin, vmax);
    analysisManager->SetH1Activation(ih, false);
  }

  analysisManager->CreateNtuple("capture", "One row per neutron capture");
  analysisManager->CreateNtupleIColumn("event_id");
  analysisManager->CreateNtupleSColumn("volume");
  analysisManager->CreateNtupleSColumn("material");
  analysisManager->CreateNtupleIColumn("target_z");
  analysisManager->CreateNtupleIColumn("target_a");
  analysisManager->CreateNtupleDColumn("x_cm");
  analysisManager->CreateNtupleDColumn("y_cm");
  analysisManager->CreateNtupleDColumn("z_cm");
  analysisManager->CreateNtupleDColumn("radius_cm");
  analysisManager->CreateNtupleDColumn("time_us");
  analysisManager->CreateNtupleDColumn("path_length_cm");
  analysisManager->CreateNtupleDColumn("gamma_energy_mev");
  analysisManager->CreateNtupleIColumn("gamma_multiplicity");
  analysisManager->FinishNtuple();
}
