/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ExtConstructor.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "MapRange.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "Simulation.h"
#include "inputParam.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "DSTerm.h"
#include "Source.h"
#include "KCode.h"
#include "ModeCard.h"

#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"
#include "KGroup.h"
#include "PhysicsCards.h"

#include "EUnit.h"
#include "ExtControl.h"
#include "ExtConstructor.h" 


namespace physicsSystem
{

ExtConstructor::ExtConstructor() 
  /// Constructor
{}

bool
ExtConstructor::procZone(std::vector<std::string>& StrItem)
  /*!
    Process the zone information
    \param StrItem :: List of item from the input [Used items erased]
    \return true on success 
  */
{
  ELog::RegMethod RegA("ExtConstuctor","procZone");

  const size_t NS(StrItem.size());
  long int cut(0);
  int cNum,dNum;
  if (NS>=1 && StrItem[0]=="All" )
    {
      Zones.push_back(MapSupport::Range<int>(0,100000000));
      cut=1;
    }
  else if (NS>=2 && StrItem[0]=="Object")
    {
      const ModelSupport::objectRegister& OR= 
	ModelSupport::objectRegister::Instance();
      const int cellN=OR.getCell(StrItem[1]);
      const int rangeN=OR.getRange(StrItem[1]);
      if (cellN==0)
	throw ColErr::InContainerError<std::string>(StrItem[1],"Object name");
      Zones.push_back(MapSupport::Range<int>(cellN,cellN+rangeN));
      cut=2;
    }
  else if (NS>=2 && StrItem[0]=="Cells")
    {
      size_t index(1);
      while(index<NS &&
	    StrFunc::convert(StrItem[index],cNum))
	{
	  Zones.push_back(MapSupport::Range<int>(cNum,cNum));
	  index++;
	}
      if (index!=1) cut=static_cast<long int>(index);
    }
  else if (NS>=3 && StrItem[0]=="Range")
    {
      if (StrFunc::convert(StrItem[1],cNum) &&
	  StrFunc::convert(StrItem[2],dNum) )
	{
	  Zones.push_back(MapSupport::Range<int>(cNum,dNum));	
	  cut=3;
	}
    }
  
  if (!cut)
    return 0;
      
  StrItem.erase(StrItem.begin(),StrItem.begin()+cut);
  return 1;
}

bool
ExtConstructor::procType(std::vector<std::string>& StrItem,
			 ExtControl& EX)
  /*!
    Process the Type information
    \param StrItem :: List of item from the input [Used items erased]
    \param EX :: Control card to place data [and zone]
    \return true on success 
  */
{
  ELog::RegMethod RegA("ExtConstuctor","procType");

  const size_t NS(StrItem.size());

  if (NS>=1 && StrItem[0]=="simple" )
    {
      for(const MapSupport::Range<int>& RUnit : Zones)
	EX.addUnit(RUnit,"S");
    }


  return 1;
}

  
void
ExtConstructor::sortZone()
  /*!
    This is going to sort and concaternate the zones
  */
{
  ELog::RegMethod RegA("ExtConstructor","sortZone");
  
  std::sort(Zones.begin(),Zones.end());
  return;
}


void
ExtConstructor::processUnit(Simulation& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
/*!
    Add ext component 
    \param System :: Simulation to get physics/fixed points
    \param IParam :: Main input parameters
    \param Index :: index of the -wExt card
   */
{
  ELog::RegMethod RegA("ExtConstructor","processPoint");

  const size_t NParam=IParam.itemCnt("wExt",Index);
  if (NParam<2)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wExt");
  std::vector<std::string> StrItem;
  
  // Get all values:
  for(size_t j=0;j<NParam;j++)
    StrItem.push_back
      (IParam.getCompValue<std::string>("wExt",Index,j));

  if (StrItem[0]=="help" || StrItem[0]=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  
  if (!procZone(StrItem))
    throw ColErr::InvalidLine
      ("procZone ==> StrItems",				
       std::accumulate(StrItem.begin(),StrItem.end(),std::string("")),0);
  
  sortZone();
  ExtControl& EC=System.getPC().getExtCard();
  
  
  
  if (!procType(StrItem,EC))
    throw ColErr::InvalidLine
      ("procZone ==> StrItems",				
       std::accumulate(StrItem.begin(),StrItem.end(),std::string("")),0);

  return;
}

  

  
void
ExtConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{

    OX<<"-wExt ZONE : TYPE \n"
      "ZONE :: \n"
      "   All / object [objectName] / Cells {numbers}(s) / [cellNumber] \n"
      "TYPE :: \n"
      "   simple :: implicit capture \n"
      "   simpleVec [Vec3D] :: implicit capture along a direction\n"
      "   simpleVec Object linkPt :: implicit capture along a direction\n"
      "   scale :: [frac] implicit capture \n"
      "   scaleVec :: [frac] [Vec3D] :: implicit capture along a direction\n"
      "   scaleVec :: [frac] Object linkPt :: "
      " implicit capture along a direction\n";
  return;
}


}  // NAMESPACE physicsSystem
