#include <string>
#include <iostream>
#include "DQM/HcalMonitorClient/interface/HcalMonitorClient.h"
#include "TFile.h"

int main(int argc, char *argv[]){
  
  TFile* infile = new TFile(argv[1]);
  
  HcalMonitorClient monc;
  monc.offlineSetup();
  monc.loadHistograms(infile);
  monc.htmlOutput();

  return 0;
}
