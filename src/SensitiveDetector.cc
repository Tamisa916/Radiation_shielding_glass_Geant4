#include "SensitiveDetector.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4StateManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

#include "RunAction.hh"
#include "RunConfig.hh"

namespace TC
{

G4Mutex mutex = G4MUTEX_INITIALIZER;

SensitiveDetector::SensitiveDetector(const G4String& name, const G4String& hitsCollectionName)
  : G4VSensitiveDetector(name), fHitsCollection(nullptr)
{
  collectionName.insert(hitsCollectionName);
  G4int fPhotonCount;
}

void SensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection = new HitsCollection(SensitiveDetectorName, collectionName[0]);
  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // Only process gamma photons
  if (aStep->GetTrack()->GetDefinition() != G4Gamma::GammaDefinition()) {
    return false;
  }

  // Reject secondary gammas
  if (aStep->GetTrack()->GetParentID() != 0) {
    return false;
  }

  // Reject backscattered photons
  G4ThreeVector momentumDir = aStep->GetPreStepPoint()->GetMomentumDirection();
  if (momentumDir.z() <= 0) {
    return false;
  }

  // ~~~ FIX 1: count each photon ONCE per detector, per event ~~~
  // A single photon can take multiple steps inside this volume (e.g. if
  // it Compton-scatters while still inside it), which would otherwise be
  // logged as multiple separate hits. Since this collection is reset at
  // the start of every event (see Initialize()), "already has an entry"
  // means "we already recorded this event's photon" -- skip duplicates.
  if (fHitsCollection->entries() > 0) {
    return false;
  }

  // Create hit and mark as gamma
  TCHit* hit = new TCHit();
  hit->MarkAsGamma();

  // ~~~ FIX 2: correct definition of "uncollided" ~~~
  // Uses the TrackInformation flag set by SteppingAction, which watches
  // EVERY step of the photon's life (not just this detector) and marks
  // it the moment any real physics process fires (Compton, photoelectric,
  // Rayleigh, pair production). This replaces the old energy-loss check,
  // which missed Rayleigh scattering (it changes direction but not
  // energy, so an energy-only check would wrongly call it "uncollided").
  auto* info = static_cast<TrackInformation*>(aStep->GetTrack()->GetUserInformation());
  G4bool interacted = (info != nullptr) && info->HasInteracted();
  if (!interacted) {
    hit->MarkAsUncollided();
  }

  fHitsCollection->insert(hit);
  return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  G4AutoLock lock(&mutex);  // Thread-safe output

  G4int gammaCount = fHitsCollection->CountGammaHits();
  G4int uncollidedCount = fHitsCollection->CountUncollidedHits();

  //G4cout << "Gamma photons detected in this event: " << gammaCount << G4endl;
  //G4cout << "Uncollided photons detected in this event: " << uncollidedCount << G4endl;

  // Update RunAction
  const G4UserRunAction* userAction = G4RunManager::GetRunManager()->GetUserRunAction();
  RunAction* runAction = const_cast<RunAction*>(static_cast<const RunAction*>(userAction));

  if (runAction) {
    if (GetName() == "PreDetector") {
      runAction->AddPrePhoton(gammaCount);
    }
    else if (GetName() == "PostDetector") {
      runAction->AddPostPhoton(gammaCount);
      runAction->AddUncollidedPhoton(uncollidedCount);
    }
  }
}
}  // namespace TC