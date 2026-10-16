#ifndef Run_hh
#define Run_hh

#include "G4Run.hh"
#include "G4VProcess.hh"
#include "globals.hh"

#include <array>
#include <map>

class NeuDetectorConstruction;
class G4ParticleDefinition;

class Run : public G4Run
{
public:
  explicit Run(NeuDetectorConstruction*);
  ~Run() override = default;

  void SetPrimary(G4ParticleDefinition* particle, G4double energy);
  void CountProcesses(const G4VProcess* process);
  void CountCapture(G4int Z, G4int A);
  void RecordCaptureVolume(const G4String& volume);
  void RecordCaptureLengthByIsotope(const G4String& isotope, G4double length);
  void RecordCaptureGamma(G4double energy, G4int multiplicity);
  void ResetEventCaptureGamma();
  void RecordCaptureLength(G4double length);
  void RecordCaptureTime(G4double time);
  void RecordCaptureRadius(G4double radius);
  void ParticleCount(G4String, G4double, G4double);
  void SumEnergies(G4double edep, G4double eflow, G4double etot);
  void ParticleFlux(G4String, G4double);
  void CountEscapedPrimaryNeutron();
  void RecordEventEnergyDeposit(G4double energy);

  void Merge(const G4Run*) override;
  void EndOfRun();

private:
  struct ParticleData
  {
    ParticleData() : fCount(0), fEmean(0.), fEmin(0.), fEmax(0.), fTmean(-1.) {}
    ParticleData(G4int count, G4double ekin, G4double emin, G4double emax, G4double meanLife)
      : fCount(count), fEmean(ekin), fEmin(emin), fEmax(emax), fTmean(meanLife)
    {}
    G4int fCount;
    G4double fEmean;
    G4double fEmin;
    G4double fEmax;
    G4double fTmean;
  };

private:
  NeuDetectorConstruction* fDetector = nullptr;
  G4ParticleDefinition* fParticle = nullptr;
  G4double fEkin = 0.;

  G4double fEnergyDeposit = 0., fEnergyDeposit2 = 0.;
  G4double fEnergyFlow = 0., fEnergyFlow2 = 0.;
  G4double fEnergyTotal = 0., fEnergyTotal2 = 0.;
  std::map<G4String, G4int> fProcCounter;
  std::map<G4String, ParticleData> fParticleDataMap1;
  std::map<G4String, ParticleData> fParticleDataMap2;

  // neutron capture counters by target element
  G4int fCapH = 0, fCapGd = 0, fCapOther = 0;
  G4int fEscapedNeutrons = 0;
  G4int fCaptureGammaEvents = 0, fCaptureGammaCount = 0;
  G4double fCaptureGammaEnergy = 0., fCaptureGammaEnergy2 = 0.;
  G4int fEventsAbove1MeV = 0, fEventsAbove3MeV = 0, fEventsAbove5MeV = 0,
        fEventsAbove7MeV = 0;
  G4double fEventCaptureGammaEnergy = 0.;
  G4int fContainmentEvents = 0;
  G4double fContainmentSum = 0., fContainmentSum2 = 0.;
  static constexpr G4int kContainmentBins = 60;
  std::array<G4int, kContainmentBins> fContainmentHistogram{};
  std::map<G4String, G4int> fCaptureIsotopes;
  std::map<G4String, G4int> fCaptureVolumes;
  struct CaptureLengthStats
  {
    G4int count = 0;
    G4double sum = 0.;
    G4double sum2 = 0.;
  };
  std::map<G4String, CaptureLengthStats> fCaptureLengthByIsotope;
  static constexpr G4int kCaptureLengthBins = 150;
  std::array<G4int, kCaptureLengthBins> fCaptureLengthHistogram{};
  G4int fCaptureLengthOverflow = 0;
  static constexpr G4int kCaptureTimeBins = 200;
  static constexpr G4int kCaptureRadiusBins = 100;
  std::array<G4int, kCaptureTimeBins> fCaptureTimeHistogram{};
  std::array<G4int, kCaptureRadiusBins> fCaptureRadiusHistogram{};
};

#endif
