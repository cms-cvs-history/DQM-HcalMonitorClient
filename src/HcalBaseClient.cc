#include <DQM/HcalMonitorClient/interface/HcalBaseClient.h>
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

HcalBaseClient::HcalBaseClient(){
  dbe_ =NULL;
  clientName_ = "GenericHcalClient";
}

HcalBaseClient::~HcalBaseClient(){}

void HcalBaseClient::init(const ParameterSet& ps, DQMStore* dbe, string clientName)
{
  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  dqmQtests_.clear();
  
  dbe_ = dbe;
  ievt_=0; jevt_=0;
  clientName_ = clientName;
  
  // cloneME switch
  cloneME_ = ps.getUntrackedParameter<bool>("cloneME", true);
  
  // verbosity switch
  debug_ = ps.getUntrackedParameter<int>("debug", 0);
  if(debug_>0) cout << clientName_ <<" debugging switch is on"<<endl;
  
  // timing switch
  showTiming_ = ps.getUntrackedParameter<bool>("showTiming",false); 

  // DQM default process name
  process_ = ps.getUntrackedParameter<string>("processName", "Hcal/");
  
  vector<string> subdets = ps.getUntrackedParameter<vector<string> >("subDetsOn");
  for(int i=0; i<4; i++)
    {
      subDetsOn_[i] = false;
    }

  for(unsigned int i=0; i<subdets.size(); i++)
    {
      if(subdets[i]=="HB") subDetsOn_[0] = true;
      else if(subdets[i]=="HE") subDetsOn_[1] = true;
      else if(subdets[i]=="HF") subDetsOn_[2] = true;
      else if(subdets[i]=="HO") subDetsOn_[3] = true;
    }
  
  // Define error palette, ranging from yellow for low to red for high. 
  for( int i=0; i<20; ++i )
    {
      //pcol_error_[19-i] = 901+i;
      TColor* color = gROOT->GetColor( 901+i );
      if( ! color ) color = new TColor( 901+i, 0, 0, 0, "" );
      color->SetRGB( 1.,
		     1.-.05*i,
		     0);
      pcol_error_[i]=901+i;
    } // for (int i=0;i<20;++i)

  return; 
} // void HcalBaseClient::init(const ParameterSet& ps, DQMStore* dbe, string clientName)


void HcalBaseClient::errorOutput(){
  
  if(!dbe_) return;

  dqmReportMapErr_.clear(); dqmReportMapWarn_.clear(); dqmReportMapOther_.clear();
  
  for (map<string, string>::iterator testsMap=dqmQtests_.begin(); 
       testsMap!=dqmQtests_.end();testsMap++){
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

  if (debug_>0) cout << clientName_ << " Error Report: "<< dqmQtests_.size() << " tests, "<<dqmReportMapErr_.size() << " errors, " <<dqmReportMapWarn_.size() << " warnings, "<< dqmReportMapOther_.size() << " others" << endl;

  return;
}

void HcalBaseClient::getTestResults(int& totalTests, 
				    map<string, vector<QReport*> >& outE, 
				    map<string, vector<QReport*> >& outW, 
				    map<string, vector<QReport*> >& outO){
  this->errorOutput();
  //  outE.clear(); outW.clear(); outO.clear();

  for(map<string, vector<QReport*> >::iterator i=dqmReportMapErr_.begin(); i!=dqmReportMapErr_.end(); i++){
    outE[i->first] = i->second;
  }
  for(map<string, vector<QReport*> >::iterator i=dqmReportMapWarn_.begin(); i!=dqmReportMapWarn_.end(); i++){
    outW[i->first] = i->second;
  }
  for(map<string, vector<QReport*> >::iterator i=dqmReportMapOther_.begin(); i!=dqmReportMapOther_.end(); i++){
    outO[i->first] = i->second;
  }

  totalTests += dqmQtests_.size();

  return;
}
/*
void HcalBaseClient::getSJ6histos(char* dir, char* name, TH2F* &h)
{
  TH2F* dummy;
  ostringstream hname;
  hname <<process_.c_str()<<dir<<"HB HF Depth 1 "<<name;
  h[0]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HB HF Depth 2 "<<name;
  h[1]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HE Depth 3 "<<name;
  h[2]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HO ZDC "<<name;
  h[3]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HE Depth 1 "<<name;
  h[4]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HE Depth 2 "<<name;
  h[5]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  return;
} // void HcalBaseClient::getSJ6histos(2D)

void HcalBaseClient::getSJ6histos(char* dir, char* name, TH1F* &h)
{
  TH1F* dummy;
  ostringstream hname;
  hname <<process_.c_str()<<dir<<"HB HF Depth 1 "<<name;
  h[0]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HB HF Depth 2 "<<name;
  h[1]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HE Depth 3 "<<name;
  h[2]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HO ZDC "<<name;
  h[3]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HE Depth 1 "<<name;
  h[4]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  hname <<process_.c_str()<<dir<<"HE Depth 2 "<<name;
  h[5]=getAnyHisto(dummy, hname.str(),process_,dbe_,debug_,cloneME_);
  hname.str("");
  return;
} // void HcalBaseClient::getSJ6histos(1D)
*/
