#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void AddPrePhoton(G4int count = 1);
    void AddPostPhoton(G4int count = 1);
    void AddUncollidedPhoton(G4int count = 1);

  private:
    G4int fPreShieldPhotons;
    G4int fPostShieldPhotons;
    G4int fUncollidedPhotons;

    // Writes one row of results (composition, energy, computed
    // shielding parameters) to the CSV file named in RunConfig.
    void WriteResultToCSV();
};

#endif
