#ifndef TC_RUN_CONFIG_HH
#define TC_RUN_CONFIG_HH

#include "globals.hh"

namespace TC
{
// Simple shared configuration values.
// main() sets these ONCE, before runManager->Initialize().
// Every other class (PrimaryGeneratorAction, RunAction) just READS
// these values when it needs them. Nothing writes to these after
// the run starts, so this is safe even with multiple threads.
struct RunConfig
{
    static G4double energy;  // gamma energy (Geant4 internal units)
    static G4double bi2o3Frac;  // mass fraction, 0.0-1.0
    static G4double baoFrac;  // mass fraction, 0.0-1.0
    static G4double b2o3Frac;  // mass fraction, 0.0-1.0
    static G4double density;  // known experimental density (Geant4 internal units)
    static G4double thickness;  // shield thickness (Geant4 internal units)
    static G4int numPrimaries;  // photons per run (BeamOn count)
    static G4String csvFileName;  // output CSV file path
    static G4String series;  // "BBB" (borate) or "BBS" (borosilicate)
};
}  // namespace TC

#endif  // !TC_RUN_CONFIG_HH
