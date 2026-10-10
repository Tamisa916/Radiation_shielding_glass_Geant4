#include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "RunConfig.hh"

#include <cstdlib>

using namespace TC;

int main(int argc, char** argv)
{
  // ------------------------------------------------------------
  // BATCH MODE (for ML dataset generation):
  // ./Test_Calorimeter <series> <frac1> <frac2> <frac3> <density_g_cm3> <energy_MeV>
  //   series = "BBB" -> frac1/2/3 = Bi2O3 / BaO / B2O3
  //   series = "BBS" -> frac1/2/3 = Bi2O3 / BaO / borosilicate-blend
  // Examples:
  // ./Test_Calorimeter BBB 0.65 0.02 0.33 4.502 0.662
  // ./Test_Calorimeter BBS 0.20 0.50 0.30 4.210 0.662
  //
  // No UI, no visualization -- just runs N photons and appends
  // one row to results.csv, then exits. Runs single-threaded so the
  // photon counts in RunAction are correct (no multi-thread merging
  // needed).
  // ------------------------------------------------------------
  if (argc == 7) {
    RunConfig::series = argv[1];
    RunConfig::bi2o3Frac = std::atof(argv[2]);
    RunConfig::baoFrac = std::atof(argv[3]);
    RunConfig::b2o3Frac = std::atof(argv[4]);
    RunConfig::density = std::atof(argv[5]) * g / cm3;
    RunConfig::energy = std::atof(argv[6]) * MeV;

    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

    auto* detector = new DetectorConstruction();
    detector->SetGlassComposition(RunConfig::series, RunConfig::bi2o3Frac, RunConfig::baoFrac,
                                  RunConfig::b2o3Frac, RunConfig::density / (g / cm3));
    detector->SetShieldThickness(RunConfig::thickness);

    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(detector);
    runManager->SetUserInitialization(new ActionInitialization());

    G4SDManager::GetSDMpointer()->SetVerboseLevel(0);

    runManager->Initialize();
    runManager->BeamOn(RunConfig::numPrimaries);

    delete runManager;
    return 0;
  }

  // ------------------------------------------------------------
  // INTERACTIVE / VISUALIZATION MODE (original behavior, unchanged)
  // ------------------------------------------------------------
  G4UIExecutive* ui = nullptr;
  G4UIterminal* uiT = nullptr;

  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }
  else {
    uiT = new G4UIterminal;
  }

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  runManager->SetNumberOfThreads(4);
  runManager->SetUserInitialization(new PhysicsList());
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new ActionInitialization());

  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  if (ui) {
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  }
  else if (uiT) {
    runManager->Initialize();
    uiT->SessionStart();
    delete uiT;
  }
  else {
    G4cout << "Error: No UI Definied" << G4endl;
  }

  delete runManager;
  delete visManager;
  return 0;
}