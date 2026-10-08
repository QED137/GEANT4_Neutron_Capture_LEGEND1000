#include "Run.hh"

#include "NeuDetectorConstruction.hh"
#include "HistoManager.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

namespace
{
constexpr G4double kCaptureLengthMax = 300. * cm;
constexpr G4double kCaptureLengthBinWidth = kCaptureLengthMax / 150;
constexpr G4double kCaptureTimeBinWidth = 5. * microsecond;
constexpr G4double kCaptureRadiusBinWidth = 0.3 * cm;
}

Run::Run(NeuDetectorConstruction* det) : fDetector(det) {}

void Run::SetPrimary(G4ParticleDefinition* particle, G4double energy)
{
  fParticle = particle;
  fEkin = energy;
}

void Run::CountProcesses(const G4VProcess* process)
{
  if (process == nullptr) return;
  G4String procName = process->GetProcessName();
  std::map<G4String, G4int>::iterator it = fProcCounter.find(procName);
  if (it == fProcCounter.end()) {
    fProcCounter[procName] = 1;
  }
  else {
    fProcCounter[procName]++;
  }
}

void Run::CountCapture(G4int Z)
{
  // Z=64: excited Gd-156/158 from nCapture on Gd-155/157
  // Z=1 : deuteron from nCapture on H-1
  if (Z == 64)
    fCapGd++;
  else if (Z == 1)
    fCapH++;
  else
    fCapOther++;
}

void Run::RecordCaptureLength(G4double length)
{
  const G4int bin = static_cast<G4int>(length / kCaptureLengthBinWidth);
  if (bin >= 0 && bin < kCaptureLengthBins)
    ++fCaptureLengthHistogram[bin];
  else
    ++fCaptureLengthOverflow;
}

void Run::RecordCaptureTime(G4double time)
{
  const G4int bin = static_cast<G4int>(time / kCaptureTimeBinWidth);
  if (bin >= 0 && bin < kCaptureTimeBins)
    ++fCaptureTimeHistogram[bin];
}

void Run::RecordCaptureRadius(G4double radius)
{
  const G4int bin = static_cast<G4int>(radius / kCaptureRadiusBinWidth);
  if (bin >= 0 && bin < kCaptureRadiusBins)
    ++fCaptureRadiusHistogram[bin];
}

void Run::ParticleCount(G4String name, G4double Ekin, G4double meanLife)
{
  std::map<G4String, ParticleData>::iterator it = fParticleDataMap1.find(name);
  if (it == fParticleDataMap1.end()) {
    fParticleDataMap1[name] = ParticleData(1, Ekin, Ekin, Ekin, meanLife);
  }
  else {
    ParticleData& data = it->second;
    data.fCount++;
    data.fEmean += Ekin;
    G4double emin = data.fEmin;
    if (Ekin < emin) data.fEmin = Ekin;
    G4double emax = data.fEmax;
    if (Ekin > emax) data.fEmax = Ekin;
    data.fTmean = meanLife;
  }
}

void Run::SumEnergies(G4double edep, G4double eflow, G4double etot)
{
  fEnergyDeposit += edep;
  fEnergyDeposit2 += edep * edep;
  fEnergyFlow += eflow;
  fEnergyFlow2 += eflow * eflow;
  fEnergyTotal += etot;
  fEnergyTotal2 += etot * etot;
}

void Run::ParticleFlux(G4String name, G4double Ekin)
{
  std::map<G4String, ParticleData>::iterator it = fParticleDataMap2.find(name);
  if (it == fParticleDataMap2.end()) {
    fParticleDataMap2[name] = ParticleData(1, Ekin, Ekin, Ekin, -1 * ns);
  }
  else {
    ParticleData& data = it->second;
    data.fCount++;
    data.fEmean += Ekin;
    G4double emin = data.fEmin;
    if (Ekin < emin) data.fEmin = Ekin;
    G4double emax = data.fEmax;
    if (Ekin > emax) data.fEmax = Ekin;
    data.fTmean = -1 * ns;
  }
}

void Run::Merge(const G4Run* run)
{
  const Run* localRun = static_cast<const Run*>(run);

  fParticle = localRun->fParticle;
  fEkin = localRun->fEkin;

  fEnergyDeposit += localRun->fEnergyDeposit;
  fEnergyDeposit2 += localRun->fEnergyDeposit2;
  fEnergyFlow += localRun->fEnergyFlow;
  fEnergyFlow2 += localRun->fEnergyFlow2;
  fEnergyTotal += localRun->fEnergyTotal;
  fEnergyTotal2 += localRun->fEnergyTotal2;

  // accumulate neutron capture counts
  fCapH += localRun->fCapH;
  fCapGd += localRun->fCapGd;
  fCapOther += localRun->fCapOther;
  for (G4int i = 0; i < kCaptureLengthBins; ++i)
    fCaptureLengthHistogram[i] += localRun->fCaptureLengthHistogram[i];
  fCaptureLengthOverflow += localRun->fCaptureLengthOverflow;
  for (G4int i = 0; i < kCaptureTimeBins; ++i)
    fCaptureTimeHistogram[i] += localRun->fCaptureTimeHistogram[i];
  for (G4int i = 0; i < kCaptureRadiusBins; ++i)
    fCaptureRadiusHistogram[i] += localRun->fCaptureRadiusHistogram[i];

  std::map<G4String, G4int>::const_iterator itp;
  for (itp = localRun->fProcCounter.begin(); itp != localRun->fProcCounter.end(); ++itp) {
    G4String procName = itp->first;
    G4int localCount = itp->second;
    if (fProcCounter.find(procName) == fProcCounter.end()) {
      fProcCounter[procName] = localCount;
    }
    else {
      fProcCounter[procName] += localCount;
    }
  }

  std::map<G4String, ParticleData>::const_iterator itc;
  for (itc = localRun->fParticleDataMap1.begin(); itc != localRun->fParticleDataMap1.end(); ++itc) {
    G4String name = itc->first;
    const ParticleData& localData = itc->second;
    if (fParticleDataMap1.find(name) == fParticleDataMap1.end()) {
      fParticleDataMap1[name] = ParticleData(localData.fCount, localData.fEmean, localData.fEmin,
                                             localData.fEmax, localData.fTmean);
    }
    else {
      ParticleData& data = fParticleDataMap1[name];
      data.fCount += localData.fCount;
      data.fEmean += localData.fEmean;
      G4double emin = localData.fEmin;
      if (emin < data.fEmin) data.fEmin = emin;
      G4double emax = localData.fEmax;
      if (emax > data.fEmax) data.fEmax = emax;
      data.fTmean = localData.fTmean;
    }
  }

  std::map<G4String, ParticleData>::const_iterator itn;
  for (itn = localRun->fParticleDataMap2.begin(); itn != localRun->fParticleDataMap2.end(); ++itn) {
    G4String name = itn->first;
    const ParticleData& localData = itn->second;
    if (fParticleDataMap2.find(name) == fParticleDataMap2.end()) {
      fParticleDataMap2[name] = ParticleData(localData.fCount, localData.fEmean, localData.fEmin,
                                             localData.fEmax, localData.fTmean);
    }
    else {
      ParticleData& data = fParticleDataMap2[name];
      data.fCount += localData.fCount;
      data.fEmean += localData.fEmean;
      G4double emin = localData.fEmin;
      if (emin < data.fEmin) data.fEmin = emin;
      G4double emax = localData.fEmax;
      if (emax > data.fEmax) data.fEmax = emax;
      data.fTmean = localData.fTmean;
    }
  }

  G4Run::Merge(run);
}

void Run::EndOfRun()
{
  G4int prec = 5, wid = prec + 2;
  G4int dfprec = G4cout.precision(prec);

  G4Material* material = fDetector->GetMaterial();
  G4double density = material->GetDensity();

  G4String Particle = fParticle->GetParticleName();
  G4cout << "\n The run is " << numberOfEvent << " " << Particle << " of "
         << G4BestUnit(fEkin, "Energy") << " through "
         << G4BestUnit(fDetector->GetRadius(), "Length") << " of " << material->GetName()
         << " (density: " << G4BestUnit(density, "Volumic Mass") << ")" << G4endl;

  if (numberOfEvent == 0) {
    G4cout.precision(dfprec);
    return;
  }

  G4cout << "\n Process calls frequency :" << G4endl;
  G4int index = 0;
  std::map<G4String, G4int>::iterator it;
  for (it = fProcCounter.begin(); it != fProcCounter.end(); it++) {
    G4String procName = it->first;
    G4int count = it->second;
    G4String space = " ";
    if (++index % 3 == 0) space = "\n";
    G4cout << " " << std::setw(20) << procName << "=" << std::setw(7) << count << space;
  }
  G4cout << G4endl;

  // ---- neutron capture statistics by target element ----
  G4int nCap = fCapH + fCapGd + fCapOther;
  G4cout << "\n Neutron captures : H = " << fCapH << "   Gd = " << fCapGd
         << "   other = " << fCapOther << "   (total " << nCap << ")"
         << "\n Gd capture share = " << 100. * fCapGd / (nCap > 0 ? nCap : 1) << " %"
         << G4endl;

  G4cout << "\n Capture length histogram (cm):" << G4endl;
  for (G4int i = 0; i < kCaptureLengthBins; ++i) {
    if (fCaptureLengthHistogram[i] == 0) continue;
    const G4double low = i * kCaptureLengthBinWidth / cm;
    const G4double high = (i + 1) * kCaptureLengthBinWidth / cm;
    G4cout << " capture_length_bin " << low << " " << high << " "
           << fCaptureLengthHistogram[i] << G4endl;
  }
  if (fCaptureLengthOverflow > 0)
    G4cout << " capture_length_overflow " << fCaptureLengthOverflow << G4endl;

  G4cout << "\n Capture time histogram (microseconds):" << G4endl;
  for (G4int i = 0; i < kCaptureTimeBins; ++i) {
    if (fCaptureTimeHistogram[i] == 0) continue;
    G4cout << " capture_time_bin " << i * 5 << " " << (i + 1) * 5 << " "
           << fCaptureTimeHistogram[i] << G4endl;
  }

  G4cout << "\n Capture radius histogram (cm):" << G4endl;
  for (G4int i = 0; i < kCaptureRadiusBins; ++i) {
    if (fCaptureRadiusHistogram[i] == 0) continue;
    G4cout << " capture_radius_bin " << i * 0.3 << " " << (i + 1) * 0.3 << " "
           << fCaptureRadiusHistogram[i] << G4endl;
  }

  G4int TotNbofEvents = numberOfEvent;
  fEnergyDeposit /= TotNbofEvents;
  fEnergyDeposit2 /= TotNbofEvents;
  G4double rmsEdep = fEnergyDeposit2 - fEnergyDeposit * fEnergyDeposit;
  if (rmsEdep > 0.)
    rmsEdep = std::sqrt(rmsEdep);
  else
    rmsEdep = 0.;

  G4cout << "\n Mean energy deposit per event = " << G4BestUnit(fEnergyDeposit, "Energy")
         << ";  rms = " << G4BestUnit(rmsEdep, "Energy") << G4endl;

  fEnergyFlow /= TotNbofEvents;
  fEnergyFlow2 /= TotNbofEvents;
  G4double rmsEflow = fEnergyFlow2 - fEnergyFlow * fEnergyFlow;
  if (rmsEflow > 0.)
    rmsEflow = std::sqrt(rmsEflow);
  else
    rmsEflow = 0.;

  G4cout << " Mean energy leakage per event = " << G4BestUnit(fEnergyFlow, "Energy")
         << ";  rms = " << G4BestUnit(rmsEflow, "Energy") << G4endl;

  fEnergyTotal /= TotNbofEvents;
  fEnergyTotal2 /= TotNbofEvents;
  G4double rmsEtotal = fEnergyTotal2 - fEnergyTotal * fEnergyTotal;
  if (rmsEtotal > 0.)
    rmsEtotal = std::sqrt(rmsEtotal);
  else
    rmsEflow = 0.;

  G4cout << "\n Mean energy total   per event = " << G4BestUnit(fEnergyTotal, "Energy")
         << ";  rms = " << G4BestUnit(rmsEtotal, "Energy") << G4endl;

  if (fParticleDataMap1.size() > 0) {
    G4cout << "\n List of particles at creation :" << G4endl;
    std::map<G4String, ParticleData>::iterator itc;
    for (itc = fParticleDataMap1.begin(); itc != fParticleDataMap1.end(); itc++) {
      G4String name = itc->first;
      ParticleData data = itc->second;
      G4int count = data.fCount;
      G4double eMean = data.fEmean / count;
      G4double eMin = data.fEmin;
      G4double eMax = data.fEmax;
      G4double meanLife = data.fTmean;

      G4cout << "  " << std::setw(13) << name << ": " << std::setw(7) << count
             << "  Emean = " << std::setw(wid) << G4BestUnit(eMean, "Energy") << "\t( "
             << G4BestUnit(eMin, "Energy") << " --> " << G4BestUnit(eMax, "Energy") << ")";
      if (meanLife >= 0.)
        G4cout << "\tmean life = " << G4BestUnit(meanLife, "Time") << G4endl;
      else
        G4cout << "\tstable" << G4endl;
    }
  }

  G4cout << "\n List of particles emerging from the absorber :" << G4endl;

  std::map<G4String, ParticleData>::iterator itn;
  for (itn = fParticleDataMap2.begin(); itn != fParticleDataMap2.end(); itn++) {
    G4String name = itn->first;
    ParticleData data = itn->second;
    G4int count = data.fCount;
    G4double eMean = data.fEmean / count;
    G4double eMin = data.fEmin;
    G4double eMax = data.fEmax;
    G4double Eflow = data.fEmean / TotNbofEvents;

    G4cout << "  " << std::setw(13) << name << ": " << std::setw(7) << count
           << "  Emean = " << std::setw(wid) << G4BestUnit(eMean, "Energy") << "\t( "
           << G4BestUnit(eMin, "Energy") << " --> " << G4BestUnit(eMax, "Energy")
           << ") \tEleak/event = " << G4BestUnit(Eflow, "Energy") << G4endl;
  }

  // normalize histograms
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  G4int ih = 2;
  G4double binWidth = analysisManager->GetH1Width(ih);
  G4double fac = (1. / (numberOfEvent * binWidth)) * (mm / MeV);
  analysisManager->ScaleH1(ih, fac);

  fProcCounter.clear();
  fParticleDataMap2.clear();

  G4cout.precision(dfprec);
}
