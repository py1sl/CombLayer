/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/TriangleMod.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef moderatorSystem_TriangleMod_h
#define moderatorSystem_TriangleMod_h

class Simulation;

namespace Geometry
{
  class Convex2D;
}

namespace moderatorSystem
{

/*!
  \class TriangleMod
  \version 1.0
  \author S. Ansell
  \date January 2014
  \brief TriangleMod [insert object]
*/

class TriangleMod : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:
  
  const int triIndex;      ///< Index of surface offset
  int cellIndex;            ///< Cell index

  double xStep;           ///< Offset relative to origin 
  double yStep;           ///< Offset relative to origin 
  double zStep;           ///< Offset relative to origin 

  double xyAngle;         ///< Offset relative to origin 
  double zAngle;          ///< Offset relative to origin 

  TriUnit Outer;          ///< Outer points 

  size_t nIUnits;               ///< number of inner units
  std::vector<TriUnit> IUnits;  ///< Inner unit data

  double height;           ///< Height/depth
  double wallThick;        ///< Wall thickness
  double flatClearance;    ///< Tri-level clearance
  double topClearance;     ///< Top clearance
  double baseClearance;    ///< Base clearance

  double innerWall;        ///< Inner wall thickness
  int innerMat;            ///< Inner material

  double modTemp;          ///< Moderator temperature

  int modMat;              ///< Moderator material
  int wallMat;             ///< Wall material
  int pCladMat;            ///< Al poison support
  int poisonMat;           ///< Poison (Gadolinium)

  Geometry::Vec3D realPt(const Geometry::Vec3D&) const;
  Geometry::Vec3D realAxis(const Geometry::Vec3D&) const;
  Geometry::Vec3D sideNorm(const std::pair<Geometry::Vec3D,
			   Geometry::Vec3D>&) const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    cornerPair(const std::vector<Geometry::Vec3D>&,
	       const size_t,const size_t,const double) const;
  std::string getExcludeShape(const std::string&) const;

  std::string createInnerObject(Simulation&);
  void cutInnerObj(Simulation&,const TriUnit&);
  void cutOuterObj(Simulation&,const TriUnit&,const TriUnit&);
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createLinks();
  void createSurfaces();
  void createObjects(Simulation&);
  
  void createConvex();
  void createConvexUnit(std::vector<Geometry::Vec3D>&,
			std::vector<int>&) const;
  void calcOverlaps(Simulation& System);


 public:

  TriangleMod(const std::string&);
  TriangleMod(const TriangleMod&);
  TriangleMod& operator=(const TriangleMod&);
  /// Clone function
  virtual TriangleMod* clone() const { return new TriangleMod(*this); }
  virtual ~TriangleMod();

  // special for 5 sides!!
  virtual int getExitWindow(const size_t,std::vector<int>&) const;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
