#ifndef TC_ACTION_INIT_HH
#define TC_ACTION_INIT_HH

#include "G4UserSteppingAction.hh"
#include "G4VUserActionInitialization.hh"

namespace TC
{
class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization() = default;
    ~ActionInitialization() override = default;

  private:
    void Build() const override;
    void BuildForMaster() const override;
};

// NEW: watches every step of every track. Its only job is to mark
// primary gamma tracks as "interacted" the moment they undergo any
// real physics process (not just a geometry-boundary crossing).
// See TrackInformation in HitsCollection.hh for the flag itself.
class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction() = default;
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;
};
}  // namespace TC

#endif  // !TC_ACTION_INIT_HH
