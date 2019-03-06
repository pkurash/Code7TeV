#if !defined (__CINT__) || defined (__CLING__)
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskGammaPHOS7TeV.h"
#include "AliCaloPhoton.h"
#include "AliTender.h"
#endif
void LoadEnv(); 
void runAnalysis2(Bool_t local = kTRUE, TString period = "LHC14j4b", TString runmode = "terminate")
{

    // if you run on grid, specify test mode (kTRUE) or full grid model (kFALSE)
    Bool_t gridTest = kFALSE;

    LoadEnv();

    // since we will compile a class, tell root where to look for headers  
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
    gInterpreter->ProcessLine(".include $ALICE_PHYSICS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/PHOS");

#else
    gROOT->ProcessLine(".include $ROOTSYS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/include");
    gROOT->ProcessLine(".include $ALICE_PHYSICS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/PHOS");
#endif
     
    // create the analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager("GammaAnalysis");
    AliAODInputHandler *aodH = new AliAODInputHandler();
    mgr->SetInputEventHandler(aodH);

/*-----------------------------------*/

 TGrid::Connect("alien://");

/*----------------------------------*/

 Bool_t isMC=kTRUE; // kTRUE in case of MC

  AliPHOSTenderTask *tender = reinterpret_cast<AliPHOSTenderTask *>( gInterpreter ->
  ExecuteMacro(Form("$ALICE_PHYSICS/PWGGA/PHOSTasks/PHOS_PbPb/AddAODPHOSTender.C(\"%s\", \"%s\", \"%s\", %d, %d)", "PHOSTenderTask",  "PHOStender", "LHC14j4b", 1, kTRUE)));
  
  AliPHOSTenderSupply *supply = tender->GetPHOSTenderSupply();

    TString nonlinearity = isMC ? "MC" : "Default";
    supply->SetNonlinearityVersion(nonlinearity); 

  Double_t NonlinPar[3]={1.008, 0.045, 0.4};
  supply->SetNonlinearityParams(3, NonlinPar); 

  supply->ForceUsingBadMap("alien:///alice/cern.ch/user/p/pkurash/BadMap_LHC10ef_Majority300416.root");

  //gROOT->LoadMacro("CreatePHOSTender.C");
  // gInterpreter->LoadMacro("./CreatePHOSTender.C");

/*--------------------------*/
 Int_t recoPass;

 TMacro addresp(gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"));
 addresp.Exec(Form("%d", isMC));
 addresp.Exec("4");

/*----------------------------------*/


    // compile the class and load the add task macro
    // here we have to differentiate between using the just-in-time compiler
    // from root6, or the interpreter of root5
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->LoadMacro("AliCaloPhoton.cxx++g");
    gInterpreter->LoadMacro("AliAnalysisTaskGammaPHOS7TeV.cxx++g");
    AliAnalysisTaskGammaPHOS7TeV *task = reinterpret_cast<AliAnalysisTaskGammaPHOS7TeV*>(gInterpreter->ExecuteMacro("AddMyTask2.C"));
#else
    gROOT->LoadMacro("AliCaloPhoton.cxx++g");
    gROOT->LoadMacro("AliAnalysisTaskGammaPHOS7TeV.cxx++g");
    gROOT->LoadMacro("AddMyTask2.C");
    AliAnalysisTaskGammaPHOS7TeV *task = AddMyTask2();
#endif
/*----------------------------------*/

    if(!mgr->InitAnalysis()) return;
    mgr->SetDebugLevel(2);
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);

    if(local) 
    {
        // if you want to run locally, we need to define some input
          TChain* chain = new TChain("aodTree");
        // add a few files to the chain (change this so that your local files are added)
     //   chain->Add("alien:///alice/sim/2014/LHC14j4b/117060/666/AliAOD.root");
          chain->Add("../MC_AODs/AliAOD.root");
        //chain->Add("AliAOD.root");
        // start the analysis locally, reading the events from the tchain
        mgr->StartAnalysis("local", chain);
     } 
     else 
     {
        // if we want to run on grid, we create and configure the plugin
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
        // also specify the include (header) paths on grid
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
        // make sure your source files get copied to grid
        //alienHandler->SetAdditionalLibs("AliAnalysisTaskMyTask.cxx AliAnalysisTaskMyTask.h");
        alienHandler->SetAdditionalLibs("AliAnalysisTaskGammaPHOS7TeV.h AliAnalysisTaskGammaPHOS7TeV.cxx AliCaloPhoton.h AliCaloPhoton.cxx  libTender.so libTenderSupplies.so libPWGGAPHOSTasks.so");
        alienHandler->SetAnalysisSource("AliAnalysisTaskGammaPHOS7TeV.cxx");
        // select the aliphysics version. all other packages
        // are LOADED AUTOMATICALLY!
        alienHandler->SetAliPhysicsVersion("vAN-20190303_ROOT6-1");
        // set the Alien API version
        alienHandler->SetAPIVersion("V1.1x");
        // select the input data
        alienHandler->SetGridDataDir(Form("/alice/sim/2014/%s", period.Data()));
        alienHandler->SetDataPattern("AOD/*/AliAOD.root");
        // MC has no prefix, data has prefix 000
        //alienHandler->SetRunPrefix("000");
        // runnumber
        //alienHandler->AddRunNumber(167813);
        /*---------------------*/
        //runnumber
        Int_t evN[500];
        Int_t nn = 0;
        ifstream ff;
         ff.open(Form("datasets/runs_%s.list", period.Data()));
        //Add runs
        while( !ff.eof() )
        {
         ff>>evN[nn];
         nn = nn + 1;
        }
        ff.close();
        /*--------------------*/
        for(Int_t  i = 0; i < nn; i ++)
          alienHandler->AddRunNumber(evN[i]);
          
        /*---------------------*/
        // number of files per subjob
        alienHandler->SetSplitMaxInputFileNumber(50);
        alienHandler->SetExecutable("myTask.sh");
        // specify how many seconds your job may take
        alienHandler->SetTTL(21600);
        alienHandler->SetJDLName("myTask.jdl");

        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);
        // merging: run with kTRUE to merge on grid
        // after re-running the jobs in SetRunMode("terminate") 
        // (see below) mode, set SetMergeViaJDL(kFALSE) 
        // to collect final results
        alienHandler->SetMaxMergeStages(1);
        alienHandler->SetMergeViaJDL(kTRUE);

        // define the output folders
        //alienHandler->SetGridWorkingDir(Form("pp_Analysis/LHC14j4%s", per));
        alienHandler->SetGridWorkingDir(Form("pp_Analysis/%s_matrix_conv", period.Data()));
        alienHandler->SetGridOutputDir("pass4");

        // connect the alien plugin to the manager
        mgr->SetGridHandler(alienHandler);
        if(gridTest) {
            // speficy on how many files you want to run
            alienHandler->SetNtestFiles(1);
            // and launch the analysis
            alienHandler->SetRunMode("test");
            mgr->StartAnalysis("grid");
        } else {
            // else launch the full grid analysis
            alienHandler->SetRunMode(runmode.Data());
            mgr->StartAnalysis("grid");
        }
      }
}
void LoadEnv()
{

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
