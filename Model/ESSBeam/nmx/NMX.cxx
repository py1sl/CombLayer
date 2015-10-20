/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/nmx/NMX.cxx
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
#include <utility>
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
#include "debugMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "LineShield.h"

#include "NMX.h"

namespace essSystem
{

NMX::NMX() :
  nmxAxis(new attachSystem::FixedOffset("nmxAxis",4)),
  GuideA(new beamlineSystem::GuideLine("nmxGA")),
  VPipeA(new constructSystem::VacuumPipe("nmxPipeA")),
  VPipeB(new constructSystem::VacuumPipe("nmxPipeB")),
  BendA(new beamlineSystem::GuideLine("nmxBA")),
  BInsert(new BunkerInsert("nmxBInsert")),
  FocusWall(new beamlineSystem::GuideLine("nmxFWall")),
  ShieldA(new constructSystem::LineShield("nmxShieldA"))
  /*!
    Constructor
 */
{
  ELog::RegMethod RegA("NMX","NMX");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This necessary:
  OR.cell("nmxAxis");
  OR.addObject(nmxAxis);

  OR.addObject(GuideA);
  OR.addObject(VPipeA);
  OR.addObject(VPipeB);
  OR.addObject(BendA);
  OR.addObject(BInsert);
  OR.addObject(FocusWall);
}



NMX::~NMX()
  /*!
    Destructor
  */
{}

void
NMX::setBeamAxis(const FuncDataBase& Control,
		 const GuideItem& GItem,
		 const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Data base of info on variables
    \param GItem :: Guide Item to 
   */
{
  ELog::RegMethod RegA("NMX","setBeamAxis");

  nmxAxis->populate(Control);
  nmxAxis->createUnitVector(GItem);
  nmxAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  nmxAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  nmxAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  nmxAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  // BEAM needs to be rotated:
  nmxAxis->linkAngleRotate(3);
  nmxAxis->linkAngleRotate(4);

  if (reverseZ)
    nmxAxis->reverseZ();
  return;
}
  
void 
NMX::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("NMX","build");
  ELog::EM<<"\nBuilding NMX on : "<<GItem.getKeyName()<<ELog::endDiag;

  setBeamAxis(System.getDataBase(),GItem,1);

  GuideA->addInsertCell(GItem.getCells("Void"));
  GuideA->addEndCut(GItem.getKey("Beam"),-2);
  GuideA->createAll(System,*nmxAxis,-3,*nmxAxis,-3); // beam front reversed

  // PIPE out of monolith

  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->setFront(GItem.getKey("Beam"),2);
  VPipeA->setDivider(GItem.getKey("Beam"),2);  
  VPipeA->createAll(System,GuideA->getKey("Guide0"),2);


  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->setFront(*VPipeA,2);
  VPipeB->setBack(bunkerObj,1);
  VPipeB->createAll(System,*VPipeA,2);

  BendA->addInsertCell(VPipeA->getCells("Void"));
  BendA->addInsertCell(VPipeB->getCells("Void"));
  BendA->createAll(System,GuideA->getKey("Guide0"),2,
		   GuideA->getKey("Guide0"),2);

  
  // First collimator [In WALL]
  //  const attachSystem::FixedComp& GFC(BendA->getKey("Guide0"));
  const attachSystem::FixedComp& GFC(*VPipeB);
  const std::string BSector=
    bunkerObj.calcSegment(System,GFC.getSignedLinkPt(2),
			  GFC.getSignedLinkAxis(2));

  BInsert->setInsertCell(bunkerObj.getCells(BSector));
  BInsert->createAll(System,GFC,2,bunkerObj);

  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,-1,
			 BendA->getKey("Guide0"),2);

  // Section to 17m
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(bunkerObj,2);
  ShieldA->setDivider(bunkerObj,2);
  ShieldA->createAll(System,*BInsert,2);

  return;
}


}   // NAMESPACE essSystem
