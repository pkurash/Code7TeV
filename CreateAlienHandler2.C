AliAnalysisGrid* CreateAlienHandler2()
{
// Check if user has a valid token, otherwise make one. This has limitations.
// One can always follow the standard procedure of calling alien-token-init then
//   source /tmp/gclient_env_$UID in the current shell.
//   if (!AliAnalysisGrid::CreateToken()) return NULL;
   AliAnalysisAlien *plugin = new AliAnalysisAlien();
   plugin->SetOverwriteMode();

// Set the run mode (can be "full", "test", "offline", "submit" or "terminate")

/*============================================================================*/  
  plugin->SetRunMode("terminate");

   char per[1]="e";

/*============================================================================*/  

// Set versions of used packages
   plugin->SetAPIVersion("V1.1x");
   plugin->SetAliPhysicsVersion("vAN-20180425-1");

// Declare input data to be processed.
// Method 1: Create automatically XML collections using alien 'find' command.

// Define production directory LFN
   plugin->SetGridDataDir(Form("/alice/data/2010/LHC10%s",per));
   plugin->SetGridWorkingDir(Form("pp_Analysis/LHC10%s",per));


//Add include path
   plugin->AddIncludePath("-I$ALICE_ROOT/include");
   plugin->AddIncludePath(" -I$ALICE_PHYSICS/include");
   // On real reconstructed data:

// Set data search pattern
   plugin->SetDataPattern("pass4/AOD172/*/AliAOD.root");

// Data pattern for reconstructed data
//   plugin->SetDataPattern("*ESDs/pass4/*ESDs.root");
   plugin->SetRunPrefix("000");   // real data
// ...then add run numbers to be considered

Int_t evN;
 ifstream ff;

     ff.open(Form("runs_LHC10%s.list",per));
//   ff.open("runs_LHC10e_lost.list");
//Add runs

while( !ff.eof() ){
      ff>>evN;  
      plugin->AddRunNumber(evN);
  }

  ff.close();


// Declare alien output directory. Relative to working directory.
   plugin->SetGridOutputDir("pass4"); // In this case will be $HOME/work/output

// Declare the analysis source files names separated by blancs. To be compiled runtime
// using ACLiC on the worker nodes.
   plugin->SetAnalysisSource("AliAnalysisTaskGamma_aod.cxx");

// Declare all libraries (other than the default ones for the framework. These will be
// loaded by the generated analysis macro. Add all extra files (task .cxx/.h) here.
   plugin->SetAdditionalLibs("AliAnalysisTaskGamma_aod.h AliAnalysisTaskGamma_aod.cxx AliCaloPhoton.h AliCaloPhoton.cxx  libTender.so libTenderSupplies.so libPWGGAPHOSTasks.so");

// Declare the output file names separated by blancs.
// (can be like: file.root or file.root@ALICE::Niham::File)
   plugin->SetOutputToRunNo(kTRUE);
   plugin->SetDefaultOutputs(kFALSE);
   plugin->SetOutputFiles("PtMC.AOD.1.root");
   plugin->SetMergeViaJDL(kTRUE);
   plugin->SetCheckCopy(kFALSE);

// Optionally define the files to be archived.
//   plugin->SetOutputArchive("log_archive.zip:stdout,stderr@ALICE::NIHAM::File 
//root_archive.zip:*.root@ALICE::NIHAM::File");
//   plugin->SetOutputArchive("log_archive.zip:stdout,stderr");

// Optionally set a name for the generated analysis macro (default MyAnalysis.C)
   plugin->SetAnalysisMacro("TaskPt.C");

// Optionally set maximum number of input files/subjob (default 100, put 0 to ignore)
   plugin->SetSplitMaxInputFileNumber(20);

// Optionally modify the executable name (default analysis.sh)
   plugin->SetExecutable("TaskPt.sh");

// Optionally set number of failed jobs that will trigger killing waiting sub-jobs.
//   plugin->SetMaxInitFailed(5);
// Optionally resubmit threshold.
//   plugin->SetMasterResubmitThreshold(90);

// Optionally set time to live (default 30000 sec)
   plugin->SetTTL(50000);

// Optionally set input format (default xml-single)
   plugin->SetInputFormat("xml-single");

// Optionally modify the name of the generated JDL (default analysis.jdl)
   plugin->SetJDLName("TaskPt.jdl");

// Optionally modify job price (default 1)
   plugin->SetPrice(1);      

// Optionally modify split mode (default 'se')    
   plugin->SetSplitMode("se");
   return plugin;
     }
