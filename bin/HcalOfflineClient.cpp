#include <string>
#include <iostream>
#include "DQM/HcalMonitorClient/interface/HcalMonitorClient.h"
#include "TFile.h"

int main(int argc, char *argv[]){
  
  TFile* infile = new TFile(argv[1]);

  printf("\n\n*****************************\n");
  printf("   Hcal Offline DQM Client\n");
  printf("*****************************\n");
  
  HcalMonitorClient monc;
  printf("\nSetting up clients...\n");
  monc.offlineSetup();
  printf("\nLoading histograms...\n");
  monc.loadHistograms(infile,argv[1]);
  printf("\nGenerating HTML output...\n");
  monc.htmlOutput();
  printf("\nDONE!\n");

  return 0;
}
