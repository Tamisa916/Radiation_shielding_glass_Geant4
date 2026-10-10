#include "RunConfig.hh"

#include "G4SystemOfUnits.hh"

namespace TC
{
// Defaults = your original published-paper setup.
// If main() never changes these (e.g. running the old UI/vis mode),
// everything behaves exactly like before.
G4double RunConfig::energy = 0.356 * MeV;
G4double RunConfig::bi2o3Frac = 0.65;
G4double RunConfig::baoFrac = 0.02;
G4double RunConfig::b2o3Frac = 0.33;
G4double RunConfig::density = 4.502 * g / cm3;
G4double RunConfig::thickness = 1. * cm;
G4int RunConfig::numPrimaries = 100000;
G4String RunConfig::csvFileName = "results.csv";
G4String RunConfig::series = "BBB";
}  // namespace TC