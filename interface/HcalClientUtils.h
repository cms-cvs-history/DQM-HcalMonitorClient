#ifndef DQM_HCALCLIENTUTILS_H
#define DQM_HCALCLIENTUTILS_H

#include "TH1F.h"
#include "TH1.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <string>
#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/CollateMonitorElement.h"
#include "DQMServices/UI/interface/MonitorUIRoot.h"
#include "DQMServices/QualityTests/interface/QCriterionRoot.h"
#include "DQMServices/Core/interface/QReport.h"

#include "TROOT.h"
#include "TGaxis.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void resetME(const char* name, DaqMonitorBEInterface* dbe);

bool isValidGeom(int subdet, int iEta, int iPhi, int depth);

TH2F* getHisto2(string name, string process, DaqMonitorBEInterface* dbe_, bool verb=false, bool clone=false);
TH1F* getHisto(string name, string process, DaqMonitorBEInterface* dbe_, bool verb=false, bool clone=false);

TH2F* getHisto2(const MonitorElement* me, bool verb=false, bool clone=false);
TH1F* getHisto(const MonitorElement* me, bool verb=false, bool clone=false);

string getIMG(int runNo,TH1F* hist, int size, string htmlDir, const char* xlab, const char* ylab);
string getIMG2(int runNo,TH2F* hist, int size, string htmlDir, const char* xlab, const char* ylab, bool color=false);
  
void histoHTML(int runNo,TH1F* hist, const char* xlab, const char* ylab, int width, ofstream& htmlFile, string htmlDir);
void histoHTML2(int runNo,TH2F* hist, const char* xlab, const char* ylab, int width, ofstream& htmlFile, string htmlDir, bool color=false);

void htmlErrors(int runNo,string htmlDir, string client, string process, DaqMonitorBEInterface* dbe, map<string, vector<QReport*> > mapE, map<string, vector<QReport*> > mapW, map<string, vector<QReport*> > mapO);

void createXRangeTest(DaqMonitorBEInterface* dbe, vector<string>& params);
void createYRangeTest(DaqMonitorBEInterface* dbe, vector<string>& params);
void createMeanValueTest(DaqMonitorBEInterface* dbe, vector<string>& params);
void createH2CompTest(DaqMonitorBEInterface* dbe, vector<string>& params, TH2F* ref);
void createH2ContentTest(DaqMonitorBEInterface* dbe, vector<string>& params);

void dumpHisto(TH1F* hist, vector<string> &names, 
	       vector<double> &meanX, vector<double> &meanY, 
	       vector<double> &rmsX, vector<double> &rmsY);
void dumpHisto2(TH2F* hist, vector<string> &names, 
	       vector<double> &meanX, vector<double> &meanY, 
	       vector<double> &rmsX, vector<double> &rmsY);

#endif
