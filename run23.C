void run23(const char *collectionfile = "LHC10a2.xml")
{

  LoadEnv();

 TGrid::Connect("alien://");

  // for includes use either global setting in $HOME/.rootrc
  // ACLiC.IncludePaths: -I$(ALICE_ROOT)/include
  // or in each macro
  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/include");

  // Create the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("testAnalysis");

//===
 gROOT->LoadMacro("CreateAlienHandler23.C");
  AliAnalysisGrid *alienHandler = CreateAlienHandler23();  
  if (!alienHandler) return;

  // Create the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("testAnalysis");

  // Connect plug-in to the analysis manager
  mgr->SetGridHandler(alienHandler);


  // AOD input handler
  AliAODInputHandler* aodH = new AliAODInputHandler();
  mgr->SetInputEventHandler(aodH);

  // Tender


const Int_t recoPass=2;
Bool_t isMC=kTRUE; // kTRUE in case of MC
Bool_t isMC=kFALSE; // kTRUE in case of MC

   gROOT->LoadMacro("$ALICE_PHYSICS/PWGGA/PHOSTasks/PHOS_PbPb/AddAODPHOSTender.C"); 
   TString tenderOption = isMC ? "Run2Default" : "";
 //  AliPHOSTenderTask * tenderPHOS = AddAODPHOSTetSTender("PHOSTenderTask", "PHOStender", tenderOption, 1, isMC);
   AliPHOSTenderTask * tenderPHOS = AddAODPHOSTender("PHOSTenderTask", "PHOStender", tenderOption, 1, isMC);
  // tenderPHOS->SetHandleOCDB(kTRUE);//!!!
   AliPHOSTenderSupply * PHOSSupply = tenderPHOS->GetPHOSTenderSupply();
   PHOSSupply->ForceUsingBadMap("alien:///alice/cern.ch/user/p/pkurash/BadMap_LHC16-updated.root");

if(isMC)
{
    // Use custom Zero Suppression threshold if needed
    Double_t zs_threshold = 0.020;
    PHOSSupply->ApplyZeroSuppression(zs_threshold); 
}

  gROOT->LoadMacro("CreatePHOSTender.C");


 // gROOT->LoadMacro("CreatePHOSTender.C");
  
  //PID responce
//===============
 gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
 AddTaskPIDResponse(isMC); 
 AddTaskPIDResponse(recoPass); 
//=========================

  //Create task
  gROOT->LoadMacro("./AliCaloPhoton.cxx+g");
  gROOT->LoadMacro("./AliAnalysisTaskGamma_aod.cxx+g");
  AliAnalysisTaskGamma_aod *taskpt = new AliAnalysisTaskGamma_aod("TaskPtMC");

  
  taskpt->SetRecalib(1,0.9822696);
  taskpt->SetRecalib(2,0.9861288);
  taskpt->SetRecalib(3,1.0072);

//Physics selection
   gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
   AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection();
   //taskpt->SelectCollisionCandidates(AliVEvent::kMB);
   taskpt->SelectCollisionCandidates(AliVEvent::kINT7);
 
 // Add task(s)
  mgr->AddTask(taskpt);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
 // AliAnalysisDataContainer *coutput0 = mgr->GetCommonOutputContainer();
  AliAnalysisDataContainer *coutputpt = mgr->CreateContainer("Data", TList::Class(),   
      AliAnalysisManager::kOutputContainer, "PtMC.AOD.1.root");

  
  AliAnalysisDataContainer *coutput0 = mgr->CreateContainer("aod", TTree::Class(),    AliAnalysisManager::kOutputContainer, "default");


  // Connect input/output
  mgr->ConnectInput(taskpt, 0, cinput);
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


