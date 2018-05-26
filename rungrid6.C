#include "AliCaloPhoton7.cxx"
#include "AliAnalysisTaskGamma_aod.cxx"
#include "AliTender.h"
//#include "./CreatePHOSTender.C"
void LoadEnv(); 
void rungrid6(const char *collectionfile = "LHC10a2.xml")
{
     
    LoadEnv();

  //add include path
  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_ROOT/PHOS");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/include");
//
  // A task can be compiled dynamically with AClic

  TGrid::Connect("alien://");

  //gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C");
 // TChain* chain = CreateAODChain("fileAODs.txt", 4);
 // TChain* chain = CreateAODChain("files.txt2", 4);
//TMacro chainadd(gSystem->ExpandPathName("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C"));
//TChain *chain = reinterpret_cast<TChain *>(chainadd.Exec("files.txt2"));

  TChain *chain = reinterpret_cast<TChain *>(gInterpreter->ExecuteMacro(Form("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C(\"%s\", %d)", "files.txt2", 4)));


  

  // for includes use either global setting in $HOME/.rootrc
  // ACLiC.IncludePaths: -I$(ALICE_ROOT)/include
  // or in each macro

  // Create the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("testGridAnalysis");



  // AOD input handler
  AliAODInputHandler* aodH = new AliAODInputHandler();
  mgr->SetInputEventHandler(aodH);


  //Tender
//   gROOT->LoadMacro("$ALICE_PHYSICS/PWGGA/PHOSTasks/PHOS_PbPb/AddAODPHOSTender.C");
  // AliPHOSTenderTask *tenderPHOS = AddAODPHOSTender("PHOSTenderTask","PHOStender","Default",2, kTRUE); 
 // gROOT->LoadMacro("$ALICE_PHYSICS/PWGGA/PHOSTasks/PHOS_PbPb/AddAODPHOSTender.C");

 // AliPHOSTenderTask *tender = AddAODPHOSTender("PHOSTenderTask","PHOStender","LHC14j4b",1,kTRUE);

//----/
  AliPHOSTenderTask *tender = reinterpret_cast<AliPHOSTenderTask *>(gInterpreter->ExecuteMacro(Form("$ALICE_PHYSICS/PWGGA/PHOSTasks/PHOS_PbPb/AddAODPHOSTender.C(\"%s\", \"%s\", \"%s\", %d, %d)", "PHOSTenderTask",  "PHOStender","LHC14j4b",1, kTRUE)));
/*----*/
  AliPHOSTenderSupply *supply = tender->GetPHOSTenderSupply();
  supply->ForceUsingBadMap("alien:///alice/cern.ch/user/p/pkurash/BadMap_LHC10ef_Majority300416.root");
  mgr->AddTask(tender);

  //gROOT->LoadMacro("CreatePHOSTender.C");
   gInterpreter->LoadMacro("./CreatePHOSTender.C");

/*-------------------*/
/*
  AliTender *tender1=new AliTender("AnalysisTender");
  tender1->SetDefaultCDBStorage("alien://folder=/alice/data/2010/OCDB");
  mgr->AddTask(tender);

  AliPHOSTenderSupply *PHOSSupply=new AliPHOSTenderSupply("PHOStender");
  tender1->AddSupply(PHOSSupply);
  PHOSSupply->SetMCProduction("LHC14j4b");
//  PHOSSupply->ForceUsingBadMap("BadMap_LHC10ef_Majority300416.root");
  PHOSSupply->SetReconstructionPass(1);
*/
/*--------------------------*/
Int_t recoPass;
Bool_t isMC=kFALSE; // kTRUE in case of MC
//Bool_t isMC=kTRUE;
//===============
//gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
//AddTaskPIDResponse(isMC); 
//AddTaskPIDResponse(recoPass=4); 

 TMacro addresp(gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"));
 addresp.Exec(Form("%d", isMC));
 addresp.Exec("4");
//=========================

#if !defined (__CINT__) || defined (__CLING__)
//    gInterpreter->LoadMacro("AliAnalysisTaskMyTask.cxx++g");
   gInterpreter->LoadMacro("./AliCaloPhoton7.cxx+g");
   gInterpreter->LoadMacro("./AliAnalysisTaskGamma_aod.cxx+g");
    //AliAnalysisTaskGamma_aod *taskpt = reinterpret_cast<AliAnalysisTaskGamma_aod*>(gInterpreter->ExecuteMacro("AliAnalysisTaskGamma_aod(\"taskpt\")"));
  AliAnalysisTaskGamma_aod *taskpt = new AliAnalysisTaskGamma_aod("taskpt");
#else
  gROOT->LoadMacro("./AliCaloPhoton7.cxx++g");
  gROOT->LoadMacro("./AliAnalysisTaskGamma_aod.cxx++g");
    //gROOT->LoadMacro("AddMyTask.C");
  AliAnalysisTaskGamma_aod *taskpt = new AliAnalysisTaskGamma_aod("taskpt");
#endif

  //Create Task


 // AliAnalysisTaskGamma_aod *taskpt = new AliAnalysisTaskGamma_aod("taskpt");
  /*
   gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
   AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection(kTRUE);
   taskpt->SelectCollisionCandidates(AliVEvent::kMB);
*/
//Physics selection
 //  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
 //  AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection();
   TMacro physseladd(gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C"));
   AliPhysicsSelectionTask *physseltask = reinterpret_cast<AliPhysicsSelectionTask *>(physseladd.Exec());
   taskpt->SelectCollisionCandidates(AliVEvent::kMB);

  // Add task
  mgr->AddTask(taskpt);

    // Create containers for input/output

 AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();




 AliAnalysisDataContainer *coutput = mgr->CreateContainer("Data", TList::Class(),          AliAnalysisManager::kOutputContainer, "PtMC.AOD.1.root");

  
  AliAnalysisDataContainer *coutput0 = mgr->CreateContainer("aod", TTree::Class(),    AliAnalysisManager::kOutputContainer, "default");



//-----------------------------------------------------------------------

  // Connect input/output
  mgr->ConnectInput(taskpt, 0, cinput);
  mgr->ConnectOutput(taskpt, 0, coutput0);
  mgr->ConnectOutput(taskpt, 1, coutput);


  // Enable debug printouts
  mgr->SetDebugLevel(2);

  if (!mgr->InitAnalysis()) return;

//  mgr->PrintStatus();
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);


  mgr->StartAnalysis("local", chain);

  gObjectTable->Print();
}

/*
TChain *CreateChainFromCollection(const char* xmlfile, const char *treeName="aodTree")
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
*/
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
