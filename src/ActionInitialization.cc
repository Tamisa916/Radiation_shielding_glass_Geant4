#include "ActionInitialization.hh"

#include "G4Gamma.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

#include "HitsCollection.hh"  // for TrackInformation
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

namespace TC
{
void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction);
  SetUserAction(new RunAction());
  SetUserAction(new SteppingAction());  // NEW: tracks real interactions
}

void ActionInitialization::BuildForMaster() const {}

// NEW: implementation of SteppingAction (declared in ActionInitialization.hh)
void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();

  // Only bother tracking this for primary gamma photons -- they're
  // the only ones SensitiveDetector cares about anyway.
  if (track->GetDefinition() != G4Gamma::GammaDefinition()) return;
  if (track->GetParentID() != 0) return;

  // Attach a TrackInformation tag the first time we see this track.
  if (track->GetUserInformation() == nullptr) {
    track->SetUserInformation(new TrackInformation());
  }

  auto* info = static_cast<TrackInformation*>(track->GetUserInformation());

  // What process caused this step? "Transportation" just means the
  // photon crossed a geometry boundary -- not a real interaction.
  // Anything else (Compton, photoelectric, Rayleigh, pair
  // production...) means it genuinely interacted with the material.
  const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
  if (process != nullptr && process->GetProcessName() != "Transportation") {
    info->MarkInteracted();
  }
}
}  // namespace TC