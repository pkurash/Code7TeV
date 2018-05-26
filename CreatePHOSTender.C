void CreatePHOSTender(){

  AliTender *tender=new AliTender("AnalysisTender");
  tender->SetDefaultCDBStorage("alien://folder=/alice/data/2010/OCDB");
  mgr->AddTask(tender);

  AliPHOSTenderSupply *PHOSSupply=new AliPHOSTenderSupply("PHOStender");
  tender->AddSupply(PHOSSupply);
  PHOSSupply->SetMCProduction("LHC14j4b");
//  PHOSSupply->ForceUsingBadMap("BadMap_LHC10ef_Majority300416.root");
  PHOSSupply->SetReconstructionPass(1);
}
