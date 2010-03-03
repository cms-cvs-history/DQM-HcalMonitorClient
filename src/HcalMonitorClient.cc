/*
 * \file HcalMonitorClient.cc
 * 
 * $Date: 2010/03/03 18:07:08 $
 * $Revision: 1.92.2.3 $
 * \author J. Temple
 * 
 */

#include "DQM/HcalMonitorClient/interface/HcalMonitorClient.h"
#include "DQM/HcalMonitorClient/interface/HcalDeadCellClient.h"
#include "DQM/HcalMonitorClient/interface/HcalHotCellClient.h"
#include "DQM/HcalMonitorClient/interface/HcalRecHitClient.h"
#include "DQM/HcalMonitorClient/interface/HcalDigiClient.h"
#include "DQM/HcalMonitorClient/interface/HcalTrigPrimClient.h"

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "CondFormats/HcalObjects/interface/HcalChannelStatus.h"
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/HcalObjects/interface/HcalCondObjectContainer.h"


#include <iostream>
#include <iomanip>
#include <fstream>

using namespace cms;
using namespace edm;
using namespace std;

HcalMonitorClient::HcalMonitorClient(const ParameterSet& ps)
{
  debug_ = ps.getUntrackedParameter<int>("debug",0);
  inputFile_ = ps.getUntrackedParameter<string>("inputFile","");
  mergeRuns_ = ps.getUntrackedParameter<bool>("mergeRuns", false);
  cloneME_ = ps.getUntrackedParameter<bool>("cloneME", true);
  prescaleFactor_ = ps.getUntrackedParameter<int>("prescaleFactor", -1);
  prefixME_ = ps.getUntrackedParameter<string>("prefixME", "Hcal/");
  enableCleanup_ = ps.getUntrackedParameter<bool>("enableCleanup", false);
  enabledClients_ = ps.getUntrackedParameter<vector<string> >("enabledClients", enabledClients_);

  baseHtmlDir_ = ps.getUntrackedParameter<string>("baseHtmlDir", "");
  htmlUpdateTime_ = ps.getUntrackedParameter<int>("htmlUpdateTime", 0);
  databasedir_   = ps.getUntrackedParameter<std::string>("databasedir","");
  databaseUpdateTime_ = ps.getUntrackedParameter<int>("databaseUpdateTime",0);

  if (debug_>0)
    {
      std::cout <<"HcalMonitorClient:: The following clients are enabled:"<<std::endl;
      for (unsigned int i=0;i<enabledClients_.size();++i)
	  std::cout <<enabledClients_[i]<<std::endl;
    } // if (debug_>0)
  
  // Add all relevant clients
  clients_.reserve(12); // any reason to reserve ahead of time?

  clients_.push_back(new HcalBaseDQClient((string)"HcalMonitorModule",ps));
  if (find(enabledClients_.begin(), enabledClients_.end(),"DeadCellMonitor")!=enabledClients_.end())
    clients_.push_back(new HcalDeadCellClient((string)"DeadCellMonitor",ps));
  if (find(enabledClients_.begin(), enabledClients_.end(),"HotCellMonitor")!=enabledClients_.end())
   clients_.push_back(new HcalHotCellClient((string)"HotCellMonitor",ps));
  if (find(enabledClients_.begin(), enabledClients_.end(),"RecHitMonitor")!=enabledClients_.end())
    clients_.push_back(new HcalRecHitClient((string)"RecHitMonitor",ps));
  if (find(enabledClients_.begin(), enabledClients_.end(),"DigiMonitor")!=enabledClients_.end())
    clients_.push_back(new HcalDigiClient((string)"DigiMonitor",ps));
  if (find(enabledClients_.begin(), enabledClients_.end(),"TrigPrimMonitor")!=enabledClients_.end())
    clients_.push_back(new HcalTrigPrimClient((string)"TrigPrimMonitor",ps));

} // HcalMonitorClient constructor


HcalMonitorClient::~HcalMonitorClient()
{
  if (debug_>0) std::cout <<"<HcalMonitorClient>  Exiting..."<<std::endl;
  for (unsigned int i=0;i<clients_.size();++i)
    delete clients_[i];
  if (summaryClient_) delete summaryClient_;

}

void HcalMonitorClient::beginJob(void)
{

  begin_run_ = false;
  end_run_   = false;

  run_=-1;
  evt_=-1;
  ievt_=0;
  jevt_=0;

  current_time_ = time(NULL);
  last_time_update_ = current_time_;
  last_time_db_ = current_time_;

  // get hold of back-end interface

  dqmStore_ = Service<DQMStore>().operator->();

  if ( inputFile_.size() != 0 ) 
    {
      if ( dqmStore_ )    dqmStore_->open(inputFile_);
    }

  for ( unsigned int i=0; i<clients_.size();++i ) 
    clients_[i]->beginJob();

  //if ( summaryClient_ ) summaryClient_->beginJob();

} // void HcalMonitorClient::beginJob(void)


void HcalMonitorClient::beginRun(const Run& r, const EventSetup& c) 
{
  if (debug_>0) std::cout <<"<HcalMonitorClient::beginRun(r,c)>"<<std::endl;
  begin_run_ = true;
  end_run_   = false;

  run_=r.id().run();
  evt_=0;
  jevt_=0;

  // Store list of bad channels and their values
  std::map <HcalDetId, unsigned int> badchannelmap; 
  badchannelmap.clear();

  // Let's get the channel status quality
  edm::ESHandle<HcalChannelQuality> p;
  c.get<HcalChannelQualityRcd>().get(p);
  chanquality_= new HcalChannelQuality(*p.product());
 
  // Find only channels with non-zero quality, and add them to badchannelmap
  std::vector<DetId> mydetids = chanquality_->getAllChannels();
  for (std::vector<DetId>::const_iterator i = mydetids.begin();i!=mydetids.end();++i)
    {
      if (i->det()!=DetId::Hcal) continue; // not an hcal cell
      HcalDetId id=HcalDetId(*i);
      int status=(chanquality_->getValues(id))->getValue();
      if (status==0) continue;
      badchannelmap[id]=status;
    }
    
  for (unsigned int i=0;i<clients_.size();++i)
    {
      clients_[i]->beginRun();
      clients_[i]->setStatusMap(badchannelmap);
    }
} // void HcalMonitorClient::beginRun(const Run& r, const EventSetup& c)

void HcalMonitorClient::beginRun()
{
  // What is the difference between this and beginRun above?
  // When would this be called?
  begin_run_ = true;
  end_run_   = false;
  jevt_ = 0;
} // void HcalMonitorClient::beginRun()

void HcalMonitorClient::setup(void)
{
  // no setup required
}

void HcalMonitorClient::beginLuminosityBlock(const LuminosityBlock &l, const EventSetup &c) 
{
  if (debug_>0) std::cout <<"<HcalMonitorClient::beginLuminosityBlock>"<<std::endl;
} // void HcalMonitorClient::beginLuminosityBlock

void HcalMonitorClient::analyze(const edm::Event & e, const edm::EventSetup & c)
{
  ievt_++;
  jevt_++;
  if (debug_>4) std::cout <<"HcalMonitorClient::analyze(const edm::Event&, const edm::EventSetup&) ievt_ = "<<ievt_<<std::endl;

  run_=e.id().run();
  evt_=e.id().event();
  if (prescaleFactor_>0 && jevt_%prescaleFactor_==0) this->analyze();

} // void HcalMonitorClient::analyze(const edm::Event & e, const edm::EventSetup & c)

void HcalMonitorClient::analyze()
{
  if (debug_>0) std::cout <<"HcalMonitorClient::analyze() "<<std::endl;
  current_time_ = time(NULL);
  ievt_++;
  jevt_++;
  for (unsigned int i=0;i<clients_.size();++i)
    clients_[i]->analyze();
} // void HcalMonitorClient::analyze()


void HcalMonitorClient::endLuminosityBlock(const LuminosityBlock &l, const EventSetup &c) 
{
  if (debug_>0) std::cout <<"<HcalMonitorClient::endLuminosityBlock>"<<std::endl;
  
  if (updateTime_>0)
    {
      if ((current_time_-last_time_update_)<60*updateTime_)
	return;
      last_time_update_ = current_time_;
    }
  this->analyze();
} // void HcalMonitorClient::endLuminosityBlock

void HcalMonitorClient::endRun(void)
{
  begin_run_ = false;
  end_run_   = true;

  if (baseHtmlDir_.size()>0)
    this->writeHtml();
  if (databasedir_.size()>0)
    this->writeChannelStatus();
}

void HcalMonitorClient::endRun(const Run& r, const EventSetup& c) 
{
  this->analyze();
  this->endRun();
}

void HcalMonitorClient::endJob(void)
{
  if (! end_run_)
    {
      this->analyze();
      this->endRun();
    }
  this->cleanup();

  for ( unsigned int i=0; i<clients_.size(); i++ ) 
    clients_[i]->endJob();
  if ( summaryClient_ ) summaryClient_->endJob();

} // void HcalMonitorClient::endJob(void)

void HcalMonitorClient::cleanup(void)
{
  if (!enableCleanup_) return;
  // other cleanup?
} // void HcalMonitorClient::cleanup(void)


void HcalMonitorClient::writeHtml()
{
  if (debug_>0) std::cout << "Preparing HcalMonitorClient html output ..." << std::endl;
  
  // global ROOT style
  gStyle->Reset("Default");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetFillColor(0);
  gStyle->SetTitleFillColor(10);
  //  gStyle->SetOptStat(0);
  gStyle->SetOptStat("ouemr");
  gStyle->SetPalette(1);

  char tmp[20];
  if(run_!=-1) sprintf(tmp, "DQM_Hcal_R%09d", run_);
  else sprintf(tmp, "DQM_Hcal_R%09d", 0);
  string htmlDir = baseHtmlDir_ + "/" + tmp + "/";
  system(("/bin/mkdir -p " + htmlDir).c_str());

  ofstream htmlFile;
  htmlFile.open((htmlDir + "index.html").c_str());

  // html page header
  htmlFile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">  " << std::endl;
  htmlFile << "<html>  " << std::endl;
  htmlFile << "<head>  " << std::endl;
  htmlFile << "  <meta content=\"text/html; charset=ISO-8859-1\"  " << std::endl;
  htmlFile << " http-equiv=\"content-type\">  " << std::endl;
  htmlFile << "  <title>Hcal Data Quality Monitor</title> " << std::endl;
  htmlFile << "</head>  " << std::endl;
  htmlFile << "<body>  " << std::endl;
  htmlFile << "<br>  " << std::endl;
 htmlFile << "<center><h1>Hcal Data Quality Monitor</h1></center>" << std::endl;
  htmlFile << "<h2>Run Number:&nbsp;&nbsp;&nbsp;" << std::endl;
  htmlFile << "<span style=\"color: rgb(0, 0, 153);\">" << run_ <<"</span></h2> " << std::endl;
  htmlFile << "<h2>Events processed:&nbsp;&nbsp;&nbsp;" << std::endl;
  htmlFile << "<span style=\"color: rgb(0, 0, 153);\">" << ievt_ <<"</span></h2> " << std::endl;
  htmlFile << "<hr>" << std::endl;
  htmlFile << "<ul>" << std::endl;

  for (unsigned int i=0;i<clients_.size();++i)
    {
      clients_[i]->htmlOutput(htmlDir);
      htmlFile << "<table border=0 WIDTH=\"50%\"><tr>" << std::endl;
      htmlFile << "<td WIDTH=\"35%\"><a href=\"" << clients_[i]->name_ << ".html"<<"\">"<<clients_[i]->name_<<"</a></td>" << std::endl;
      if(clients_[i]->hasErrors_Temp()) htmlFile << "<td bgcolor=red align=center>This monitor task has errors.</td>" << std::endl;
      else if(clients_[i]->hasWarnings_Temp()) htmlFile << "<td bgcolor=yellow align=center>This monitor task has warnings.</td>" << std::endl;
      else if(clients_[i]->hasOther_Temp()) htmlFile << "<td bgcolor=aqua align=center>This monitor task has messages.</td>" << std::endl;
      else htmlFile << "<td bgcolor=lime align=center>This monitor task has no problems</td>" << std::endl;
      htmlFile << "</tr></table>" << std::endl;
    }

  // Add call to reportSummary html output

  htmlFile << "</ul>" << std::endl;

  // html page footer
  htmlFile << "</body> " << std::endl;
  htmlFile << "</html> " << std::endl;

  htmlFile.close();
  if (debug_>0) std::cout << "HcalMonitorClient html output done..." << std::endl;
  
} // void HcalMonitorClient::writeHtml()

void HcalMonitorClient::writeChannelStatus()
{

} // void HcalMonitorClient::writeChannelStatus()

DEFINE_FWK_MODULE(HcalMonitorClient);
