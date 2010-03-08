#ifndef HcalRawDataClient_GUARD_H
#define HcalRawDataClient_GUARD_H
#define NUMDCCS 32
#define NUMSPGS 15

#include "DQM/HcalMonitorClient/interface/HcalBaseDQClient.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

class HcalRawDataClient : public HcalBaseDQClient {

 public:

  /// Constructors
  HcalRawDataClient(){name_="";};
  HcalRawDataClient(std::string myname);//{ name_=myname;};
  HcalRawDataClient(std::string myname, const edm::ParameterSet& ps);

  void analyze(void);
  void calculateProblems(void); // calculates problem histogram contents
  void updateChannelStatus(std::map<HcalDetId, unsigned int>& myqual);
  void beginJob(void);
  void endJob(void);
  void beginRun(void);
  void endRun(void); 
  void setup(void);  
  void cleanup(void);
  void endLuminosityBlock(void);
  bool hasErrors_Temp(void);  
  bool hasWarnings_Temp(void);
  bool hasOther_Temp(void);
  bool test_enabled(void);
  
  /// Destructor
  ~HcalRawDataClient();

 private:
  int nevts_;
  float numTS_[NUMDCCS*NUMSPGS]; //For how many timesamples per channel were the half-HTRs configured?
  TH2F*  LRBDataCorruptionIndicators_;
  TH2F*  HalfHTRDataCorruptionIndicators_;
  TH2F*  DataFlowInd_;
  TH2F*  ChannSumm_DataIntegrityCheck_;
  // handy array of pointers to pointers...
  TH2F* Chann_DataIntegrityCheck_[NUMDCCS];

  void getHistosToNormalize(void);
  void normalizeHardwareSpaceHistos(void);
};

#endif
