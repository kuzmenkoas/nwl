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
                if ((*it).GetFirstDetectorHit(detId)) { // Fill data only if particles got outside from detector
                    if (PhysQ == "Energy") { val = (*it).GetDetectorKineticEnergy(detId); }
                    else if (PhysQ == "Time") { val = (*it).GetDetectorTime(detId); }
                    else if (PhysQ == "X") { val = (*it).GetOriginPoint().x(); }
                    else if (PhysQ == "Y") { val = (*it).GetOriginPoint().y(); }
                    else if (PhysQ == "Z") { val = (*it).GetOriginPoint().z(); }
                    else if (PhysQ == "ProcessID") { val = fRunAction->GetProcessID((*it).GetCreatorProcess()); }
                    else if (PhysQ == "NucleusA") { val = (*it).GetOriginNucleusA(); }
                    else if (PhysQ == "NucleusZ") { val = (*it).GetOriginNucleusZ(); }
                    else if (PhysQ == "PDG") { val = (*it).GetPDG(); }
                    else if (PhysQ == "Deposit") { val = GetDetectorTotalDeposit(&(*it), detId);}
                    G4double weight = (*it).GetWeight();
                    analysisManager->FillH1((*h1).first, val, weight);
                }
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
                std::string PhysQ_x = (*h2).second.first.substr(0, (*h2).second.first.find("_"));
                std::string PhysQ_y = (*h2).second.second.substr(0, (*h2).second.second.find("_"));
                std::string detId = (*h2).second.first.substr((*h2).second.first.find("_")+1, (*h2).second.first.size());
                
                if ((*it).GetFirstDetectorHit(detId)) { // Fill data only if particles got outside from detector
                    if (PhysQ_x == "Energy") { valx = (*it).GetDetectorKineticEnergy(detId); }
                    else if (PhysQ_x == "Time") { valx = (*it).GetDetectorTime(detId); }
                    else if (PhysQ_x == "X") { valx = (*it).GetOriginPoint().x(); }
                    else if (PhysQ_x == "Y") { valx = (*it).GetOriginPoint().y(); }
                    else if (PhysQ_x == "Z") { valx = (*it).GetOriginPoint().z(); }
                    else if (PhysQ_x == "ProcessID")
                    {
                        valx = fRunAction->GetProcessID((*it).GetCreatorProcess());
                    }
                    else if (PhysQ_x == "NucleusA") { valx = (*it).GetOriginNucleusA(); }
                    else if (PhysQ_x == "NucleusZ") { valx = (*it).GetOriginNucleusZ(); }
                    else if (PhysQ_x == "PDG") { valx = (*it).GetPDG(); }
                    else if (PhysQ_x == "Deposit") { valx = GetDetectorTotalDeposit(&(*it), detId); }

                    if (PhysQ_y == "Energy") { valy = (*it).GetDetectorKineticEnergy(detId); }
                    else if (PhysQ_y == "Time") { valy = (*it).GetDetectorTime(detId); }
                    else if (PhysQ_y == "X") { valy = (*it).GetOriginPoint().x(); }
                    else if (PhysQ_y == "Y") { valy = (*it).GetOriginPoint().y(); }
                    else if (PhysQ_y == "Z") { valy = (*it).GetOriginPoint().z(); }
                    else if (PhysQ_y == "ProcessID")
                    {
                        valy = fRunAction->GetProcessID((*it).GetCreatorProcess());
                    }
                    else if (PhysQ_y == "NucleusA") { valy = (*it).GetOriginNucleusA(); }
                    else if (PhysQ_y == "NucleusZ") { valy = (*it).GetOriginNucleusZ(); }
                    else if (PhysQ_y == "PDG") { valy = (*it).GetPDG(); }
                    else if (PhysQ_y == "Deposit") { valy = GetDetectorTotalDeposit(&(*it), detId); }

                    G4double weight = (*it).GetWeight();
                    analysisManager->FillH2((*h2).first, valx, valy, weight);
                }
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

            G4int iNtuple = 0;
            for (std::vector<std::string>::iterator itDet = m_Detectors.begin(); itDet != m_Detectors.end(); itDet++) {
                if (!cfg->StoreDetectorMissed()) {
                    if (!it->GetFirstDetectorHit(*itDet) && !it->GetStopInTheDetector(*itDet)) continue; // if particle neither crossed nor interacted in the detector
                }
                G4int counter = 0;
                analysisManager->FillNtupleIColumn(iNtuple, counter++, event->GetEventID());
                analysisManager->FillNtupleIColumn(iNtuple, counter++, it->GetTrackID());
                analysisManager->FillNtupleIColumn(iNtuple, counter++, it->GetParentID());
                analysisManager->FillNtupleIColumn(iNtuple, counter++, it->GetPDG());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetOriginPoint().x());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetOriginPoint().y());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetOriginPoint().z());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetOriginTime());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetOriginKineticEnergy());
                analysisManager->FillNtupleSColumn(iNtuple, counter++, it->GetOriginVolumeName());
                analysisManager->FillNtupleSColumn(iNtuple, counter++, it->GetCreatorProcess());
                analysisManager->FillNtupleIColumn(iNtuple, counter++, it->GetOriginNucleusA());
                analysisManager->FillNtupleIColumn(iNtuple, counter++, it->GetOriginNucleusZ());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetEntrancePoint(*itDet).x());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetEntrancePoint(*itDet).y());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetEntrancePoint(*itDet).z());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetEntranceDirection(*itDet).x());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetEntranceDirection(*itDet).y());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetEntranceDirection(*itDet).z());
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetDetectorTime(*itDet));
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetDetectorKineticEnergy(*itDet));
                analysisManager->FillNtupleSColumn(iNtuple, counter++, it->GetStopInDetectorID());
                analysisManager->FillNtupleSColumn(iNtuple, counter++, it->GetReactionInTheDetector(*itDet));
                analysisManager->FillNtupleDColumn(iNtuple, counter++, it->GetWeight(*itDet));
                analysisManager->FillNtupleDColumn(iNtuple, counter++, GetDetectorTotalDeposit(&(*it), *itDet));
            

                nwlParticleInfo* parentNeutron = getParentNeutronParticle(&(*it));
                if (parentNeutron != NULL) {
                    analysisManager->FillNtupleIColumn(iNtuple, counter++, parentNeutron->GetTrackID());
                    analysisManager->FillNtupleDColumn(iNtuple, counter++, parentNeutron->GetOriginPoint().x());
                    analysisManager->FillNtupleDColumn(iNtuple, counter++, parentNeutron->GetOriginPoint().y());
                    analysisManager->FillNtupleDColumn(iNtuple, counter++, parentNeutron->GetOriginPoint().z());
                    analysisManager->FillNtupleDColumn(iNtuple, counter++, parentNeutron->GetOriginTime());
                    analysisManager->FillNtupleDColumn(iNtuple, counter++, parentNeutron->GetOriginKineticEnergy());
                    analysisManager->FillNtupleSColumn(iNtuple, counter++, parentNeutron->GetOriginVolumeName());
                    analysisManager->FillNtupleSColumn(iNtuple, counter++, parentNeutron->GetCreatorProcess());
                    analysisManager->FillNtupleIColumn(iNtuple, counter++, parentNeutron->GetOriginNucleusA());
                    analysisManager->FillNtupleIColumn(iNtuple, counter++, parentNeutron->GetOriginNucleusZ());
                }

                analysisManager->AddNtupleRow(iNtuple++);
            }
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