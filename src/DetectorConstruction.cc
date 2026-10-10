#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4ios.hh"

#include "SensitiveDetector.hh"

namespace TC
{

DetectorConstruction::DetectorConstruction() : glass(nullptr)
{
  DefineMaterials();

  // Default composition = the original published BBB composition
  // (65% Bi2O3, 2% BaO, 33% B2O3), so nothing breaks if
  // SetGlassComposition() is never called.
  fSeries = "BBB";
  fBi2O3Frac = 0.65;
  fBaOFrac = 0.02;
  fB2O3Frac = 0.33;
  fGlassDensity = 4.502 * g / cm3;

  fShieldThickness = 1. * cm;
}

void DetectorConstruction::SetGlassComposition(const G4String& series, G4double frac1,
                                               G4double frac2, G4double frac3,
                                               G4double densityGramPerCm3)
{
  // Sanity check: fractions should add up to 1.0 (100%).
  // Only a warning, not fatal, in case of small rounding in your CSV.
  G4double sum = frac1 + frac2 + frac3;
  if (std::abs(sum - 1.0) > 0.01) {
    G4cout << "WARNING: glass composition fractions sum to " << sum
           << " (expected 1.0). Check your input values." << G4endl;
  }
  /*
  if (series != "BBB" && series != "BBS") {
    G4cout << "WARNING: unknown series '" << series
           << "'. Expected 'BBB' or 'BBS'. Defaulting to 'BBB'." << G4endl;
    fSeries = "BBB";
  }
  else {
    fSeries = series;
  }
  */

  if (series != "BBB" && series != "BBS" && series != "BBSi" && series != "PB") {
    G4cout << "WARNING: unknown series '" << series
           << "'. Expected 'BBB', 'BBS', 'BBSi' or 'PB'. Defaulting to 'BBB'." << G4endl;
    fSeries = "BBB";
  }
  else {
    fSeries = series;  // <-- this line actually switches the recipe
  }

  fBi2O3Frac = frac1;
  fBaOFrac = frac2;
  fB2O3Frac = frac3;
  fGlassDensity = densityGramPerCm3 * g / cm3;
}

G4Material* DetectorConstruction::BuildBorosilicateBlend()
{
  // The "borosilicate blend" used inside the BBS series.
  // Internal ratios (within this blend, must sum to 100%) are FIXED,
  // taken from the paper's BBS glass network formers/modifiers.
  // *** DOUBLE-CHECK THESE FIVE NUMBERS AGAINST YOUR PAPER'S TABLE ***
  const G4double B2O3_pct = 20.20 * perCent;
  const G4double Na2O_pct = 8.21 * perCent;
  const G4double Al2O3_pct = 17.35 * perCent;
  const G4double SiO2_pct = 48.51 * perCent;
  const G4double K2O_pct = 5.73 * perCent;

  G4Element* B = new G4Element("Boron", "B", 5., 10.81 * g / mole);
  G4Element* O = new G4Element("Oxygen", "O", 8., 16.00 * g / mole);
  G4Element* Na = new G4Element("Sodium", "Na", 11., 23.00 * g / mole);
  G4Element* Al = new G4Element("Aluminium", "Al", 13., 27.00 * g / mole);
  G4Element* Si = new G4Element("Silicon", "Si", 14., 28.00 * g / mole);
  G4Element* K = new G4Element("Potassium", "K", 19., 39.00 * g / mole);

  G4Material* B2O3 = new G4Material("B2O3_bbs", 2.5 * g / cm3, 2);
  B2O3->AddElement(B, 2);
  B2O3->AddElement(O, 3);

  G4Material* Na2O = new G4Material("Na2O", 2.27 * g / cm3, 2);
  Na2O->AddElement(Na, 2);
  Na2O->AddElement(O, 1);

  G4Material* Al2O3 = new G4Material("Al2O3", 3.99 * g / cm3, 2);
  Al2O3->AddElement(Al, 2);
  Al2O3->AddElement(O, 3);

  G4Material* SiO2 = new G4Material("SiO2", 2.65 * g / cm3, 2);
  SiO2->AddElement(Si, 1);
  SiO2->AddElement(O, 2);

  G4Material* K2O = new G4Material("K2O", 2.35 * g / cm3, 2);
  K2O->AddElement(K, 2);
  K2O->AddElement(O, 1);

  // Density here is only used internally by Geant4 to mix this
  // sub-blend -- it does NOT become your final glass density
  // (that's set separately below, from your real experimental value).
  G4Material* borosilicate = new G4Material("borosilicate", 2.55 * g / cm3, 5);
  borosilicate->AddMaterial(B2O3, B2O3_pct);
  borosilicate->AddMaterial(Na2O, Na2O_pct);
  borosilicate->AddMaterial(Al2O3, Al2O3_pct);
  borosilicate->AddMaterial(SiO2, SiO2_pct);
  borosilicate->AddMaterial(K2O, K2O_pct);

  return borosilicate;
}

void DetectorConstruction::BuildGlassMaterial()
{
  // Elements shared by both series
  G4Element* Bi = new G4Element("Bismuth", "Bi", 83., 208.98 * g / mole);
  G4Element* Ba = new G4Element("Barium", "Ba", 56., 137.33 * g / mole);
  G4Element* O = new G4Element("Oxygen", "O", 8., 16.00 * g / mole);

  G4Material* Bi2O3 = new G4Material("Bi2O3", 8.96 * g / cm3, 2);
  Bi2O3->AddElement(Bi, 2);
  Bi2O3->AddElement(O, 3);

  G4Material* BaO = new G4Material("BaO", 5.72 * g / cm3, 2);
  BaO->AddElement(Ba, 1);
  BaO->AddElement(O, 1);

   if (fSeries == "PB") {
    // Lead is a standard NIST material — already has the correct
    // real density built in (11.35 g/cm3), so we don't build
    // anything custom, and we ignore the composition fractions.
    G4NistManager* nist = G4NistManager::Instance();
    glass = nist->FindOrBuildMaterial("G4_Pb");

    G4cout << "Built shield material: NIST Lead (G4_Pb), "
           << "density=" << glass->GetDensity() / (g / cm3) << " g/cm3" << G4endl;
    return;  // skip the BBB/BBS mixing logic below
  }

  if (fSeries == "BBS") {
    // BBS recipe: Bi2O3 + BaO + borosilicate blend
    G4Material* borosilicate = BuildBorosilicateBlend();

    glass = new G4Material("glass", fGlassDensity, 3);
    glass->AddMaterial(Bi2O3, fBi2O3Frac * 100. * perCent);
    glass->AddMaterial(BaO, fBaOFrac * 100. * perCent);
    glass->AddMaterial(borosilicate, fB2O3Frac * 100. * perCent);

    G4cout << "Built BBS glass: Bi2O3=" << fBi2O3Frac * 100 << "%, "
           << "BaO=" << fBaOFrac * 100 << "%, "
           << "borosilicate-blend=" << fB2O3Frac * 100 << "%, "
           << "density=" << fGlassDensity / (g / cm3) << " g/cm3" << G4endl;
  }
  if (fSeries == "BBB") {
    // BBB recipe (default): Bi2O3 + BaO + B2O3 directly
    G4Element* B = new G4Element("Boron", "B", 5., 10.81 * g / mole);
    G4Material* B2O3 = new G4Material("B2O3", 2.5 * g / cm3, 2);
    B2O3->AddElement(B, 2);
    B2O3->AddElement(O, 3);

    glass = new G4Material("glass", fGlassDensity, 3);
    glass->AddMaterial(Bi2O3, fBi2O3Frac * 100. * perCent);
    glass->AddMaterial(BaO, fBaOFrac * 100. * perCent);
    glass->AddMaterial(B2O3, fB2O3Frac * 100. * perCent);

    G4cout << "Built BBB glass: Bi2O3=" << fBi2O3Frac * 100 << "%, "
           << "BaO=" << fBaOFrac * 100 << "%, "
           << "B2O3=" << fB2O3Frac * 100 << "%, "
           << "density=" << fGlassDensity / (g / cm3) << " g/cm3" << G4endl;
  }
  if (fSeries == "BBSi") {
    // Bi2O3-B2O3-SiO2 glass. Order: frac1 = Bi2O3, frac2 = SiO2, frac3 = B2O3
    G4Element* B = new G4Element("Boron", "B", 5., 10.81 * g / mole);
    G4Element* Si = new G4Element("Silicon", "Si", 14., 28.00 * g / mole);

    G4Material* B2O3 = new G4Material("B2O3_bbsi", 2.5 * g / cm3, 2);
    B2O3->AddElement(B, 2);
    B2O3->AddElement(O, 3);

    G4Material* SiO2 = new G4Material("SiO2_bbsi", 2.65 * g / cm3, 2);
    SiO2->AddElement(Si, 1);
    SiO2->AddElement(O, 2);

    glass = new G4Material("glass", fGlassDensity, 3);
    glass->AddMaterial(Bi2O3, fBi2O3Frac * 100. * perCent);
    glass->AddMaterial(SiO2, fBaOFrac * 100. * perCent);  // 2nd number = SiO2
    glass->AddMaterial(B2O3, fB2O3Frac * 100. * perCent);

    G4cout << "Built BBSi glass: Bi2O3=" << fBi2O3Frac * 100 << "%, SiO2=" << fBaOFrac * 100
           << "%, B2O3=" << fB2O3Frac * 100 << "%, density=" << fGlassDensity / (g / cm3)
           << " g/cm3" << G4endl;
  }

}

void DetectorConstruction::DefineMaterials()
{
  G4NistManager* nist = G4NistManager::Instance();

  // Define NaI material
  G4Element* Na = nist->FindOrBuildElement("Na");
  G4Element* I = nist->FindOrBuildElement("I");

  fNaIMaterial = new G4Material("NaI", 3.67 * g / cm3, 2);
  fNaIMaterial->AddElement(Na, 1);
  fNaIMaterial->AddElement(I, 1);

  // Optical properties for scintillation
  const G4int nEntries = 2;
  G4double photonEnergy[nEntries] = {2.034 * eV, 4.136 * eV};
  G4double refractiveIndex[nEntries] = {1.85, 1.85};

  G4MaterialPropertiesTable* mptNaI = new G4MaterialPropertiesTable();
  mptNaI->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries, true);
  mptNaI->AddConstProperty("RESOLUTIONSCALE", 1.0, true);
  mptNaI->AddConstProperty("FASTTIMECONSTANT", 250. * ns, true);
  mptNaI->AddConstProperty("YIELDRATIO", 1.0, true);

  fNaIMaterial->SetMaterialPropertiesTable(mptNaI);
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Build the glass material now, using whatever composition/density
  // is currently set (default, or whatever SetGlassComposition() set).
  BuildGlassMaterial();

  G4NistManager* nist = G4NistManager::Instance();

  // World volume
  G4double worldSize = 50 * cm;
  G4Material* vacuum = nist->FindOrBuildMaterial("G4_Galactic");

  auto solidWorld = new G4Box("World", worldSize / 2, worldSize / 2, worldSize / 2);
  auto logicWorld = new G4LogicalVolume(solidWorld, vacuum, "World");
  auto physWorld =
    new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0);

  // Detector parameters
  G4double detXY = 5. * cm;
  G4double detZ = 1. * cm;
  G4double detZ_pre = 1. * cm;
  G4double detXY_pre = 5. * cm;

  // Shield
  G4double shXY = 10. * cm;
  G4double shieldZpos = 5. * cm;

  auto solidShield = new G4Box("Shield", shXY / 2, shXY / 2, fShieldThickness / 2);
  auto logicShield = new G4LogicalVolume(solidShield, glass, "Shield");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, shieldZpos), logicShield, "Shield", logicWorld,
                    false, 0);

  // Pre-detector (before shield)
  G4double preDetZpos = shieldZpos - fShieldThickness / 2 - 8. * cm;
  auto preSolid = new G4Box("PreDetector", detXY_pre, detXY_pre, detZ_pre);
  fPreDetectorLogic = new G4LogicalVolume(preSolid, vacuum, "PreDetector");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, preDetZpos), fPreDetectorLogic, "PreDetector",
                    logicWorld, false, 0);

  // Post-detector (after shield)
  G4double postDetZpos = shieldZpos + fShieldThickness / 2 + 10. * cm;
  auto postSolid = new G4Box("PostDetector", detXY, detXY, detZ);
  fPostDetectorLogic = new G4LogicalVolume(postSolid, vacuum, "PostDetector");
  //fPostDetectorLogic = new G4LogicalVolume(postSolid, fNaIMaterial, "PostDetector");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, postDetZpos), fPostDetectorLogic, "PostDetector",
                    logicWorld, false, 0);

  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
  auto preSD = new SensitiveDetector("PreDetector", "PreHitsCollection");
  auto postSD = new SensitiveDetector("PostDetector", "PostHitsCollection");

  G4SDManager::GetSDMpointer()->AddNewDetector(preSD);
  G4SDManager::GetSDMpointer()->AddNewDetector(postSD);

  SetSensitiveDetector(fPreDetectorLogic, preSD);
  SetSensitiveDetector(fPostDetectorLogic, postSD);
}
}  // namespace TC