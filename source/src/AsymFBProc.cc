#include "AsymFBProc.h"
#include "ISRemoval.h"
#include <EventShape.h>
#include <iostream>
#include <cmath>

#include <EVENT/LCCollection.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <UTIL/PIDHandler.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/Vertex.h>

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

#include <TFile.h>
#include <TH2.h>
#include <TLorentzVector.h>
#include <TVector3.h>

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TMath.h"

using namespace lcio ;
using namespace marlin ;


AsymFBProc aAsymFBProc ;

AsymFBProc::AsymFBProc() : Processor("AsymFBProc") {

    _description = "AsymFBProc does whatever it does ..." ;
 
    //****** collections
   registerInputCollection( LCIO::MCPARTICLE,
            "CollectionName" , 
            "Name of the MCParticle collection"  ,
            _colMCName ,
            std::string("MCParticle")
    );

   registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			    "jetOut", 
			    "jets identified by FastJet algo",
			    _colFastJetName,
			    std::string("JetOut")
    );
 
   registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			    "tagJets", 
			    "jets used for b/c tagging (RefinedJets)",
			    _colTagJetsName,
			    std::string("RefinedJets")
    );

   registerInputCollection( LCIO::VERTEX,
			    "inVtx", 
			    "collection to look for vertices into",
			    _colVtxName,
			    std::string("BuildUpVertex")
			    );
   //used in optional calculating thrust, using particles' momenta in collection instead of tracks from jet coll
   registerInputCollection(  LCIO::RECONSTRUCTEDPARTICLE,
			     "recInPand_ISRFree" ,
			     "Name of the input Pandora collection after ISR Removal"  ,
			     _colPandName_ISRFree ,
			     std::string("ISRFreePandora")
			     );
 

   registerInputCollection(  LCIO::RECONSTRUCTEDPARTICLE,
			     "recInPand" ,
			     "Name of the standard input Pandora collection"  ,
			     _colPandName ,
			     std::string("SelectedPandoraPFANewPFOs")
			     );

   //****** opt/proc parameters
   
   registerOptionalParameter("outputRootName",
			     "sets the name of the output root file",
			     _outFileName,
			     std::string("AsymFBProc.root")
			     );
   
}



void AsymFBProc::init() { 

  streamlog_out(DEBUG) << "   AsymFBProc init called  " << std::endl ;

  TString name(_outFileName);
  _fOutFile = new TFile(name,"RECREATE");

  printParameters() ;
  _nRun = 0 ;
  _nEvt = 0 ;
  
  _evInfo.AddTree("genFull_1");
  _evInfo.AddTree("recFull_1");
  _evInfo.AddTree("main_1");
  
}


void AsymFBProc::processRunHeader( LCRunHeader* run) { 

    _nRun++ ;
} 



void AsymFBProc::processEvent( LCEvent * evt ) { 

  _evt = evt;
  _evInfo.resetVars();
  resetObjects();

  boostCalcParameters();
  calcGenVariables();
  calcStandardPandoraVariables(_colPandName);
  calcThrustVars(_colTagJetsName);   
  calcFastJetVars(_colFastJetName);
  calcTagJetsVars(_colTagJetsName);
  calcVrtxVars("PrimaryVertex");
  calcVrtxVars(_colVtxName);
  calcHemVars(_colTagJetsName);
  calcRecParticlesBasicInfo(_colTagJetsName);
  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
		       << "   in run:  " << evt->getRunNumber() << std::endl ;

  _evInfo.fillTree("main_1");  
  //_evInfo.fillTree("genFull_1");
  //_evInfo.fillTree("recFull_1");

  _nEvt ++ ;
}

void AsymFBProc::calcFastJetVars(std::string colN){

  LCCollection* col = openCollection(colN);
  if(col != NULL){

    int nJets = col->getNumberOfElements();
    streamlog_out(DEBUG) << " nmb of elements: "<< nJets << std::endl ;
    TLorentzVector j1,j2;     
     
    if(nJets==2){
      ReconstructedParticle* rec1 = dynamic_cast<ReconstructedParticle*>( col->getElementAt( 0 ) );
      ReconstructedParticle* rec2 = dynamic_cast<ReconstructedParticle*>( col->getElementAt( 1 ) );
      j1.SetPxPyPzE(( rec1->getMomentum() )[0],( rec1->getMomentum() )[1],( rec1->getMomentum() )[2],rec1->getEnergy());
      j2.SetPxPyPzE(( rec2->getMomentum() )[0],( rec2->getMomentum() )[1],( rec2->getMomentum() )[2],rec2->getEnergy());
      TLorentzVector V = j1+j2;
      _evInfo.rJetInvMass =  V.M();
      _evInfo.rTheta1 =  TMath::RadToDeg() * j1.Theta();
      _evInfo.rTheta2 =  TMath::RadToDeg() * j2.Theta();
      _evInfo.rPhi1 =  TMath::RadToDeg() * j1.Phi();
      _evInfo.rPhi2 = TMath::RadToDeg() * j2.Phi();
      _evInfo.rMissEt_FastJet = sqrt( j1.Energy()*j1.Energy()*( sin(j1.Theta())*cos(j1.Phi())*sin(j1.Theta())*cos(j1.Phi()) + sin(j1.Theta())*sin(j1.Phi())*sin(j1.Theta())*sin(j1.Phi()) )
				 + j2.Energy()*j2.Energy()*( sin(j2.Theta())*cos(j2.Phi())*sin(j2.Theta())*cos(j2.Phi()) + sin(j2.Theta())*sin(j2.Phi())*sin(j2.Theta())*sin(j2.Phi()) ) );
  
      double s_G = 1400 * 1400;
      double E_Gamma = std::abs(sin(j1.Theta()+j2.Theta()))*sqrt(s_G) / ( sin(j1.Theta()) + sin(j2.Theta())+ std::abs(sin(j1.Theta()+j2.Theta())) );
      _evInfo.recSPrime = s_G - 2. * E_Gamma * sqrt(s_G); 
      _evInfo.recSPrimeRatio = 1. - 2. * std::abs( sin(j1.Theta()+j2.Theta()) ) / (sin(j1.Theta())+sin(j2.Theta())+ std::abs(sin(j1.Theta()+j2.Theta())));
      
   

    }//end of njets check


  }//end of coll=null check
}

void AsymFBProc::calcTagJetsVars(std::string colN){

  LCCollection* col = openCollection(colN);
  if(col != NULL){

    PIDHandler pidh(col);
    int algo = pidh.getAlgorithmID("lcfiplus");
    int ibtag = pidh.getParameterIndex(algo,"BTag");
    int ictag = pidh.getParameterIndex(algo,"CTag");
    TLorentzVector j1,j2;     
    
    int nEl =  col->getNumberOfElements();
    streamlog_out(DEBUG) << "NumOfElem in RefindJets: "<< nEl <<  std::endl ;
    
    for(int idJet = 0 ; idJet < nEl ; idJet++){
      ReconstructedParticle* refjet = dynamic_cast<ReconstructedParticle*>( col->getElementAt( idJet ) );
      const ParticleID &pid = pidh.getParticleID(refjet,algo);
      TVector3 jetVec( refjet->getMomentum() );
      
      float PtSum = 0;
      float PtQSum = 0;
      float PSum = 0;
      float PQSum = 0;
      float PlSum = 0;
      float PlQSum = 0;
         
  
      for(unsigned int prtID = 0 ; prtID <  ( refjet->getParticles() ).size() ; prtID++ ){
	ReconstructedParticle*  part = dynamic_cast<ReconstructedParticle*>(  ( refjet->getParticles() )[prtID] );
	for( unsigned int trkID = 0 ; trkID < ( part->getTracks() ).size() ; trkID++ ){
	  TVector3 partVec( part->getMomentum() );
	  float omega, pt, p, pl, tanL, q, pwch;
	  omega = (( part->getTracks() )[trkID])->getOmega();
	  pt = std::abs(1/omega) ;//constant parameter a, B =4T - all will cancel out  
	  tanL = (( part->getTracks() )[trkID])->getTanLambda();
	  q = copysign(1,omega);//
	  p = pt*sqrt(1+tanL*tanL);
	  pl = partVec*jetVec;
	  
	  PtSum+=pt;
	  PtQSum+=pt*q;
	  PSum+= pow( p , 1.8 );
	  PQSum+= pow( p, 1.8 ) * q ;
	  PlSum+=pl;
	  PlQSum+=pl*q;
	}//end of loops over tracks for a given tagjet particle
      }//end of loop over particles in tag jet
      float ptwq, pwq,plwq;
      ptwq = PtQSum/PtSum;
      pwq = PQSum/PSum;
      plwq = PlQSum/PlSum;   

      if(idJet==0){
	j1.SetPxPyPzE(( refjet->getMomentum() )[0],( refjet->getMomentum() )[1],( refjet->getMomentum() )[2],refjet->getEnergy());
	_evInfo.rRefJetCharge1 = refjet->getCharge();
	_evInfo.rPtWCharge1 = ptwq ;
	_evInfo.rPWCharge1 = pwq ;
	_evInfo.rPlWCharge1 = plwq ;
	_evInfo.rbTag1 =  pid.getParameters()[ibtag];
	_evInfo.rcTag1 =  pid.getParameters()[ictag];
      }
      if(idJet==1){
	j2.SetPxPyPzE(( refjet->getMomentum() )[0],( refjet->getMomentum() )[1],( refjet->getMomentum() )[2],refjet->getEnergy());
	_evInfo.rRefJetCharge2 = refjet->getCharge();
	_evInfo.rPtWCharge2 = ptwq ;
	_evInfo.rPWCharge2 = pwq ;
	_evInfo.rPlWCharge2 = plwq ;
	_evInfo.rbTag2 =  pid.getParameters()[ibtag];
	_evInfo.rcTag2 =  pid.getParameters()[ictag];
      }
   
    }//end of for loop over jets in col

    findCoMBoost(j1,j2);

    TLorentzVector V = j1+j2;
    _evInfo.rRefJetInvMass =  V.M();
    _evInfo.rRefTheta1 =  TMath::RadToDeg() * j1.Theta();
    _evInfo.rRefTheta2 =  TMath::RadToDeg() * j2.Theta();
    _evInfo.rRefPhi1 =  TMath::RadToDeg() * j1.Phi();
    _evInfo.rRefPhi2 = TMath::RadToDeg() * j2.Phi();
    _evInfo.rMissEt = sqrt( j1.Energy()*j1.Energy()*( sin(j1.Theta())*cos(j1.Phi())*sin(j1.Theta())*cos(j1.Phi()) + sin(j1.Theta())*sin(j1.Phi())*sin(j1.Theta())*sin(j1.Phi()) )
			       + j2.Energy()*j2.Energy()*( sin(j2.Theta())*cos(j2.Phi())*sin(j2.Theta())*cos(j2.Phi()) + sin(j2.Theta())*sin(j2.Phi())*sin(j2.Theta())*sin(j2.Phi()) ) );
    
    _evInfo.recSPrimeRatioTagJets = 1. - 2. * std::abs( sin(j1.Theta()+j2.Theta()) ) / (sin(j1.Theta())+sin(j2.Theta())+ std::abs(sin(j1.Theta()+j2.Theta())));
    
    _evInfo.rMissEt4V = (V.Vect()).Mag();

    float r1,r2,g1,g2;
    float rp1,rp2,gp1,gp2;
    r1 = _evInfo.rRefTheta1;    r2 = _evInfo.rRefTheta2;
    g1 = _evInfo.gShowerTheta1;    g2 = _evInfo.gShowerTheta2;
    rp1 = _evInfo.rRefPhi1;    rp2 = _evInfo.rRefPhi2;
    gp1 = _evInfo.gShowerPhi1;    gp2 = _evInfo.gShowerPhi2;

    _evInfo.rSdR11 =  sqrt( (r1 -g1)*(r1 -g1) + (rp1 - gp1)*(rp1 - gp1) );
    _evInfo.rSdR12 =   sqrt( (r1 -g2)*(r1 -g2) + (rp1 - gp2)*(rp1 - gp2) );
    _evInfo.rSdR21 =  sqrt( (r2 -g1)*(r2 -g1) + (rp2 - gp1)*(rp2 - gp1) );
    _evInfo.rSdR22 =  sqrt( (r2 -g2)*(r2 -g2) + (rp2 - gp2)*(rp2 - gp2) );
 
    g1 = _evInfo.gTheta1;    g2 = _evInfo.gTheta2;
    gp1 = _evInfo.gPhi1;    gp2 = _evInfo.gPhi2;

   _evInfo.rdR11 =   sqrt( (r1 -g1)*(r1 -g1) + (rp1 - gp1)*(rp1 - gp1) );
    _evInfo.rdR12 =  sqrt( (r1 -g2)*(r1 -g2) + (rp1 - gp2)*(rp1 - gp2) );
    _evInfo.rdR21 =   sqrt( (r2 -g1)*(r2 -g1) + (rp2 - gp1)*(rp2 - gp1) );
    _evInfo.rdR22 =  sqrt( (r2 -g2)*(r2 -g2) + (rp2 - gp2)*(rp2 - gp2) );

  }//end of coll-null check
}

void AsymFBProc::calcHemVars(std::string colN){
  
  if( _evInfo.rPVx !=-500 ){//rPVx is set to -500 in initialisation
    for(unsigned int v = 0 ; v < _evInfo.rBx.size() ; ++v ){ 
      TVector3 bvVec(_evInfo.rBx.at(v)-_evInfo.rPVx, _evInfo.rBy.at(v)-_evInfo.rPVy, _evInfo.rBz.at(v)-_evInfo.rPVz);
      int hemSign = bvVec.Dot(_evInfo._thrstAxis) > 0 ? 1: -1 ;
      streamlog_out(DEBUG) << " distance to sec vrtx: "<< v << " with TVec3 is " << bvVec.Mag() << std::endl;
      streamlog_out(DEBUG) << "This vrtx is in "<< hemSign <<" hemisphere"<< std::endl;
      _evInfo.BVDists.push_back( bvVec.Mag()*hemSign );
      hemSign = bvVec.Dot(_evInfo._thrstAxisCalo) > 0 ? 1: -1 ;
      streamlog_out(DEBUG) << "This vrtx is in "<< hemSign <<" Calo hemisphere"<< std::endl;
      _evInfo.BVDistsPand.push_back( bvVec.Mag()*hemSign );
    } 
    //SOME OLD CODE
    /*Double_t A,B,C;
    if( _evInfo.thrTheta > 90 ){ 
      A = -1 * thrAx.X();
      B = -1 * thrAx.Y();
      C = -1 * thrAx.Z();
    }
    else { 
      A = thrAx.X();
      B = thrAx.Y();
      C = thrAx.Z();
    }
    Double_t paramD =  -1* A *_evInfo.rPx - B *_evInfo.rPy - C *_evInfo.rPz;
    for(unsigned int v = 0 ; v < _evInfo.rBx.size() ; ++v ){      
      Double_t dist = ( A * _evInfo.rBx.at(v) + B * _evInfo.rBy.at(v) + C * _evInfo.rBz.at(v) + paramD ) / sqrt( A*A + B*B + C*C );
      TVector3 bvVec(_evInfo.rBx.at(v)-_evInfo.rPx, _evInfo.rBy.at(v)-_evInfo.rPy, _evInfo.rBz.at(v)-_evInfo.rPz);
      streamlog_out(DEBUG) << " distance to sec vrtx: "<< v << " is " << dist << std::endl;
      streamlog_out(DEBUG) << " distance to sec vrtx: "<< v << " with TVec3 is " << bvVec.Mag() << std::endl;
      int hemSign = bvVec.Dot(thrAx) > 0 ? 1: -1 ;
      streamlog_out(DEBUG) << "This vrtx is in "<< hemSign <<" hemisphere"<< std::endl;
      _evInfo.BVDists.push_back( dist );
    }
    */

  }else
    streamlog_out(WARNING) <<"No primamry vertex found or wrong methods order"<< std::endl;

  //colN - name of the jet collection
  LCCollection* col = openCollection(colN);
  if(col != NULL){

    float hemChFrw=0.;
    float  hemChBck = 0.;
    float den = 0.;
    float hemChFrwCalo=0.;
    float  hemChBckCalo = 0.;
    float denCalo = 0.;
 
    float hemChFrwPow12=0.;
    float  hemChBckPow12 = 0.;
    float denPow12 = 0.;
    float hemChFrwCaloPow12=0.;
    float  hemChBckCaloPow12 = 0.;
    float denCaloPow12 = 0.;
 
    float hemChFrwPow08=0.;
    float  hemChBckPow08 = 0.;
    float denPow08 = 0.;
    float hemChFrwCaloPow08=0.;
    float  hemChBckCaloPow08 = 0.;
    float denCaloPow08 = 0.;
 
   for(int i = 0 ; i < col->getNumberOfElements() ; i++){
      ReconstructedParticle* refjet = dynamic_cast<ReconstructedParticle*>( col->getElementAt( i ) );
      for(unsigned int prtID = 0 ; prtID <  ( refjet->getParticles() ).size() ; prtID++ ){
	
	ReconstructedParticle*  part = dynamic_cast<ReconstructedParticle*>(  ( refjet->getParticles() )[prtID] );
	for( unsigned int trkID = 0 ; trkID < ( part->getTracks() ).size() ; trkID++ ){
	  TVector3 tempV( part->getMomentum() ); 
	  float chrg = tempV.Dot(_evInfo._thrstAxis);
	  float chrgCalo = tempV.Dot(_evInfo._thrstAxisCalo);

	  if(chrg>0){
	    hemChFrw +=std::abs(chrg)*part->getCharge();
	    hemChFrwPow12 +=std::pow(std::abs(chrg),1.2)*part->getCharge();
	    hemChFrwPow08 +=std::pow(std::abs(chrg),0.8)*part->getCharge();
	  }
	  else{
	    hemChBck +=std::abs(chrg)*part->getCharge();
	    hemChBckPow12 +=std::pow(std::abs(chrg),1.2)*part->getCharge();
	    hemChBckPow08 +=std::pow(std::abs(chrg),0.8)*part->getCharge();
	  }
	  den += std::abs(chrg);
	  denPow12 += std::pow(std::abs(chrg),1.2);
	  denPow08 += std::pow(std::abs(chrg),0.8);
	  
	  if(chrgCalo>0){
	    hemChFrwCalo +=std::abs(chrgCalo)*part->getCharge();
	    hemChFrwCaloPow12 +=std::pow(std::abs(chrgCalo),1.2)*part->getCharge();
	    hemChFrwCaloPow08 +=std::pow(std::abs(chrgCalo),0.8)*part->getCharge();
	  }
	  else{
	    hemChBckCalo +=std::abs(chrgCalo)*part->getCharge();
	    hemChBckCaloPow12 +=std::pow(std::abs(chrgCalo),1.2)*part->getCharge();
	    hemChBckCaloPow08 +=std::pow(std::abs(chrgCalo),0.8)*part->getCharge();
	  }
	  denCalo += std::abs(chrgCalo);
	  denCaloPow12 += std::pow(std::abs(chrgCalo),1.2);
	  denCaloPow08 += std::pow(std::abs(chrgCalo),0.8);

	}//end of loop over tracks assigned to a jet particle
	
      }//end of loop over jet constituents
      
    }//end of loop over jets in col
    _evInfo.rFrwHemCh = hemChFrw/den;
    _evInfo.rBckHemCh = hemChBck/den;
    _evInfo.rHemChFlow = _evInfo.rFrwHemCh - _evInfo.rBckHemCh ;
    _evInfo.rFrwHemChCalo = hemChFrwCalo/denCalo;
    _evInfo.rBckHemChCalo = hemChBckCalo/denCalo;
    _evInfo.rHemChFlowCalo = _evInfo.rFrwHemChCalo - _evInfo.rBckHemChCalo ;

   _evInfo.rFrwHemChPow12 = hemChFrwPow12/denPow12;
    _evInfo.rBckHemChPow12 = hemChBckPow12/denPow12;
    _evInfo.rHemChFlowPow12 = _evInfo.rFrwHemChPow12 - _evInfo.rBckHemChPow12 ;
    _evInfo.rFrwHemChCaloPow12 = hemChFrwCaloPow12/denCaloPow12;
    _evInfo.rBckHemChCaloPow12 = hemChBckCaloPow12/denCaloPow12;
    _evInfo.rHemChFlowCaloPow12 = _evInfo.rFrwHemChCaloPow12 - _evInfo.rBckHemChCaloPow12 ;

  _evInfo.rFrwHemChPow08 = hemChFrwPow08/denPow08;
    _evInfo.rBckHemChPow08 = hemChBckPow08/denPow08;
    _evInfo.rHemChFlowPow08 = _evInfo.rFrwHemChPow08 - _evInfo.rBckHemChPow08 ;
    _evInfo.rFrwHemChCaloPow08 = hemChFrwCaloPow08/denCaloPow08;
    _evInfo.rBckHemChCaloPow08 = hemChBckCaloPow08/denCaloPow08;
    _evInfo.rHemChFlowCaloPow08 = _evInfo.rFrwHemChCaloPow08 - _evInfo.rBckHemChCaloPow08 ;
  }//end of colN check
 
}

void AsymFBProc::calcVrtxVars(std::string colN){

  Vertex* v1 = NULL;
  LCCollection* col = openCollection(colN);
  if(col != NULL){
    const int nVrt= col->getNumberOfElements();

  if(nVrt > 0 )
      for(int vID = 0 ; vID < nVrt ; ++vID){
	int charge = 0;
	int flavVt = 0;
   
	v1 = dynamic_cast<Vertex*>( col->getElementAt( vID ) ) ;
	ReconstructedParticle* p1 = v1->getAssociatedParticle();
	for(unsigned int pID = 0 ; pID < p1->getParticles().size() ; ++pID){
	  
	  ReconstructedParticle* inner = p1->getParticles()[pID] ;
	  charge+= inner->getCharge();
	  int fl = motherMC(inner);
	  if( abs(fl)==5 )   flavVt+=(fl/5.);
	}
	if (v1->isPrimary() ) {
	  const float* pos = v1->getPosition();
	  streamlog_out(DEBUG) << " PrimVrtx position: x = " << pos[0] << " y = " << pos[1] << " z = "<< pos[2] << std::endl;
	  _evInfo.rPVx = pos[0];
	  _evInfo.rPVy = pos[1];
	  _evInfo.rPVz = pos[2];
	}  
	if( colN.compare("BuildUpVertex") == 0 ){
	  _evInfo.rBVCh.push_back(charge); _evInfo.gBVFl.push_back(flavVt); 
	  const float* pos = v1->getPosition();
	  _evInfo.rBx.push_back( pos[0] );
	  _evInfo.rBy.push_back( pos[1] );
	  _evInfo.rBz.push_back( pos[2] );
	  streamlog_out(DEBUG) << " BuildUpVrtx position: x = " << _evInfo.rBx.at(vID) << " y = " << _evInfo.rBy.at(vID) << " z = "<< _evInfo.rBz.at(vID) << std::endl;
	}
      }//koniec po vertexach
  
  }//end of coll-null check
}

int AsymFBProc::motherMC( LCObject* part ){

  //streamlog_out(DEBUG) << "  MyProcessor::motherMC() : starting.."<< std::endl ;
  int type = 0;
  EVENT::LCCollection* links = openCollection("RecoMCTruthLink");

  if(links){
    LCRelationNavigator* relMCLink =  new LCRelationNavigator( links ) ;
    
    LCObjectVec objectVec = relMCLink->getRelatedToObjects(part);
    //streamlog_out(DEBUG) << "#elem: " << objectVec.size()  << std::endl;
    for(unsigned int k = 0 ; k < objectVec.size() ; ++k){  

      MCParticle* mp = dynamic_cast<MCParticle*>( objectVec[k] );
      if(!mp) continue;
      MCParticle* opart = mp;
      while(opart->getGeneratorStatus()) {
	type = opart->getPDG();
	int mthr = 0;
	if( opart->getParents().size() ) mthr = ( (MCParticle*) opart->getParents()[0] )->getPDG();
	if( abs(type) > 500 && abs(type) % 500 < 100 && mthr == 92) {
	  streamlog_out(DEBUG) << "Found a match!!"<< std::endl;
	  streamlog_out(DEBUG) << "Particle type:" << type << " #mothers: "<< opart->getParents().size() << std::endl;
	  for(unsigned int k = 0 ; k < opart->getParents().size() ; ++k )
	    streamlog_out(DEBUG) << " mother "<< k << " type:"  << ((MCParticle*) opart->getParents()[0])->getPDG() << std::endl;
	  return copysign(5,type);
	}
	if((opart->getParents()).size()) {
	  opart = (MCParticle*) opart->getParents()[ opart->getParents().size() - 1 ];
	} else {
	  break;
	}
      }
    }

    delete relMCLink;
  }
  return type;

}

//copied&modified
bool AsymFBProc::findGeneratedQuarks(){

  bool q_present = false;
  EVENT::LCCollection* mcp = _evt->getCollection(_colMCName);
  for (int i = 0; i < mcp->getNumberOfElements(); i++){
    EVENT::MCParticle* mcpi = (EVENT::MCParticle*) mcp->getElementAt(i);
    if (abs( mcpi->getPDG() ) < 9 ) {
      if(mcpi->getParents().size() > 0){
	if ( abs(mcpi->getParents()[0]->getPDG()) == 11){// && mcpi->getParents()[0]->getParents()[0]->getParents.size()==0){
	  q_present = true;
	  _genQuarks.push_back(mcpi ) ;
	  _evInfo.gQuarkInd.push_back( i );
	}
	if (abs(mcpi->getParents()[0]->getPDG()) == 94){
	  _genQuarksShvr.push_back(mcpi );
	}
      }
    }
  }
  return q_present;
}
bool AsymFBProc::findGeneratedNeutrinos(){

  bool n_present = false;
  EVENT::LCCollection* mcp = _evt->getCollection(_colMCName);
  for (int i = 0; i < mcp->getNumberOfElements(); i++){
    EVENT::MCParticle* mcpi = (EVENT::MCParticle*) mcp->getElementAt(i);
    if (abs(mcpi->getPDG()) == 12 || abs(mcpi->getPDG()) == 14 || abs(mcpi->getPDG()) == 16){
      streamlog_out(DEBUG) << "findGeneratedNeutrinos: found some neutrino..."<< std::endl;
      if(mcpi->getParents().size() > 0){
	streamlog_out(DEBUG) << "findGeneratedNeutrinos: its parents non-empty: "<< mcpi->getParents().size() << std::endl;
	streamlog_out(DEBUG) << "pdg of first parent: "<< mcpi->getParents()[0]->getPDG()<< std::endl;
	streamlog_out(DEBUG) << "daughters: "<< mcpi->getDaughters().size()<< std::endl;
	if ( mcpi->getParents()[0]->getParents().size() > 0 && abs(mcpi->getParents()[0]->getParents()[0]->getPDG()) == 11 && mcpi->getDaughters().size()==0){
	  streamlog_out(DEBUG) << "findGeneratedNeutrinos: parents' parents of pdg_type=11 and no daugthers"<< std::endl;
	  n_present = true;
	  _genNeutrinos.push_back( mcpi ) ;
	  _evInfo.gNuInd.push_back( i );
	}
      }
    }
  }
  return n_present;
}

///////////////////
void AsymFBProc::calcGenVariables(){

  LCCollection* col =  openCollection(_colMCName);
  if( col != NULL){

    if( findGeneratedNeutrinos())
      if( _genNeutrinos[1]!=NULL)
      _evInfo.gE_nunu = _genNeutrinos[0]->getEnergy() +_genNeutrinos[1]->getEnergy() ;
      else
	if( _genNeutrinos[0]!=NULL)
	  _evInfo.gE_nunu = _genNeutrinos[0]->getEnergy();

    if( findGeneratedQuarks() ){
      if(_genQuarks.size()<2){ std::cout << "not enough quarks"<< std::endl;exit(0);}
      if( abs( _genQuarks[0]->getPDG() ) < 4 ) 	
	_evInfo.gUDS = true;
      if( abs( _genQuarks[0]->getPDG() ) == 4 ) 	
	_evInfo.gC = true;
      if( abs( _genQuarks[0]->getPDG() ) == 5 ) 	
	_evInfo.gB = true;
     if( abs( _genQuarks[0]->getPDG() ) == 6 ) 	
	_evInfo.gT = true;
    
      TLorentzVector v1,v2;
      v1.SetPxPyPzE(( _genQuarks[0]->getMomentum() )[0],( _genQuarks[0]->getMomentum() )[1],( _genQuarks[0]->getMomentum() )[2],_genQuarks[0]->getEnergy());
      v2.SetPxPyPzE(( _genQuarks[1]->getMomentum() )[0],( _genQuarks[1]->getMomentum() )[1],( _genQuarks[1]->getMomentum() )[2],_genQuarks[1]->getEnergy());
      TLorentzVector V = v1+v2;
      _evInfo.gInvMass = V.M();
      _evInfo.gTheta1 =  TMath::RadToDeg() * v1.Theta();
      _evInfo.gTheta2 = TMath::RadToDeg() * v2.Theta();
      _evInfo.gPhi1 =  TMath::RadToDeg() * v1.Phi();
      _evInfo.gPhi2 =  TMath::RadToDeg() * v2.Phi();

      TLorentzVector TotGen;
      EVENT::MCParticle* mc_el = (EVENT::MCParticle*) col->getElementAt(0);
      for(int i = 0 ; i < mc_el->getDaughters().size() ; i++){
	TLorentzVector temp( mc_el->getDaughters()[i]->getMomentum() , mc_el->getDaughters()[i]->getEnergy());
	TotGen+=temp;
      } 
      getRecoElec();

      _evInfo.gMissEt =  sqrt( v1.Energy()*v1.Energy()*( sin(v1.Theta())*cos(v1.Phi())*sin(v1.Theta())*cos(v1.Phi()) + sin(v1.Theta())*sin(v1.Phi())*sin(v1.Theta())*sin(v1.Phi()) )
			       + v2.Energy()*v2.Energy()*( sin(v2.Theta())*cos(v2.Phi())*sin(v2.Theta())*cos(v2.Phi()) + sin(v2.Theta())*sin(v2.Phi())*sin(v2.Theta())*sin(v2.Phi()) ) );
      _evInfo.gMissEt4V = (TotGen.Vect()).Mag();
      
      _evInfo.gSPrime = ( 2. * 0.5 * 0.5 + 2. * v1*v2 ) / (1400*1400) ;
      _evInfo.gSPrimeRatio = 1. - 2. * std::abs( sin(v1.Theta()+v2.Theta()) ) / (sin(v1.Theta())+sin(v2.Theta())+ std::abs(sin(v1.Theta()+v2.Theta())));
   
      boostParticle(&v1,"mc");
      boostParticle(&v2,"mc");
      V=v1+v2;
      _evInfo.gTheta1Bstd =  TMath::RadToDeg() * v1.Theta();
      _evInfo.gTheta2Bstd = TMath::RadToDeg() * v2.Theta();
      _evInfo.gPhi1Bstd =  TMath::RadToDeg() * v1.Phi();
      _evInfo.gPhi2Bstd =  TMath::RadToDeg() * v2.Phi();
      _evInfo.gMissEtBstd =  sqrt( v1.Energy()*v1.Energy()*( sin(v1.Theta())*cos(v1.Phi())*sin(v1.Theta())*cos(v1.Phi()) + sin(v1.Theta())*sin(v1.Phi())*sin(v1.Theta())*sin(v1.Phi()) )
			       + v2.Energy()*v2.Energy()*( sin(v2.Theta())*cos(v2.Phi())*sin(v2.Theta())*cos(v2.Phi()) + sin(v2.Theta())*sin(v2.Phi())*sin(v2.Theta())*sin(v2.Phi()) ) );
      _evInfo.gMissEtBstd4V = (V.Vect()).Mag();
      

      if( _genQuarksShvr.size() > 0 ){
	TLorentzVector vS1,vS2;
	vS1.SetPxPyPzE(( _genQuarksShvr[0]->getMomentum() )[0],( _genQuarksShvr[0]->getMomentum() )[1],( _genQuarksShvr[0]->getMomentum() )[2],_genQuarksShvr[0]->getEnergy());
	vS2.SetPxPyPzE(( _genQuarksShvr[1]->getMomentum() )[0],( _genQuarksShvr[1]->getMomentum() )[1],( _genQuarksShvr[1]->getMomentum() )[2],_genQuarksShvr[1]->getEnergy());
	TLorentzVector sV = vS1+vS2;
	
	_evInfo.gShowerInvMass = sV.M() ;
	_evInfo.gShowerTheta1 = TMath::RadToDeg() * vS1.Theta();
	_evInfo.gShowerTheta2 =  TMath::RadToDeg() * vS2.Theta();
	_evInfo.gShowerPhi1 = TMath::RadToDeg() * vS1.Phi();
	_evInfo.gShowerPhi2 =TMath::RadToDeg() * vS2.Phi();
      }
    }//if (findGenQuarks)
    if(_genQuarks.size()==0) streamlog_out(DEBUG) << "WARNING: Not a quark found, maybe they got squarked" <<std::endl;
    if(_genQuarks.size()>2) streamlog_out(DEBUG) << "WARNING: A bit too many quarks on gen lvl" <<std::endl;

    //for genFull Trees
    std::map< MCParticle*, int > map;
    for(int i=0; i< col->getNumberOfElements()  ; i++){
      MCParticle* p = dynamic_cast<MCParticle*>( col->getElementAt( i ) ) ;
      map[ p ] = i;
    }
 
     
    for(int i=0; i< col->getNumberOfElements()  ; i++){
      MCParticle* p = dynamic_cast<MCParticle*>( col->getElementAt( i ) ) ;
      _evInfo.gindex.push_back(i);
      _evInfo.gpdg.push_back(p->getPDG());
      _evInfo.gx.push_back(p->getEndpoint()[0]);
      _evInfo.gy.push_back(p->getEndpoint()[1]);
      _evInfo.gz.push_back(p->getEndpoint()[2]);
      _evInfo.gpx.push_back(p->getMomentum()[0]);
      _evInfo.gpy.push_back(p->getMomentum()[1]);
      _evInfo.gpz.push_back(p->getMomentum()[2]);
      _evInfo.ge.push_back(p->getEnergy());
      _evInfo.gmass.push_back(p->getMass());
      _evInfo.gcharge.push_back(p->getCharge());
      _evInfo.gvx.push_back(p->getVertex()[0]);
      _evInfo.gvy.push_back(p->getVertex()[1]);
      _evInfo.gvz.push_back(p->getVertex()[2]);
      //to get parents and daughters
      TString par = "";
      TString dgt = "";
      //std::vector<int> par = std::vector<int>();
      //std::vector<int> dgt = std::vector<int>();
      for(unsigned int k=0;k<p->getParents().size();k++){
	if(k>0) par+=",";
	  par+= map[ p->getParents()[k] ];
	  //par.push_back( map[ p->getParents()[k] ] ) ;
      }
      for(unsigned int k=0;k<p->getDaughters().size();k++){
	if(k>0) dgt+=",";
	  dgt+= map[ p->getDaughters()[k] ];
	  //dgt.push_back( map[ p->getDaughters()[k] ] ) ;
      }
      /*   if(i<10){
	std::cout << "id "<< i << " has parents: " << par <<std::endl;
		for(unsigned int k=0; k < par.size(); k++)
	  std::cout << par.at(k)<< " ";
	  std::cout <<"\n"; 
	  }*/
      _evInfo.g_parents.push_back(par.Data());   
      _evInfo.g_daughters.push_back(dgt.Data());   
    }//end of loop over MCParticles  
   
  }//end of MC coll check

}

void AsymFBProc::calcThrustVars(std::string colN){

  //colN - name of the jet collection
  LCCollection* col = openCollection(colN);
  if(col != NULL){
    TObjArray* veclist = new TObjArray();
    for(int i = 0 ; i < col->getNumberOfElements() ; i++){
      ReconstructedParticle* refjet = dynamic_cast<ReconstructedParticle*>( col->getElementAt( i ) );
      for(unsigned int prtID = 0 ; prtID <  ( refjet->getParticles() ).size() ; prtID++ ){
	
	ReconstructedParticle*  part = dynamic_cast<ReconstructedParticle*>(  ( refjet->getParticles() )[prtID] );
	for( unsigned int trkID = 0 ; trkID < ( part->getTracks() ).size() ; trkID++ ){
	  
	  veclist->Add(new TVector3( part->getMomentum() ) );//for testing thrust
	  
	}//end of loop over tracks assigned to a jet particle
	
      }//end of loop over jet constituents
      
    }//end of loop over jets in col
    EventShape* eshape = new EventShape();
    eshape->setPartList(veclist); 
    double thrust = eshape->thrust().X();
    TVector3 thrAx = eshape->thrustAxis();
    float theta =   TMath::RadToDeg() * thrAx.Theta() ;
    //streamlog_out(DEBUG)<< "theta, phi before orienting: "<< theta <<"  " <<   TMath::RadToDeg() * thrAx.Phi() << " mag: "<< thrAx.Mag()<< std::endl;
    if( theta > 90 ) thrAx *= -1;
    //streamlog_out(DEBUG)<< "theta, phi after orienting: "<<  TMath::RadToDeg() * thrAx.Theta() <<"  " <<   TMath::RadToDeg() * thrAx.Phi() << " mag: "<< thrAx.Mag()<< std::endl;
    _evInfo.thrTheta =  TMath::RadToDeg() * thrAx.Theta() ;
    _evInfo.thrPhi =   TMath::RadToDeg() * thrAx.Phi() ; 
    _evInfo._thrstAxis = thrAx;
    _evInfo.thrust = thrust;   
    veclist->Clear();veclist->Delete();  
    
    streamlog_out(DEBUG)<<"thrust : " << thrust << " theta: "<< thrAx.Theta() << std::endl;
      
  }//end of col-existence check
  
  //############  same approach, but using particles in Pandora coll, no tracks involved
  
  LCCollection* colPand = openCollection(_colPandName_ISRFree);
  if(colPand != NULL){
    TObjArray* veclist = new TObjArray();
    for(int i = 0 ; i < colPand->getNumberOfElements() ; i++){
      ReconstructedParticle* panda = dynamic_cast<ReconstructedParticle*>( colPand->getElementAt( i ) );
      veclist->Add(new TVector3( panda->getMomentum() ) );//for testing thrust
      TVector3 temp( panda->getMomentum() );        
  
    }//end of loop over particles in col

    EventShape* eshape = new EventShape();
    eshape->setPartList(veclist); 
    double thrust = eshape->thrust().X();
    TVector3 thrAx = eshape->thrustAxis();
    float theta =   TMath::RadToDeg() * thrAx.Theta() ;
    if( theta > 90 ) thrAx *= -1;
    _evInfo._thrstAxisCalo = thrAx;
    _evInfo.thrustPand = thrust;
    _evInfo.thrThetaPand =   TMath::RadToDeg() * thrAx.Theta() ;
    _evInfo.thrPhiPand =   TMath::RadToDeg() * thrAx.Phi() ;
    veclist->Clear();veclist->Delete();  
    
  }//end of col-existence check
  
  
}

//***************** misc methods
LCCollection* AsymFBProc::openCollection(std::string name){

  LCCollection* col = NULL;
  try{
    col = _evt->getCollection( name );
  }
  catch( lcio::DataNotAvailableException e )
    {
      streamlog_out(WARNING) << name << " collection not available" << std::endl;
      col = NULL;
    }

  return col;
}


void AsymFBProc::resetObjects(){

  _mcBoost.SetX(0.);_mcBoost.SetY(0.);_mcBoost.SetZ(0.);
  _mcBoostAxis.SetX(0.);_mcBoostAxis.SetY(0.);_mcBoostAxis.SetZ(0.);
  _mcBoostTheta =0.;

  _genQuarks.clear();
  _genQuarksShvr.clear();
  _genNeutrinos.clear();

}

void AsymFBProc::check( LCEvent * evt ) { 
    // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void AsymFBProc::end(){ 

  _fOutFile->cd();
  //_evInfo.writeTree("genFull_1","genFullHouse");
  //_evInfo.writeTree("recFull_1","recFullHouse");
  _evInfo.writeTree("main_1","main_AnotherBrickInTheWall");
  _fOutFile->Close();
}

void AsymFBProc::calcRecParticlesBasicInfo(std::string colN){

  LCCollection* col = openCollection(colN);
  if(col != NULL){
    int nEl =  col->getNumberOfElements();
    for(int idJet = 0 ; idJet < nEl ; idJet++){
      ReconstructedParticle* refjet = dynamic_cast<ReconstructedParticle*>( col->getElementAt( idJet ) );
      for(unsigned int prtID = 0 ; prtID <  refjet->getParticles().size() ; prtID++ ){
	ReconstructedParticle*  part = dynamic_cast<ReconstructedParticle*>(  ( refjet->getParticles() )[prtID] );
	_evInfo.rJetID.push_back(idJet);
	_evInfo.rPx.push_back( part->getMomentum()[0]);
	_evInfo.rPy.push_back( part->getMomentum()[1]);
	_evInfo.rPz.push_back( part->getMomentum()[2]);
	_evInfo.rE.push_back( part->getEnergy());
	_evInfo.rCharge.push_back( part->getCharge());
      }
    }
  }
}


void AsymFBProc::calcStandardPandoraVariables(std::string colN){


  //colN should be an unmodified PandoraPFOs collection
  LCCollection* colPand = openCollection(colN);
  if(colPand != NULL){

    TLorentzVector V(0.,0.,0.,0.);     

    for(int i = 0 ; i < colPand->getNumberOfElements() ; i++){
      TLorentzVector V_temp; 
      ReconstructedParticle* panda = dynamic_cast<ReconstructedParticle*>( colPand->getElementAt( i ) );
      V_temp.SetPxPyPzE(( panda->getMomentum() )[0],( panda->getMomentum() )[1],( panda->getMomentum() )[2],panda->getEnergy());
      V+=V_temp;
    }//end of loop over particles in col

      _evInfo.rMissEt_PFOColl =  (V.Vect()).Mag();

  }//end of if(col)

}


////////////////////////////////

void AsymFBProc::boostCalcParameters(){

  TLorentzVector V(0.,0.,0.,0.);     
  TLorentzVector el1,el2,lab,cm; 
  TVector3 axis_rot,boost;
  Double_t theta_rot;
  
  ////////// generated part
  EVENT::LCCollection* mcp = openCollection(_colMCName);
  if(mcp){
    EVENT::MCParticle* mcp1 = (EVENT::MCParticle*) mcp->getElementAt(0);
    EVENT::MCParticle* mcp2 = (EVENT::MCParticle*) mcp->getElementAt(1);
    TLorentzVector v1,v2;
    v1.SetPxPyPzE(( mcp1->getMomentum() )[0],( mcp1->getMomentum() )[1],( mcp1->getMomentum() )[2],mcp1->getEnergy());
    v2.SetPxPyPzE(( mcp2->getMomentum() )[0],( mcp2->getMomentum() )[1],( mcp2->getMomentum() )[2],mcp2->getEnergy());
    
    boost = v1.Vect() + v2.Vect();
    boost *= -1/(v1.E() + v2.E());
    TLorentzVector el_b = v1;
    el_b.Boost(boost);
    TVector3 z(0,0,1);
    TVector3 zpr = (el_b.Vect()).Unit();
    axis_rot = z.Cross(zpr);
    theta_rot = TMath::ASin(axis_rot.Mag());
    
    _mcBoost = boost;
    _mcBoostAxis=axis_rot;
    _mcBoostTheta=theta_rot;
    
    el1 = v1;
    el2 = v2;
    
    el1.Rotate(theta_rot,axis_rot);
    el1.Boost(boost);
    el2.Rotate(theta_rot,axis_rot);
    el2.Boost(boost);
    TLorentzVector V,El;
    V=v1+v2;
    El=el1+el2;
    streamlog_out(DEBUG)<< "boost.x:"<< _mcBoost.X() << std::endl;
    streamlog_out(DEBUG)<<"TLorentzVector original: " << std::endl;
    streamlog_out(DEBUG)<< v1.Px() << " " <<  v1.Py() << " " <<  v1.Pz() << " " <<  v1.E() << std::endl;
    streamlog_out(DEBUG)<< v2.Px() << " " <<  v2.Py() << " " <<  v2.Pz() << " " <<  v2.E() << std::endl;
    streamlog_out(DEBUG)<<" s = " << V.Mag() << std::endl;
    streamlog_out(DEBUG)<<"V_sum 3V: "<< V.Px() << " " << V.Py() << " " << V.Pz() << std::endl;
    
    streamlog_out(DEBUG)<<"TLorentzVector boosted: " << std::endl;
    streamlog_out(DEBUG)<< el1.Px() << " " <<  el1.Py() << " " <<  el1.Pz() << " " <<  el1.E() << std::endl;
    streamlog_out(DEBUG)<< el2.Px() << " " <<  el2.Py() << " " <<  el2.Pz() << " " <<  el2.E() << std::endl;
    streamlog_out(DEBUG)<<" s = " << El.Mag() << std::endl;
    streamlog_out(DEBUG)<<"V_sum 3V: "<< El.Px() << " " << El.Py() << " " << El.Pz() << std::endl;
    
    
  }
  
  /////////// reconstructed part
  
  EVENT::LCCollection* links = openCollection("RecoMCTruthLink");
  LCCollection* colPand = openCollection(_colPandName);
  if(links && colPand){
    int NoE =   colPand->getNumberOfElements();
    for(int i = 0 ; i < NoE ; i++){
      ReconstructedParticle* panda = dynamic_cast<ReconstructedParticle*>( colPand->getElementAt( i ) );
      if(!panda) continue;
      LCRelationNavigator* relMCLink =  new LCRelationNavigator( links ) ;
      LCObjectVec objectVec_el1 = relMCLink->getRelatedToObjects(panda);
      LCObjectVec objectVec_el2 = relMCLink->getRelatedFromObjects(panda);
      
      //streamlog_out(DEBUG) << "RelatedTo MCPart(0) size: "<<  objectVec_el1.size() << std::endl; 
      for(unsigned int k = 0 ; k < objectVec_el1.size() ; ++k){  
	MCParticle* mp = dynamic_cast<MCParticle*>( objectVec_el1[k] );
	if(mp) streamlog_out(DEBUG) << "pdg:"<< mp->getPDG() << " mothers.size:"<< mp->getParents().size()<<" gen status: " << mp->getGeneratorStatus() << std::endl; 
      }
      
      //streamlog_out(DEBUG) << "RelatedFrom MCPart(0) size:"<<  objectVec_el2.size() << std::endl; 
      for(unsigned int k = 0 ; k < objectVec_el2.size() ; ++k){  
	ReconstructedParticle* reco = dynamic_cast<ReconstructedParticle*>(  objectVec_el2[k]);
	//if(reco) streamlog_out(DEBUG) << "type: " << reco->getType() << " energy: " << reco->getEnergy() << std::endl; 
      }
      delete relMCLink;
    }
  }
  /*
  LCCollection* colPand = openCollection(_colPandName);
  if(colPand != NULL){
    int NoE =   colPand->getNumberOfElements();
    if(NoE > 2){
      ReconstructedParticle* panda = dynamic_cast<ReconstructedParticle*>( colPand->getElementAt( 0 ) );
      el1.SetPxPyPzE(( panda->getMomentum() )[0],( panda->getMomentum() )[1],( panda->getMomentum() )[2],panda->getEnergy());
 panda = dynamic_cast<ReconstructedParticle*>( colPand->getElementAt( 1 ) );
      el2.SetPxPyPzE(( panda->getMomentum() )[0],( panda->getMomentum() )[1],( panda->getMomentum() )[2],panda->getEnergy());

      TLorentzVector sum = el1 + el2;
      //streamlog_out(DEBUG)<<"s = "<< sum.Mag()<< " momenta consv:" << sum.Px()<< " " << sum.Py() << " " << sum.Pz()<< " " << sum.E() << std::endl;

      boost = el1.Vect() - el2.Vect();
      boost *= 1/(el1.E() + el2.E());
      TLorentzVector el_b = el1;
      el_b.Boost(boost);
      TVector3 z(0,0,1);
      TVector3 zpr = (el_b.Vect()).Unit();
      axis_rot = z.Cross(zpr);
      theta_rot = TMath::ASin(axis_rot.Mag());

    } 

    for(int i = 0 ; i < NoE ; i++){
      TLorentzVector V_temp;
      ReconstructedParticle* panda = dynamic_cast<ReconstructedParticle*>( colPand->getElementAt( i ) );
      V_temp.SetPxPyPzE(( panda->getMomentum() )[0],( panda->getMomentum() )[1],( panda->getMomentum() )[2],panda->getEnergy());
      V+=V_temp;

      lab = V_temp;
      lab.Rotate(theta_rot,axis_rot);
      lab.Boost(boost);
     


      if(i < 10){

	streamlog_out(DEBUG)<<"TLorentzVector original: " << std::endl;
	streamlog_out(DEBUG)<< V_temp.Px() << " " <<  V_temp.Py() << " " <<  V_temp.Pz() << " " <<  V_temp.E() << std::endl;
	
	
	streamlog_out(DEBUG)<<"TLorentzVector boosted: " << std::endl;
	streamlog_out(DEBUG)<< lab.Px() << " " <<  lab.Py() << " " <<  lab.Pz() << " " <<  lab.E() << std::endl;
      }
      
    }//end of loop over particles in col

    //     _evInfo.rMissEt_PFOColl =  (V.Vect()).Mag();

  }//end of if(col)
  */

}

////////////////////////

void AsymFBProc::boostParticle(TLorentzVector* V, std::string transf){

  if(transf == "mc"){
    V->Rotate(_mcBoostTheta,_mcBoostAxis);
    V->Boost(_mcBoost);
  }
  if(transf.compare("sim")){
    //to be implemented
  }

}
////////////////////////

bool AsymFBProc::findMCBeamstrahlungPhotons(){

  bool n_present = false;
  EVENT::LCCollection* mcp = _evt->getCollection(_colMCName);
  for (int i = 0; i < mcp->getNumberOfElements(); i++){
    EVENT::MCParticle* mcpi = (EVENT::MCParticle*) mcp->getElementAt(i);
    if (abs(mcpi->getPDG()) == 22) 
      if(mcpi->getParents().size() > 0)
	if ( abs(mcpi->getParents()[0]->getParents()[0]->getPDG()) == 11 && mcpi->getDaughters().size()==0){
	  n_present = true;
	  _genStrahlung.push_back( mcpi ) ;
	  //_evInfo.gNuInd.push_back( i );
	}
  }
  return n_present;
}

//////////////////

ReconstructedParticle* AsymFBProc::getRecoElec(){
	  // to return the reconstructed e/mu signal lepton
	  // if a tau was generated, return nothing
	
	  LCRelation* reco_elec_link = 0;
	  MCParticle* mc_elec = 0;
	  ReconstructedParticle* reco_elec = 0;
	  LCCollection* links = _evt->getCollection("RecoMCTruthLink");
	  if(findGenElec()) mc_elec = _genElectrons[0];
	  for (int i = 0; i < links->getNumberOfElements(); i++){
	    LCRelation* lcrel = (EVENT::LCRelation*) links->getElementAt(i);
	    MCParticle* mcpi = (EVENT::MCParticle*) lcrel->getTo();
	    if (mc_elec == mcpi){
	      reco_elec_link = lcrel;
	    }
	  }
	  if (reco_elec_link){
	    EVENT::ReconstructedParticle* rpi = (EVENT::ReconstructedParticle*) reco_elec_link->getFrom();
	    reco_elec = rpi;
	    streamlog_out(DEBUG) << " reco link type:  " <<  rpi->getType() << std::endl ;
	  }
	  return reco_elec;
	}
/////////////////////////

bool AsymFBProc::findGenElec(){

  bool n_present = false;
  EVENT::MCParticle* pointer_to_mc_lepton = 0;
  EVENT::LCCollection* mcp = _evt->getCollection(_colMCName);
  EVENT::MCParticle* mc_el = (EVENT::MCParticle*) mcp->getElementAt(0);
  for(int i = 0 ; i < mc_el->getDaughters().size() ; i++){
    if( abs(mc_el->getDaughters()[i]->getPDG()) == 11){
      n_present = true;
      _genElectrons.push_back(mc_el);   
    }
  }

  return n_present;
}

///////////////////

void AsymFBProc::findCoMBoost(TLorentzVector V1, TLorentzVector V2){

  TVector3 v1 = V1.Vect();
  TVector3 v2 = V2.Vect();
  TVector3 axis_rot,boost;
  Double_t theta_rot;
 
  TVector3 b = v1+v2;
  b *= -1./(V1.E()+V2.E());
  TLorentzVector el_b = V1;
  el_b.Boost(boost);
  TVector3 z(0,0,1);
  TVector3 zpr = (el_b.Vect()).Unit();
  axis_rot = z.Cross(zpr);
  theta_rot = TMath::ASin(axis_rot.Mag());
  
  
  /*
  TLorentzVector V=V1+V2;
  streamlog_out(DEBUG)<< "theta " << v1.Angle(v2) << std::endl;
  V.Rotate(theta_rot,axis_rot);
  V.Boost(b);
  V1.Rotate(theta_rot,axis_rot);
  V1.Boost(b);
  */
  //return b;
}


/////////////////

TLorentzVector* AsymFBProc::makeTLVec(LCObject* part, std::string type="mc"){

  TLorentzVector* V=NULL;
  if(type=="mc"){
    MCParticle* mcpart =  dynamic_cast<MCParticle*>( part );
    V = new TLorentzVector(mcpart->getMomentum(),mcpart->getEnergy() );
  }
  if(type=="recp"){
    ReconstructedParticle* recopart =  dynamic_cast<ReconstructedParticle*>( part );
    V = new TLorentzVector(recopart->getMomentum(),recopart->getEnergy() );
  }
  return V;
}
