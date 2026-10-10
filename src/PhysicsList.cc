#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmExtraPhysics.hh"


namespace TC {
	PhysicsList::PhysicsList() {

                           RegisterPhysics(new G4EmLivermorePhysics());
                   		//RegisterPhysics(new G4EmStandardPhysics());
                             //RegisterPhysics(new G4EmStandardPhysics_option4());//greater than 1 mev
                           //RegisterPhysics(new G4OpticalPhysics());
             	}

	void PhysicsList::ConstructParticle() {
		G4VModularPhysicsList::ConstructParticle();
	}

	void PhysicsList::ConstructProcess() {
		G4VModularPhysicsList::ConstructProcess();
	}
}