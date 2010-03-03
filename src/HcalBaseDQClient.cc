#include "DQM/HcalMonitorClient/interface/HcalBaseDQClient.h"
#include "DQM/HcalMonitorClient/interface/HcalHistoUtils.h"
#include "DQM/HcalMonitorClient/interface/HcalClientUtils.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace edm;

void HcalBaseDQClient::setStatusMap(std::map<HcalDetId, unsigned int>& map)
  {
    /* Get the list of all bad channels in the status map,
       and combine it with the bad cell requirements for the particular task
       to form a new map
    */

    if (debug_>1) std::cout <<"<HcalBaseDQClient::setStatusMap>  Input map size = "<<map.size()<<endl;
    for (std::map<HcalDetId, unsigned int>::const_iterator iter = map.begin(); 
	 iter!=map.end();++iter)
      {
	if ((iter->second & badChannelStatusMask_) == 0 ) continue; // channel not marked as bad by this test
	badstatusmap[iter->first]=iter->second;
      }
    
    if (debug_>1) std::cout <<"<HcalBaseDQClient::setStatusMap>  "<<name_<<" Output map size = "<<badstatusmap.size()<<endl;
  } // void HcalBaseDQClient::getStatusMap


void HcalBaseDQClient::htmlOutput(string htmlDir)
{
  if (dqmStore_==0) 
    {
      if (debug_>0) std::cout <<"<HcalBaseDQClient::htmlOutput> dqmStore object does not exist!"<<std::endl;
      return;
    }

  int pcol_error[105];
 for( int i=0; i<105; ++i )
    {
      
      TColor* color = gROOT->GetColor( 901+i );
      if( ! color ) color = new TColor( 901+i, 0, 0, 0, "" );
      if (i<5)
	color->SetRGB(i/5.,1.,0);
      else if (i>100)
	color->SetRGB(0,0,0);
      else
	color->SetRGB(1,1-0.01*i,0);
      pcol_error[i]=901+i;
    } // for (int i=0;i<105;++i)


  

  ofstream htmlFile;
  string outfile=htmlDir+name_+".html";
  htmlFile.open(outfile.c_str());

  // html page header
  htmlFile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">  " << std::endl;
  htmlFile << "<html>  " << std::endl;
  htmlFile << "<head>  " << std::endl;
  htmlFile << "  <meta content=\"text/html; charset=ISO-8859-1\"  " << std::endl;
  htmlFile << " http-equiv=\"content-type\">  " << std::endl;
  htmlFile << "  <title>Monitor: Hcal "<<name_<<" output</title> " << std::endl;
  htmlFile << "</head>  " << std::endl;
  htmlFile << "<style type=\"text/css\"> td { font-weight: bold } </style>" << std::endl;
  htmlFile << "<body>  " << std::endl;
  htmlFile << "<br>  " << std::endl;
  htmlFile << "<hr>" << std::endl;

  gStyle->SetPalette(105,pcol_error);
  gStyle->SetNumberContours(105);
  gROOT->ForceStyle();

  if (ProblemCells!=0)
    {
      (ProblemCells->getTH2F())->SetMaximum(1.05);
      (ProblemCells->getTH2F())->SetMinimum(0.);
      htmlFile << "<table align=\"center\" border=\"0\" cellspacing=\"0\" " << std::endl;
      htmlFile << "cellpadding=\"10\"> " << std::endl;
      htmlFile<<"<tr align=\"center\">"<<std::endl;
      htmlAnyHisto(-1,ProblemCells->getTH2F(),"ieta","iphi",92, htmlFile,htmlDir,debug_);
      htmlFile<<"</tr>"<<endl;
      htmlFile<<"</table>"<<endl;
    }
  if (ProblemCellsByDepth.depth.size()>0)
    {
      htmlFile << "<table align=\"center\" border=\"0\" cellspacing=\"0\" " << std::endl;
      htmlFile << "cellpadding=\"10\"> " << std::endl;
      for (int i=0;i<2;++i)
	{
	  (ProblemCellsByDepth.depth[2*i]->getTH2F())->SetMaximum(1.05);
	  (ProblemCellsByDepth.depth[2*i]->getTH2F())->SetMinimum(0.);
	  (ProblemCellsByDepth.depth[2*i+1]->getTH2F())->SetMaximum(1.05);
	  (ProblemCellsByDepth.depth[2*i+1]->getTH2F())->SetMinimum(0.);
	  htmlFile<<"<tr align=\"center\">"<<std::endl;
	  htmlAnyHisto(-1,ProblemCellsByDepth.depth[2*i]->getTH2F(),"ieta","iphi",92, htmlFile,htmlDir,debug_);
	  htmlAnyHisto(-1,ProblemCellsByDepth.depth[2*i+1]->getTH2F(),"ieta","iphi",92, htmlFile,htmlDir,debug_);
	  
	  htmlFile<<"</tr>"<<endl;
	}
      htmlFile<<"</table>"<<endl;
   }
    
  htmlFile << "<table align=\"center\" border=\"0\" cellspacing=\"0\" " << std::endl;
  htmlFile << "cellpadding=\"10\"> " << std::endl;
  

  std::vector<MonitorElement*> hists = dqmStore_->getAllContents(subdir_);
  gStyle->SetPalette(1);
  
  int counter=0;
  for (unsigned int i=0;i<hists.size();++i)
    {
      if (hists[i]->kind()==MonitorElement::DQM_KIND_TH1F)
	{
	  ++counter;
	  if (counter%2==1) 
	    htmlFile << "<tr align=\"center\">" << std::endl;
	  htmlAnyHisto(-1,(hists[i]->getTH1F()),"","", 92, htmlFile, htmlDir,debug_);
	  if (counter%2==2)
	    htmlFile <<"</tr>"<<endl;
	}

      else if (hists[i]->kind()==MonitorElement::DQM_KIND_TH2F)
	{
	  std::string histname=hists[i]->getName();
	  bool isproblem=false;
	  for (unsigned int j=0;j<problemnames_.size();++j)
	    {
	      if (problemnames_[j]==histname)
		{
		  isproblem=true;
		  if (debug_>1) std::cout <<"<HcalBaseDQClient::htmlOutput>  Found Problem Histogram '"<<histname<<"' in list of histograms"<<std::endl;
		  break;
		}	
	    }
	  if (isproblem) continue; // don't redraw problem histograms
	  ++counter;
	  if (counter%2==1) 
	    htmlFile << "<tr align=\"center\">" << std::endl;
	  htmlAnyHisto(-1,(hists[i]->getTH2F()),"","", 92, htmlFile, htmlDir,debug_);
	  if (counter%2==2)
	    htmlFile <<"</tr>"<<endl;
	}

      else if (hists[i]->kind()==MonitorElement::DQM_KIND_TPROFILE)
	{
	  ++counter;
	  if (counter%2==1) 
	    htmlFile << "<tr align=\"center\">" << std::endl;
	  htmlAnyHisto(-1,(hists[i]->getTProfile()),"","", 92, htmlFile, htmlDir,debug_);
	  if (counter%2==2)
	    htmlFile <<"</tr>"<<endl;
	}
    }
  htmlFile<<"</table>"<<endl;
  htmlFile << "</body> " << std::endl;
  htmlFile << "</html> " << std::endl;
  htmlFile.close();
  return;
}
