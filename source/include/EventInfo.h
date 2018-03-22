#ifndef MyEventInfo_h
#define MyEventInfo_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <iostream>
#include <vector>

#include <TH1.h>
#include <TVector3.h>
#include <TObjArray.h>
#include <TString.h>
#include <TTree.h>
#include <EVENT/ReconstructedParticle.h>
#include <UTIL/LCRelationNavigator.h>


class EventInfo{

 public: 
  EventInfo();
  void AddTree(TString tname="genFullHouse");//checks existence and calls for initTree and resetVars
  void initTree(TString tname="genFullHouse");
  void resetVars();
  void fillTree(TString tname="genFullHouse");
  void writeTree(TString tname="genFullHouse",TString oname="genFullHouse");


  //########## for 'Main'-type Tree
  double gInvMass,  gTheta1, gTheta2, gPhi1, gPhi2, rPhi1, rPhi2, rJetInvMass, rTheta1, rTheta2;
  double gTheta1Bstd, gTheta2Bstd, gPhi1Bstd, gPhi2Bstd,gMissEtBstd, gMissEtBstd4V;
  double gShowerInvMass,  gShowerTheta1, gShowerTheta2, gShowerPhi1, gShowerPhi2;

  double rRefPhi1, rRefPhi2, rRefJetInvMass, rRefTheta1, rRefTheta2,rRefJetCharge1,rRefJetCharge2;
  double rSdR11, rSdR12, rSdR21, rSdR22, rdR11, rdR12, rdR21, rdR22; 
  double  rPtWCharge1, rPtWCharge2;
  double  rPWCharge1, rPWCharge2;
  double  rPlWCharge1, rPlWCharge2;
  double rSVCh;
  double gSVFl;
  double rPVx,rPVy,rPVz;
  std::vector<double> rBx,rBy,rBz,rBTheta,rBPhi,rBVCh,gBVFl,rBVJetAsg;
  double gJM1, gJM2;
  double  rcTag1, rcTag2, rbTag1, rbTag2;
  double rMissEt, rMissEt_PFOColl, rMissEt_FastJet, gMissEt, gE_nunu, gMissEt4V, rMissEt4V;
  double    dRConeE,cosThetaConeE,refE;

  std::vector<double> BVDists, BVDistsPand;

  double gSPrime, gSPrimeRatio, recSPrime, recSPrimeRatio, recSPrimeRatioTagJets;

  bool bDRIsolated,bCosIsolated;
  bool gB,gC,gUDS,gT;

  TVector3 _thrstAxis, _thrstAxisCalo;
  double thrust, thrTheta,thrPhi,thrustPand, thrThetaPand, thrPhiPand;
  double rFrwHemCh, rBckHemCh,rFrwHemChCalo, rBckHemChCalo;
  double rHemChFlow,rHemChFlowCalo;
  double rFrwHemChPow12, rBckHemChPow12,rFrwHemChCaloPow12, rBckHemChCaloPow12;
  double rHemChFlowPow12,rHemChFlowCaloPow12;
  double rFrwHemChPow08, rBckHemChPow08,rFrwHemChCaloPow08, rBckHemChCaloPow08;
  double rHemChFlowPow08,rHemChFlowCaloPow08;

  //######## end of 'Main'type Tree

  //######### MCParticles Tree
  std::vector<double> gx,gy,gz,gpz,gpy,gpx,ge,gvx,gvy,gvz, gmass,gcharge;   
  std::vector<int> gindex, gpdg;
  std::vector<std::string> g_parents, g_daughters;
  //std::vector< std::vector<int> > *g_parents;
  //std::vector< std::vector<int> > *g_daughters;
  std::vector<double> gNuInd,gQuarkInd;
  //######## end of MCParticles TRee




  //########## RecLevel Full Tree
  std::vector<double> rPx, rPy, rPz, rE,rCharge ;
  std::vector<int> rJetID;

 private:

  TObjArray* _trees; 
  // std::vector<TTree*,std::string> _vecTrees;
  TTree* _Tree;
  std::string _outFileName;
   
};
#endif
