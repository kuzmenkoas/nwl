#ifndef nwlEventAction_h
#define nwlEventAction_h 1

#include "G4UserEventAction.hh"
#include "nwlParticleInfo.hh"

const G4int PDG_NEUTRON = 2112;
const G4int PDG_GAMMA = 22;
const G4String PROCESS_NCAPTURE = "nCapture";
const G4String PROCESS_NEUTRONINELASTIC = "neutronInelastic";

class nwlRunAction;

class nwlEventAction : public G4UserEventAction
{
public:
  nwlEventAction(nwlRunAction*);
  virtual ~nwlEventAction();
  
  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  nwlRunAction* GetRunAction(){return fRunAction;};
  void StoreParticleInfo(nwlParticleInfo& pinfo);
  
  void Reset();
  
private:
  G4int     fPrintModulo;
  nwlRunAction* fRunAction;  
  nwlParticleInfoVector particles;
  nwlParticleInfo* getParticleByTrackId(G4int trackID);
  nwlParticleInfo* getParentNeutronParticle(nwlParticleInfo* p);
};

#endif

    
