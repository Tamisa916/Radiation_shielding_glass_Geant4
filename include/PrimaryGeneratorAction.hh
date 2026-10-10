#ifndef TC_PRIMARY_GEN_HH
#define TC_PRIMARY_GEN_HH

#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

namespace TC
{
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;
};
}  // namespace TC
#endif  // !TC_PRIMARY_GEN_HH
