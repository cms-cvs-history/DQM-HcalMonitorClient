#include <DQM/HcalMonitorClient/interface/HcalDeadCellClient.h>
#include <DQM/HcalMonitorClient/interface/HcalClientUtils.h>

HcalDeadCellClient::HcalDeadCellClient(const ParameterSet& ps, DaqMonitorBEInterface* dbe){
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();

  dbe_ = dbe;
  for(int i=0; i<4; i++){
    occ_geo_[i][0]=0;
    occ_en_[i][0]=0;
    occ_geo_[i][1]=0;
    occ_en_[i][1]=0;
    gl_geo_[i]=0;
    gl_en_[i]=0;
    max_en_[i]=0;
    max_t_[i]=0;
  }

  // cloneME switch
  cloneME_ = ps.getUntrackedParameter<bool>("cloneME", true);
  
  // verbosity switch
  verbose_ = ps.getUntrackedParameter<bool>("verbose", false);

  // DQM default process name
  process_ = ps.getUntrackedParameter<string>("processName", "HcalMonitor/");
  
  vector<string> subdets = ps.getUntrackedParameter<vector<string> >("subDetsOn");
  for(int i=0; i<4; i++) subDetsOn_[i] = false;
  
  for(unsigned int i=0; i<subdets.size(); i++){
    if(subdets[i]=="HB") subDetsOn_[0] = true;
    else if(subdets[i]=="HE") subDetsOn_[1] = true;
    else if(subdets[i]=="HF") subDetsOn_[2] = true;
    else if(subdets[i]=="HO") subDetsOn_[3] = true;
  }
}

HcalDeadCellClient::HcalDeadCellClient(){
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();

  dbe_ = 0;
  for(int i=0; i<4; i++){
    occ_geo_[i][0]=0;
    occ_en_[i][0]=0;
    occ_geo_[i][1]=0;
    occ_en_[i][1]=0;
    gl_geo_[i]=0;
    gl_en_[i]=0;
    max_en_[i]=0;
    max_t_[i]=0;
  }

  // verbosity switch
  verbose_ = false;
  for(int i=0; i<4; i++) subDetsOn_[i] = false;
}

HcalDeadCellClient::~HcalDeadCellClient(){

  this->cleanup();

}

void HcalDeadCellClient::beginJob(void){
  
  if ( verbose_ ) cout << "HcalDeadCellClient: beginJob" << endl;
  
  ievt_ = 0;
  jevt_ = 0;

  this->setup();
  this->resetAllME();
  return;
}

void HcalDeadCellClient::beginRun(void){

  if ( verbose_ ) cout << "HcalDeadCellClient: beginRun" << endl;

  jevt_ = 0;
  this->setup();
  this->resetAllME();
  return;
}

void HcalDeadCellClient::endJob(void) {

  if ( verbose_ ) cout << "HcalDeadCellClient: endJob, ievt = " << ievt_ << endl;

  this->cleanup(); 
  return;
}

void HcalDeadCellClient::endRun(void) {

  if ( verbose_ ) cout << "HcalDeadCellClient: endRun, jevt = " << jevt_ << endl;

  this->cleanup();  
  return;
}

void HcalDeadCellClient::setup(void) {
  
  return;
}

void HcalDeadCellClient::cleanup(void) {

  if ( cloneME_ ) {
    for(int i=0; i<4; i++){
      if ( occ_geo_[i][0]) delete occ_geo_[i][0];  
      if ( occ_en_[i][0]) delete occ_en_[i][0];  
      if ( occ_geo_[i][1]) delete occ_geo_[i][1];  
      if ( occ_en_[i][1]) delete occ_en_[i][1];  
      if ( gl_geo_[i]) delete gl_geo_[i];  
      if ( gl_en_[i]) delete gl_en_[i];  
      if ( max_en_[i]) delete max_en_[i];  
      if ( max_t_[i]) delete max_t_[i];  

    }    
  }
  for(int i=0; i<4; i++){
    occ_geo_[i][0]=0;
    occ_en_[i][0]=0;
    occ_geo_[i][1]=0;
    occ_en_[i][1]=0;
    gl_geo_[i]=0;
    gl_en_[i]=0;
    max_en_[i]=0;
    max_t_[i]=0;
  }

  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();

  return;
}

void HcalDeadCellClient::errorOutput(){
  if(!dbe_) return;
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  
  for (map<string, string>::iterator testsMap=dqmQtests_.begin(); testsMap!=dqmQtests_.end();testsMap++){
    string testName = testsMap->first;
    string meName = testsMap->second;
    MonitorElement* me = dbe_->get(meName);
    if(me){
      if (me->hasError()){
	vector<QReport*> report =  me->getQErrors();
	dqmReportMapErr_[meName] = report;
      }
      if (me->hasWarning()){
	vector<QReport*> report =  me->getQWarnings();
	dqmReportMapWarn_[meName] = report;
      }
      if(me->hasOtherReport()){
	vector<QReport*> report= me->getQOthers();
	dqmReportMapOther_[meName] = report;
      }
    }
  }
  printf("DeadCell Task: %d errors, %d warnings, %d others\n",dqmReportMapErr_.size(),dqmReportMapWarn_.size(),dqmReportMapOther_.size());

  return;
}

void HcalDeadCellClient::getErrors(map<string, vector<QReport*> > outE, map<string, vector<QReport*> > outW, map<string, vector<QReport*> > outO){

  this->errorOutput();
  outE.clear(); outW.clear(); outO.clear();

  for(map<string, vector<QReport*> >::iterator i=dqmReportMapErr_.begin(); i!=dqmReportMapErr_.end(); i++){
    outE[i->first] = i->second;
  }
  for(map<string, vector<QReport*> >::iterator i=dqmReportMapWarn_.begin(); i!=dqmReportMapWarn_.end(); i++){
    outW[i->first] = i->second;
  }
  for(map<string, vector<QReport*> >::iterator i=dqmReportMapOther_.begin(); i!=dqmReportMapOther_.end(); i++){
    outO[i->first] = i->second;
  }

  return;
}

void HcalDeadCellClient::report(){

  if ( verbose_ ) cout << "HcalDeadCellClient: report" << endl;
  //  this->setup();  
  
  char name[256];
  sprintf(name, "%sHcalMonitor/DeadCellMonitor/DeadCell Task Event Number",process_.c_str());
  MonitorElement* me = 0;
  if(dbe_) me = dbe_->get(name);
  if ( me ) {
    string s = me->valueString();
    ievt_ = -1;
    sscanf((s.substr(2,s.length()-2)).c_str(), "%d", &ievt_);
    if ( verbose_ ) cout << "Found '" << name << "'" << endl;
  }

  getHistograms();

  return;
}

void HcalDeadCellClient::analyze(void){

  jevt_++;
  int updates = 0;
  //  if(dbe_) dbe_->getNumUpdates();
  if ( updates % 10 == 0 ) {
    if ( verbose_ ) cout << "HcalDeadCellClient: " << updates << " updates" << endl;
  }
  
  return;
}

void HcalDeadCellClient::getHistograms(){
  if(!dbe_) return;
  char name[150];    
  
  for(int i=0; i<4; i++){
    sprintf(name,"DeadCellMonitor/DeadCell Depth %d Occupancy Map",i+1);
    gl_geo_[i] = getHisto2(name, process_, dbe_,verbose_,cloneME_);
    
    sprintf(name,"DeadCellMonitor/DeadCell Depth %d Energy Map",i+1);
    gl_en_[i] = getHisto2(name, process_, dbe_,verbose_,cloneME_);    
  }
    
  for(int i=0; i<4; i++){
    if(!subDetsOn_[i]) continue;
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HF"; 
    if(i==3) type = "HO"; 
    sprintf(name,"DeadCellMonitor/%s/%s DeadCell Geo Occupancy Map, Threshold 0",type.c_str(),type.c_str());
    occ_geo_[i][0] = getHisto2(name, process_, dbe_,verbose_,cloneME_);      
    sprintf(name,"DeadCellMonitor/%s/%s DeadCell Geo Energy Map, Threshold 0",type.c_str(),type.c_str());
    occ_en_[i][0] = getHisto2(name, process_, dbe_,verbose_,cloneME_);

    sprintf(name,"DeadCellMonitor/%s/%s DeadCell Geo Occupancy Map, Threshold 1",type.c_str(),type.c_str());
    occ_geo_[i][1] = getHisto2(name, process_, dbe_,verbose_,cloneME_);      
    sprintf(name,"DeadCellMonitor/%s/%s DeadCell Geo Energy Map, Threshold 1",type.c_str(),type.c_str());
    occ_en_[i][1] = getHisto2(name, process_, dbe_,verbose_,cloneME_);

    sprintf(name,"DeadCellMonitor/%s/%s DeadCell Energy",type.c_str(),type.c_str());
    max_en_[i] = getHisto(name, process_, dbe_,verbose_,cloneME_);
    sprintf(name,"DeadCellMonitor/%s/%s DeadCell Time",type.c_str(),type.c_str());
    max_t_[i] = getHisto(name, process_, dbe_,verbose_,cloneME_);    
  }
  return;
}

void HcalDeadCellClient::resetAllME(){
  if(!dbe_) return;

  Char_t name[150];    

  sprintf(name,"%sHcalMonitor/DeadCellMonitor/DeadCell Energy",process_.c_str());
  resetME(name,dbe_);
  sprintf(name,"%sHcalMonitor/DeadCellMonitor/DeadCell Time",process_.c_str());
  resetME(name,dbe_);
  for(int i=1; i<5; i++){
    sprintf(name,"%sHcalMonitor/DeadCellMonitor/DeadCell Depth %d Occupancy Map",process_.c_str(),i);
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DeadCellMonitor/DeadCell Depth %d Energy Map",process_.c_str(),i);
    resetME(name,dbe_);
  }
  sprintf(name,"%sHcalMonitor/DeadCellMonitor/DeadCell Occupancy Map",process_.c_str());
  resetME(name,dbe_);
  sprintf(name,"%sHcalMonitor/DeadCellMonitor/DeadCell Energy Map",process_.c_str());
  resetME(name,dbe_);


  for(int i=0; i<4; i++){
    if(!subDetsOn_[i]) continue;
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HF"; 
    if(i==3) type = "HO"; 
    
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Energy",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Time",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell ID",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Geo Occupancy Map, Threshold 0",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Geo Energy Map, Threshold 0",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Geo Occupancy Map, Threshold 1",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Geo Energy Map, Threshold 1",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Geo Occupancy Map, Max Cell",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
    sprintf(name,"%sHcalMonitor/DigiMonitor/%s/%s DeadCell Geo Energy Map, Max Cell",process_.c_str(),type.c_str(),type.c_str());
    resetME(name,dbe_);
  }

  return;
}

void HcalDeadCellClient::htmlOutput(int runNo, string htmlDir, string htmlName){

  cout << "Preparing HcalDeadCellClient html output ..." << endl;
  string client = "DeadCellMonitor";
  htmlErrors(runNo,htmlDir,client,process_,dbe_,dqmReportMapErr_,dqmReportMapWarn_,dqmReportMapOther_);
  
  ofstream htmlFile;
  htmlFile.open((htmlDir + htmlName).c_str());

  // html page header
  htmlFile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">  " << endl;
  htmlFile << "<html>  " << endl;
  htmlFile << "<head>  " << endl;
  htmlFile << "  <meta content=\"text/html; charset=ISO-8859-1\"  " << endl;
  htmlFile << " http-equiv=\"content-type\">  " << endl;
  htmlFile << "  <title>Monitor: Hcal DeadCell Task output</title> " << endl;
  htmlFile << "</head>  " << endl;
  htmlFile << "<style type=\"text/css\"> td { font-weight: bold } </style>" << endl;
  htmlFile << "<body>  " << endl;
  htmlFile << "<br>  " << endl;
  htmlFile << "<h2>Run:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << runNo << "</span></h2>" << endl;
  htmlFile << "<h2>Monitoring task:&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">Hcal DeadCells</span></h2> " << endl;
  htmlFile << "<h2>Events processed:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;

  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << ievt_ << "</span></h2>" << endl;
  htmlFile << "<hr>" << endl;
  htmlFile << "<table  width=100% border=1><tr>" << endl;
  if(hasErrors())htmlFile << "<td bgcolor=red><a href=\"DeadCellMonitorErrors.html\">Errors in this task</a></td>" << endl;
  else htmlFile << "<td bgcolor=lime>No Errors</td>" << endl;
  if(hasWarnings()) htmlFile << "<td bgcolor=yellow><a href=\"DeadCellMonitorWarnings.html\">Warnings in this task</a></td>" << endl;
  else htmlFile << "<td bgcolor=lime>No Warnings</td>" << endl;
  if(hasOther()) htmlFile << "<td bgcolor=aqua><a href=\"DeadCellMonitorMessages.html\">Messages in this task</a></td>" << endl;
  else htmlFile << "<td bgcolor=lime>No Messages</td>" << endl;
  htmlFile << "</tr></table>" << endl;
  htmlFile << "<hr>" << endl;

  htmlFile << "<h2><strong>Hcal Dead Cell Histograms</strong></h2>" << endl;
  htmlFile << "<h3>" << endl;
  if(subDetsOn_[0]) htmlFile << "<a href=\"#HB_Plots\">HB Plots </a></br>" << endl;  
  if(subDetsOn_[1]) htmlFile << "<a href=\"#HE_Plots\">HE Plots </a></br>" << endl;
  if(subDetsOn_[2]) htmlFile << "<a href=\"#HF_Plots\">HF Plots </a></br>" << endl;
  if(subDetsOn_[3]) htmlFile << "<a href=\"#HO_Plots\">HO Plots </a></br>" << endl;
  htmlFile << "</h3>" << endl;
  htmlFile << "<hr>" << endl;

  htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  htmlFile << "<td>&nbsp;&nbsp;&nbsp;<h3>Global Histograms</h3></td></tr>" << endl;

  htmlFile << "<tr align=\"left\">" << endl;	
  histoHTML2(runNo,gl_geo_[0],"iEta","iPhi", 92, htmlFile,htmlDir);
  histoHTML2(runNo,gl_en_[0],"iEta","iPhi", 100, htmlFile,htmlDir);
  htmlFile << "</tr>" << endl;

  htmlFile << "<tr align=\"left\">" << endl;	
  histoHTML2(runNo,gl_geo_[1],"iEta","iPhi", 92, htmlFile,htmlDir);
  histoHTML2(runNo,gl_en_[1],"iEta","iPhi", 100, htmlFile,htmlDir);
  htmlFile << "</tr>" << endl;

  htmlFile << "<tr align=\"left\">" << endl;	
  histoHTML2(runNo,gl_geo_[2],"iEta","iPhi", 92, htmlFile,htmlDir);
  histoHTML2(runNo,gl_en_[2],"iEta","iPhi", 100, htmlFile,htmlDir);
  htmlFile << "</tr>" << endl;

  htmlFile << "<tr align=\"left\">" << endl;	
  histoHTML2(runNo,gl_geo_[3],"iEta","iPhi", 92, htmlFile,htmlDir);
  histoHTML2(runNo,gl_en_[3],"iEta","iPhi", 100, htmlFile,htmlDir);
  htmlFile << "</tr>" << endl;

  for(int i=0; i<4; i++){
    if(!subDetsOn_[i]) continue;
    
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HF"; 
    if(i==3) type = "HO"; 

    htmlFile << "<tr align=\"left\">" << endl;
    htmlFile << "<td>&nbsp;&nbsp;&nbsp;<a name=\""<<type<<"_Plots\"><h3>" << type << " Histograms</h3></td></tr>" << endl;

    htmlFile << "<tr align=\"left\">" << endl;	
    histoHTML2(runNo,occ_geo_[i][0],"iEta","iPhi", 92, htmlFile,htmlDir);
    histoHTML2(runNo,occ_en_[i][0],"iEta","iPhi", 100, htmlFile,htmlDir);
    htmlFile << "</tr>" << endl;

    htmlFile << "<tr align=\"left\">" << endl;	
    histoHTML2(runNo,occ_geo_[i][1],"iEta","iPhi", 92, htmlFile,htmlDir);
    histoHTML2(runNo,occ_en_[i][1],"iEta","iPhi", 100, htmlFile,htmlDir);
    htmlFile << "</tr>" << endl;

    htmlFile << "<tr align=\"left\">" << endl;	
    histoHTML(runNo,max_en_[i],"GeV","Evts", 92, htmlFile,htmlDir);
    histoHTML(runNo,max_t_[i],"nS","Evts", 100, htmlFile,htmlDir);
    htmlFile << "</tr>" << endl;
  }
  htmlFile << "</table>" << endl;
  htmlFile << "<br>" << endl;

  // html page footer
  htmlFile << "</body> " << endl;
  htmlFile << "</html> " << endl;

  htmlFile.close();

  return;
}

void HcalDeadCellClient::createTests(){
  //  char meTitle[250], name[250];    
  //  vector<string> params;
  
  if(verbose_) printf("There are NO dead cell client tests....\n");
   
  return;
}

void HcalDeadCellClient::loadHistograms(TFile* infile){

  TNamed* tnd = (TNamed*)infile->Get("DQMData/HcalMonitor/DeadCellMonitor/DeadCell Task Event Number");
  if(tnd){
    string s =tnd->GetTitle();
    ievt_ = -1;
    sscanf((s.substr(2,s.length()-2)).c_str(), "%d", &ievt_);
  }

  char name[150];    
  for(int i=0; i<4; i++){
    if(!subDetsOn_[i]) continue;
    string type = "HB";
    if(i==1) type = "HE"; 
    if(i==2) type = "HF"; 
    if(i==3) type = "HO"; 

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/DeadCell Depth %d Occupancy Map",i+1);
    gl_geo_[i] = (TH2F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/DeadCell Depth %d Energy Map",i+1);
    gl_en_[i] = (TH2F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/%s/%s DeadCell Geo Occupancy Map, Threshold 0",type.c_str(),type.c_str());
    occ_geo_[i][0] = (TH2F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/%s/%s DeadCell Geo Energy Map, Threshold 0",type.c_str(),type.c_str());
    occ_en_[i][0] = (TH2F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/%s/%s DeadCell Geo Occupancy Map, Threshold 1",type.c_str(),type.c_str());
    occ_geo_[i][1] = (TH2F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/%s/%s DeadCell Geo Energy Map, Threshold 1",type.c_str(),type.c_str());
    occ_en_[i][1] = (TH2F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/%s/%s DeadCell Energy",type.c_str(),type.c_str());
    max_en_[i] = (TH1F*)infile->Get(name);

    sprintf(name,"DQMData/HcalMonitor/DeadCellMonitor/%s/%s DeadCell Time",type.c_str(),type.c_str());
    max_t_[i] = (TH1F*)infile->Get(name);

  }
  return;
}