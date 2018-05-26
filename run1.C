void run1(const char *collectionfile = "LHC10a2.xml")
{

  LoadEnv();

 TGrid::Connect("alien://");

  // for includes use either global setting in $HOME/.rootrc
  // ACLiC.IncludePaths: -I$(ALICE_ROOT)/include
  // or in each macro
   gSystem->AddIncludePath("-I$ALICE_ROOT/include");
   gSystem->AddIncludePath("-I$ALICE_PHYSICS/include");

   Bool_t isMC = kTRUE; // kTRUE in case of MC
   Int_t  recoPass = 4;

  // Create the analysis manager
   AliAnalysisManager *mgr = new AliAnalysisManager("testAnalysis");

  // Create and connect Alien handler to the analysis manager
   gROOT->LoadMacro("CreateAlienHandler.C");
   AliAnalysisGrid *alienHandler = CreateAlienHandler();  
   if (!alienHandler) return;
   mgr->SetGridHandler(alienHandler);

  // AOD input handler
   AliAODInputHandler* aodH = new AliAODInputHandler();
   mgr->SetInputEventHandler(aodH);

  //Tender 
   gROOT->LoadMacro("$ALICE_PHYSICS/PWGGA/PHOSTasks/PHOS_PbPb/AddAODPHOSTender.C");
   AliPHOSTenderTask *tender = AddAODPHOSTender("PHOSTenderTask","PHOStender","LHC14j4b",1,kTRUE);
   AliPHOSTenderSupply *supply = tender->GetPHOSTenderSupply();
   supply->ForceUsingBadMap("alien:///alice/cern.ch/user/p/pkurash/BadMap_LHC10ef_Majority300416.root");
  // gROOT->LoadMacro("CreatePHOSTender.C");

  //PID
   gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
   AddTaskPIDResponse(isMC); 
   AddTaskPIDResponse(recoPass); 

  // Create task
   gROOT->LoadMacro("AliCaloPhoton.cxx+g");
   gROOT->LoadMacro("AliAnalysisTaskGamma_aod.cxx+g");
   AliAnalysisTaskGamma_aod *taskpt = new AliAnalysisTaskGamma_aod("taskpt");

  //Physics selection   
   gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
   AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection(kTRUE);
   taskpt->SelectCollisionCandidates(AliVEvent::kMB); 

   // Add task(s) to the manager
   mgr->AddTask(taskpt);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutputpt = mgr->CreateContainer("Data", TList::Class(),   
      AliAnalysisManager::kOutputContainer, "PtMC.AOD.1.root");
  AliAnalysisDataContainer *coutput0 = mgr->CreateContainer("aod", TTree::Class(),   
			 AliAnalysisManager::kOutputContainer, "default");


  // Connect input/output
  mgr->ConnectInput(taskpt,  0, cinput);
  mgr->ConnectOutput(taskpt, 0, coutput0);
  mgr->ConnectOutput(taskpt, 1, coutputpt);


  // Enable debug printouts
  mgr->SetDebugLevel(2);

  if (!mgr->InitAnalysis()) return;
  mgr->PrintStatus();
  mgr->StartAnalysis("grid");
}



TChain *CreateChainFromCollection(const char* xmlfile, const char *treeName="esdTree")
{
// Create a chain from an alien collection.
   TAlienCollection * myCollection  = TAlienCollection::Open(xmlfile);

   if (!myCollection) {
      ::Error("CreateChainSingle", "Cannot create an AliEn collection from %s", xmlfile) ;
      return NULL ;
  }

  TChain* chain = new TChain(treeName);
  myCollection->Reset() ;
  while ( myCollection->Next() ) chain->Add(myCollection->GetTURL("")) ;
  chain->ls();
  return chain;
}
void LoadEnv(){
    
    
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");
  gSystem->Load("libPhysics.so");
  
  //load analysis framework
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libPHOSUtils.so"); 
  gSystem->Load("libPWGGAUtils.so");

   //PHOS Tender

  gSystem->Load("libTender.so");
  gSystem->Load("libTenderSupplies.so");
  gSystem->Load("libPWGGAPHOSTasks.so"); 


}


