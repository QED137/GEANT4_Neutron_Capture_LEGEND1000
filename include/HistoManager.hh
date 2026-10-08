#ifndef HistoManager_hh
#define HistoManager_hh

#include "G4AnalysisManager.hh"

class HistoManager
{
public:
  HistoManager();
  ~HistoManager() = default;

private:
  void Book();

  G4String fFileName = "neutron_capture";
};

#endif
