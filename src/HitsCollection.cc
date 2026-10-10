
#include "HitsCollection.hh"

namespace TC {
	TCHit::TCHit() {
		
		// Set the energy and position to zero
		//hEnergy = 0.;
		//hPosition = G4ThreeVector();
                hIsGamma = false;
                hIsUncollided = false;  // <-- the missing line
                fIsFirstStep = false;

	}

	void TCHit::Print() {
		G4cout
			//<< "Energy: "
			//<< G4BestUnit(hEnergy, "Energy")
			//<< " "
			//<< "Position: "
			//<< G4BestUnit(hPosition.getX(), "Length")
			//<< G4BestUnit(hPosition.getY(), "Length")
			//<< G4BestUnit(hPosition.getZ(), "Length")
                        <<"Type : " 
                        <<  (hIsGamma ? "Gamma" : "Other")
                        << G4endl;
      
	}

	// Methods to set the energy and position
	//void TCHit::SetEnergy(G4double e) { hEnergy = e; }
	//void TCHit::SetPos(G4ThreeVector p) { hPosition = p; }
        //void TCHit::SetPhotonCount(G4int c) { hPhotonCount = c; }

         //G4int TCHit::GetPhotonCount() const { return hPhotonCount; } 


	// Very simple HitsCollection, just a list to hold all our hits
	HitsCollection::HitsCollection(
		const G4String& name,
		const G4String& collectionName)
		: G4THitsCollection<TCHit>(name, collectionName) {}

 G4int HitsCollection::CountGammaHits() const {
        G4int count = 0;
        for (G4int i = 0; i < entries(); i++) {
            if ((*this)[i]->IsGamma()) {
                count++;
            }
        }
        return count;
    }
 
 G4int HitsCollection::CountUncollidedHits() const
    {
      G4int count = 0;
      for (G4int i = 0; i < entries(); i++) {
        if ((*this)[i]->IsUncollided()) {
          count++;
        }
      }
      return count;
    }
	


}
