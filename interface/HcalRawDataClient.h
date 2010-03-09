#ifndef HcalRawDataClient_GUARD_H
#define HcalRawDataClient_GUARD_H
#define DEPTHBINS      4
#define  IETAMIN     -43
#define  IETAMAX      43
#define  IPHIMIN       0
#define  IPHIMAX      71
#define  NUMDCCS      32
#define  NUMSPGS     15
#define  HTRCHANMAX   24

#include "DQM/HcalMonitorClient/interface/HcalBaseDQClient.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"

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
  // Machinery here for transforming hardware space into ieta/iphi/depth
  const HcalElectronicsMap*    readoutMap_;
  //Electronics map -> geographic channel map
  inline int hashup(uint32_t d=0, uint32_t s=0, uint32_t c=1) {
    return (int) ( (d*NUMSPGS*HTRCHANMAX)+(s*HTRCHANMAX)+(c)); }
  void stashHDI(int thehash, HcalDetId thehcaldetid);
  //Protect against indexing past array.
  inline HcalDetId HashToHDI(int thehash) {
    return ( ( (thehash<0) || (thehash>(NUMDCCS*NUMSPGS*HTRCHANMAX)) )
	     ?(HcalDetId::Undefined)
	     :(hashedHcalDetId_[thehash]));
  };
  HcalDetId hashedHcalDetId_[NUMDCCS * NUMSPGS * HTRCHANMAX];

  float numTS_[NUMDCCS*NUMSPGS]; //For how many timesamples per channel were the half-HTRs configured?
  //Histograms indicating problems in hardware space
  TH2F*  meCDFErrorFound_;
  TH2F*  meDCCEventFormatError_;
  TH2F*  meOrNSynch_;
  TH2F*  meBCNSynch_;
  TH2F*  meEvtNumberSynch_;
  TH2F*  LRBDataCorruptionIndicators_;
  TH2F*  HalfHTRDataCorruptionIndicators_;
  TH2F*  DataFlowInd_;
  TH2F*  ChannSumm_DataIntegrityCheck_;
  // handy array of pointers to pointers...
  TH2F* Chann_DataIntegrityCheck_[NUMDCCS];

  void getHardwareSpaceHistos(void);
  void normalizeHardwareSpaceHistos(void);
};

#endif
