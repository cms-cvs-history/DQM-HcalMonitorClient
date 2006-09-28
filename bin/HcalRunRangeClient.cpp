#include <string>
#include <iostream>
#include <iostream>
#include <fstream>
#include <map>
#include "TFile.h"
#include "TObject.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNamed.h"
#include "DQM/HcalMonitorClient/interface/HcalMonitorClient.h"

using namespace std;

void createHistograms(int lo, int hi, map<string,TH2F*> &out, vector<TH1F*> histos1, vector<TH2F*> histos2){
  printf("Creating histograms...\n");
  out.clear();

  int lowRange = lo-20;
  int highRange = hi+20;
  int bins = highRange-lowRange;
  string name = "";
  for(unsigned int i = 0; i<histos1.size(); i++){    
    TH1F* in = (TH1F*)histos1[i];
    if(in){
      name = (string)((string)(in->GetTitle())+", Mean Run Range");
      TH2F* histM = new TH2F(name.c_str(),name.c_str(),bins,lowRange,highRange,in->GetNbinsX(),in->GetXaxis()->GetXmin(),in->GetXaxis()->GetXmax());
      out[name] = histM;
      name = (string)((string)(in->GetTitle())+", RMS Run Range");
      TH2F* histR = new TH2F(name.c_str(),name.c_str(),bins,lowRange,highRange,in->GetNbinsX(),0,fabs(in->GetXaxis()->GetXmax()-in->GetXaxis()->GetXmin())/2);
      out[name] = histR;
    }
  }

  for(unsigned int i = 0; i<histos2.size(); i++){    
    //    TH2F* hist = new TH2F((string)(names[i]+", Run Range").c_str(),(string)(names[i]+", Run Range").c_str(),bins,lowRange,highRange,X,X,X);
  }
  return;
}

void fillHistograms(int run, map<string,TH2F*> out, vector<TH1F*> histos1, vector<TH2F*> histos2){
  map<string,TH2F*>::const_iterator iter;
  for(unsigned int i = 0; i<histos1.size(); i++){    
    TH1F* in = (TH1F*)histos1[i];
    string name = "";
    if(in){
      name = (string)((string)(in->GetTitle())+", Mean Run Range");
      iter = out.find(name);
      if(iter!=out.end()) {
	iter->second->Fill(run,in->GetMean());
      }
      name = (string)((string)(in->GetTitle())+", RMS Run Range");
      if(out.find(name)!=out.end()) out[name]->Fill(run,in->GetRMS());
    }
  }
  return;
}

int main(int argc, char *argv[]){
  string line;
  ifstream myfile(argv[1]);
  int runLow = atoi(argv[2]);
  int runHigh = atoi(argv[3]);
  printf("Run low: %d, high: %d\n",runLow,runHigh);
  vector<TH1F*> hist1d; hist1d.clear();
  vector<TH2F*> hist2d; hist2d.clear();
  map<string,TH2F*> out; out.clear();

  int runNum=-1;
  bool init = false;
  TFile outfile("globalHistos.root","RECREATE");
  outfile.cd();
  if (myfile.is_open()){
    while (! myfile.eof() ){
      getline(myfile,line);
      hist1d.clear();
      hist2d.clear();
      cout << line << endl;
      TFile* infile = NULL;
      if(line.size()>10) infile = new TFile(line.c_str());
      if(infile!=NULL){
	runNum=-1;
	HcalMonitorClient monc;
	monc.offlineSetup();
	monc.loadHistograms(infile);
	monc.dumpHistograms(runNum,hist1d,hist2d);
	if(!init){
	  outfile.cd();
	  createHistograms(runLow,runHigh,out,hist1d,hist2d);
	  init = true;
	}
	if(runNum<runHigh && runNum>runLow)
	  fillHistograms(runNum,out,hist1d,hist2d);
      }
      if(infile) infile->Close();
      if(infile) delete infile;
      infile = NULL;
    }
  }
  printf("Finishing...\n");
  outfile.cd();
  outfile.Write();
  outfile.Close();
  printf("     Done!\n");
  return 0;
}
