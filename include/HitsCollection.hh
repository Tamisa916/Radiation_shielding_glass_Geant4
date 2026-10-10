// includes Hit and HitsCollection objects

#ifndef TC_HIT_HH
#define TC_HIT_HH

#include "G4Allocator.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4VHit.hh"
#include "G4VUserTrackInformation.hh"
#include "G4ios.hh"

namespace TC
{
class TCHit : public G4VHit
{
  public:
    TCHit();
    ~TCHit() override = default;

    void Print() override;

    // void SetEnergy(G4double e);
    // void SetPos(G4ThreeVector p);

    // void SetPhotonCount(G4int count);  // <-- Declare here
    // G4int GetPhotonCount() const;
    void MarkAsGamma() { hIsGamma = true; }
    void MarkAsUncollided() { hIsUncollided = true; }  // NEW

    G4bool IsGamma() const { return hIsGamma; }
    G4bool IsUncollided() const { return hIsUncollided; }  // NEW
    void MarkAsFirstStep() { fIsFirstStep = true; }
    G4bool IsFirstStep() const { return fIsFirstStep; }

  private:
    // G4double hEnergy;
    // G4ThreeVector hPosition;

    G4bool hIsGamma;
    G4bool fIsFirstStep;  // New member variable
    G4bool hIsUncollided;  // NEW
};

// NEW: a small tag attached to each primary gamma track to remember
// whether it has undergone any REAL physics interaction yet
// (Compton, photoelectric, Rayleigh, pair production...), as opposed
// to just crossing geometry boundaries (which Geant4 also counts as
// "steps" but isn't a real interaction). Updated by SteppingAction
// (see ActionInitialization.hh/.cc) on every step of the track's life,
// and read by SensitiveDetector when deciding if a photon is
// "uncollided". This is more accurate than checking energy loss alone,
// because Rayleigh scattering changes direction but NOT energy --
// an energy-only check would miss it.
class TrackInformation : public G4VUserTrackInformation
{
  public:
    TrackInformation() : G4VUserTrackInformation("TrackInfo"), fHasInteracted(false) {}
    ~TrackInformation() override = default;

    void MarkInteracted() { fHasInteracted = true; }
    G4bool HasInteracted() const { return fHasInteracted; }

  private:
    G4bool fHasInteracted;
};
}  // namespace TC

#endif  // !TC_HIT_HH

#ifndef TC_HITS_COLLECTION_HH
#define TC_HITS_COLLECTION_HH

#include "G4THitsCollection.hh"

namespace TC
{
class HitsCollection : public G4THitsCollection<TCHit>
{
  public:
    HitsCollection(const G4String& name, const G4String& collectionName);

    G4int CountGammaHits() const;
    G4int CountUncollidedHits() const;
};
}  // namespace TC
#endif  // !TC_HITS_COLLECTION_HH
