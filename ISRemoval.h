#ifndef ISRemoval_h
#define ISRemoval_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <math.h> 

#include <TH1.h>
#include <TObjArray.h>
#include <TString.h>
#include <TTree.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/LCCollection.h>

//#include "ISRData.h"

using namespace lcio ;
using namespace marlin ;


/**  Example processor for marlin.
 * 
 *  If compiled with MARLIN_USE_AIDA 
 *  it creates a histogram (cloud) of the MCParticle energies.
 * 
 *  <h4>Input - Prerequisites</h4>
 *  Needs the collection of MCParticles.
 *
 *  <h4>Output</h4> 
 *  A histogram.
 * 
 * @param CollectionName Name of the MCParticle collection
 * 
 * @author F. Gaede, DESY
 * @version $Id: ISRemoval.h,v 1.4 2005-10-11 12:57:39 gaede Exp $ 
 */

class ISRemoval : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new ISRemoval ; }
  
  bool isIsolatedCone( ReconstructedParticle* p );
  bool isIsolatedFromTrack( ReconstructedParticle* p );
  
  ISRemoval() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;

  //#######  convenience functions
  //  void addHist(TH1* hist, TString* name="defName", TString* title="")
  //######  
  
 protected:

  /** Input collection name.
   */
  std::string _colName ;
  std::string _colOutName ;
  std::string _colOutData ;

  std::string _colPandName ;

  int _nRun ;
  int _nEvt ;

  //void addAllEnergies(ISRData* isrD);

  float _photE1, _photE2;
  float _photTh1, _photTh2;
  int _photPV;
 
  float _phot_genE1, _phot_genE2;
  float _phot_genTh1, _phot_genTh2;

  float _dRConeCut, _cosConeAngle, _coneERatioCut;
  float _dRConeE, _cosThetaConeE, _refE;
  bool _bDRIsolated, _bCosIsolated;

 
  std::string _outFileName;
  std::string _islCrt;

  TTree* _Tree;

  LCCollection* _pfoCol;
 
} ;

#endif



