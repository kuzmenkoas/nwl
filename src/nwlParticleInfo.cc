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
                nwlDetRecord tmp{false, *it, 0, 0, G4ThreeVector(0,0,0), G4ThreeVector(0,0,0), 0, 0, false, false, "", ""};
                detRecord.push_back(tmp);
        }
        // Add for a case if detId will not found
        nwlDetRecord tmp{false, "", 0, 0, G4ThreeVector(0,0,0), G4ThreeVector(0,0,0), 0, 0, false, false, "", ""};
        detRecord.push_back(tmp);
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
  const G4int id = GetDetectorRecordID(DetId); 
  detRecord[id].firstHit = true;
  detRecord[id].detectorTime = Time;
  detRecord[id].detectorKineticEnergy = Kine;
  detRecord[id].entrancePoint = EntrancePoint;
  detRecord[id].entranceDirection = EntranceDir;
  detRecord[id].weight = W;
}

void nwlParticleInfo::SetFinalInfo(G4bool StopInDet, G4String detId, G4String DetProcess)
{
   stopInTheDetector = StopInDet;
   stopInDetectorID = detId;
   reactionInTheDetector = DetProcess;
   const G4int id = GetDetectorRecordID(detId); 
   detRecord[id].stopInTheDetector = StopInDet;
   detRecord[id].stopInDetectorID = detId;
   detRecord[id].reactionInTheDetector = DetProcess;
}


void nwlParticleInfo::SetDeposit(G4double dE, G4String DetId) {
        deposit += dE;
        detRecord[GetDetectorRecordID(DetId)].Deposit += dE;
}

G4bool nwlParticleInfo::GetFirstDetectorHit(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].firstHit;
}

void nwlParticleInfo::SetOutsideDetector(G4String detId) {
        isOutside = true;
        detRecord[GetDetectorRecordID(detId)].isOutside = true;
}

G4double nwlParticleInfo::GetDetectorDeposit(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].Deposit;
}

G4bool nwlParticleInfo::GetOutsideDetector(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].isOutside;
}

G4double nwlParticleInfo::GetDetectorKineticEnergy(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].detectorKineticEnergy;
}

G4double nwlParticleInfo::GetDetectorTime(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].detectorTime;
}

G4ThreeVector& nwlParticleInfo::GetEntrancePoint(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].entrancePoint;
}

G4ThreeVector& nwlParticleInfo::GetEntranceDirection(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].entranceDirection;
}

G4double nwlParticleInfo::GetWeight(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].weight;
}

G4bool nwlParticleInfo::GetStopInTheDetector(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].stopInTheDetector;
}

// Send detector name, where track killed
G4String nwlParticleInfo::GetStopInDetectorID() {
        G4String res = "";
        for (G4int i = 0; i < detRecord.size(); i++) {
                if (detRecord[i].detectorId != res) {
                        res = detRecord[i].detectorId;
                        return res;
                }
        }
        return res;
}

G4String nwlParticleInfo::GetReactionInTheDetector(G4String detId) {
        return detRecord[GetDetectorRecordID(detId)].reactionInTheDetector;
}

G4int nwlParticleInfo::GetDetectorRecordID(G4String detId) {
        G4int res = 0;
        for (G4int i = 0; i < detRecord.size()-1; i++) {
                if (detRecord[i].detectorId == detId) {
                        res = i;
                        return res;
                }
        }
        return detRecord.size(); // protect return empty data, if detector not found
}