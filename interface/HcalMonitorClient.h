#ifndef HcalMonitorClient_GUARD_H
#define HcalMonitorClient_GUARD_H

/*
 * \file HcalMonitorClient.h
 * 
 * $Date: 2010/03/03 15:11:13 $
 * $Revision: 1.1.2.1 $
 * \author J. Temple
 * 
 */



#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "TROOT.h"
#include "TH1.h"

#include "DQM/HcalMonitorClient/interface/HcalSummaryClient.h"
#include "DQM/HcalMonitorClient/interface/HcalBaseDQClient.h"

class DQMStore;
class HcalChannelQuality;

class HcalMonitorClient: public edm::EDAnalyzer
{

public:

  // Constructor
  HcalMonitorClient(const edm::ParameterSet & ps);
  
  // Destructor
  virtual ~HcalMonitorClient();

 /// Analyze
  void analyze(void);
  void analyze(const edm::Event & e, const edm::EventSetup & c);
  
  /// BeginJob
  void beginJob(void);
  
  /// EndJob
  void endJob(void);
  
  /// BeginRun
  void beginRun();
  void beginRun(const edm::Run & r, const edm::EventSetup & c);
  
  /// EndRun
  void endRun();
  void endRun(const edm::Run & r, const edm::EventSetup & c);
  
  /// BeginLumiBlock
  void beginLuminosityBlock(const edm::LuminosityBlock & l, const edm::EventSetup & c);
  
  /// EndLumiBlock
  void endLuminosityBlock(const edm::LuminosityBlock & l, const edm::EventSetup & c);
  
  /// Reset
  void reset(void);
  
  /// Setup
  void setup(void);
  
  /// Cleanup
  void cleanup(void);
  
  /// SoftReset
  void softReset(bool flag);
 
  // Write channelStatus info
  void writeChannelStatus();
  
  // Write html output
  void writeHtml();

private:
  // Event counters
  int ievt_; // all events
  int jevt_; // events in current run
  int run_;
  int evt_;
  bool begin_run_;
  bool end_run_;

  // parameter set inputs
  int debug_;
  std::string inputFile_;
  bool mergeRuns_;
  bool cloneME_;
  int prescaleFactor_;
  std::string prefixME_;
  bool enableCleanup_;
  std::vector<std::string > enabledClients_;
  
  std::string baseHtmlDir_;
  int htmlUpdateTime_;
  std::string databasedir_;
  int databaseUpdateTime_;

  // time parameters
  time_t current_time_;
  time_t last_time_update_;
  time_t last_time_db_;
  time_t updateTime_;
  time_t dbUpdateTime_;

  // std::vector<HcalClient*> clients_;
  std::vector<HcalBaseDQClient*> clients_;  // dummy

  DQMStore* dqmStore_;
  HcalChannelQuality* chanquality_;

  HcalSummaryClient* summaryClient_;
};


#endif
