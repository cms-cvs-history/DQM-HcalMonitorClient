#include <DQM/HcalMonitorClient/interface/HcalRecHitClient.h>
#include <DQM/HcalMonitorClient/interface/HcalClientUtils.h>
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include <math.h>
#include <iostream>

HcalRecHitClient::HcalRecHitClient(){} // constructor 

void HcalRecHitClient::init(const ParameterSet& ps, DQMStore* dbe,string clientName){
  //Call the base class first
  HcalBaseClient::init(ps,dbe,clientName);

  // Get variable values from cfg file

  rechitclient_checkNevents_ = ps.getUntrackedParameter<int>("RecHitClient_checkNevents",100);

  minErrorFlag_ = ps.getUntrackedParameter<double>("RecHitClient_minErrorFlag",0.0);

  rechitclient_makeDiagnostics_ = ps.getUntrackedParameter<bool>("RecHitClient_makeDiagnosticPlots",false);

  // Set histograms to NULL
  ProblemRecHits=0;
  for (int i=0;i<6;++i)
    {
      // Set each array's pointers to NULL
      ProblemRecHitsByDepth[i]    =0;
      OccupancyByDepth[i]         =0;
      OccupancyThreshByDepth[i]   =0;
      EnergyByDepth[i]            =0;
      EnergyThreshByDepth[i]      =0;
      TimeByDepth[i]              =0;
      TimeThreshByDepth[i]        =0;
    }  

  if (rechitclient_makeDiagnostics_)
    {
      d_HBEnergy                  =0;
      d_HBTotalEnergy             =0;
      d_HBTime                    =0;
      d_HBOccupancy               =0;
      d_HBThreshEnergy            =0;
      d_HBThreshTotalEnergy       =0;
      d_HBThreshTime              =0;
      d_HBThreshOccupancy         =0;
      
      d_HEEnergy                  =0;
      d_HETotalEnergy             =0;
      d_HETime                    =0;
      d_HEOccupancy               =0;
      d_HEThreshEnergy            =0;
      d_HEThreshTotalEnergy       =0;
      d_HEThreshTime              =0;
      d_HEThreshOccupancy         =0;
      
      d_HOEnergy                  =0;
      d_HOTotalEnergy             =0;
      d_HOTime                    =0;
      d_HOOccupancy               =0;
      d_HOThreshEnergy            =0;
      d_HOThreshTotalEnergy       =0;
      d_HOThreshTime              =0;
      d_HOThreshOccupancy         =0;
      
      d_HFEnergy                  =0;
      d_HFTotalEnergy             =0;
      d_HFTime                    =0;
      d_HFOccupancy               =0;
      d_HFThreshEnergy            =0;
      d_HFThreshTotalEnergy       =0;
      d_HFThreshTime              =0;
      d_HFThreshOccupancy         =0;
    } // if (rechitclient_makeDiagnostics_)

  subdets_.push_back("HB HF Depth 1 ");
  subdets_.push_back("HB HF Depth 2 ");
  subdets_.push_back("HE Depth 3 ");
  subdets_.push_back("HO ZDC ");
  subdets_.push_back("HE Depth 1 ");
  subdets_.push_back("HE Depth 2 ");

  return;
} // void HcalRecHitClient::init(...)


HcalRecHitClient::~HcalRecHitClient()
{
  this->cleanup();
} // destructor


void HcalRecHitClient::beginJob(){

  if ( debug_>1 ) cout << "HcalRecHitClient: beginJob" << endl;

  ievt_ = 0;
  jevt_ = 0;
  this->setup();
  return;
} // void HcalRecHitClient::beginJob(const EventSetup& eventSetup);


void HcalRecHitClient::beginRun(void)
{
  if ( debug_>1 ) cout << "HcalRecHitClient: beginRun" << endl;

  jevt_ = 0;
  this->setup();
  this->resetAllME();
  return;
} // void HcalRecHitClient::beginRun(void)


void HcalRecHitClient::endJob(void) 
{
  if ( debug_>1 ) cout << "HcalRecHitClient: endJob, ievt = " << ievt_ << endl;

  this->cleanup();
  return;
} // void HcalRecHitClient::endJob(void)


void HcalRecHitClient::endRun(void) 
{
  if ( debug_>1 ) cout << "HcalRecHitClient: endRun, jevt = " << jevt_ << endl;

  this->cleanup();
  return;
} // void HcalRecHitClient::endRun(void)


void HcalRecHitClient::setup(void) 
{
  return;
} // void HcalRecHitClient::setup(void)


void HcalRecHitClient::cleanup(void) 
{
  if(cloneME_)
    {
      // delete individual histogram pointers
      if (ProblemRecHits) delete ProblemRecHits;
      
      for (int i=0;i<6;++i)
	{
	  // delete pointers within arrays of histograms
	  if (ProblemRecHitsByDepth[i])           delete ProblemRecHitsByDepth[i];
	  if (OccupancyByDepth[i])                delete OccupancyByDepth[i];
	  if (OccupancyThreshByDepth[i])          delete OccupancyThreshByDepth[i];
	  if (EnergyByDepth[i])                   delete EnergyByDepth[i];
	  if (EnergyThreshByDepth[i])             delete EnergyThreshByDepth[i];
	  if (TimeByDepth[i])                     delete TimeByDepth[i];
	  if (TimeThreshByDepth[i])               delete TimeThreshByDepth[i];
	}
      
      if (rechitclient_makeDiagnostics_)
	{
	  if (d_HBEnergy)                        delete d_HBEnergy;
	  if (d_HBTotalEnergy)                   delete d_HBTotalEnergy;
	  if (d_HBTime)                          delete d_HBTime;
	  if (d_HBOccupancy)                     delete d_HBOccupancy;
	  if (d_HBThreshEnergy)                  delete d_HBThreshEnergy;
	  if (d_HBThreshTotalEnergy)             delete d_HBThreshTotalEnergy;
	  if (d_HBThreshTime)                    delete d_HBThreshTime;
	  if (d_HBThreshOccupancy)               delete d_HBThreshOccupancy;

	  if (d_HEEnergy)                        delete d_HEEnergy;
	  if (d_HETotalEnergy)                   delete d_HETotalEnergy;
	  if (d_HETime)                          delete d_HETime;
	  if (d_HEOccupancy)                     delete d_HEOccupancy;
	  if (d_HEThreshEnergy)                  delete d_HEThreshEnergy;
	  if (d_HEThreshTotalEnergy)             delete d_HEThreshTotalEnergy;
	  if (d_HEThreshTime)                    delete d_HEThreshTime;
	  if (d_HEThreshOccupancy)               delete d_HEThreshOccupancy;

	  if (d_HOEnergy)                        delete d_HOEnergy;
	  if (d_HOTotalEnergy)                   delete d_HOTotalEnergy;
	  if (d_HOTime)                          delete d_HOTime;
	  if (d_HOOccupancy)                     delete d_HOOccupancy;
	  if (d_HOThreshEnergy)                  delete d_HOThreshEnergy;
	  if (d_HOThreshTotalEnergy)             delete d_HOThreshTotalEnergy;
	  if (d_HOThreshTime)                    delete d_HOThreshTime;
	  if (d_HOThreshOccupancy)               delete d_HOThreshOccupancy;

	  if (d_HFEnergy)                        delete d_HFEnergy;
	  if (d_HFTotalEnergy)                   delete d_HFTotalEnergy;
	  if (d_HFTime)                          delete d_HFTime;
	  if (d_HFOccupancy)                     delete d_HFOccupancy;
	  if (d_HFThreshEnergy)                  delete d_HFThreshEnergy;
	  if (d_HFThreshTotalEnergy)             delete d_HFThreshTotalEnergy;
	  if (d_HFThreshTime)                    delete d_HFThreshTime;
	  if (d_HFThreshOccupancy)               delete d_HFThreshOccupancy;

	} // if (rechitclient_makeDiagnostics_)
      
    } // if (cloneME_)

  // Set individual pointers to NULL
  ProblemRecHits = 0;

  for (int i=0;i<6;++i)
    {
      ProblemRecHitsByDepth[i]    =0;
      OccupancyByDepth[i]          =0;
      OccupancyThreshByDepth[i]   =0;
      EnergyByDepth[i]            =0;
      EnergyThreshByDepth[i]      =0;
      TimeByDepth[i]              =0;
      TimeThreshByDepth[i]        =0;
    }
  
  if (rechitclient_makeDiagnostics_)
    {
      d_HBEnergy                  =0;
      d_HBTotalEnergy             =0;
      d_HBTime                    =0;
      d_HBOccupancy               =0;
      d_HBThreshEnergy            =0;
      d_HBThreshTotalEnergy       =0;
      d_HBThreshTime              =0;
      d_HBThreshOccupancy         =0;
      
      d_HEEnergy                  =0;
      d_HETotalEnergy             =0;
      d_HETime                    =0;
      d_HEOccupancy               =0;
      d_HEThreshEnergy            =0;
      d_HEThreshTotalEnergy       =0;
      d_HEThreshTime              =0;
      d_HEThreshOccupancy         =0;
      
      d_HOEnergy                  =0;
      d_HOTotalEnergy             =0;
      d_HOTime                    =0;
      d_HOOccupancy               =0;
      d_HOThreshEnergy            =0;
      d_HOThreshTotalEnergy       =0;
      d_HOThreshTime              =0;
      d_HOThreshOccupancy         =0;
      
      d_HFEnergy                  =0;
      d_HFTotalEnergy             =0;
      d_HFTime                    =0;
      d_HFOccupancy               =0;
      d_HFThreshEnergy            =0;
      d_HFThreshTotalEnergy       =0;
      d_HFThreshTime              =0;
      d_HFThreshOccupancy         =0;
    } // if (rechitclient_makeDiagnostics_)

  dqmReportMapErr_.clear(); 
  dqmReportMapWarn_.clear(); 
  dqmReportMapOther_.clear();
  dqmQtests_.clear();
  
  return;
} // void HcalRecHitClient::cleanup(void)


void HcalRecHitClient::report()
{
  if(!dbe_) return;
  if ( debug_>1 ) cout << "HcalRecHitClient: report" << endl;
  this->setup();

  ostringstream name;
  name<<process_.c_str()<<"Hcal/RecHitMonitor_Hcal/RecHit Event Number";
  MonitorElement* me = dbe_->get(name.str().c_str());
  if ( me ) 
    {
      string s = me->valueString();
      ievt_ = -1;
      sscanf((s.substr(2,s.length()-2)).c_str(), "%d", &ievt_);
      if ( debug_>1 ) cout << "Found '" << name.str().c_str() << "'" << endl;
    }
  getHistograms();

  return;
} // HcalRecHitClient::report()


void HcalRecHitClient::getHistograms()
{
  if(!dbe_) return;

  ostringstream name;
  // dummy histograms
  TH2F* dummy2D = new TH2F();
  TH1F* dummy1D = new TH1F();

  // Grab individual histograms
  name<<process_.c_str()<<"RecHitMonitor_Hcal/ ProblemRecHits";
  ProblemRecHits = getAnyHisto(dummy2D, name.str(), process_, dbe_, debug_, cloneME_);
  name.str("");

  getSJ6histos("RecHitMonitor_Hcal/problem_rechits/", " Problem RecHit Rate", ProblemRecHitsByDepth);
  getSJ6histos("RecHitMonitor_Hcal/rechit_occupancy/","Rec Hit Average Occupancy", OccupancyByDepth);
  getSJ6histos("RecHitMonitor_Hcal/rechit_occupancy/","Rec Hit Average Occupancy Above Threshold", OccupancyThreshByDepth);
  getSJ6histos("RecHitMonitor_Hcal/rechit_energy/","Rec Hit Average Energy", EnergyByDepth);
  getSJ6histos("RecHitMonitor_Hcal/rechit_energy/","Rec Hit Average Energy Above Threshold", EnergyThreshByDepth);
  getSJ6histos("RecHitMonitor_Hcal/rechit_time/","Rec Hit Average Time", TimeByDepth);
  getSJ6histos("RecHitMonitor_Hcal/rechit_time/","Rec Hit Average Time Above Threshold", TimeThreshByDepth);

  if (rechitclient_makeDiagnostics_)
    {
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_energy";
      d_HBEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_total_energy";
      d_HBTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_time";
      d_HBTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_occupancy";
      d_HBOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_energy_thresh";
      d_HBThreshEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_total_energy_thresh";
      d_HBThreshTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_time_thresh";
      d_HBThreshTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_occupancy_thresh";
      d_HBThreshOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_energy";
      d_HEEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_total_energy";
      d_HETotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_time";
      d_HETime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_occupancy";
      d_HEOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_energy_thresh";
      d_HEThreshEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_total_energy_thresh";
      d_HEThreshTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_time_thresh";
      d_HEThreshTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_occupancy_thresh";
      d_HEThreshOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_energy";
      d_HOEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_total_energy";
      d_HOTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_time";
      d_HOTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_occupancy";
      d_HOOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_energy_thresh";
      d_HOThreshEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_total_energy_thresh";
      d_HOThreshTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_time_thresh";
      d_HOThreshTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_occupancy_thresh";
      d_HOThreshOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_energy";
      d_HFEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_total_energy";
      d_HFTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_time";
      d_HFTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_occupancy";
      d_HFOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_energy_thresh";
      d_HFThreshEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_total_energy_thresh";
      d_HFThreshTotalEnergy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_time_thresh";
      d_HFThreshTime=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_occupancy_thresh";
      d_HFThreshOccupancy=getAnyHisto(dummy1D, name.str(),process_,dbe_,debug_,cloneME_);
      name.str("");
    } // if (rechitclient_makeDiagnostics_)


  // Force min/max on problemcells
  for (int i=0;i<6;++i)
    {
      if (ProblemRecHitsByDepth[i])
	{
	  ProblemRecHitsByDepth[i]->SetMaximum(1);
	  ProblemRecHitsByDepth[i]->SetMinimum(0);
	}
      name.str("");

    } // for (int i=0;i<6;++i)

  return;
} //void HcalRecHitClient::getHistograms()


void HcalRecHitClient::analyze(void)
{
  jevt_++;
  if ( jevt_ % 10 == 0 ) 
    {
      if ( debug_>1 ) cout << "<HcalRecHitClient::analyze>  Running analyze "<<endl;
    }
  getHistograms();
  return;
} // void HcalRecHitClient::analyze(void)


void HcalRecHitClient::createTests()
{
  // Removed a bunch of code that was in older versions of HcalRecHitClient
  // tests should now be handled from outside
  if(!dbe_) return;
  return;
} // void HcalRecHitClient::createTests()


void HcalRecHitClient::resetAllME()
{
  if(!dbe_) return;
  
  ostringstream name;

  // Reset individual histograms
  name<<process_.c_str()<<"RecHitMonitor_Hcal/ ProblemRecHits";
  resetME(name.str().c_str(),dbe_);
  name.str("");

  for (int i=0;i<6;++i)
    {
      // Reset arrays of histograms
      name<<process_.c_str()<<"RecHitMonitor_Hcal/problem_rechits/"<<subdets_[i]<<" Problem RecHit Rate";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_occupancy/"<<subdets_[i]<<"Rec Hit Average Occupancy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_occupancy/"<<subdets_[i]<<"Rec Hit Average Occupancy Above Threshold";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_energy/"<<subdets_[i]<<"Rec Hit Average Energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_energy/"<<subdets_[i]<<"Rec Hit Average Energy Above Threshold";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_time/"<<subdets_[i]<<"Rec Hit Average Time";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_time/"<<subdets_[i]<<"Rec Hit Average Time Above Threshold";
      resetME(name.str().c_str(),dbe_);
      name.str("");
    } // for (int i=0;i<6;++i)

 if (rechitclient_makeDiagnostics_)
    {
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_total_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_time";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_occupancy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_total_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_time_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_occupancy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_total_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_time";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_occupancy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_total_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_time_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_occupancy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_total_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_time";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_occupancy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_total_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_time_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_occupancy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_total_energy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_time";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_occupancy";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_total_energy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_time_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_occupancy_thresh";
      resetME(name.str().c_str(),dbe_);
      name.str("");
    } // if (rechitclient_makeDiagnostics_)

 return;
} // void HcalRecHitClient::resetAllME()


void HcalRecHitClient::htmlOutput(int runNo, string htmlDir, string htmlName)
{
  if (showTiming_)
    {
      cpu_timer.reset(); cpu_timer.start();
    }

  if (debug_>1) cout << "Preparing HcalRecHitClient html output ..." << endl;

  string client = "RecHitMonitor";

  ofstream htmlFile;
  htmlFile.open((htmlDir + htmlName).c_str());

  // html page header
  htmlFile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">  " << endl;
  htmlFile << "<html>  " << endl;
  htmlFile << "<head>  " << endl;
  htmlFile << "  <meta content=\"text/html; charset=ISO-8859-1\"  " << endl;
  htmlFile << " http-equiv=\"content-type\">  " << endl;
  htmlFile << "  <title>Monitor: Hcal Rec Hit Task output</title> " << endl;
  htmlFile << "</head>  " << endl;
  htmlFile << "<style type=\"text/css\"> td { font-weight: bold } </style>" << endl;
  htmlFile << "<body>  " << endl;
  htmlFile << "<br>  " << endl;
  htmlFile << "<h2>Run:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << runNo << "</span></h2>" << endl;
  htmlFile << "<h2>Monitoring task:&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">Hcal Rec Hits</span></h2> " << endl;

  htmlFile << "<h2>Events processed:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << ievt_ << "</span></h2>" << endl;
  htmlFile << "<hr>" << endl;

  htmlFile << "<h2><strong>Hcal Rec Hit Status</strong></h2>" << endl;
  htmlFile << "<h3>" << endl;
  htmlFile << "</h3>" << endl;

  htmlFile << "<table align=\"center\" border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  htmlFile << "<tr align=\"center\">" << endl;
  gStyle->SetPalette(20,pcol_error_); // set palette to standard error color scheme
  htmlAnyHisto(runNo,ProblemRecHits,"i#eta","i#phi", 92, htmlFile, htmlDir);
  htmlFile<<"</tr>"<<endl;
  htmlFile<<"<tr align=\"center\"><td> There are as yet no criteria for marking a rec hit as 'bad'.  The plots in the link below are for diagnostic purposes only."<<endl;
 
  htmlFile<<"</td>"<<endl;
  htmlFile<<"</tr></table>"<<endl;
  htmlFile<<"<hr><table align=\"center\" border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  htmlFile << "<tr align=\"center\">" << endl;
  htmlFile<<"<tr><td align=center><a href=\"Expert_"<< htmlName<<"\"><h2>Detailed Rec Hit Plots</h2> </a></br></td>"<<endl;
  htmlFile<<"</tr></table><br><hr>"<<endl;
  
  // Now print out problem cells
  htmlFile <<"<br>"<<endl;
  htmlFile << "<h2><strong>Hcal Problem Cells</strong></h2>" << endl;
  htmlFile << "(A problem cell is listed below if its failure rate exceeds "<<(100.*minErrorFlag_)<<"%).<br><br>"<<endl;
  htmlFile << "<table align=\"center\" border=\"1\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  htmlFile << "<tr align=\"center\">" << endl;
  htmlFile <<"<td> Problem Rec Hits<br>(ieta, iphi, depth)</td><td align=\"center\"> Fraction of Events <br>in which cells are bad (%)</td></tr>"<<endl;

  if (ProblemRecHits==0)
    {
      if (debug_) cout <<"<HcalRecHitClient::htmlOutput>  ERROR: can't find Problem Rec Hit plot!"<<endl;
      return;
    }
  int etabins  = ProblemRecHits->GetNbinsX();
  int phibins  = ProblemRecHits->GetNbinsY();
  float etaMin = ProblemRecHits->GetXaxis()->GetXmin();
  float phiMin = ProblemRecHits->GetYaxis()->GetXmin();

  int eta,phi;

  ostringstream name;
  for (int depth=0;depth<6; ++depth)
    {
      for (int ieta=1;ieta<=etabins;++ieta)
        {
          for (int iphi=1; iphi<=phibins;++iphi)
            {
              eta=ieta+int(etaMin)-1;
              phi=iphi+int(phiMin)-1;
	      int mydepth=depth+1;
	      if (mydepth>4) mydepth-=4; // last two depth values are for HE depth 1,2
	      if (ProblemRecHitsByDepth[depth]==0)
		{
		  continue;
		}
	      if (ProblemRecHitsByDepth[depth]->GetBinContent(ieta,iphi)>minErrorFlag_)
		{
		  if (depth<2)
		    (fabs(eta)<29) ? name<<"HB" : name<<"HF";
		  else if (depth==3)
		    (fabs(eta)<42) ? name<<"HO" : name<<"ZDC";
		  else name <<"HE";
		  htmlFile<<"<td>"<<name.str().c_str()<<" ("<<eta<<", "<<phi<<", "<<mydepth<<")</td><td align=\"center\">"<<ProblemRecHitsByDepth[depth]->GetBinContent(ieta,iphi)*100.<<"</td></tr>"<<endl;

		  name.str("");
		}
	    } // for (int iphi=1;...)
	} // for (int ieta=1;...)
    } // for (int depth=0;...)
  
  
  // html page footer
  htmlFile <<"</table> " << endl;
  htmlFile << "</body> " << endl;
  htmlFile << "</html> " << endl;

  htmlFile.close();
  htmlExpertOutput(runNo, htmlDir, htmlName);

  if (showTiming_)
    {
      cpu_timer.stop();  cout <<"TIMER:: HcalRecHitClient HTMLOUTPUT  -> "<<cpu_timer.cpuTime()<<endl;
    }

  return;
} //void HcalRecHitClient::htmlOutput(int runNo, ...) 


void HcalRecHitClient::htmlExpertOutput(int runNo, string htmlDir, string htmlName)
{

  if (showTiming_)
    {
      cpu_timer.reset(); cpu_timer.start();
    }

  if (debug_>1) 
    cout <<" <HcalRecHitClient::htmlExpertOutput>  Preparing Expert html output ..." <<endl;
  
  string client = "RecHitMonitor";
  htmlErrors(runNo,htmlDir,client,process_,dbe_,dqmReportMapErr_,dqmReportMapWarn_,dqmReportMapOther_); // does this do anything?

  ofstream htmlFile;
  htmlFile.open((htmlDir +"Expert_"+ htmlName).c_str());

  // html page header
  htmlFile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">  " << endl;
  htmlFile << "<html>  " << endl;
  htmlFile << "<head>  " << endl;
  htmlFile << "  <meta content=\"text/html; charset=ISO-8859-1\"  " << endl;
  htmlFile << " http-equiv=\"content-type\">  " << endl;
  htmlFile << "  <title>Monitor: Hcal Rec Hit Task output</title> " << endl;
  htmlFile << "</head>  " << endl;
  htmlFile << "<style type=\"text/css\"> td { font-weight: bold } </style>" << endl;
  htmlFile << "<body>  " << endl;
  htmlFile <<"<a name=\"EXPERT_RECHIT_TOP\" href = \".\"> Back to Main HCAL DQM Page </a><br>"<<endl;
  htmlFile <<"<a href= \""<<htmlName.c_str()<<"\" > Back to Rec Hit Status Page </a><br>"<<endl;
  htmlFile << "<br>  " << endl;
  htmlFile << "<h2>Run:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << runNo << "</span></h2>" << endl;
  htmlFile << "<h2>Monitoring task:&nbsp;&nbsp;&nbsp;&nbsp; <span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">Hcal Rec Hits</span></h2> " << endl;
  htmlFile << "<h2>Events processed:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << endl;
  htmlFile << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span " << endl;
  htmlFile << " style=\"color: rgb(0, 0, 153);\">" << ievt_ << "</span></h2>" << endl;
  htmlFile << "<hr>" << endl;

  htmlFile << "<table width=100%  border = 1>"<<endl;
  htmlFile << "<tr><td align=\"center\" colspan=1><a href=\"#OVERALL_PROBLEMS\">PROBLEM CELLS BY DEPTH </a></td></tr>"<<endl;
  htmlFile << "<tr><td align=\"center\">"<<endl;
  htmlFile<<"<br><a href=\"#OCC_PLOTS\">RecHit Occupancy Plots </a>"<<endl;
  htmlFile<<"<br><a href=\"#ENERGY_PLOTS\">RecHit Energy Plots</a>"<<endl;
  htmlFile<<"<br><a href=\"#TIME_PLOTS\">RecHit Time Plots </a>"<<endl;

  htmlFile << "</td></tr>"<<endl;
  htmlFile <<"</table>"<<endl;
  htmlFile <<"<br><br>"<<endl;


  // Plot overall errors
  htmlFile << "<h2><strong><a name=\"OVERALL_PROBLEMS\">Eta-Phi Maps of Problem Cells By Depth</strong></h2>"<<endl;
  htmlFile <<" These plots of problem cells combine results from all rec hit tests<br>"<<endl;
  htmlFile <<"<a href= \"#EXPERT_RECHIT_TOP\" > Back to Top</a><br>"<<endl;
  htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  gStyle->SetPalette(20,pcol_error_); // set palette to standard error color scheme
  
  // Depths are stored as:  0:  HB/HF depth 1, 1:  HB/HF 2, 2:  HE 3, 3:  HO/ZDC, 4: HE 1, 5:  HE2
  // remap so that HE depths are plotted consecutively
  int mydepth[6]={0,1,4,5,2,3};
  for (int i=0;i<3;++i)
    {
      htmlFile << "<tr align=\"left\">" << endl;
      htmlAnyHisto(runNo,ProblemRecHitsByDepth[mydepth[2*i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,ProblemRecHitsByDepth[mydepth[2*i]+1],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
    }

  htmlFile <<"</table>"<<endl;
  htmlFile <<"<br><hr><br>"<<endl;
  
  // Occupancy Plots
  htmlFile << "<h2><strong><a name=\"OCC_PLOTS\">Occupancy Plots</strong></h2>"<<endl;
  htmlFile <<"This shows average rechit occupancy of each cell per event<br>"<<endl;
  htmlFile <<"<a href= \"#EXPERT_RECHIT_TOP\" > Back to Top</a><br>"<<endl;
  htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  gStyle->SetPalette(1);
  for (int i=0;i<6;++i)
    {
      htmlFile << "<tr align=\"left\">" << endl;
      htmlAnyHisto(runNo,OccupancyByDepth[mydepth[i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,OccupancyThreshByDepth[mydepth[i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
    }
  htmlFile <<"</table>"<<endl;
  if (rechitclient_makeDiagnostics_)
    {
      htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
      htmlFile << "cellpadding=\"10\"> " << endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HBOccupancy,"HB Occupancy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HBThreshOccupancy,"HB Occupancy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HEOccupancy,"HE Occupancy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HEThreshOccupancy,"HE Occupancy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HOOccupancy,"HO Occupancy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HOThreshOccupancy,"HO Occupancy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HFOccupancy,"HF Occupancy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HFThreshOccupancy,"HF Occupancy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile <<"</table>"<<endl;
    }

  htmlFile <<"<br><hr><br>"<<endl;

  // energy Plots
  htmlFile << "<h2><strong><a name=\"ENERGY_PLOTS\">Energy Plots</strong></h2>"<<endl;
  htmlFile <<"This shows average rechit energy of each cell per event<br>"<<endl;
  htmlFile <<"<a href= \"#EXPERT_RECHIT_TOP\" > Back to Top</a><br>"<<endl;
  htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  gStyle->SetPalette(1);
  for (int i=0;i<6;++i)
    {
      htmlFile << "<tr align=\"left\">" << endl;
      htmlAnyHisto(runNo,EnergyByDepth[mydepth[i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,EnergyThreshByDepth[mydepth[i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
    }
  htmlFile <<"</table>"<<endl;
  if (rechitclient_makeDiagnostics_)
    {
      htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
      htmlFile << "cellpadding=\"10\"> " << endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HBEnergy,"HB Energy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HBThreshEnergy,"HB Energy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HEEnergy,"HE Energy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HEThreshEnergy,"HE Energy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HOEnergy,"HO Energy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HOThreshEnergy,"HO Energy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HFEnergy,"HF Energy","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HFThreshEnergy,"HF Energy","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile <<"</table>"<<endl;
    }

  htmlFile <<"<br><hr><br>"<<endl;

  htmlFile << "<h2><strong><a name=\"TIME_PLOTS\">Time Plots</strong></h2>"<<endl;
  htmlFile <<"This shows average rechit time of each cell per event<br>"<<endl;
  htmlFile <<"<a href= \"#EXPERT_RECHIT_TOP\" > Back to Top</a><br>"<<endl;
  htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
  htmlFile << "cellpadding=\"10\"> " << endl;
  gStyle->SetPalette(1);
  for (int i=0;i<6;++i)
    {
      htmlFile << "<tr align=\"left\">" << endl;
      htmlAnyHisto(runNo,TimeByDepth[mydepth[i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,TimeThreshByDepth[mydepth[i]],"i#eta","i#phi", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
    }
  htmlFile <<"</table>"<<endl;
  if (rechitclient_makeDiagnostics_)
    {
      htmlFile << "<table border=\"0\" cellspacing=\"0\" " << endl;
      htmlFile << "cellpadding=\"10\"> " << endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HBTime,"HB Time","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HBThreshTime,"HB Time","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HETime,"HE Time","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HEThreshTime,"HE Time","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HOTime,"HO Time","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HOThreshTime,"HO Time","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile<<"<tr align=\"left\">"<<endl;
      htmlAnyHisto(runNo,d_HFTime,"HF Time","", 92, htmlFile, htmlDir);
      htmlAnyHisto(runNo,d_HFThreshTime,"HF Time","", 92, htmlFile, htmlDir);
      htmlFile <<"</tr>"<<endl;
      htmlFile <<"</table>"<<endl;
    }

  htmlFile <<"<br><hr><br>"<<endl;



  htmlFile <<"<br><hr><br><a href= \"#EXPERT_RECHIT_TOP\" > Back to Top of Page </a><br>"<<endl;
  htmlFile <<"<a href = \".\"> Back to Main HCAL DQM Page </a><br>"<<endl;
  htmlFile <<"<a href= \""<<htmlName.c_str()<<"\" > Back to Rec Hit Status Page </a><br>"<<endl;

  htmlFile << "</body> " << endl;
  htmlFile << "</html> " << endl;
  
  htmlFile.close();

  if (showTiming_)
    {
      cpu_timer.stop();  cout <<"TIMER:: HcalRecHitClient  HTMLEXPERTOUTPUT ->"<<cpu_timer.cpuTime()<<endl;
    }
  return;
} // void HcalRecHitClient::htmlExpertOutput(...)



void HcalRecHitClient::loadHistograms(TFile* infile)
{
  TNamed* tnd = (TNamed*)infile->Get("DQMData/Hcal/RecHitMonitor_Hcal/Rec Hit Task Event Number");
  if(tnd)
    {
      string s =tnd->GetTitle();
      ievt_ = -1;
      sscanf((s.substr(2,s.length()-2)).c_str(), "%d", &ievt_);
    }

  ostringstream name;
  // Grab individual histograms
  name<<process_.c_str()<<"RecHitMonitor_Hcal/ ProblemRecHits";
  ProblemRecHits = (TH2F*)infile->Get(name.str().c_str());
  name.str("");
  
  for (int i=0;i<6;++i)
    {
      // Grab arrays of histograms
      name<<process_.c_str()<<"RecHitMonitor_Hcal/problem_rechits/"<<subdets_[i]<<" Problem RecHit Rate";
      ProblemRecHitsByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name.str("");
      OccupancyByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_occupancy/"<<subdets_[i]<<"Rec Hit Average Occupancy";
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_occupancy/"<<subdets_[i]<<"Rec Hit Average Occupancy Above Threshold";
      OccupancyThreshByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name.str("");
      
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_energy/"<<subdets_[i]<<"Rec Hit Average Energy";
      EnergyByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_energy/"<<subdets_[i]<<"Rec Hit Average Energy Above Threshold";
      EnergyThreshByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name.str("");
      
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_time/"<<subdets_[i]<<"Rec Hit Average Time";
      TimeByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/rechit_time/"<<subdets_[i]<<"Rec Hit Average Time Above Threshold";
      TimeThreshByDepth[i] = (TH2F*)infile->Get(name.str().c_str());
      name.str("");
    } //for (int i=0;i<6;++i)

  if (rechitclient_makeDiagnostics_)
    {
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_energy";
      d_HBEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_total_energy";
      d_HBTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_time";
      d_HBTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_occupancy";
      d_HBOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_energy_thresh";
      d_HBThreshEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_total_energy_thresh";
      d_HBThreshTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_time_thresh";
      d_HBThreshTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hb/HB_occupancy_thresh";
      d_HBThreshOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_energy";
      d_HEEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_total_energy";
      d_HETotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_time";
      d_HETime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_occupancy";
      d_HEOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_energy_thresh";
      d_HEThreshEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_total_energy_thresh";
      d_HEThreshTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_time_thresh";
      d_HEThreshTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/he/HE_occupancy_thresh";
      d_HEThreshOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_energy";
      d_HOEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_total_energy";
      d_HOTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_time";
      d_HOTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_occupancy";
      d_HOOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_energy_thresh";
      d_HOThreshEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_total_energy_thresh";
      d_HOThreshTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_time_thresh";
      d_HOThreshTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/ho/HO_occupancy_thresh";
      d_HOThreshOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");

      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_energy";
      d_HFEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_total_energy";
      d_HFTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_time";
      d_HFTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_occupancy";
      d_HFOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_energy_thresh";
      d_HFThreshEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_total_energy_thresh";
      d_HFThreshTotalEnergy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_time_thresh";
      d_HFThreshTime=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
      name<<process_.c_str()<<"RecHitMonitor_Hcal/diagnostics/hf/HF_occupancy_thresh";
      d_HFThreshOccupancy=(TH1F*)infile->Get(name.str().c_str());
      name.str("");
    }
  return;

} // void HcalRecHitClient::loadHistograms(...)


