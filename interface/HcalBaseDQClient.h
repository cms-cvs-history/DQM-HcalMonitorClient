#ifndef HcalBaseDQClient_GUARD_H
#define HcalBaseDQClient_GUARD_H

#include <string>
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DQM/HcalMonitorTasks/interface/HcalEtaPhiHists.h"
#include "DQM/HcalMonitorClient/interface/HcalHistoUtils.h"

/*
 * \file HcalBaseDQClient.h
 * 
 * $Date: 2010/03/02 09:24:00 $
 * $Revision:  1.00 $
 * \author J. Temple
 * \brief Hcal Monitor Client base class
 * based on code in EcalBarrelMonitorClient/interface/EBClient.h
 */


class HcalBaseDQClient
{
 public:
  HcalBaseDQClient(){}
  HcalBaseDQClient(std::string, const edm::ParameterSet&){}
  virtual void analyze(void)           =0;
  virtual void calculateProblems(void) =0;
  virtual void beginJob(void)          =0;
  virtual void endJob(void)            =0;
  virtual void beginRun(void)          =0;
  virtual void endRun(void)            =0;
  virtual void setup(void)             =0;
  virtual void cleanup(void)           =0;
  
  virtual void htmlOutput(std::string htmlDir);
  virtual void setStatusMap(std::map<HcalDetId, unsigned int>& map) ;

  virtual bool hasErrors_Temp(void)    =0;
  virtual bool hasWarnings_Temp(void)  =0;
  virtual bool hasOther_Temp(void)     =0;
  virtual bool test_enabled(void)      =0;
  ~HcalBaseDQClient(void){}
  
  std::string name(){return name_;};
  std::string name_;
  std::string prefixME_;
  std::string subdir_;
  bool cloneME_;
  bool enableCleanup_;
  int debug_;
  int badChannelStatusMask_; 

  bool testenabled_;
  int minevents_; // minimum number of events for test to pass
  double minerrorrate_;

  MonitorElement* ProblemCells;
  EtaPhiHists ProblemCellsByDepth;
  std::vector<std::string> problemnames_;

  std::map<HcalDetId, unsigned int> badstatusmap;
  DQMStore* dqmStore_;
}; // class HcalBaseDQClient



#endif
