/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/TallySelector.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "TallyCreate.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "inputParam.h"
#include "TallySelector.h" 
#include "basicConstruct.h"
#include "pointConstruct.h"
#include "meshConstruct.h"
#include "fluxConstruct.h"
#include "heatConstruct.h"
#include "itemConstruct.h"
#include "surfaceConstruct.h"
#include "tallyConstructFactory.h"
#include "tallyConstruct.h"

int
tallySelection(Simulation& System,const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IP :: InputParam
    \return flag to indicate that more work is required after renumbering
  */
{
  ELog::RegMethod RegA("TallySelector","tallySelection(basic)");

  tallySystem::tallyConstructFactory FC;
  tallySystem::tallyConstruct TallyBuilder(FC);

  return TallyBuilder.tallySelection(System,IParam);
}
 

void
tallyModification(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Applies a large number of modifications to the tally system
    \param System :: Simulation to get tallies from 
    \param IParam :: Parameters
  */
{
  ELog::RegMethod RegA("TallySelector","tallyModification");
  const size_t nP=IParam.setCnt("TMod");
  for(size_t i=0;i<nP;i++)
    {
      std::vector<std::string> StrItem;
      // This is enforced a >1
      const size_t nV=IParam.itemCnt("TMod",i);
      const std::string key=
	IParam.getValue<std::string>("TMod",i,0);
      for(size_t j=1;j<nV;j++)
	StrItem.push_back
	  (IParam.getValue<std::string>("TMod",i,j));

      int errFlag(1);
      if(key=="help")
	{
	  ELog::EM<<"TMod Help "<<ELog::endBasic;
	  ELog::EM<<
	    " -- particle {tallyNumber} [oldtype] [newtype] \n"
	    "    Change the particle on a tally to the new type\n"
	    " -- nowindow {tallyNum}\n"
	    "    Remove the window on an f5 tally\n"
	    " -- energy {tallyNumber} [string] \n"
	    " -- time {tallyNumber} [string] \n"
	    " -- divide {tallyNumber} [xPts,yPts] : Split tally into "
	    " multiple pieces \n"
	    " -- scaleWindow[X/Y] {tallyNumber scale} : Scale the window"
            " by the factor \n"
	    " -- movePoint {tallyNumber Vec3D} : Add Vec3D to tally\n"
	    " -- single {tallyNumber} : Split cell/surface tally into "
	    " individual sum [rather than total] \n"
            " -- format {tallyNumber} [string] : Set the format card \n";

          ELog::EM<<ELog::endBasic;
	  ELog::EM<<ELog::endErr;
          return;
	}

      int tNumber(0);
      if (!StrFunc::convert(StrItem[0],tNumber))
        ELog::EM<<"Failed to convert tally number "<<ELog::endErr;	  
      
      if(key=="particle" && nV>=3)
        {
          tallySystem::setParticleType(System,tNumber,StrItem[1]);
        }
      
      else if (key=="energy" && nV>=3)
	{
          if (tallySystem::setEnergy(System,tNumber,StrItem[1]))
            ELog::EM<<"Error setting tally energy "<<
              StrItem[1]<<ELog::endErr;
	}
      
      else if (key=="single" && nV>=2)
	{
          size_t index(1);
          while(tallySystem::setSingle(System,tNumber) &&
                index<nV)
            {
              if (!StrFunc::convert(StrItem[index-1],tNumber))
                ELog::EM<<"Failed to understand TNumber :"	      
                        <<StrItem[index-1]<<ELog::endErr;
              index++;
            }
        }
      else if (key=="movePoint" && nV>=3)
	{	  
	  Geometry::Vec3D offsetPt;
	  if (!StrFunc::convert(StrItem[1],offsetPt))
	    {
	      size_t ii=0;
	      if (nV>=5)
		for(ii=0;ii<3 &&
		      StrFunc::convert(StrItem[1+ii],offsetPt[ii]);
		    ii++) ;
	      if (ii!=3)
		{
		  ELog::EM<<"Failed to understand Vector :"   
			  <<StrItem[1]<<" ";
		  if (nV>=5)
		    ELog::EM<<StrItem[2]<<" "
			    <<StrItem[3]<<" ";
		  ELog::EM<<ELog::endErr;
		}
	    }
	  tallySystem::moveF5Tally(System,tNumber,offsetPt);
	  ELog::EM<<"Move Point == "<<offsetPt<<ELog::endDiag;
	}

      else if ((key=="scaleWindow" ||
                key=="scaleXWindow" ||
                key=="scaleYWindow")
               && nV>=3)
	{
	  double scale(1.0);
	  if (!StrFunc::convert(StrItem[1],scale))
            {
              ELog::EM<<"Failed to understand Scale :"	     
                      <<StrItem[1]<<ELog::endErr;
              return;
            }
          if (key[5]!='Y') 
            tallySystem::widenF5Tally(System,tNumber,0,scale);
          if (key[5]!='X') 
            tallySystem::widenF5Tally(System,tNumber,1,scale);
	}

      else if (key=="time" && nV>=3)
	{
          if (tallySystem::setTime(System,tNumber,StrItem[1]))
            ELog::EM<<"Error setting tally time "<<
              StrItem[1]<<ELog::endErr;
        }
      
      else if (key=="nowindow" && nV>=2)
	{
          tallySystem::removeF5Window(System,tNumber);          
	}
      
      else if (key=="divide" && nV>=4)
	{
	  int xPts,yPts;
          if (!StrFunc::convert(StrItem[1],xPts) || 
              !StrFunc::convert(StrItem[2],yPts) )
            {
              ELog::EM<<"Failed to understand divide input :"
                      <<StrItem[0]<<":"<<StrItem[1]
                      <<":"<<StrItem[2]<<ELog::endErr;
                  return;
            }
          tallySystem::divideF5Tally(System,tNumber,xPts,yPts);
	}
      else if (key=="format" && nV>=3)
	{
          tallySystem::setFormat(System,tNumber,StrItem[1]);
	}
      else if (key=="merge" && nV>=3)
	{
          int bTnumber;
          if (!StrFunc::convert(StrItem[1],bTNumber))
            {
              ELog::EM<<"Failed to convert to a tally number"
                      <<StrItem[0]<<":"<<StrItem[1]<<ELog::endErr;
              return;
            }
          tallySystem::mergeTally(System,tNumber,bTnumber);
	}
      else if (key=="setSD" && nV>=3)
	{
          tallySystem::setSDField(System,tNumber,StrItem[1]);
	}
      else 
        ELog::EM<<"Failed to process TMod : "<<key<<ELog::endErr;
    }
  return;
}

void
tallyRenumberWork(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IP :: InputParam
  */
{
  ELog::RegMethod RegA("TallySelector","tallyRenumberWork");


  tallySystem::tallyConstructFactory FC;
  tallySystem::tallyConstruct TallyBuilder(FC);
  TallyBuilder.tallyRenumber(System,IParam);
 
  return;
}


