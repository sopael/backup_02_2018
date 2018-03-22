#ifndef MyAsymFBProc_h
#define MyAsymFBProc_h 1

#include "marlin/Processor.h"
#include "EventInfo.h"
#include "lcio.h"
#include <string>
#include <vector>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TFile.h>

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>

using namespace lcio ;
using namespace marlin ;

class AsymFBProc : public Processor {
 
 public:
  
  //built-in methods  
  virtual Processor*  newProcessor() { return new AsymFBProc ; }
  virtual void init() ;
  virtual void processRunHeader( LCRunHeader* run ) ;
  virtual void processEvent( LCEvent * evt ) ; 
  virtual void check( LCEvent * evt ) ; 
  virtual void end() ;
  //end of built-in

  AsymFBProc() ;

  bool findGeneratedQuarks();
  bool findGeneratedNeutrinos();
  bool findMCBeamstrahlungPhotons();
  bool findGenElec();
  void boostCalcParameters();
  void boostParticle(TLorentzVector*, std::string);
  void calcGenVariables();
  void calcStandardPandoraVariables(std::string);
  void calcFastJetVars(std::string);
  void calcHemVars(std::string);
  void calcTagJetsVars(std::string);
  void calcVrtxVars(std::string);
  void calcThrustVars(std::string);
  void findCoMBoost(TLorentzVector,TLorentzVector);
  LCCollection* openCollection(std::string);
  ReconstructedParticle* getRecoElec();

  void resetObjects();

  void calcRecParticlesBasicInfo(std::string);//dla studentow,podstawowe parametry z skladowych RefFets

  EventInfo _evInfo;  

  //'inherited', to be revised:
  int motherMC(LCObject* p);


 protected:

  TLorentzVector* makeTLVec(LCObject*,std::string);
  LCEvent* _evt ; 
  TFile* _fOutFile;
  std::string _colMCName ;
  std::string _outFileName ; 
  std::string _colFastJetName ;
  std::string _colVtxName ;
  std::string _colTagJetsName ;
  std::string _colPandName ;
  std::string _colPandName_ISRFree ;
  std::vector<EVENT::MCParticle*> _genQuarks;
  std::vector<EVENT::MCParticle*> _genElectrons;
  std::vector<EVENT::MCParticle*> _genQuarksShvr;
  std::vector<EVENT::MCParticle*> _genNeutrinos;
  std::vector<EVENT::MCParticle*> _genStrahlung;
 
  TVector3 _mcBoost, _mcBoostAxis;
  double _mcBoostTheta;
 
  int _nRun ;
  int _nEvt ;
} ;

#endif



