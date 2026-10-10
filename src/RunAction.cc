#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

#include "RunConfig.hh"

#include <cmath>
#include <fstream>

using namespace TC;

RunAction::RunAction()
  : G4UserRunAction(), fPreShieldPhotons(0), fPostShieldPhotons(0), fUncollidedPhotons(0)
{}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{
  fPreShieldPhotons = 0;
  fPostShieldPhotons = 0;
  fUncollidedPhotons = 0;
}

void RunAction::EndOfRunAction(const G4Run*)
{
  G4cout << "\n========== Run Summary ==========" << G4endl;
  G4cout << "Photons before shield (PreDetector): " << fPreShieldPhotons << G4endl;
  G4cout << "Photons after shield (PostDetector): " << fPostShieldPhotons << G4endl;
  G4cout << "Uncollided photons after shield (PostDetector): " << fUncollidedPhotons << G4endl;

 
  G4cout << "==================================" << G4endl;

  // Only the master thread/run should write the final CSV row
  // (in batch/data-generation mode we run single-threaded, so this
  // always fires exactly once per composition+energy combination).
  WriteResultToCSV();
}

void RunAction::AddPrePhoton(G4int count)
{
  fPreShieldPhotons += count;
}

void RunAction::AddPostPhoton(G4int count)
{
  fPostShieldPhotons += count;
}

void RunAction::AddUncollidedPhoton(G4int count)
{
  fUncollidedPhotons += count;
}

void RunAction::WriteResultToCSV()
{
  // Need at least some pre-shield and uncollided counts to compute
  // a meaningful attenuation coefficient. If a run produced zero
  // (e.g. extreme composition/energy), skip the physics calculation
  // but still log the row so you notice it in the CSV.
  G4double thicknessCm = RunConfig::thickness / cm;
  G4double densityGcm3 = RunConfig::density / (g / cm3);
  G4double energyMeV = RunConfig::energy / MeV;

  G4double mu = 0.;  // linear attenuation coefficient, cm^-1
  G4double mac = 0.;  // mass attenuation coefficient, cm^2/g
  G4double hvl = 0.;  // half-value layer, cm
  G4double buildupFactor = 0.;  // ratio of total to uncollided photons

   // Statistical uncertainty (Poisson counting statistics): for a
  // count of N photons, the relative uncertainty is 1/sqrt(N).
  // This is the same math behind error bars in your published paper --
  // fewer photons detected = noisier result, more photons = more stable.
  //G4double uncollidedUncertaintyPct = 0.;  // relative uncertainty on the count itself, in %
  //G4double muUncertainty = 0.;  // propagated uncertainty on mu, cm^-1
  //G4double hvlUncertainty = 0.;  // propagated uncertainty on HVL, cm

  if (fPreShieldPhotons > 0 && fUncollidedPhotons > 0 && fPostShieldPhotons > 0 && thicknessCm > 0) {
    G4double transmission = static_cast<G4double>(fUncollidedPhotons) / (fPreShieldPhotons);
    mu = -std::log(transmission) / thicknessCm;
    mac = mu / densityGcm3;
    hvl = std::log(2.0) / mu;
    buildupFactor = static_cast<G4double>(fPostShieldPhotons) / fUncollidedPhotons;

    //G4double relErr = 1.0 / std::sqrt(static_cast<G4double>(fPostShieldPhotons));
    //uncollidedUncertaintyPct = relErr * 100.0;

    // Propagate: mu = -ln(T)/thickness  ->  sigma_mu = relErr / thickness
   // muUncertainty = relErr / thicknessCm;

    // Propagate: HVL = ln(2)/mu  ->  sigma_HVL = (ln2/mu^2) * sigma_mu
    //if (mu > 0) {
    //  hvlUncertainty = (std::log(2.0) / (mu * mu)) * muUncertainty;
   // }
  }

  // Write header only if the file doesn't exist yet.
  std::ifstream checkFile(RunConfig::csvFileName);
  bool fileExists = checkFile.good();
  checkFile.close();

  std::ofstream csv(RunConfig::csvFileName, std::ios::app);
  if (!fileExists) {
    csv << "series,Bi2O3_frac,BaO_frac,frac3,density_g_cm3,energy_MeV,"
        << "thickness_cm,pre_shield_photons,post_shield_photons,"
        << "uncollided_photons,buildupFactor,mu_cm-1,MAC_cm2_g,HVL_cm\n";
  }

  csv << RunConfig::series << "," << RunConfig::bi2o3Frac << "," << RunConfig::baoFrac << ","
      << RunConfig::b2o3Frac << "," << densityGcm3 << "," << energyMeV << "," << thicknessCm << ","
      << fPreShieldPhotons << "," << fPostShieldPhotons << "," << fUncollidedPhotons << "," << buildupFactor << "," << mu
      << "," << mac << "," << hvl << "\n";

  csv.close();

  G4cout << "Result appended to " << RunConfig::csvFileName << G4endl;
}