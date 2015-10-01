/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BilbaoWheel.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "General.h"
#include "Plane.h"
#include "WheelBase.h"
#include "BilbaoWheel.h"


namespace essSystem
{

BilbaoWheel::BilbaoWheel(const std::string& Key) :
  WheelBase(Key)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BilbaoWheel::BilbaoWheel(const BilbaoWheel& A) : 
  WheelBase(A),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),targetHeight(A.targetHeight),
  coolantThickIn(A.coolantThickIn),coolantThickOut(A.coolantThickOut),
  caseThick(A.caseThick),voidThick(A.voidThick),
  innerRadius(A.innerRadius),caseRadius(A.caseRadius),
  voidRadius(A.voidRadius),aspectRatio(A.aspectRatio),
  nLayers(A.nLayers),radius(A.radius),
  matTYPE(A.matTYPE),shaftHeight(A.shaftHeight),nShaftLayers(A.nShaftLayers),
  wMat(A.wMat),heMat(A.heMat),steelMat(A.steelMat)
  /*!
    Copy constructor
    \param A :: BilbaoWheel to copy
  */
{}
  
BilbaoWheel&
BilbaoWheel::operator=(const BilbaoWheel& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      WheelBase::operator=(A);
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      targetHeight=A.targetHeight;
      coolantThickIn=A.coolantThickIn;
      coolantThickOut=A.coolantThickOut;
      caseThick=A.caseThick;
      voidThick=A.voidThick;
      innerRadius=A.innerRadius;
      caseRadius=A.caseRadius;
      voidRadius=A.voidRadius;
      aspectRatio=A.aspectRatio;
      nLayers=A.nLayers;
      radius=A.radius;
      matTYPE=A.matTYPE;
      shaftHeight=A.shaftHeight;
      nShaftLayers=A.nShaftLayers;
      wMat=A.wMat;
      heMat=A.heMat;
      steelMat=A.steelMat;
    }
  return *this;
}

BilbaoWheel::~BilbaoWheel()
  /*!
    Destructor
   */
{}

BilbaoWheel*
BilbaoWheel::clone() const
  /*!
    Virtual copy constructor
    \return new(this)
   */
{
  return new BilbaoWheel(*this);
}

void
BilbaoWheel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BilbaoWheel","populate");

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   
  double R;
  for(size_t i=0;i<nLayers;i++)
    {
      R=Control.EvalVar<double>(StrFunc::makeString(keyName+"Radius",i+1));   
      if (i && R<=radius.back())
	ELog::EM<<"Radius["<<i+1<<"] not ordered "
		<<R<<" "<<radius.back()<<ELog::endErr;

      radius.push_back(R);
      matTYPE.push_back(Control.EvalVar<int>
			(StrFunc::makeString(keyName+"MatTYPE",i+1)));   
    }

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  coolantRadiusIn=Control.EvalVar<double>(keyName+"CoolantRadiusIn");  
  coolantRadiusOut=Control.EvalVar<double>(keyName+"CoolantRadiusOut");  
  caseRadius=Control.EvalVar<double>(keyName+"CaseRadius");  
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  aspectRatio=Control.EvalVar<double>(keyName+"AspectRatio");

  targetHeight=Control.EvalVar<double>(keyName+"TargetHeight");
  voidTungstenThick=Control.EvalVar<double>(keyName+"VoidTungstenThick");
  coolantThickIn=Control.EvalVar<double>(keyName+"CoolantThickIn");  
  coolantThickOut=Control.EvalVar<double>(keyName+"CoolantThickOut");  
  caseThick=Control.EvalVar<double>(keyName+"CaseThick");  
  voidThick=Control.EvalVar<double>(keyName+"VoidThick");  

  nShaftLayers=Control.EvalVar<size_t>(keyName+"NShaftLayers"); 
  for (size_t i=0; i<nShaftLayers; i++)
    {
      R = Control.EvalVar<double>(StrFunc::makeString(keyName+"ShaftRadius",i+1));   
      if (i && R<=shaftRadius.back())
	ELog::EM<<"ShaftRadius["<<i+1<<"] not ordered "
		<<R<<" "<<shaftRadius.back()<<ELog::endErr;
      shaftRadius.push_back(R);
      shaftMat.push_back(ModelSupport::EvalMat<int>(Control, 
						    StrFunc::makeString(keyName+"ShaftMat",i+1)));
    }


  shaftHeight=Control.EvalVar<double>(keyName+"ShaftHeight");

  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");  
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat");  
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");  
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");  

  return;
}

void
BilbaoWheel::makeShaftSurfaces()
  /*!
    Construct surfaces for the inner shaft
  */
{
  ELog::RegMethod RegA("BilbaoWheel","makeShaftSurfaces");
  
  ModelSupport::buildPlane(SMap,wheelIndex+2006,Origin+Z*shaftHeight,Z);  

  int SI(wheelIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      ModelSupport::buildCylinder(SMap,SI+2007,Origin,Z,shaftRadius[i]);
      SI+=10;
    }

  return;
}

void
BilbaoWheel::makeShaftObjects(Simulation& System)
  /*!
    Construct the objects
    \param System :: Simulation object
   */
{
  ELog::RegMethod RegA("BilbaoWheel","makeShaftObjects");
  std::string Out;

  // Main body [disk]
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-7 5 -6");	  
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,mainTemp,Out));
  // Coolant
  Out=ModelSupport::getComposite(SMap,wheelIndex," -1027 15 -16 (-5 : 6 2007)" );	// below W and inside innerRad
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mainTemp,Out));

  // steel
  Out=ModelSupport::getComposite(SMap,wheelIndex," -1027 25 -26 (-15 : 16 2017)" );  // inner shroud above/below
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // void
  Out=ModelSupport::getComposite(SMap,wheelIndex, " -1027 35 -36 (-25 : 26 2027)" );	
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // shaft
  int SI(wheelIndex);
  for (size_t i=0; i<nShaftLayers; i++)
    {
      if (i==0)
	{
	  Out=ModelSupport::getComposite(SMap,SI,wheelIndex," -2007 6M -2006M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat[i],mainTemp,Out));
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,SI,SI-10, " -2007 2007M 6 ");
	  Out += ModelSupport::getComposite(wheelIndex, " -2006 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat[i],mainTemp,Out));
	}
      if (i==nShaftLayers-1)
	{
	  Out=ModelSupport::getComposite(SMap,SI,wheelIndex," -2007 36M -2006M ");
	  addOuterSurf("Shaft",Out);  
	}
      SI += 10;
    }
  
  return;
}

std::string
BilbaoWheel::getSQSurface(const double R, const double e)
{
  /*
    Return MCNP(X) surface card for SQ ellipsoids
  */
  std::string surf = "sq " + StrFunc::makeString(1./pow(R,2)) + " " +
    StrFunc::makeString(1./pow(R,2)) + " " +
    StrFunc::makeString(e) + " 0 0 0 -1 " +
    StrFunc::makeString(Origin[1]) + " " + // Y SA: why X and Y are swapped? 
    StrFunc::makeString(Origin[0]) + " " + // X 
    StrFunc::makeString(Origin[2]);        // Z

  return surf;
}

void
BilbaoWheel::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
BilbaoWheel::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createSurfaces");

  // Dividing surface
  ModelSupport::buildPlane(SMap,wheelIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,wheelIndex+2,Origin,X);

  double H(targetHeight/2.0);
  ModelSupport::buildPlane(SMap,wheelIndex+5,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+6,Origin+Z*H,Z);  

  H+= voidTungstenThick;
  ModelSupport::buildPlane(SMap,wheelIndex+45,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+46,Origin+Z*H,Z);  

  H+=coolantThickIn;
  ModelSupport::buildPlane(SMap,wheelIndex+15,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+16,Origin+Z*H,Z);  

  H+=caseThick;
  ModelSupport::buildPlane(SMap,wheelIndex+25,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+26,Origin+Z*H,Z);  

  H+=voidThick;
  ModelSupport::buildPlane(SMap,wheelIndex+35,Origin-Z*H,Z);  
  ModelSupport::buildPlane(SMap,wheelIndex+36,Origin+Z*H,Z);  

  ModelSupport::buildCylinder(SMap,wheelIndex+7,Origin,Z,innerRadius);

  // step to outer radius: 45/46
  ModelSupport::buildCylinder(SMap,wheelIndex+1027,Origin,
			      Z,coolantRadiusIn+caseThick);


  H  = targetHeight/2.0;
  H += coolantThickOut;
  ModelSupport::buildPlane(SMap,wheelIndex+115,Origin-Z*H, Z); // 12 : 13 
  ModelSupport::buildPlane(SMap,wheelIndex+116,Origin+Z*H, Z);
  
  H += caseThick;
  ModelSupport::buildPlane(SMap,wheelIndex+125,Origin-Z*H,Z);  // 22 : 23 
  ModelSupport::buildPlane(SMap,wheelIndex+126,Origin+Z*H,Z);
  
  
  int SI(wheelIndex+10);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);  
      SI+=10;
    }
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  Geometry::General *GA;

  GA = SurI.createUniqSurf<Geometry::General>(wheelIndex+517);
  GA->setSurface(getSQSurface(coolantRadiusOut, aspectRatio));
  SMap.registerSurf(GA);

  GA = SurI.createUniqSurf<Geometry::General>(wheelIndex+527);
  GA->setSurface(getSQSurface(caseRadius, aspectRatio));
  SMap.registerSurf(GA);

  ModelSupport::buildCylinder(SMap,wheelIndex+537,Origin,Z,voidRadius);  

  return; 
}

void
BilbaoWheel::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createObjects");

  // Inner Radius is 7
  
  const int matNum[4]={0,steelMat,heMat,wMat};
  std::string Out;
  // 
  // Loop through each item and build inner section
  // 

  int SI(wheelIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      //      if (matTYPE[i]!=1)
	Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -17M 5 -6 ");
	//      else
	//	Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -17M 15 -16 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,
				       matNum[matTYPE[i]],mainTemp,Out));  
      SI+=10;
    }
  // Now make sections for the coolant
  int frontIndex(wheelIndex);
  int backIndex(wheelIndex);
  const std::string TopBase=
    ModelSupport::getComposite(SMap,wheelIndex," 15 -16 (-5:6) ");
  
  for(size_t i=0;i<nLayers;i++)
    {
      if (matTYPE[i]==1)
	{
	  if (i)  // otherwize this space has zero size
	    {
	      Out=ModelSupport::getComposite(SMap,frontIndex,backIndex,
					     " 7 -7M ");
	      //    System.addCell(MonteCarlo::Qhull(cellIndex++,heMat*0, mainTemp,Out+TopBase));
	    }
	  ELog::EM<<"++ Index = "<<wheelIndex<<ELog::endDiag;
	  frontIndex=backIndex+10;
	}
      backIndex+=10;
    }
  // Final coolant section [ UNACCEPTABLE JUNK CELL]
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 6 -116 -7M 1027 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mainTemp,Out)); // cooling above W

  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," -5 115 -7M 1027 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,mainTemp,Out)); // cooling below W

  
  // Back coolant:
  Out=ModelSupport::getComposite(SMap,wheelIndex,SI," 7M -517 115 -116");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,mainTemp,Out));

  // Metal surround [ UNACCEPTABLE JUNK CELL]
  // Metal front:
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 517 115 -116");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  // forward Main sections:
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 1027 -16 116");	 // outer above W
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out));

  Out=ModelSupport::getComposite(SMap,wheelIndex,"-527 1027 15 -115");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,mainTemp,Out)); // outer below W

  // Void surround
  Out=ModelSupport::getComposite(SMap,wheelIndex,
				 "1027 35 -36 -537 (-125:126:527)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,wheelIndex,"-537 35 -36");	
  addOuterSurf("Wheel",Out);

  return; 
}

void
BilbaoWheel::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("BilbaoWheel","createLinks");
  // set Links :: Inner links:

  FixedComp::setConnect(0,Origin-Y*innerRadius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(wheelIndex+537));
  FixedComp::addLinkSurf(0,-SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(1,Origin+Y*innerRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(wheelIndex+537));
  FixedComp::addLinkSurf(1,SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(2,Origin-Y*voidRadius,-Y);
  FixedComp::setLinkSurf(2,SMap.realSurf(wheelIndex+1037));
  FixedComp::addLinkSurf(2,-SMap.realSurf(wheelIndex+1));

  FixedComp::setConnect(3,Origin+Y*voidRadius,Y);
  FixedComp::setLinkSurf(3,SMap.realSurf(wheelIndex+1037));
  FixedComp::addLinkSurf(3,SMap.realSurf(wheelIndex+1));

  const double H=(targetHeight/2.0)+coolantThickIn+caseThick+voidThick;
  FixedComp::setConnect(4,Origin-Z*H,-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(wheelIndex+35));

  FixedComp::setConnect(5,Origin+Z*H,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(wheelIndex+36));

  
  return;
}

void
BilbaoWheel::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
   */
{
  ELog::RegMethod RegA("BilbaoWheel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);

  makeShaftSurfaces();
  makeShaftObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem