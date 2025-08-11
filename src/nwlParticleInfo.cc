#include "nwlParticleInfo.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"
#include "nwlConfigParser.hh"

using namespace CLHEP;

nwlParticleInfo::nwlParticleInfo()
{
	trackID = 0;
	parentID = 0;
        pdg = 0;
        originTime = 0;
        originKineticEnergy = 0;
        creatorProcess = "";
        originNucleusA = 0;
        originNucleusZ = 0;
        detectorId = "";
        detectorTime = 0;
        detectorKineticEnergy = 0;
        stopInTheDetector = false;
        stopInDetectorID = "";
        reactionInTheDetector = "";
        weight = 0;
        deposit = 0;
        isOutside = false;

        nwlConfigParser::Instance()->GetDetector(m_Detector);
        for (std::vector<std::string>::iterator it = m_Detector.begin(); it != m_Detector.end(); it++) {
                nwlDetRecord tmp{false, *it, 0, 0, G4ThreeVector(0,0,0), G4ThreeVector(0,0,0), 0, 0, false};
                detRecord.push_back(tmp);
        }
}

nwlParticleInfo::~nwlParticleInfo()
{}

void nwlParticleInfo::Print() const
{
  G4cout << "******" << G4endl
         << "TrackID =    " << trackID << G4endl
         << "ParentID =    " << parentID << G4endl 
	 << "PDG          " << pdg  << G4endl
         << "OriginXYZ    " << originPoint << G4endl
	 << "OriginTime   " << originTime << G4endl
	 << "OriginKine   " << originKineticEnergy << G4endl
	 << "OriginVolu   " << originVolume->GetName() << G4endl
	 << "OriginReac   " << creatorProcess << G4endl
	 << "     A   Z   " << originNucleusA << " " << originNucleusZ << G4endl
	 << "DetectorID   " << detectorId << G4endl
	 << "DetectorTime " << detectorTime << G4endl
  	 << "DetectorKine " << detectorKineticEnergy  << G4endl
         << "EntranceXYZ  " << entrancePoint << G4endl
	 << "EntranceDir  " << entranceDirection << G4endl
	 << "StopInDet    " << stopInTheDetector << " " << stopInDetectorID   << G4endl
	 << "      reac   " << reactionInTheDetector  << G4endl
	 << "Weight       " << weight << G4endl;
}

void nwlParticleInfo::Write(std::ostream& outs)
{
   outs << std::scientific
	<< pdg  << ", "
        << originPoint.x()/mm << ", "
	<< originPoint.y()/mm << ", "
	<< originPoint.z()/mm << ", "
	<< originTime/ns << ", "
        << originKineticEnergy/MeV << ", "
        << originVolume->GetName() << ", " 
        << creatorProcess << ", "
        << originNucleusA << ", "
	<< originNucleusZ << ", "
	<< detectorId << ", "
	<< detectorTime/ns << ", "
	<< detectorKineticEnergy/MeV  << ", "
        << entrancePoint.x()/mm << ", "
        << entrancePoint.y()/mm << ", "
        << entrancePoint.z()/mm << ", "
        << entranceDirection.x() << ", "
        << entranceDirection.y() << ", "
        << entranceDirection.z() << ", "
        << stopInDetectorID   << ", "
        << reactionInTheDetector  << ", "
	<< weight << std::endl;
}

void nwlParticleInfo::SetOriginInfo(G4int TrackID, G4int ParentID, G4int Pdg, G4double Kine, G4double Time, G4ThreeVector OriginPoint, G4VPhysicalVolume* vol, G4String CreatorProcess, G4int nA, G4int nZ)
{
	trackID = TrackID;
	parentID = ParentID;
	pdg = Pdg;
	originKineticEnergy = Kine;
	originTime = Time;
	originPoint = OriginPoint;
	originVolume = vol;
	creatorProcess = CreatorProcess;
	originNucleusA = nA;
	originNucleusZ = nZ;
}

void nwlParticleInfo::SetDetectorInfo(G4String DetId, G4double Time, G4double Kine, G4ThreeVector EntrancePoint, G4ThreeVector EntranceDir, G4double W)
{
  detectorId = DetId;
  detectorTime = Time;
  detectorKineticEnergy = Kine;
  entrancePoint = EntrancePoint;
  entranceDirection = EntranceDir;
  weight = W;
  for (G4int i = 0; i < detRecord.size(); i++) {
        if (detRecord[i].detectorId == DetId) {
                detRecord[i].firstHit = true;
                detRecord[i].detectorTime = Time;
                detRecord[i].detectorKineticEnergy = Kine;
                detRecord[i].entrancePoint = EntrancePoint;
                detRecord[i].entranceDirection = EntranceDir;
                detRecord[i].weight = W;
        }
  }
}

void nwlParticleInfo::SetFinalInfo(G4bool StopInDet, G4String detId, G4String DetProcess)
{
   stopInTheDetector = StopInDet;
   stopInDetectorID = detId;
   reactionInTheDetector = DetProcess;
}


void nwlParticleInfo::SetDeposit(G4double dE, G4String DetId) {
        deposit += dE;
        for (G4int i = 0; i < detRecord.size(); i++) {
        if (detRecord[i].detectorId == DetId) {
                detRecord[i].Deposit += dE;
        }
  }
}

G4bool nwlParticleInfo::GetFirstDetectorHit(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].firstHit;
        }
}

void nwlParticleInfo::SetOutsideDetector(G4String detId) {
        isOutside = true;
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) detRecord[i].isOutside = true;
        }
}

G4double nwlParticleInfo::GetDetectorDeposit(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].Deposit;
        }
}

G4bool nwlParticleInfo::GetOutsideDetector(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].isOutside;
        }
}

G4double nwlParticleInfo::GetDetectorKineticEnergy(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].detectorKineticEnergy;
        }
}

G4double nwlParticleInfo::GetDetectorTime(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].detectorTime;
        }
}

G4ThreeVector& nwlParticleInfo::GetEntrancePoint(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].entrancePoint;
        }
}

G4ThreeVector& nwlParticleInfo::GetEntranceDirection(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].entranceDirection;
        }
}

G4double nwlParticleInfo::GetWeight(G4String detId) {
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId == detId) return detRecord[i].weight;
        }
}