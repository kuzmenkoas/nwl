#ifndef nwlParticleInfo_hh
#define nwlParticleInfo_hh

#include "G4VUserTrackInformation.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"

#include <vector>
#include <ostream>

class G4VPhysicalVolume;

struct nwlDetRecord {
	G4bool firstHit;
	G4String detectorId;
	G4double detectorTime;
	G4double detectorKineticEnergy;
	G4ThreeVector entrancePoint;
	G4ThreeVector entranceDirection;
	G4double weight;
	G4double Deposit;
	G4bool isOutside;
};

class  nwlParticleInfo : public G4VUserTrackInformation
{
    public:
        nwlParticleInfo();
	~nwlParticleInfo();

        void Print() const;

	void SetOriginInfo(G4int TrackID, G4int ParentID, G4int Pdg, G4double Kine, G4double Time, G4ThreeVector OriginPoint, G4VPhysicalVolume* vol, G4String CreatorProcess, G4int nA, G4int nZ);

	void SetDetectorInfo(G4String DetId, G4double Time, G4double Kine, G4ThreeVector EntrancePoint, G4ThreeVector EntranceDir, G4double W);

	void SetFinalInfo(G4bool StopInDet, G4String detId, G4String ctorDetProcess);

	G4bool GetFirstDetectorHit(G4String); 
	void SetDeposit(G4double, G4String);
	void SetOutsideDetector(G4String);

	G4int GetTrackID() {return trackID;}
	G4int GetParentID() {return parentID;}
	G4int GetPDG() {return pdg;}
	G4ThreeVector& GetOriginPoint() {return originPoint;}
	G4double GetOriginTime() {return originTime;}
	G4double GetOriginKineticEnergy() {return originKineticEnergy;}
	G4String GetOriginVolumeName() {G4String name = originVolume->GetName(); return name;}
	G4String GetCreatorProcess() {return creatorProcess;}
	G4int GetOriginNucleusA() {return originNucleusA;}
	G4int GetOriginNucleusZ() {return originNucleusZ;}
	G4String GetDetectorID() {return detectorId;}
	G4ThreeVector& GetEntrancePoint() {return entrancePoint;}
	G4ThreeVector& GetEntranceDirection() {return entranceDirection;}
	G4double GetDetectorTime() {return detectorTime;}
	G4double GetDetectorKineticEnergy() {return detectorKineticEnergy;}
	G4bool GetStopInTheDetector() {return stopInTheDetector;}
	G4String GetStopInDetectorID() {return stopInDetectorID;}
	G4String GetReactionInTheDetector() {return reactionInTheDetector;}
	G4double GetWeight() {return weight;}
	G4double GetDeposit() {return deposit;}
	G4double GetDetectorDeposit(G4String);
	G4bool GetOutsideDetector(G4String);
	G4bool GetOutsideDetector() {return isOutside;}
	G4double GetDetectorKineticEnergy(G4String);
	G4double GetDetectorTime(G4String);
	G4ThreeVector& GetEntrancePoint(G4String);
	G4ThreeVector& GetEntranceDirection(G4String);
	G4double GetWeight(G4String);

        void Write(std::ostream& outs); 

    private:
	G4int trackID;
	G4int parentID;
	G4int pdg;
	G4ThreeVector originPoint;
	G4double originTime;
	G4double originKineticEnergy;
	G4VPhysicalVolume* originVolume;
	G4String creatorProcess;
	G4int originNucleusA;
	G4int originNucleusZ;
	G4String detectorId;
	G4ThreeVector entrancePoint;
	G4ThreeVector entranceDirection;
	G4double detectorTime;
	G4double detectorKineticEnergy;
	G4bool stopInTheDetector;
	G4String stopInDetectorID;
	G4String reactionInTheDetector;
	G4double weight;
	G4double deposit;
	G4bool isOutside;
	std::vector<std::string> m_Detector;
	std::vector<nwlDetRecord> detRecord;
};

typedef std::vector<nwlParticleInfo> nwlParticleInfoVector;

#endif
