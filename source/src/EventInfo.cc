#include "EventInfo.h" 
#include "marlin/VerbosityLevels.h"

using namespace std ;

EventInfo::EventInfo(){

  _Tree = NULL;
  _trees = new TObjArray();

}
void EventInfo::resetVars(){


  //resets ALL variables in ALL trees!!

  //########### 'Main'-type Trees

  gInvMass = -500.;
  gTheta1  = -500.;
  gTheta2 = -500.;
  gPhi1 = -500.;
  gPhi2 = -500.;
  gTheta1Bstd  = -500.;
  gTheta2Bstd = -500.;
  gPhi1Bstd = -500.;
  gPhi2Bstd = -500.;
  gShowerInvMass = -500.;
  gShowerTheta1  = -500.;
  gShowerTheta2 = -500.;
  gShowerPhi1 = -500.;
  gShowerPhi2 = -500.;
  gMissEt = -500.;
  gMissEt4V = -500.;
  gMissEtBstd = -500.;
  gMissEtBstd4V = -500.;
  gE_nunu = -500.;

  gB = false;
  gC = false;
  gUDS = false;
  gT = false;

  rJetInvMass = -500.;
  rTheta1 = -500.;
  rTheta2 = -500.;
  rPhi1 = -500.;
  rPhi2 = -500.;
  rbTag1 = -500.;
  rbTag2 = -500.;
  rcTag1 = -500.;
  rcTag2 = -500.;
  rMissEt = -500.;
  rMissEt_PFOColl = -500.;
  rMissEt_FastJet = -500.;
 
  //for RefinedJet from flavour tagging
  rRefJetInvMass = -500.;
  rRefTheta1 = -500.;
  rRefTheta2 = -500.;
  rRefPhi1 = -500.;
  rRefPhi2 = -500.;
   
  rRefJetCharge1 = -500.;
  rRefJetCharge2 = -500.;
  rPtWCharge1 = -500.;
  rPtWCharge2 = -500.;
  rPWCharge1 = -500.;
  rPWCharge2 = -500.;
  rPlWCharge1 = -500.;
  rPlWCharge2 = -500.;
  

  rSdR11 = rSdR12 = rSdR21 = rSdR22 = rdR11 = rdR12 = rdR21 = rdR22 = -500;

  gJM1 = gJM2 = rSVCh = gSVFl = -500;
  rPVx = rPVy = rPVz = -500;

  rBx.clear();  rBy.clear();  rBz.clear();
  rBTheta.clear();  rBPhi.clear(); rBVJetAsg.clear();
  rBVCh.clear(); gBVFl.clear();

  _thrstAxis.Clear();  _thrstAxisCalo.Clear();
  thrust = -1;//for testing thrust
  thrTheta = -1;//for testing thrust
  thrPhi = -1;//for testing thrust
  thrustPand = -1;//for testing thrust
  thrThetaPand = -1;//for testing thrust
  thrPhiPand = -1;//for testing thrust
  BVDists.clear();//for testing thrust
  BVDistsPand.clear();//for testing thrust
  rFrwHemCh = rBckHemCh = rFrwHemChCalo = rBckHemChCalo = -500;
  rHemChFlow = rHemChFlowCalo = -500;
  rFrwHemChPow12 =  rBckHemChPow12 = rFrwHemChCaloPow12 =  rBckHemChCaloPow12 = -500;
  rHemChFlowPow12 = rHemChFlowCaloPow12 = -500;
  rFrwHemChPow08 =  rBckHemChPow08 = rFrwHemChCaloPow08 =  rBckHemChCaloPow08 = -500;
  rHemChFlowPow08 = rHemChFlowCaloPow08= -500;

  gSPrimeRatio = -1;
  gSPrime = -1;
  recSPrimeRatio = -1;
  recSPrimeRatioTagJets = -1;
  recSPrime = -1;

  //########### MCParticles type Trees
  gx .clear(); gy.clear(); gz.clear(); gpz.clear(); gpy.clear(); gpx.clear();ge.clear(); 
  gvx .clear(); gvy .clear(); gvz .clear();  gmass .clear(); gcharge .clear(); 
  gindex .clear();  gpdg .clear(); 
  gNuInd.clear();gQuarkInd.clear();
  g_parents.clear();
  g_daughters.clear();
  //############ RecLevel Full TRee
  rPx.clear();  rPy.clear();  rPz.clear();  rE.clear();  rCharge.clear();rJetID.clear();
}

void EventInfo::initTree(TString  TName){

  _Tree = (TTree*) _trees->FindObject(TName);

  if( _Tree != NULL ){

    if( TName.Contains("Main") || TName.Contains("main")){

      _Tree->Branch("gInvMass",&gInvMass,"gInvMass/D");
      _Tree->Branch("gTheta1",&gTheta1,"gTheta1/D");
      _Tree->Branch("gTheta2",&gTheta2,"gTheta2/D");
      _Tree->Branch("gPhi1",&gPhi1,"gPhi1/D");
      _Tree->Branch("gPhi2",&gPhi2,"gPhi2/D");  
      _Tree->Branch("gB",&gB,"gB/O");
      _Tree->Branch("gC",&gC,"gC/O");
      _Tree->Branch("gT",&gT,"gT/O");
      _Tree->Branch("gUDS",&gUDS,"gUDS/O");
      _Tree->Branch("gTheta1Bstd",&gTheta1Bstd,"gTheta1Bstd/D");
      _Tree->Branch("gTheta2Bstd",&gTheta2Bstd,"gTheta2Bstd/D");
      _Tree->Branch("gPhi1Bstd",&gPhi1Bstd,"gPhi1Bstd/D");
      _Tree->Branch("gPhi2Bstd",&gPhi2Bstd,"gPhi2Bstd/D"); 
     _Tree->Branch("gE_nunu",&gE_nunu,"gE_nunu/D");  
     _Tree->Branch("gMissEt",&gMissEt,"gMissEt/D");
      _Tree->Branch("gMissEtBstd",&gMissEtBstd,"gMissEtBstd/D");
        _Tree->Branch("gMissEt4V",&gMissEt4V,"gMissEt4V/D");
      _Tree->Branch("gMissEtBstd4V",&gMissEtBstd4V,"gMissEtBstd4V/D");
  
      _Tree->Branch("gShowerInvMass",&gShowerInvMass,"gShowerInvMass/D");
      _Tree->Branch("gShowerTheta1",&gShowerTheta1,"gShowerTheta1/D");
      _Tree->Branch("gShowerTheta2",&gShowerTheta2,"gShowerTheta2/D");
      _Tree->Branch("gShowerPhi1",&gShowerPhi1,"gShowerPhi1/D");
      _Tree->Branch("gShowerPhi2",&gShowerPhi2,"gShowerPhi2/D");  
      
      _Tree->Branch("gSPrime",&gSPrime,"gSPrime/D");  
      _Tree->Branch("gSPrimeRatio",&gSPrimeRatio,"gSPrimeRatio/D");  
      _Tree->Branch("recSPrime",&recSPrime,"recSPrime/D");  
      _Tree->Branch("recSPrimeRatio",&recSPrimeRatio,"recSPrimeRatio/D");  
      _Tree->Branch("recSPrimeRatioTagJets",&recSPrimeRatioTagJets,"recSPrimeRatioTagJets/D");  
      
      _Tree->Branch("rJetInvMass",&rJetInvMass,"rJetInvMass/D");
      _Tree->Branch("rTheta1",&rTheta1,"rTheta1/D");
      _Tree->Branch("rTheta2",&rTheta2,"rTheta2/D");
      _Tree->Branch("rPhi1",&rPhi1,"rPhi1/D");
      _Tree->Branch("rPhi2",&rPhi2,"rPhi2/D");
      
      _Tree->Branch("rRefJetInvMass",&rRefJetInvMass,"rRefJetInvMass/D");
      _Tree->Branch("rRefTheta1",&rRefTheta1,"rRefTheta1/D");
      _Tree->Branch("rRefTheta2",&rRefTheta2,"rRefTheta2/D");
      _Tree->Branch("rRefPhi1",&rRefPhi1,"rRefPhi1/D");
      _Tree->Branch("rRefPhi2",&rRefPhi2,"rRefPhi2/D");
      
      _Tree->Branch("rbTag1",&rbTag1,"rbTag1/D");
      _Tree->Branch("rbTag2",&rbTag2,"rbTag2/D");
      _Tree->Branch("rcTag1",&rcTag1,"rcTag1/D");
      _Tree->Branch("rcTag2",&rcTag2,"rcTag2/D");
      _Tree->Branch("rMissEt",&rMissEt,"rMissEt/D");
      _Tree->Branch("rMissEt_PFOColl",&rMissEt_PFOColl,"rMissEt_PFOColl/D");
      _Tree->Branch("rMissEt_FastJet",&rMissEt_FastJet,"rMissEt_FastJet/D");
      _Tree->Branch("rMissEt4V",&rMissEt4V,"rMissEt4V/D");
      
      //weighted charges for jets
      _Tree->Branch("rRefJetCharge1",&rRefJetCharge1,"rRefJetCharge1/D");
      _Tree->Branch("rRefJetCharge2",&rRefJetCharge2,"rRefJetCharge2/D");
      _Tree->Branch("rPtWCharge1",&rPtWCharge1,"rPtWCharge1/D");
      _Tree->Branch("rPtWCharge2",&rPtWCharge2,"rPtWCharge2/D");
      _Tree->Branch("rPWCharge1",&rPWCharge1,"rPWCharge1/D");
      _Tree->Branch("rPWCharge2",&rPWCharge2,"rPWCharge2/D");
      _Tree->Branch("rPlWCharge1",&rPlWCharge1,"rPlWCharge1/D");
      _Tree->Branch("rPlWCharge2",&rPlWCharge2,"rPlWCharge2/D");
      
      /// dR distances for jet-parton matching
      _Tree->Branch("rSdR11",&rSdR11,"rSdR11/D");
      _Tree->Branch("rSdR12",&rSdR12,"rSdR12/D");
      _Tree->Branch("rSdR21",&rSdR21,"rSdR21/D");
      _Tree->Branch("rSdR22",&rSdR22,"rSdR22/D");
      _Tree->Branch("rdR11",&rdR11,"rdR11/D");
      _Tree->Branch("rdR12",&rdR12,"rdR12/D");
      _Tree->Branch("rdR21",&rdR21,"rdR21/D");
      _Tree->Branch("rdR22",&rdR22,"rdR22/D");
      
      //vertices charges and j matching
      _Tree->Branch("rSVCh",&rSVCh,"rSVCh/D");
      _Tree->Branch("gSVFl",&gSVFl,"gSVFl/D");  
      _Tree->Branch("rPVx",&rPVx,"rPVx/D");
      _Tree->Branch("rPVy",&rPVy,"rPVy/D");
      _Tree->Branch("rPVz",&rPVz,"rPVz/D");
      _Tree->Branch("rBx",&rBx);
      _Tree->Branch("rBy",&rBy);
      _Tree->Branch("rBz",&rBz);
      _Tree->Branch("rBTheta",&rBTheta);
      _Tree->Branch("rBPhi",&rBPhi);
      _Tree->Branch("rBVCh",&rBVCh);  
      _Tree->Branch("rBVJetAsg",&rBVJetAsg);  
      _Tree->Branch("gBVFl",&gBVFl);
      _Tree->Branch("gJM1",&gJM1,"gJM1/D");
      _Tree->Branch("gJM2",&gJM2,"gJM2/D");
      
      //thrust and hemispheres vars
      _Tree->Branch("thrust",&thrust,"thrust/D");
      _Tree->Branch("thrTheta",&thrTheta,"thrTheta/D");
      _Tree->Branch("thrPhi",&thrPhi,"thrPhi/D");
      _Tree->Branch("thrustPand",&thrustPand,"thrustPand/D");
      _Tree->Branch("thrThetaPand",&thrThetaPand,"thrThetaPand/D");
      _Tree->Branch("thrPhiPand",&thrPhiPand,"thrPhiPand/D");
      _Tree->Branch("BVDists",&BVDists);
      _Tree->Branch("BVDistsPand",&BVDistsPand);
      
      _Tree->Branch("rFrwHemCh",&rFrwHemCh,"rFrwHemCh/D");
      _Tree->Branch("rBckHemCh",&rBckHemCh,"rBckHemCh/D");
      _Tree->Branch("rFrwHemChCalo",&rFrwHemChCalo,"rFrwHemChCalo/D");
      _Tree->Branch("rBckHemChCalo",&rBckHemChCalo,"rBckHemChCalo/D");
      _Tree->Branch("rHemChFlow",&rHemChFlow,"rHemChFlow/D");
      _Tree->Branch("rHemChFlowCalo",&rHemChFlowCalo,"rHemChFlowCalo/D");
      
      _Tree->Branch("rFrwHemChPow12",&rFrwHemChPow12,"rFrwHemChPow12/D");
      _Tree->Branch("rBckHemChPow12",&rBckHemChPow12,"rBckHemChPow12/D");
      _Tree->Branch("rFrwHemChCaloPow12",&rFrwHemChCaloPow12,"rFrwHemChCaloPow12/D");
      _Tree->Branch("rBckHemChCaloPow12",&rBckHemChCaloPow12,"rBckHemChCaloPow12/D");
      _Tree->Branch("rHemChFlowPow12",&rHemChFlowPow12,"rHemChFlowPow12/D");
      _Tree->Branch("rHemChFlowCaloPow12",&rHemChFlowCaloPow12,"rHemChFlowCaloPow12/D");
      _Tree->Branch("rFrwHemChPow08",&rFrwHemChPow08,"rFrwHemChPow08/D");
      _Tree->Branch("rBckHemChPow08",&rBckHemChPow08,"rBckHemChPow08/D");
      _Tree->Branch("rFrwHemChCaloPow08",&rFrwHemChCaloPow08,"rFrwHemChCaloPow08/D");
      _Tree->Branch("rBckHemChCaloPow08",&rBckHemChCaloPow08,"rBckHemChCaloPow08/D");
      _Tree->Branch("rHemChFlowPow08",&rHemChFlowPow08,"rHemChFlowPow08/D");
      _Tree->Branch("rHemChFlowCaloPow08",&rHemChFlowCaloPow08,"rHemChFlowCaloPow08/D");
    }
    if(TName.Contains("genFull") || TName.Contains("genfull")){
      _Tree->Branch("gx",&gx);
      _Tree->Branch("gy",&gy);
      _Tree->Branch("gz",&gz);
      _Tree->Branch("gpx",&gpx);
      _Tree->Branch("gpy",&gpy);
      _Tree->Branch("gpz",&gpz);
      _Tree->Branch("ge",&ge);
      _Tree->Branch("gvx",&gvx);
      _Tree->Branch("gvy",&gvy);
      _Tree->Branch("gvz",&gvz);
      _Tree->Branch("gmass",&gmass);
      _Tree->Branch("gcharge",&gcharge);
      _Tree->Branch("gindex",&gindex);
      _Tree->Branch("gpdg",&gpdg);
      //*g_parents = std::vector< std::vector<int> >();
      //*g_daughters = std::vector< std::vector<int> >();
      _Tree->Branch("g_parents",&g_parents);
      _Tree->Branch("g_daughters",&g_daughters);
      _Tree->Branch("gNuInd",&gNuInd);
      _Tree->Branch("gQuarkInd",&gQuarkInd);
    }
    if(TName.Contains("recFull") || TName.Contains("recfull")){
      _Tree->Branch("rPx",&rPx);
      _Tree->Branch("rPy",&rPy);
      _Tree->Branch("rPz",&rPz);
      _Tree->Branch("rE",&rE);
      _Tree->Branch("rCharge",&rCharge);
      _Tree->Branch("rJetID",&rJetID);
    }

  }else//end of tree-existence check
    streamlog_out(WARNING) << "WARNING in EventInfo::initTree : a TTree with name "<< TName << " does not exist!"<<std::endl;

}

void EventInfo::fillTree(TString TName){
  
  if(TName.Sizeof()>0){
    TTree* temp = (TTree*) _trees->FindObject(TName) ;
    if(temp!=NULL)
      temp->Fill();
  }else
    _Tree->Fill();
  
}
void EventInfo::writeTree(TString TName, TString OutName){
  streamlog_out(DEBUG)<< "writing "<< TName << " as " << OutName <<std::endl;
  if(TName.Sizeof()>0){
    TTree* temp = (TTree*) _trees->FindObject(TName) ;
    if(temp!=NULL)
      if(OutName.Sizeof()>0)
	temp->Write(OutName);
      else
	temp->Write(temp->GetName());
  }else
    _Tree->Write();
  
}

void EventInfo::AddTree(TString TName){

  //this method should be called only once for each tree thus the warning below
  _Tree = (TTree*) _trees->FindObject(TName);
  if( _Tree != NULL )
    std::cout << "WARNING in EventInfo::AddTree : a TTree with name "<< TName << " already exists!"<<std::endl;
  else{
    if(TName.Contains("GenFull") || TName.Contains("genFull"))
      std::cout<< "Adding generator level Full-type Tree"<< std::endl;
    else if(TName.Contains("RecFull") || TName.Contains("recFull"))
      std::cout<< "Adding reconstructed level Full-type Tree"<< std::endl;
    else if(TName.Contains("Main") || TName.Contains("main"))
      std::cout<< "Adding Main-type Tree"<< std::endl;
    else
      streamlog_out(WARNING)<< "Tree Name does not contain tree-type info!"<< std::endl;
    _Tree = new TTree(TName, TName);
    _trees->Add(_Tree);
    initTree(TName);
  }

}
