void merge(TString input_list = "files.dat",TString output_file = "" ){
  
  TGrid::Connect("alien://");
  // Create the input stream
  TString filename;
  TString alienname;
  ifstream infile(input_list.Data());
  if (! infile.is_open()){
    printf ("error openening %s \n",input_list.Data());
    return;
  }    
  

Int_t k=1;


  alienname="alien://";
  char ofile[50];

while (infile.good()){

  Int_t i=0;
  TFileMerger *FM = new TFileMerger();

    while (infile.good() && i<500){
     infile >> filename;
     
     filename=alienname+filename;
     FM->AddFile(filename);
     i=i+1;
 }

 sprintf(ofile,"merged%d.root",k);
 k=k+1;
 FM->OutputFile(ofile);
 FM->SetFastMethod(kFALSE);
 cout << "Merging into " << ofile << "..." << endl;
 FM->Merge();
}
 cout << "Done!!" << endl;
}
