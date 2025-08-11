#include "nwlEventAction.hh"

#include "nwlRunAction.hh"
#include "nwlAnalysis.hh"
#include "nwlSteppingAction.hh"
#include "nwlParticleSource.hh"
#include "nwlConfigParser.hh"

#include <time.h>

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"

nwlEventAction::nwlEventAction(nwlRunAction* runAction)
    : G4UserEventAction(),
    fPrintModulo(100),
    fRunAction(runAction)
{ }

nwlEventAction::~nwlEventAction()
{ }

void nwlEventAction::BeginOfEventAction(const G4Event* event)
{
    G4long total = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
    fPrintModulo = G4int(total / 20);
    if (fPrintModulo < 1) fPrintModulo = 1;

    G4int eventNb = event->GetEventID();
    if (eventNb % fPrintModulo == 0) {
        char outstr[200];
        time_t t;
        struct tm* tmp;

        t = time(NULL);
        tmp = localtime(&t);
        strftime(outstr, sizeof(outstr), "%H%M%S.%d%m%Y", tmp);

        G4cout << "\n---> Begin of event: " << eventNb << " " << outstr << G4endl;
    }

    Reset();
}

void nwlEventAction::StoreParticleInfo(nwlParticleInfo& pinfo)
{
    particles.push_back(pinfo);
}

void nwlEventAction::EndOfEventAction(const G4Event* event)
{
    // Store particle info to file and histo
    auto analysisManager = G4AnalysisManager::Instance();

    nwlConfigParser* cfg = nwlConfigParser::Instance();

    std::vector<std::string> m_Detectors;
    cfg->GetDetector(m_Detectors);

    nwlParticleInfoVector::iterator it;

    // one may put some selection logic here
    //     (*it).Write(ra->GetStream());
    if (cfg->CreateH1())
    {
        const std::map<G4int, std::string>& h1map = fRunAction->GetH1map();
        for (it = particles.begin(); it != particles.end(); ++it)
        {
            std::map<G4int, std::string>::const_iterator h1;
            for (h1 = h1map.begin(); h1 != h1map.end(); ++h1)
            {
                G4double val;
                std::string PhysQ = (*h1).second.substr(0, (*h1).second.find("_"));
                std::string detId = (*h1).second.substr((*h1).second.find("_")+1, (*h1).second.size());
                // G4cout << (*h1).second << G4endl;
                if (PhysQ == "Energy") { val = (*it).GetDetectorKineticEnergy(detId); }
                else if (PhysQ == "Time") { val = (*it).GetDetectorTime(detId); }
                else if (PhysQ == "X") { val = (*it).GetOriginPoint().x(); }
                else if (PhysQ == "Y") { val = (*it).GetOriginPoint().y(); }
                else if (PhysQ == "Z") { val = (*it).GetOriginPoint().z(); }
                else if (PhysQ == "ProcessID") { val = fRunAction->GetProcessID((*it).GetCreatorProcess()); }
                else if (PhysQ == "NucleusA") { val = (*it).GetOriginNucleusA(); }
                else if (PhysQ == "NucleusZ") { val = (*it).GetOriginNucleusZ(); }
                else if (PhysQ == "DetectorID") { val = fRunAction->GetDetectorID((*it).GetDetectorID()); }
                else if (PhysQ == "PDG") { val = (*it).GetPDG(); }
                else if (PhysQ == "Deposit") { val = GetDetectorTotalDeposit(&(*it), detId);}
                G4double weight = (*it).GetWeight();
                analysisManager->FillH1((*h1).first, val, weight);
            }
        }
    }

    if (cfg->CreateH2())
    {
        const std::map<G4int, std::pair<std::string, std::string> >& h2map = fRunAction->GetH2map();
        for (it = particles.begin(); it != particles.end(); ++it)
        {
            std::map<G4int, std::pair<std::string, std::string> >::const_iterator h2;
            for (h2 = h2map.begin(); h2 != h2map.end(); ++h2)
            {
                G4double valx = 0, valy = 0;
                if ((*h2).second.first == "Energy") { valx = (*it).GetDetectorKineticEnergy(); }
                else if ((*h2).second.first == "Time") { valx = (*it).GetDetectorTime(); }
                else if ((*h2).second.first == "X") { valx = (*it).GetOriginPoint().x(); }
                else if ((*h2).second.first == "Y") { valx = (*it).GetOriginPoint().y(); }
                else if ((*h2).second.first == "Z") { valx = (*it).GetOriginPoint().z(); }
                else if ((*h2).second.first == "ProcessID")
                {
                    valx = fRunAction->GetProcessID((*it).GetCreatorProcess());
                }
                else if ((*h2).second.first == "NucleusA") { valx = (*it).GetOriginNucleusA(); }
                else if ((*h2).second.first == "NucleusZ") { valx = (*it).GetOriginNucleusZ(); }
                else if ((*h2).second.first == "DetectorID")
                {
                    valx = fRunAction->GetDetectorID((*it).GetDetectorID());
                }
                else if ((*h2).second.first == "PDG") { valx = (*it).GetPDG(); }
                else if ((*h2).second.first == "Deposit") { valx = GetTotalDeposit(&(*it)); }

                if ((*h2).second.second == "Energy") { valy = (*it).GetDetectorKineticEnergy(); }
                else if ((*h2).second.second == "Time") { valy = (*it).GetDetectorTime(); }
                else if ((*h2).second.second == "X") { valy = (*it).GetOriginPoint().x(); }
                else if ((*h2).second.second == "Y") { valy = (*it).GetOriginPoint().y(); }
                else if ((*h2).second.second == "Z") { valy = (*it).GetOriginPoint().z(); }
                else if ((*h2).second.second == "ProcessID")
                {
                    valy = fRunAction->GetProcessID((*it).GetCreatorProcess());
                }
                else if ((*h2).second.second == "NucleusA") { valy = (*it).GetOriginNucleusA(); }
                else if ((*h2).second.second == "NucleusZ") { valy = (*it).GetOriginNucleusZ(); }
                else if ((*h2).second.second == "DetectorID")
                {
                    valy = fRunAction->GetDetectorID((*it).GetDetectorID());
                }
                else if ((*h2).second.second == "PDG") { valy = (*it).GetPDG(); }
                else if ((*h2).second.second == "Deposit") { valy = GetTotalDeposit(&(*it)); }

                if ((*h2).second.first == "DEDX" && (*h2).second.second == "DetectorID")
                {
                    valx = 0;
                    valy = fRunAction->GetDetectorID((*it).GetDetectorID());
                }

                G4double weight = (*it).GetWeight();
                analysisManager->FillH2((*h2).first, valx, valy, weight);
            }
        }
    }

    if (cfg->WriteNtuple())
    {
        for (it = particles.begin(); it != particles.end(); ++it)
        {

            if (!cfg->StoreAllParticles())
            {
                if (it->GetPDG() != PDG_GAMMA && it->GetPDG() != PDG_NEUTRON) continue;
            }

            if (!cfg->StoreDetectorMissed()) {
                if (it->GetDetectorID() == "" && !it->GetStopInTheDetector()) continue; // if particle neither crossed nor interacted in the detector
            }

            G4int counter = 0;
            analysisManager->FillNtupleIColumn(counter++, event->GetEventID());
            analysisManager->FillNtupleIColumn(counter++, it->GetTrackID());
            analysisManager->FillNtupleIColumn(counter++, it->GetParentID());
            analysisManager->FillNtupleIColumn(counter++, it->GetPDG());
            analysisManager->FillNtupleDColumn(counter++, it->GetOriginPoint().x());
            analysisManager->FillNtupleDColumn(counter++, it->GetOriginPoint().y());
            analysisManager->FillNtupleDColumn(counter++, it->GetOriginPoint().z());
            analysisManager->FillNtupleDColumn(counter++, it->GetOriginTime());
            analysisManager->FillNtupleDColumn(counter++, it->GetOriginKineticEnergy());
            analysisManager->FillNtupleSColumn(counter++, it->GetOriginVolumeName());
            analysisManager->FillNtupleSColumn(counter++, it->GetCreatorProcess());
            analysisManager->FillNtupleIColumn(counter++, it->GetOriginNucleusA());
            analysisManager->FillNtupleIColumn(counter++, it->GetOriginNucleusZ());
            analysisManager->FillNtupleSColumn(counter++, it->GetDetectorID());
            analysisManager->FillNtupleDColumn(counter++, it->GetEntrancePoint().x());
            analysisManager->FillNtupleDColumn(counter++, it->GetEntrancePoint().y());
            analysisManager->FillNtupleDColumn(counter++, it->GetEntrancePoint().z());
            analysisManager->FillNtupleDColumn(counter++, it->GetEntranceDirection().x());
            analysisManager->FillNtupleDColumn(counter++, it->GetEntranceDirection().y());
            analysisManager->FillNtupleDColumn(counter++, it->GetEntranceDirection().z());
            analysisManager->FillNtupleDColumn(counter++, it->GetDetectorTime());
            analysisManager->FillNtupleDColumn(counter++, it->GetDetectorKineticEnergy());
            analysisManager->FillNtupleSColumn(counter++, it->GetStopInDetectorID());
            analysisManager->FillNtupleSColumn(counter++, it->GetReactionInTheDetector());
            analysisManager->FillNtupleDColumn(counter++, it->GetWeight());
            for (std::vector<std::string>::iterator itDet = m_Detectors.begin(); itDet != m_Detectors.end(); itDet++) {
                analysisManager->FillNtupleDColumn(counter++, GetDetectorTotalDeposit(&(*it), *itDet));
            }

            nwlParticleInfo* parentNeutron = getParentNeutronParticle(&(*it));
            if (parentNeutron != NULL) {
                analysisManager->FillNtupleIColumn(counter++, parentNeutron->GetTrackID());
                analysisManager->FillNtupleDColumn(counter++, parentNeutron->GetOriginPoint().x());
                analysisManager->FillNtupleDColumn(counter++, parentNeutron->GetOriginPoint().y());
                analysisManager->FillNtupleDColumn(counter++, parentNeutron->GetOriginPoint().z());
                analysisManager->FillNtupleDColumn(counter++, parentNeutron->GetOriginTime());
                analysisManager->FillNtupleDColumn(counter++, parentNeutron->GetOriginKineticEnergy());
                analysisManager->FillNtupleSColumn(counter++, parentNeutron->GetOriginVolumeName());
                analysisManager->FillNtupleSColumn(counter++, parentNeutron->GetCreatorProcess());
                analysisManager->FillNtupleIColumn(counter++, parentNeutron->GetOriginNucleusA());
                analysisManager->FillNtupleIColumn(counter++, parentNeutron->GetOriginNucleusZ());
            }

            analysisManager->AddNtupleRow();
        }
    }

    Reset();

}

void nwlEventAction::Reset()
{
    particles.clear();
}

nwlParticleInfo* nwlEventAction::getParticleByTrackId(G4int trackID) {
    for (nwlParticleInfoVector::iterator it = particles.begin(); it != particles.end(); ++it)
    {
        if (it->GetTrackID() == trackID) return &(*it);
    }

    return NULL;
}

nwlParticleInfo* nwlEventAction::getParentNeutronParticle(nwlParticleInfo* p) {
    while (p != NULL) {
        if (p->GetCreatorProcess() == PROCESS_NCAPTURE || p->GetCreatorProcess() == PROCESS_NEUTRONINELASTIC) {
            return p;
        }

        p = getParticleByTrackId(p->GetParentID());
    } 

    return NULL;
}

G4double nwlEventAction::GetTotalDeposit(nwlParticleInfo* p) {
    G4double dE = p->GetDeposit();
    std::vector<G4int> particleID;
    particleID.push_back(p->GetTrackID());
    if (p->GetOutsideDetector()) {
        for (nwlParticleInfoVector::iterator it = particles.begin(); it != particles.end(); ++it) {
            for (G4int i = 0; i < particleID.size(); ++i) {
                if (it->GetParentID() == particleID[i]) {
                    dE += it->GetDeposit();
                    particleID.push_back(it->GetTrackID());
                }
            }
        }
    } else return -1;
    return dE;
}

G4double nwlEventAction::GetDetectorTotalDeposit(nwlParticleInfo* p, G4String detId) {
    G4double dE = p->GetDetectorDeposit(detId);
    std::vector<G4int> particleID;
    particleID.push_back(p->GetTrackID());
    if (p->GetOutsideDetector(detId)) {
        for (nwlParticleInfoVector::iterator it = particles.begin(); it != particles.end(); ++it) {
            for (G4int i = 0; i < particleID.size(); ++i) {
                if (it->GetParentID() == particleID[i]) {
                    dE += it->GetDetectorDeposit(detId);
                    particleID.push_back(it->GetTrackID());
                }
            }
        }
    } else return -1;
    return dE;
}