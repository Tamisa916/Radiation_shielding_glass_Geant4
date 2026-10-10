#ifndef TC_DETECTOR_CONSTRUCTION_HH
#define TC_DETECTOR_CONSTRUCTION_HH

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VUserDetectorConstruction.hh"

namespace TC
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // Existing setter
    void SetShieldThickness(G4double thick) { fShieldThickness = thick; }

    // NEW: set the glass composition (mass fractions, must sum to 1.0)
    // plus the KNOWN experimental density for that exact composition
    // (looked up from your literature, never guessed/calculated).
    // 'series' selects which recipe to build:
    //   "BBB" -> frac1/2/3 = Bi2O3 / BaO / B2O3           (borate series)
    //   "BBS" -> frac1/2/3 = Bi2O3 / BaO / borosilicate-blend  (borosilicate series)
    // Call this BEFORE runManager->Initialize().
    void SetGlassComposition(const G4String& series, G4double frac1, G4double frac2, G4double frac3,
                             G4double densityGramPerCm3);

  private:
    // Shield parameters
    G4Material* glass;
    G4double fShieldThickness;

    // Composition (mass fractions) + known experimental density.
    // Defaults match the original hardcoded values from the published study.
    G4String fSeries;  // "BBB" or "BBS"
    G4double fBi2O3Frac;  // top-level fraction 1 (both series)
    G4double fBaOFrac;  // top-level fraction 2 (both series)
    G4double fB2O3Frac;  // top-level fraction 3: B2O3 (BBB) or borosilicate-blend (BBS)
    G4double fGlassDensity;

    // Builds the 'glass' material from the current fractions/density.
    // Branches on fSeries. Called once, inside Construct().
    void BuildGlassMaterial();
    G4Material* BuildBorosilicateBlend();  // used only for BBS series

    // Logical volumes for sensitive detectors
    G4LogicalVolume* fPreDetectorLogic;
    G4LogicalVolume* fPostDetectorLogic;

    G4Material* fNaIMaterial;
    void DefineMaterials();
};
}  // namespace TC
#endif  // !TC_DETECTOR_CONSTRUCTION_HH
