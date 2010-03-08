#include "DQM/HcalMonitorClient/interface/HcalRawDataClient.h"
#include "DQM/HcalMonitorClient/interface/HcalClientUtils.h"
#include "DQM/HcalMonitorClient/interface/HcalHistoUtils.h"

#include "CondFormats/HcalObjects/interface/HcalChannelStatus.h"
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/HcalObjects/interface/HcalCondObjectContainer.h"

#include <iostream>

/*
 * \file HcalRawDataClient.cc
 * 
 * $Date: 2010/03/08 09:30:23 $
 * $Revision: 1.2 $
 * \author J. St. John
 * \brief Hcal Raw Data Client class
 */

using namespace std;
using namespace edm;

HcalRawDataClient::HcalRawDataClient(std::string myname)
{
  name_=myname;
}

HcalRawDataClient::HcalRawDataClient(std::string myname, const edm::ParameterSet& ps)
{
  name_=myname;
  enableCleanup_         = ps.getUntrackedParameter<bool>("enableCleanup",false);
  debug_                 = ps.getUntrackedParameter<int>("debug",0);
  prefixME_              = ps.getUntrackedParameter<string>("subSystemFolder","Hcal/");
  if (prefixME_.substr(prefixME_.size()-1,prefixME_.size())!="/")
    prefixME_.append("/");
  subdir_                = ps.getUntrackedParameter<string>("RawDataFolder","RawDataMonitor_Hcal/"); // RawDataMonitor
  if (subdir_.size()>0 && subdir_.substr(subdir_.size()-1,subdir_.size())!="/")
    subdir_.append("/");
  subdir_=prefixME_+subdir_;

  cloneME_ = ps.getUntrackedParameter<bool>("cloneME", true);
  badChannelStatusMask_   = ps.getUntrackedParameter<int>("RawData_BadChannelStatusMask",
							  ps.getUntrackedParameter<int>("BadChannelStatusMask",0));
  
  minerrorrate_ = ps.getUntrackedParameter<double>("RawData_minerrorrate",
						   ps.getUntrackedParameter<double>("minerrorrate",0.25));
  minevents_    = ps.getUntrackedParameter<int>("RawData_minevents",
						ps.getUntrackedParameter<int>("minevents",1));
  ProblemCells=0;
  ProblemCellsByDepth=0;
}

void HcalRawDataClient::endLuminosityBlock() {
//  if (LBprocessed_==true) return;  // LB already processed
//  UpdateMEs();
//  LBprocessed_=true; 
  if (debug_>2) std::cout <<"\tHcalRawDataClient::endLuminosityBlock()"<<std::endl;
  calculateProblems();
  return;
}


void HcalRawDataClient::analyze()
{
  if (debug_>2) std::cout <<"\tHcalRawDataClient::analyze()"<<std::endl;
  //  calculateProblems();
}

void HcalRawDataClient::calculateProblems()
{
 if (debug_>2) std::cout <<"\t\tHcalRawDataClient::calculateProblems()"<<std::endl;
  if(!dqmStore_) return;
  double totalevents=0;
  int etabins=0, phibins=0, zside=0;
  double problemvalue=0;

  // Clear away old problems
  if (ProblemCells!=0)
    {
      ProblemCells->Reset();
      (ProblemCells->getTH2F())->SetMaximum(1.05);
      (ProblemCells->getTH2F())->SetMinimum(0.);
    }
  for  (unsigned int d=0;d<ProblemCellsByDepth->depth.size();++d)
    {
      if (ProblemCellsByDepth->depth[d]!=0) 
	{
	  ProblemCellsByDepth->depth[d]->Reset();
	  (ProblemCellsByDepth->depth[d]->getTH2F())->SetMaximum(1.05);
	  (ProblemCellsByDepth->depth[d]->getTH2F())->SetMinimum(0.);
	}
    }
  enoughevents_=true;
  normalizeHardwareSpaceHistos();
  std::vector<std::string> name = HcalEtaPhiHistNames();

  // Because we're clearing and re-forming the problem cell histogram here, we don't need to do any cute
  // setting of the underflow bin to 0, and we can plot results as a raw rate between 0-1.
  
  for (unsigned int d=0;d<ProblemCellsByDepth->depth.size();++d)
    {
      if (ProblemCellsByDepth->depth[d]==0) continue;
    
      //totalevents=DigiPresentByDepth[d]->GetBinContent(0);
      totalevents=0;
      if (totalevents==0 || totalevents<minevents_) continue;
      etabins=(ProblemCellsByDepth->depth[d]->getTH2F())->GetNbinsX();
      phibins=(ProblemCellsByDepth->depth[d]->getTH2F())->GetNbinsY();
      problemvalue=0;
      for (int eta=0;eta<etabins;++eta)
	{
	  int ieta=CalcIeta(eta,d+1);
	  if (ieta==-9999) continue;
	  for (int phi=0;phi<phibins;++phi)
	    {
	      problemvalue=0;
	      //if (DigiPresentByDepth[d]!=0 && DigiPresentByDepth[d]->GetBinContent(eta+1,phi+1)==0) problemvalue=totalevents;
	      if (problemvalue==0) continue;
	      problemvalue/=totalevents; // problem value is a rate; should be between 0 and 1
	      problemvalue = min(1.,problemvalue);
	      
	      zside=0;
	      if (isHF(eta,d+1)) // shift ieta by 1 for HF
		ieta<0 ? zside = -1 : zside = 1;

	      // For problem cells that exceed our allowed rate,
	      // set the values to -1 if the cells are already marked in the status database
	      if (problemvalue>minerrorrate_)
		{
		  HcalSubdetector subdet=HcalEmpty;
		  if (isHB(eta,d+1))subdet=HcalBarrel;
		  else if (isHE(eta,d+1)) subdet=HcalEndcap;
		  else if (isHF(eta,d+1)) subdet=HcalForward;
		  else if (isHO(eta,d+1)) subdet=HcalOuter;
		  HcalDetId hcalid(subdet, ieta, phi+1, (int)(d+1));
		  if (badstatusmap.find(hcalid)!=badstatusmap.end())
		    problemvalue=999; 		
		}

	      ProblemCellsByDepth->depth[d]->setBinContent(eta+1,phi+1,problemvalue);
	      if (ProblemCells!=0) ProblemCells->Fill(ieta+zside,phi+1,problemvalue);
	    } // loop on phi
	} // loop on eta
    } // loop on depth

  if (ProblemCells==0)
    {
      if (debug_>0) std::cout <<"<HcalRawDataClient::analyze> ProblemCells histogram does not exist!"<<endl;
      return;
    }

  // Normalization of ProblemCell plot, in the case where there are errors in multiple depths
  etabins=(ProblemCells->getTH2F())->GetNbinsX();
  phibins=(ProblemCells->getTH2F())->GetNbinsY();
  for (int eta=0;eta<etabins;++eta)
    {
      for (int phi=0;phi<phibins;++phi)
	{
	  if (ProblemCells->getBinContent(eta+1,phi+1)>1. && ProblemCells->getBinContent(eta+1,phi+1)<999)
	    ProblemCells->setBinContent(eta+1,phi+1,1.);
	}
    }

  FillUnphysicalHEHFBins(*ProblemCellsByDepth);
  FillUnphysicalHEHFBins(ProblemCells);
  return;
}

void HcalRawDataClient::beginJob()
{
  dqmStore_ = Service<DQMStore>().operator->();
  if (debug_>0) 
    {
      std::cout <<"<HcalRawDataClient::beginJob()>  Displaying dqmStore directory structure:"<<std::endl;
      dqmStore_->showDirStructure();
    }
}
void HcalRawDataClient::endJob(){}

void HcalRawDataClient::beginRun(void)
{
  enoughevents_=false;
  if (!dqmStore_) 
    {
      if (debug_>0) std::cout <<"<HcalRawDataClient::beginRun> dqmStore does not exist!"<<std::endl;
      return;
    }
  dqmStore_->setCurrentFolder(subdir_);
  problemnames_.clear();

  // Put the appropriate name of your problem summary here
  ProblemCells=dqmStore_->book2D(" ProblemRawData",
				 " Problem Raw Data Rate for all HCAL;ieta;iphi",
				 85,-42.5,42.5,
				 72,0.5,72.5);
  problemnames_.push_back(ProblemCells->getName());
  if (debug_>1)
    std::cout << "Tried to create ProblemCells Monitor Element in directory "<<subdir_<<"  \t  Failed?  "<<(ProblemCells==0)<<std::endl;
  dqmStore_->setCurrentFolder(subdir_+"problem_rawdata");
  ProblemCellsByDepth = new EtaPhiHists();
  ProblemCellsByDepth->setup(dqmStore_," Problem Raw Data Rate");
  for (unsigned int i=0; i<ProblemCellsByDepth->depth.size();++i)
    problemnames_.push_back(ProblemCellsByDepth->depth[i]->getName());
  nevts_=0;
}

void HcalRawDataClient::endRun(void){analyze();}

void HcalRawDataClient::setup(void){}
void HcalRawDataClient::cleanup(void){}

bool HcalRawDataClient::hasErrors_Temp(void)
{
  if (!ProblemCells)
    {
      if (debug_>1) std::cout <<"<HcalRawDataClient::hasErrors_Temp>  ProblemCells histogram does not exist!"<<std::endl;
      return false;
    }
  int problemcount=0;
  int ieta=-9999;

  for (int depth=0;depth<4; ++depth)
    {
      int etabins  = (ProblemCells->getTH2F())->GetNbinsX();
      int phibins  = (ProblemCells->getTH2F())->GetNbinsY();
      for (int hist_eta=0;hist_eta<etabins;++hist_eta)
        {
          for (int hist_phi=0; hist_phi<phibins;++hist_phi)
            {
              ieta=CalcIeta(hist_eta,depth+1);
	      if (ieta==-9999) continue;
	      if (ProblemCellsByDepth->depth[depth]==0)
		continue;
	      if (ProblemCellsByDepth->depth[depth]->getBinContent(hist_eta,hist_phi)>minerrorrate_)
		++problemcount;

	    } // for (int hist_phi=1;...)
	} // for (int hist_eta=1;...)
    } // for (int depth=0;...)

  if (problemcount>0) return true;
  return false;
}

bool HcalRawDataClient::hasWarnings_Temp(void){return false;}
bool HcalRawDataClient::hasOther_Temp(void){return false;}
bool HcalRawDataClient::test_enabled(void){return true;}


void HcalRawDataClient::updateChannelStatus(std::map<HcalDetId, unsigned int>& myqual)
{
  // This gets called by HcalMonitorClient
  // see dead or hot cell code for an example

} //void HcalRawDataClient::updateChannelStatus


void HcalRawDataClient::getHistosToNormalize(void){
  MonitorElement* me;
  string s;
  s=subdir_+"Corruption/07 LRB Data Corruption Indicators";
  me=dqmStore_->get(s.c_str());  
  LRBDataCorruptionIndicators_=HcalUtilsClient::getHisto<TH2F*>(me, cloneME_, LRBDataCorruptionIndicators_, debug_);
  if (!LRBDataCorruptionIndicators_ & (debug_>0)) std::cout <<"<HcalRawDataClient::analyze> "<<s<<" histogram does not exist!"<<endl;
  //  if (LRBDataCorruptionIndicators_)
  //    LRBDataCorruptionIndicators_->SetBinContent(0,0,ievt_);

  s=subdir_+"Corruption/08 Half-HTR Data Corruption Indicators";
  me=dqmStore_->get(s.c_str());  
  HalfHTRDataCorruptionIndicators_=HcalUtilsClient::getHisto<TH2F*>(me, cloneME_, HalfHTRDataCorruptionIndicators_, debug_);
  if (!HalfHTRDataCorruptionIndicators_ & (debug_>0)) std::cout <<"<HcalRawDataClient::analyze> "<<s<<" histogram does not exist!"<<endl;
  //  if (HalfHTRDataCorruptionIndicators_)
  //    HalfHTRDataCorruptionIndicators_->SetBinContent(0,0,ievt_);

  s=subdir_+"Data Flow/01 Data Flow Indicators";
  me=dqmStore_->get(s.c_str());  
  DataFlowInd_=HcalUtilsClient::getHisto<TH2F*>(me, cloneME_, DataFlowInd_, debug_);
  if (!DataFlowInd_ & (debug_>0)) std::cout <<"<HcalRawDataClient::analyze> "<<s<<" histogram does not exist!"<<endl;
  //  if (DataFlowInd_)
  //    DataFlowInd_->SetBinContent(0,0,ievt_);

  s=subdir_+"Corruption/09 Channel Integrity Summarized by Spigot";
  me=dqmStore_->get(s.c_str());  
  ChannSumm_DataIntegrityCheck_=HcalUtilsClient::getHisto<TH2F*>(me, cloneME_, ChannSumm_DataIntegrityCheck_, debug_);
  if (!ChannSumm_DataIntegrityCheck_ & (debug_>0)) std::cout <<"<HcalRawDataClient::analyze> "<<s<<" histogram does not exist!"<<endl;

  char chararray[150];
  for (int i=0; i<NUMDCCS; i++) {
    sprintf(chararray,"Corruption/Channel Data Integrity/FED %03d Channel Integrity", i+700);
    s=subdir_+string(chararray);
    me=dqmStore_->get(s.c_str());  
    Chann_DataIntegrityCheck_[i]=HcalUtilsClient::getHisto<TH2F*>(me, cloneME_, Chann_DataIntegrityCheck_[i], debug_);
    if (!Chann_DataIntegrityCheck_[i] & (debug_>0)) std::cout <<"<HcalRawDataClient::analyze> "<<s<<" histogram does not exist!"<<endl;
  }
}
void HcalRawDataClient::normalizeHardwareSpaceHistos(void){
  // Get histograms that are used in testing
  getHistosToNormalize();

  int fed2offset=0;
  int fed3offset=0;
  int spg2offset=0;
  int spg3offset=0;
  int chn2offset=0;
  float tsFactor=1.0;
  float val=0.0;

  //Normalize everything by nevts_
  for (int fednum=0; fednum<NUMDCCS; fednum++) {
    fed3offset = 1 + (4*fednum); //3 bins, plus one of margin, each DCC
    fed2offset = 1 + (3*fednum); //2 bins, plus one of margin, each DCC
    for (int spgnum=0; spgnum<15; spgnum++) {
      spg3offset = 1 + (4*spgnum); //3 bins, plus one of margin, each spigot
      for (int xbin=1; xbin<=3; xbin++) {
	for (int ybin=1; ybin<=3; ybin++) {
	  if (!LRBDataCorruptionIndicators_) continue;
	  val = LRBDataCorruptionIndicators_->GetBinContent(fed3offset+xbin,
							    spg3offset+ybin);
	  if (val) 
	    LRBDataCorruptionIndicators_->SetBinContent(fed3offset+xbin,
							spg3offset+ybin,
							(float)val);
	  if (!HalfHTRDataCorruptionIndicators_) continue;
	  val = HalfHTRDataCorruptionIndicators_->GetBinContent(fed3offset+xbin,
								spg3offset+ybin);
	  if (val) {
	    HalfHTRDataCorruptionIndicators_->SetBinContent(fed3offset+xbin,
							    spg3offset+ybin,
							    (float)val );
	  }
	  if (!DataFlowInd_ || xbin>2) continue;  //DataFlowInd_;  2x by 3y
	  val = DataFlowInd_->GetBinContent(fed2offset+xbin,
					    spg3offset+ybin);
	  if (val) 
	    DataFlowInd_->SetBinContent(fed2offset+xbin,
					spg3offset+ybin,	
					( (float)val ));
	}
      }
    }
  }

  if (!ChannSumm_DataIntegrityCheck_) return;
  //Normalize by the number of events each channel spake. (Handles ZS!)
  for (int fednum=0;fednum<NUMDCCS;fednum++) {
    fed2offset = 1 + (3*fednum); //2 bins, plus one of margin, each DCC 
    for (int spgnum=0; spgnum<15; spgnum++) {
      spg2offset = 1 + (3*spgnum); //2 bins, plus one of margin, each spigot
      numTS_[(fednum*NUMSPGS)+spgnum]=ChannSumm_DataIntegrityCheck_->GetBinContent(fed2offset,
										   spg2offset+1);

      for (int xbin=1; xbin<=2; xbin++) {
  	for (int ybin=1; ybin<=2; ybin++) {
  	  val = ChannSumm_DataIntegrityCheck_->GetBinContent(fed2offset+xbin,
  							     spg2offset+ybin);
	  if ( (val) && (nevts_) ) {
	    //Lower pair of bins don't scale with just the timesamples per event.
	    if (ybin==2) tsFactor=numTS_[spgnum +(fednum*NUMSPGS)]; 
	    else {
	      if (xbin==2) tsFactor=numTS_[spgnum +(fednum*NUMSPGS)]-1;
	      else tsFactor=1.0;
	    }
	    if (tsFactor)
	      ChannSumm_DataIntegrityCheck_->SetBinContent(fed2offset+xbin,
							   spg2offset+ybin,
							   val/(nevts_*tsFactor));
	    val=0.0;
	  }
  	}
      }
      //Clear the numTS, which clutter the final plot.
      ChannSumm_DataIntegrityCheck_->SetBinContent(fed2offset  ,
						   spg2offset  , 0.0);
      ChannSumm_DataIntegrityCheck_->SetBinContent(fed2offset  ,
						   spg2offset+1, 0.0);

      if (!Chann_DataIntegrityCheck_[fednum]) continue;  
      for (int chnnum=0; chnnum<24; chnnum++) {
  	chn2offset = 1 + (3*chnnum); //2 bins, plus one of margin, each channel
	if (! (Chann_DataIntegrityCheck_[fednum]))  
	  continue;
  	for (int xbin=1; xbin<=2; xbin++) {
  	  for (int ybin=1; ybin<=2; ybin++) {
  	    val = Chann_DataIntegrityCheck_[fednum]->GetBinContent(chn2offset+xbin,
  								   spg2offset+ybin);
  	    if ( (val) && (nevts_) ) {
	      //Lower pair of bins don't scale with just the timesamples per event.
	      if (ybin==2) tsFactor=numTS_[spgnum +(fednum*NUMSPGS)]; 
	      else {
		if (xbin==2) tsFactor=numTS_[spgnum +(fednum*NUMSPGS)]-1;
		else tsFactor=1.0;
	      }
	      if (tsFactor)
		Chann_DataIntegrityCheck_[fednum]->SetBinContent(chn2offset+xbin,
								 spg2offset+ybin,
								 val/(nevts_*tsFactor));
	    }
  	  }
  	}
	//Remove the channel's event count from sight.
	Chann_DataIntegrityCheck_[fednum]->SetBinContent(chn2offset,
							 spg2offset,0.0);
      }
    }
  }  
}
