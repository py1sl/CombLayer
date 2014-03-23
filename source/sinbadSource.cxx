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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "DSTerm.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "WorkData.h"
#include "sinbadSource.h"

using namespace std;
namespace SDef

{

sinbadSource::sinbadSource() : 
  cutEnergy(0.0),weight(1.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

sinbadSource::sinbadSource(const sinbadSource& A) : 
  cutEnergy(A.cutEnergy),weight(A.weight),
  CentPoint(A.CentPoint),Direction(A.Direction),
  angleSpread(A.angleSpread),radialSpread(A.radialSpread),
  Energy(A.Energy),EWeight(A.EWeight)
  /*!
    Copy constructor
    \param A :: sinbadSource to copy
  */
{}

sinbadSource&
sinbadSource::operator=(const sinbadSource& A)
  /*!
    Assignment operator
    \param A :: sinbadSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cutEnergy=A.cutEnergy;
      weight=A.weight;
      CentPoint=A.CentPoint;
      Direction=A.Direction;
      angleSpread=A.angleSpread;
      radialSpread=A.radialSpread;
      Energy=A.Energy;
      EWeight=A.EWeight;
    }
  return *this;
}

sinbadSource::~sinbadSource() 
  /*!
    Destructor
  */
{}

void
sinbadSource::createAll(SDef::Source& sourceCard) 
  /*!
    Generic function to create everything
    \param sourceCard :: Card to update
  */
{
  ELog::RegMethod RegA("sinbadSource","createAll");
  
  createSourceSinbad(sourceCard);
  return;
}
  
void
sinbadSource::createSourceSinbad(SDef::Source& sourceCard)
  /*!
    Creates a target 2 neutron source 
    for chipIR
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("sinbadSource","createSource");
 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     
  double sinbadSource49 [17] [16] = {
 { -49.75, -46.58, -43.32, -37.08, -33.92, -27.58, -11.75, -2.25,  7.25, 16.75, 32.58, 38.92, 42.08, 48.42, 51.58, 54.75 },
 {  51.44,    0  ,    0  ,    0  ,    0  ,    0  ,    0  , 2.935, 2.967, 2.861,   0  ,   0  ,   0  ,   0  ,   0  ,   0   },
 {  47.63,    0  ,    0  ,    0  ,    0  ,    0  ,  3.222, 3.529, 3.567, 3.446, 3.023,   0  ,   0  ,   0  ,   0  ,   0	 },
 {  40.64,    0  ,    0  ,    0  ,    0  ,  3.228,  3.782, 4.152, 4.208, 4.082, 3.615, 2.959,   0  ,   0  ,   0  ,   0	 },
 {  35.56,    0  ,    0  ,    0  ,  3.234,  3.566,  4.164, 4.575, 4.648, 4.523, 4.025, 3.317, 2.915,   0  ,   0  ,   0	 },
 {  31.75,    0  ,    0  ,  3.350,  3.745,  4.099,  4.750, 5.220, 5.325, 5.205, 4.658, 3.867, 3.423, 2.916,   0  ,   0	 },
 {  19.69,    0  ,  3.300,  3.756,  4.168,  4.538,  5.226, 5.743, 5.874, 5.760, 5.174, 4.315, 3.838, 3.305, 2.738,   0	 },
 {  15.88,  3.172,  3.522,  3.990,  4.412,  4.790,  5.499, 6.042, 6.186, 6.076, 5.466, 4.569, 4.073, 3.526, 2.951, 2.558 },
 {   5.29,  3.281,  3.641,  4.119,  4.546,  4.928,  5.645, 6.199, 6.350, 6.239, 5.613, 4.690, 4.184, 3.631, 3.056, 2.670 },
 {  -5.29,  3.145,  3.500,  3.967,  4.382,  4.750,  5.440, 5.972, 6.117, 6.003, 5.379, 4.466, 3.970, 3.433, 2.883, 2.519 },
 { -15.88,    0  ,  3.275,  3.721,  4.114,  4.461,  5.109, 5.611, 5.746, 5.632, 5.021, 4.132, 3.654, 3.142, 2.625,   0	 },
 { -19.69,    0  ,    0  ,  3.294,  3.645,  3.954,  4.535, 4.990, 5.113, 5.003, 4.421, 3.583, 3.139, 2.674,   0  ,   0	 },
 { -31.75,    0  ,    0  ,    0  ,  3.052,  3.317,  3.824, 4.229, 4.340, 4.236, 3.699, 2.937, 2.541,   0  ,   0  ,   0	 },
 { -35.56,    0  ,    0  ,    0  ,    0  ,  2.888,  3.359, 3.739, 3.841, 3.742, 3.244, 2.545,   0  ,   0  ,   0  ,   0	 },
 { -40.64,    0  ,    0  ,    0  ,    0  ,    0  ,  2.681, 3.036, 3.124, 3.032, 2.607,   0  ,   0  ,   0  ,   0  ,   0	 },
 { -47.63,    0  ,    0  ,    0  ,    0  ,    0  ,    0  , 2.399, 2.470, 2.382,   0  ,   0  ,   0  ,   0  ,   0  ,   0	 },
 { -51.44,    0  ,    0  ,    0  ,    0  ,    0  ,    0  , 0    , 0    , 0    ,   0  ,   0  ,   0  ,   0  ,   0  ,   0   } 
 } ; 

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


  double sinbadSource75[13][6]={            
    { -20.10,    -12.10,     -4.10,      4.10,     12.10,     20.10 },
    {  31.75, 4.723E+07, 4.670E+07, 4.778E+07, 4.756E+07, 4.874E+07 },           
    {  26.32, 4.802E+07, 4.642E+07, 4.684E+07, 4.699E+07, 4.964E+07 },
    {  20.58, 5.006E+07, 4.764E+07, 4.806E+07, 4.855E+07, 5.144E+07 },
    {  15.44, 5.231E+07, 4.927E+07, 4.996E+07, 5.069E+07, 5.351E+07 },
    {  10.00, 5.426E+07, 5.078E+07, 5.179E+07, 5.264E+07, 5.531E+07 },
    {   3.33, 5.500E+07, 5.135E+07, 5.252E+07, 5.332E+07, 5.601E+07 },
    {  -3.33, 5.393E+07, 5.055E+07, 5.160E+07, 5.217E+07, 5.505E+07 },
    { -10.00, 5.157E+07, 4.885E+07, 4.690E+07, 4.989E+07, 5.293E+07 },
    { -15.44, 4.885E+07, 4.696E+07, 4.744E+07, 4.747E+07, 5.040E+07 },
    { -20.58, 4.609E+07, 4.518E+07, 4.557E+07, 4.535E+07, 4.772E+07 },
    { -26.32, 4.424E+07, 4.418E+07, 4.495E+07, 4.448E+07, 4.560E+07 },
    { -31.75 , 0.0     , 0.0      , 0.0      , 0.0      , 0.0       },
};



 double sinbadSource35[17][16]={            
    {-52.25, -49.08, -45.92,  -39.58, -36.42,  -30.08,  -14.25,  -4.75, 4.75,  14.25,  30.08,   36.42,  39.58,   45.92,   49.08,  52.25 },
    { 51.44,  0  ,   0  ,   0  ,   0  ,   0  ,   0  , 2.973, 2.994, 2.865,   0  ,   0  ,   0  ,   0  ,   0  ,   0   },
    { 47.63,  0  ,   0  ,   0  ,   0  ,   0  , 3.173, 3.496, 3.537, 3.404, 2.975,   0  ,   0  ,   0  ,   0  ,   0   },
    { 40.64,  0  ,   0  ,   0  ,   0  , 3.176, 3.711, 4.088, 4.149, 4.015, 3.529, 2.890,   0  ,   0  ,   0  ,   0   },
    { 35.56,  0  ,   0  ,   0  , 3.186, 3.500, 4.092, 4.514, 4.587, 4.446, 3.925, 3.240, 2.886,   0  ,   0  ,   0   },
    { 31.75,  0  ,   0  , 3.249, 3.635, 3.999, 4.696, 5.197, 5.286, 5.130, 4.555, 3.807, 3.422, 3.009,   0  ,   0   },
    { 19.69,  0  , 3.150, 3.575, 4.003, 4.412, 5.202, 5.770, 5.870, 5.699, 5.080, 4.282, 3.873, 3.431, 2.970,   0   },
    { 15.88,  3.040, 3.322, 3.769, 4.223, 4.660, 5.505, 6.110, 6.215, 6.034, 5.387, 4.559, 4.133, 3.673, 3.188, 2.850 },
    { 5.29,  3.132, 3.427, 3.893, 4.364, 4.815, 5.684, 6.304, 6.412, 6.227, 5.563, 4.710, 4.270, 3.792, 3.287, 2.932 },
    {-5.29, 3.026, 3.325, 3.784, 4.239, 4.669, 5.491, 6.079, 6.183, 6.009, 5.364, 4.523, 4.084, 3.607, 3.103, 2.751 },
    {-15.88,  0  , 3.136, 3.574, 4.001, 4.400, 5.157, 5.701, 5.802, 5.643, 5.031, 4.221, 3.795, 3.331, 2.843,   0   },
    {-19.69,  0  ,   0  , 3.182, 3.562, 3.912, 4.572, 5.053, 5.149, 5.013, 4.461, 3.713, 3.317, 2.887,   0  ,   0   },
    {-31.75,  0  ,   0  ,   0  , 3.004, 3.299, 3.855, 4.267, 4.357, 4.247, 3.766, 3.101, 2.748,   0  ,   0  ,   0   },
    {-35.56,  0  ,   0  ,   0  ,   0  , 2.896, 3.396, 3.770, 3.855, 3.758, 3.320, 2.712,   0  ,   0  ,   0  ,   0   },
    {-40.64,  0  ,   0  ,   0  ,   0  ,   0  , 2.745, 3.072, 3.150, 3.067, 2.685,   0  ,   0  ,   0  ,   0  ,   0   },
    {-47.63,  0  ,   0  ,   0  ,   0  ,   0  ,   0  , 2.458, 2.525, 2.448,   0  ,   0  ,   0  ,   0  ,   0  ,   0   },
    {-51.44,  0  ,   0  ,   0  ,   0  ,   0  ,   0  , 0    ,   0  , 0    ,   0  ,   0  ,   0  ,   0  ,   0  ,   0   },
   };


  double sinbadSource36[13][6]={            
    { -20.10,    -12.10,     -4.10,      4.10,     12.10,     20.10 },
    {  31.75, 4.884E+07, 4.734E+07,  4.705E+07, 4.695E+07, 5.055E+07 },           
    {  26.32, 4.849E+07, 4.533E+07,  4.505E+07, 4.534E+07, 5.031E+07 },
    {  20.58, 4.987E+07, 4.573E+07,  4.554E+07,  4.606E+07,  5.166E+07 },
    {  15.44, 5.178E+07, 4.713E+07,  4.698E+07,  4.763E+07,  5.351E+07 },
    {  10.00, 5.363E+07, 4.871E+07,  4.853E+07,  4.930E+07,  5.533E+07 },
    {   3.33, 5.459E+07, 4.957E+07,  4.931E+07,  5.015E+07,  5.629E+07 },
    {  -3.33, 5.418E+07, 4.914E+07,  4.883E+07,  4.973E+07,  5.589E+07 },
    { -10.00, 5.288E+07, 4.794E+07,  4.769E+07,  4.860E+07,  5.457E+07 },
    { -15.44, 5.157E+07, 4.689E+07,  4.678E+07,  4.765E+07,  5.324E+07 },
    { -20.58, 5.058E+07, 4.666E+07,  4.671E+07,  4.744E+07,  5.227E+07 },
    { -26.32, 5.092E+07, 4.870E+07,  4.873E+07,  4.920E+07,  5.280E+07 },
    { -31.75, 0.0      , 0.0      , 0.0      , 0.0      , 0.0       },
};





  sourceCard.setActive();
 


  double Xerg7[]=  { 0.000000E-6 ,   1.000000E-5 ,  1.100000E-5 , 1.200000E-5,  1.300000E-5 ,   1.400000E-5 , 1.500000E-5  ,      1.600000E-5 ,    1.700000E-5 ,   1.800000E-5  , 1.900000E-5 , 2.000000E-5 , 2.100000E-5 ,   2.200000E-5,  2.300000E-5  ,     2.400000E-5 ,   2.500000E-5 ,   2.600000E-5 ,  2.700000E-5 , 2.800000E-5 , 2.900000E-5,    3.000000E-5 , 3.100000E-5    ,   3.200000E-5   , 3.300000E-5   , 3.400000E-5 ,  3.500000E-5 , 3.600000E-5,  3.700000E-5 ,   3.800000E-5 ,  3.900000E-5,      4.000000E-5 ,   4.100000E-5 ,   4.200000E-5 , 4.300000E-5 ,  4.400000E-5 , 4.500000E-5 , 4.600000E-5 ,   4.700000E-5,
         4.800000E-5,  4.900000E-5 ,   5.000000E-5 , 
         5.100000E-5,  5.200000E-5 ,   5.300000E-5 , 
         5.400000E-5,  5.500000E-5 ,   5.600000E-5 , 
         5.700000E-5,  5.800000E-5 ,   5.900000E-5 , 
         6.000000E-5,  6.100000E-5 ,   6.200000E-5 ,   
         6.300000E-5,  6.400000E-5 ,   6.500000E-5 ,
         6.600000E-5,  6.700000E-5 ,   6.800000E-5 , 
         6.900000E-5,  7.000000E-5 ,   7.100000E-5 , 
         7.200000E-5,  7.300000E-5 ,   7.400000E-5 , 
         7.500000E-5,  7.600000E-5 ,   7.700000E-5 , 
         7.800000E-5,  7.900000E-5 ,   8.000000E-5 , 
         8.100000E-5,  8.200000E-5 ,   8.300000E-5 , 
         8.400000E-5,  8.500000E-5 ,   8.600000E-5 , 
         8.700000E-5,  8.800000E-5 ,   8.900000E-5 , 
         9.000000E-5,  9.100000E-5 ,   9.200000E-5 , 
         9.300000E-5,  9.400000E-5 ,   9.500000E-5 , 
         9.600000E-5,  9.700000E-5 ,   9.800000E-5 , 
         9.900000E-5,  1.000000E-4 ,   1.100000E-4 , 
         1.200000E-4,  1.300000E-4 ,   1.400000E-4 , 
         1.500000E-4,  1.600000E-4 ,   1.700000E-4 , 
         1.800000E-4,  1.900000E-4 ,   2.000000E-4 , 
         2.100000E-4,  2.200000E-4 ,   2.300000E-4 , 
         2.400000E-4,  2.500000E-4 ,   2.600000E-4 , 
         2.700000E-4,  2.800000E-4 ,   2.900000E-4 , 
         3.000000E-4,  3.100000E-4 ,   3.200000E-4 , 
         3.300000E-4,  3.400000E-4 ,   3.500000E-4 , 
         3.600000E-4,  3.700000E-4 ,   3.800000E-4 , 
         3.900000E-4,  4.000000E-4 ,   4.100000E-4 , 
         4.200000E-4,  4.300000E-4 ,   4.400000E-4 , 
         4.500000E-4,  4.600000E-4 ,   4.700000E-4 , 
         4.800000E-4,  4.900000E-4 ,   5.000000E-4 , 
         5.100000E-4,  5.200000E-4 ,   5.300000E-4 , 
         5.400000E-4,  5.500000E-4 ,   5.600000E-4 , 
         5.700000E-4,  5.800000E-4 ,   5.900000E-4 , 
         6.000000E-4,  6.100000E-4 ,   6.200000E-4 , 
         6.300000E-4,  6.400000E-4 ,   6.500000E-4 , 
         6.600000E-4,  6.700000E-4 ,   6.800000E-4 , 
         6.900000E-4,  7.000000E-4 ,   7.100000E-4 , 
         7.200000E-4,  7.300000E-4 ,   7.400000E-4 , 
         7.500000E-4,  7.600000E-4 ,   7.700000E-4 , 
         7.800000E-4,  7.900000E-4 ,   8.000000E-4 , 
         8.100000E-4,  8.200000E-4 ,   8.300000E-4 , 
         8.400000E-4,  8.500000E-4 ,   8.600000E-4 , 
         8.700000E-4,  8.800000E-4 ,   8.900000E-4 , 
         9.000000E-4,  9.100000E-4 ,   9.200000E-4 , 
         9.300000E-4,  9.400000E-4 ,   9.500000E-4 , 
         9.600000E-4,  9.700000E-4 ,   9.800000E-4 , 
         9.900000E-4,  1.000000E-3 ,   1.100000E-3 , 
         1.200000E-3,  1.300000E-3 ,   1.400000E-3 , 
         1.500000E-3,  1.600000E-3 ,   1.700000E-3 , 
         1.800000E-3,  1.900000E-3 ,   2.000000E-3 , 
         2.100000E-3,  2.200000E-3 ,   2.300000E-3 , 
         2.400000E-3,  2.500000E-3 ,   2.600000E-3 , 
         2.700000E-3,  2.800000E-3 ,   2.900000E-3 , 
         3.000000E-3,  3.100000E-3 ,   3.200000E-3 , 
         3.300000E-3,  3.400000E-3 ,   3.500000E-3 , 
         3.600000E-3,  3.700000E-3 ,   3.800000E-3 , 
         3.900000E-3,  4.000000E-3 ,   4.100000E-3 , 
         4.200000E-3,  4.300000E-3 ,   4.400000E-3 , 
         4.500000E-3,  4.600000E-3 ,   4.700000E-3 , 
         4.800000E-3,  4.900000E-3 ,   5.000000E-3 , 
         5.100000E-3,  5.200000E-3 ,   5.300000E-3 , 
         5.400000E-3,  5.500000E-3 ,   5.600000E-3 , 
         5.700000E-3,  5.800000E-3 ,   5.900000E-3 , 
         6.000000E-3,  6.100000E-3 ,   6.200000E-3 , 
         6.300000E-3,  6.400000E-3 ,   6.500000E-3 , 
         6.600000E-3,  6.700000E-3 ,   6.800000E-3 , 
         6.900000E-3,  7.000000E-3 ,   7.100000E-3 , 
         7.200000E-3,  7.300000E-3 ,   7.400000E-3 , 
         7.500000E-3,  7.600000E-3 ,   7.700000E-3 , 
         7.800000E-3,  7.900000E-3 ,   8.000000E-3 , 
         8.100000E-3,  8.200000E-3 ,   8.300000E-3 , 
         8.400000E-3,  8.500000E-3 ,   8.600000E-3 , 
         8.700000E-3,  8.800000E-3 ,   8.900000E-3 , 
         9.000000E-3,  9.100000E-3 ,   9.200000E-3 , 
         9.300000E-3,  9.400000E-3 ,   9.500000E-3 , 
         9.600000E-3,  9.700000E-3 ,   9.800000E-3 , 
         9.900000E-3,  1.000000E-2 ,   1.100000E-2 , 
         1.200000E-2,  1.300000E-2 ,   1.400000E-2 , 
         1.500000E-2,  1.600000E-2 ,   1.700000E-2 , 
         1.800000E-2,  1.900000E-2 ,   2.000000E-2 , 
         2.100000E-2,  2.200000E-2 ,   2.300000E-2 , 
         2.400000E-2,  2.500000E-2 ,   2.600000E-2 , 
         2.700000E-2,  2.800000E-2 ,   2.900000E-2 , 
         3.000000E-2,  3.100000E-2 ,   3.200000E-2 , 
         3.300000E-2,  3.400000E-2 ,   3.500000E-2 , 
         3.600000E-2,  3.700000E-2 ,   3.800000E-2 , 
         3.900000E-2,  4.000000E-2 ,   4.100000E-2 , 
         4.200000E-2,  4.300000E-2 ,   4.400000E-2 , 
         4.500000E-2,  4.600000E-2 ,   4.700000E-2 , 
         4.800000E-2,  4.900000E-2 ,   5.000000E-2 , 
         5.100000E-2,  5.200000E-2 ,   5.300000E-2 , 
         5.400000E-2,  5.500000E-2 ,   5.600000E-2 , 
         5.700000E-2,  5.800000E-2 ,   5.900000E-2 , 
         6.000000E-2,  6.100000E-2 ,   6.200000E-2 , 
         6.300000E-2,  6.400000E-2 ,   6.500000E-2 , 
         6.600000E-2,  6.700000E-2 ,   6.800000E-2 , 
         6.900000E-2,  7.000000E-2 ,   7.100000E-2 , 
         7.200000E-2,  7.300000E-2 ,   7.400000E-2 , 
         7.500000E-2,  7.600000E-2 ,   7.700000E-2 , 
         7.800000E-2,  7.900000E-2 ,   8.000000E-2 , 
         8.100000E-2,  8.200000E-2 ,   8.300000E-2 , 
         8.400000E-2,  8.500000E-2 ,   8.600000E-2 , 
         8.700000E-2,  8.800000E-2 ,   8.900000E-2 , 
         9.000000E-2,  9.100000E-2 ,   9.200000E-2 , 
         9.300000E-2,  9.400000E-2 ,   9.500000E-2 , 
         9.600000E-2,  9.700000E-2 ,   9.800000E-2 , 
         9.900000E-2,  1.000000E-1 ,   1.100000E-1 , 
         1.200000E-1,  1.300000E-1 ,   1.400000E-1 , 
         1.500000E-1,  1.600000E-1 ,   1.700000E-1 , 
         1.800000E-1,  1.900000E-1 ,   2.000000E-1 , 
         2.100000E-1,  2.200000E-1 ,   2.300000E-1 , 
         2.400000E-1,  2.500000E-1 ,   2.600000E-1 , 
         2.700000E-1,  2.800000E-1 ,   2.900000E-1 , 
         3.000000E-1,  3.100000E-1 ,   3.200000E-1 , 
         3.300000E-1,  3.400000E-1 ,   3.500000E-1 , 
         3.600000E-1,  3.700000E-1 ,   3.800000E-1 , 
         3.900000E-1,  4.000000E-1 ,   4.100000E-1 , 
         4.200000E-1,  4.300000E-1 ,   4.400000E-1 , 
         4.500000E-1,  4.600000E-1 ,   4.700000E-1 , 
         4.800000E-1,  4.900000E-1 ,   5.000000E-1 , 
         5.100000E-1,  5.200000E-1 ,   5.300000E-1 , 
         5.400000E-1,  5.500000E-1 ,   5.600000E-1 , 
         5.700000E-1,  5.800000E-1 ,   5.900000E-1 , 
         6.000000E-1,  6.100000E-1 ,   6.200000E-1 , 
         6.300000E-1,  6.400000E-1 ,   6.500000E-1 , 
         6.600000E-1,  6.700000E-1 ,   6.800000E-1 , 
         6.900000E-1,  7.000000E-1 ,   7.100000E-1 , 
         7.200000E-1,  7.300000E-1 ,   7.400000E-1 , 
         7.500000E-1,  7.600000E-1 ,   7.700000E-1 , 
         7.800000E-1,  7.900000E-1 ,   8.000000E-1 , 
         8.100000E-1,  8.200000E-1 ,   8.300000E-1 , 
         8.400000E-1,  8.500000E-1 ,   8.600000E-1 , 
         8.700000E-1,  8.800000E-1 ,   8.900000E-1 , 
         9.000000E-1,  9.100000E-1 ,   9.200000E-1 , 
         9.300000E-1,  9.400000E-1 ,   9.500000E-1 , 
         9.600000E-1,  9.700000E-1 ,   9.800000E-1 , 
         9.900000E-1,  1.000000E+0 ,   1.100000E+0 , 
         1.200000E+0,  1.300000E+0 ,   1.400000E+0 , 
         1.500000E+0,  1.600000E+0 ,   1.700000E+0 , 
         1.800000E+0,  1.900000E+0 ,   2.000000E+0 , 
         2.100000E+0,  2.200000E+0 ,   2.300000E+0 , 
         2.400000E+0,  2.500000E+0 ,   2.600000E+0 , 
         2.700000E+0,  2.800000E+0 ,   2.900000E+0 , 
         3.000000E+0,  3.100000E+0 ,   3.200000E+0 , 
         3.300000E+0,  3.400000E+0 ,   3.500000E+0 , 
         3.600000E+0,  3.700000E+0 ,   3.800000E+0 , 
         3.900000E+0,  4.000000E+0 ,   4.100000E+0 , 
         4.200000E+0,  4.300000E+0 ,   4.400000E+0 , 
         4.500000E+0,  4.600000E+0 ,   4.700000E+0 , 
         4.800000E+0,  4.900000E+0 ,   5.000000E+0 , 
         5.100000E+0,  5.200000E+0 ,   5.300000E+0 , 
         5.400000E+0,  5.500000E+0 ,   5.600000E+0 , 
         5.700000E+0,  5.800000E+0 ,   5.900000E+0 , 
         6.000000E+0,  6.100000E+0 ,   6.200000E+0 , 
         6.300000E+0,  6.400000E+0 ,   6.500000E+0 , 
         6.600000E+0,  6.700000E+0 ,   6.800000E+0 , 
         6.900000E+0,  7.000000E+0 ,   7.100000E+0 , 
         7.200000E+0,  7.300000E+0 ,   7.400000E+0 , 
         7.500000E+0,  7.600000E+0 ,   7.700000E+0 , 
         7.800000E+0,  7.900000E+0 ,   8.000000E+0 , 
         8.100000E+0,  8.200000E+0 ,   8.300000E+0 , 
         8.400000E+0,  8.500000E+0 ,   8.600000E+0 , 
         8.700000E+0,  8.800000E+0 ,   8.900000E+0 , 
         9.000000E+0,  9.100000E+0 ,   9.200000E+0 , 
         9.300000E+0,  9.400000E+0 ,   9.500000E+0 , 
         9.600000E+0,  9.700000E+0 ,   9.800000E+0 , 
         9.900000E+0,  1.000000E+1 ,   1.020000E+1 , 
         1.040000E+1,  1.060000E+1 ,   1.080000E+1 , 
         1.100000E+1,  1.120000E+1 ,   1.140000E+1 , 
         1.160000E+1,  1.180000E+1 ,   1.200000E+1 , 
         1.220000E+1,  1.240000E+1 ,   1.260000E+1 , 
         1.280000E+1,  1.300000E+1 ,   1.320000E+1 , 
         1.340000E+1,  1.360000E+1 ,   1.380000E+1 , 
         1.400000E+1,  1.420000E+1 ,   1.440000E+1 , 
         1.460000E+1,  1.480000E+1 ,   1.500000E+1 , 
         1.520000E+1,  1.540000E+1 ,   1.560000E+1 , 
         1.580000E+1,  1.600000E+1 ,   1.620000E+1 , 
         1.640000E+1,  1.660000E+1 ,   1.680000E+1 , 
         1.700000E+1,  1.720000E+1 ,   1.740000E+1 , 
         1.760000E+1,  1.780000E+1 ,   1.800000E+1 , 
         1.820000E+1,  1.840000E+1 ,   1.860000E+1 , 
         1.880000E+1,  1.900000E+1 ,   1.920000E+1 , 
         1.940000E+1,  1.960000E+1 ,   1.980000E+1 , 
         2.000000E+1,  2.020000E+1 ,   2.040000E+1 , 
         2.060000E+1,  2.080000E+1 ,   2.100000E+1 , 
         2.120000E+1,  2.140000E+1 ,   2.160000E+1 , 
         2.180000E+1,  2.200000E+1 ,   2.220000E+1 , 
         2.240000E+1,  2.260000E+1 ,   2.280000E+1 , 
         2.300000E+1,  2.320000E+1 ,   2.340000E+1 , 
         2.360000E+1,  2.380000E+1 ,   2.400000E+1 , 
         2.420000E+1,  2.440000E+1 ,   2.460000E+1 , 
         2.480000E+1,  2.500000E+1 ,   2.520000E+1 , 
         2.540000E+1,  2.560000E+1 ,   2.580000E+1 , 
         2.600000E+1,  2.620000E+1 ,   2.640000E+1 , 
         2.660000E+1,  2.680000E+1 ,   2.700000E+1 , 
         2.720000E+1,  2.740000E+1 ,   2.760000E+1 , 
         2.780000E+1,  2.800000E+1 ,   2.820000E+1 , 
         2.840000E+1,  2.860000E+1 ,   2.880000E+1 , 
         2.900000E+1,  2.920000E+1 ,   2.940000E+1 , 
         2.960000E+1,  2.980000E+1 ,   3.000000E+1 , 
	 3.100000E+1   } ;
                    
  double Yerg7[]     ={ 0.000000E+0 ,  1.850569E-9 ,  1.940894E-9  ,
         2.027196E-9 ,  2.109973E-9 ,  2.189621E-9  ,
         2.266473E-9 ,  2.340803E-9 ,  2.412844E-9  ,
         2.482795E-9 ,  2.550829E-9 ,  2.617094E-9  ,
         2.681723E-9 ,  2.744830E-9 ,  2.806519E-9  ,
         2.866880E-9 ,  2.925996E-9 ,  2.983942E-9  ,
         3.040783E-9 ,  3.096582E-9 ,  3.151392E-9  ,
         3.205264E-9 ,  3.258247E-9 ,  3.310381E-9  ,
         3.361708E-9 ,  3.412261E-9 ,  3.462077E-9  ,
         3.511186E-9 ,  3.559617E-9 ,  3.607400E-9  ,
         3.654556E-9 ,  3.701112E-9 ,  3.747089E-9  ,
         3.792509E-9 ,  3.837391E-9 ,  3.881754E-9  ,
         3.925616E-9 ,  3.968994E-9 ,  4.011902E-9  ,
         4.054356E-9 ,  4.096371E-9 ,  4.137958E-9  ,
         4.179131E-9 ,  4.219903E-9 ,  4.260285E-9  ,
         4.300288E-9 ,  4.339921E-9 ,  4.379197E-9  ,
         4.418123E-9 ,  4.456708E-9 ,  4.494963E-9  ,
         4.532894E-9 ,  4.570511E-9 ,  4.607821E-9  ,
         4.644831E-9 ,  4.681548E-9 ,  4.717980E-9  ,
         4.754132E-9 ,  4.790012E-9 ,  4.825625E-9  ,
         4.860977E-9 ,  4.896073E-9 ,  4.930920E-9  ,
         4.965522E-9 ,  4.999885E-9 ,  5.034012E-9  ,
         5.067911E-9 ,  5.101584E-9 ,  5.135036E-9  ,
         5.168271E-9 ,  5.201295E-9 ,  5.234109E-9  ,
         5.266719E-9 ,  5.299129E-9 ,  5.331342E-9  ,
         5.363361E-9 ,  5.395190E-9 ,  5.426832E-9  ,
         5.458290E-9 ,  5.489569E-9 ,  5.520670E-9  ,
         5.551597E-9 ,  5.582353E-9 ,  5.612940E-9  ,
         5.643361E-9 ,  5.673619E-9 ,  5.703717E-9  ,
         5.733656E-9 ,  5.763440E-9 ,  5.793071E-9  ,
         5.822551E-9 ,  5.851882E-9 ,  6.137490E-9  ,
         6.410384E-9 ,  6.672121E-9 ,  6.923971E-9  ,
         7.166972E-9 ,  7.401998E-9 ,  7.629785E-9  ,
         7.850964E-9 ,  8.066079E-9 ,  8.275601E-9  ,
         8.479947E-9 ,  8.679481E-9 ,  8.874527E-9  ,
         9.065376E-9 ,  9.252286E-9 ,  9.435494E-9  ,
         9.615210E-9 ,  9.791625E-9 ,  9.964917E-9  ,
         1.013525E-8 ,  1.030276E-8 ,  1.046758E-8  ,
         1.062985E-8 ,  1.078968E-8 ,  1.094717E-8  ,
         1.110243E-8 ,  1.125555E-8 ,  1.140661E-8  ,
         1.155569E-8 ,  1.170287E-8 ,  1.184822E-8  ,
         1.199181E-8 ,  1.213370E-8 ,  1.227395E-8  ,
         1.241261E-8 ,  1.254974E-8 ,  1.268539E-8  ,
         1.281960E-8 ,  1.295241E-8 ,  1.308388E-8  ,
         1.321404E-8 ,  1.334292E-8 ,  1.347057E-8  ,
         1.359703E-8 ,  1.372231E-8 ,  1.384647E-8  ,
         1.396952E-8 ,  1.409149E-8 ,  1.421240E-8  ,
         1.433231E-8 ,  1.445121E-8 ,  1.456915E-8  ,
         1.468614E-8 ,  1.480219E-8 ,  1.491735E-8  ,
         1.503162E-8 ,  1.514504E-8 ,  1.525759E-8  ,
         1.536934E-8 ,  1.548027E-8 ,  1.559041E-8  ,
         1.569977E-8 ,  1.580839E-8 ,  1.591626E-8  ,
         1.602340E-8 ,  1.612982E-8 ,  1.623555E-8  ,
         1.634060E-8 ,  1.644498E-8 ,  1.654868E-8  ,
         1.665175E-8 ,  1.675418E-8 ,  1.685598E-8  ,
         1.695718E-8 ,  1.705778E-8 ,  1.715777E-8  ,
         1.725720E-8 ,  1.735606E-8 ,  1.745434E-8  ,
         1.755208E-8 ,  1.764928E-8 ,  1.774594E-8  ,
         1.784208E-8 ,  1.793771E-8 ,  1.803282E-8  ,
         1.812744E-8 ,  1.822156E-8 ,  1.831520E-8  ,
         1.840836E-8 ,  1.850104E-8 ,  1.940357E-8  ,
         2.026585E-8 ,  2.109283E-8 ,  2.188850E-8  ,
         2.265618E-8 ,  2.339860E-8 ,  2.411811E-8  ,
         2.481669E-8 ,  2.549608E-8 ,  2.615775E-8  ,
         2.680303E-8 ,  2.743307E-8 ,  2.804890E-8  ,
         2.865144E-8 ,  2.924150E-8 ,  2.981983E-8  ,
         3.038710E-8 ,  3.094391E-8 ,  3.149083E-8  ,
         3.202834E-8 ,  3.255693E-8 ,  3.307702E-8  ,
         3.358901E-8 ,  3.409326E-8 ,  3.459011E-8  ,
         3.507987E-8 ,  3.556283E-8 ,  3.603927E-8  ,
         3.650946E-8 ,  3.697361E-8 ,  3.743196E-8  ,
         3.788472E-8 ,  3.833208E-8 ,  3.877424E-8  ,
         3.921136E-8 ,  3.964363E-8 ,  4.007118E-8  ,
         4.049418E-8 ,  4.091275E-8 ,  4.132705E-8  ,
         4.173720E-8 ,  4.214331E-8 ,  4.254551E-8  ,
         4.294388E-8 ,  4.333857E-8 ,  4.372965E-8  ,
         4.411721E-8 ,  4.450137E-8 ,  4.488219E-8  ,
         4.525978E-8 ,  4.563419E-8 ,  4.600553E-8  ,
         4.637385E-8 ,  4.673924E-8 ,  4.710174E-8  ,
         4.746145E-8 ,  4.781841E-8 ,  4.817269E-8  ,
         4.852434E-8 ,  4.887342E-8 ,  4.922000E-8  ,
         4.956412E-8 ,  4.990582E-8 ,  5.024517E-8  ,
         5.058221E-8 ,  5.091697E-8 ,  5.124953E-8  ,
         5.157989E-8 ,  5.190812E-8 ,  5.223425E-8  ,
         5.255833E-8 ,  5.288039E-8 ,  5.320046E-8  ,
         5.351858E-8 ,  5.383479E-8 ,  5.414912E-8  ,
         5.446161E-8 ,  5.477227E-8 ,  5.508117E-8  ,
         5.538829E-8 ,  5.569369E-8 ,  5.599740E-8  ,
         5.629943E-8 ,  5.659982E-8 ,  5.689859E-8  ,
         5.719577E-8 ,  5.749139E-8 ,  5.778546E-8  ,
         5.807801E-8 ,  5.836906E-8 ,  6.120187E-8  ,
         6.390639E-8 ,  6.649828E-8 ,  6.899021E-8  ,
         7.139264E-8 ,  7.371432E-8 ,  7.596264E-8  ,
         7.814394E-8 ,  8.026365E-8 ,  8.232654E-8  ,
         8.433676E-8 ,  8.629799E-8 ,  8.821349E-8  ,
         9.008618E-8 ,  9.191865E-8 ,  9.371326E-8  ,
         9.547215E-8 ,  9.719724E-8 ,  9.889030E-8  ,
         1.005529E-7 ,  1.021866E-7 ,  1.037927E-7  ,
         1.053725E-7 ,  1.069272E-7 ,  1.084578E-7  ,
         1.099652E-7 ,  1.114505E-7 ,  1.129146E-7  ,
         1.143581E-7 ,  1.157820E-7 ,  1.171867E-7  ,
         1.185733E-7 ,  1.199421E-7 ,  1.212938E-7  ,
         1.226290E-7 ,  1.239482E-7 ,  1.252519E-7  ,
         1.265405E-7 ,  1.278146E-7 ,  1.290745E-7  ,
         1.303205E-7 ,  1.315533E-7 ,  1.327731E-7  ,
         1.339802E-7 ,  1.351751E-7 ,  1.363579E-7  ,
         1.375290E-7 ,  1.386888E-7 ,  1.398375E-7  ,
         1.409754E-7 ,  1.421026E-7 ,  1.432195E-7  ,
         1.443263E-7 ,  1.454233E-7 ,  1.465105E-7  ,
         1.475884E-7 ,  1.486570E-7 ,  1.497166E-7  ,
         1.507673E-7 ,  1.518093E-7 ,  1.528429E-7  ,
         1.538680E-7 ,  1.548850E-7 ,  1.558941E-7  ,
         1.568951E-7 ,  1.578886E-7 ,  1.588745E-7  ,
         1.598528E-7 ,  1.608240E-7 ,  1.617879E-7  ,
         1.627447E-7 ,  1.636947E-7 ,  1.646379E-7  ,
         1.655743E-7 ,  1.665042E-7 ,  1.674276E-7  ,
         1.683445E-7 ,  1.692553E-7 ,  1.701598E-7  ,
         1.710583E-7 ,  1.719508E-7 ,  1.728373E-7  ,
         1.737182E-7 ,  1.745931E-7 ,  1.754626E-7  ,
         1.763265E-7 ,  1.771848E-7 ,  1.780378E-7  ,
         1.788853E-7 ,  1.797277E-7 ,  1.878767E-7  ,
         1.955684E-7 ,  2.028536E-7 ,  2.097738E-7  ,
         2.163633E-7 ,  2.226501E-7 ,  2.286576E-7  ,
         2.344065E-7 ,  2.399146E-7 ,  2.451976E-7  ,
         2.502693E-7 ,  2.551414E-7 ,  2.598248E-7  ,
         2.643292E-7 ,  2.686635E-7 ,  2.728355E-7  ,
         2.768522E-7 ,  2.807201E-7 ,  2.844444E-7  ,
         2.880290E-7 ,  2.914800E-7 ,  2.948012E-7  ,
         2.979947E-7 ,  3.010642E-7 ,  3.040154E-7  ,
         3.068522E-7 ,  3.095810E-7 ,  3.122060E-7  ,
         3.147276E-7 ,  3.171490E-7 ,  3.194710E-7  ,
         3.216904E-7 ,  3.238020E-7 ,  3.258003E-7  ,
         3.276796E-7 ,  3.294435E-7 ,  3.311039E-7  ,
         3.326695E-7 ,  3.341500E-7 ,  3.355530E-7  ,
         3.368817E-7 ,  3.381350E-7 ,  3.393089E-7  ,
         3.403998E-7 ,  3.414036E-7 ,  3.423157E-7  ,
         3.431423E-7 ,  3.438922E-7 ,  3.445729E-7  ,
         3.451919E-7 ,  3.457555E-7 ,  3.462689E-7  ,
         3.467341E-7 ,  3.471537E-7 ,  3.475310E-7  ,
         3.478668E-7 ,  3.481599E-7 ,  3.484092E-7  ,
         3.486161E-7 ,  3.487823E-7 ,  3.489091E-7  ,
         3.489987E-7 ,  3.490531E-7 ,  3.490737E-7  ,
         3.490615E-7 ,  3.490170E-7 ,  3.489408E-7  ,
         3.488347E-7 ,  3.487002E-7 ,  3.485388E-7  ,
         3.483514E-7 ,  3.481389E-7 ,  3.479022E-7  ,
         3.476420E-7 ,  3.473592E-7 ,  3.470544E-7  ,
         3.467281E-7 ,  3.463811E-7 ,  3.460147E-7  ,
         3.456295E-7 ,  3.452265E-7 ,  3.448058E-7  ,
         3.443675E-7 ,  3.439118E-7 ,  3.434397E-7  ,
         3.429511E-7 ,  3.424460E-7 ,  3.419244E-7  ,
         3.413862E-7 ,  3.408314E-7 ,  3.344139E-7  ,
         3.266933E-7 ,  3.181551E-7 ,  3.089767E-7  ,
         2.992498E-7 ,  2.890636E-7 ,  2.784857E-7  ,
         2.675919E-7 ,  2.564713E-7 ,  2.452123E-7  ,
         2.339058E-7 ,  2.226394E-7 ,  2.114964E-7  ,
         2.005485E-7 ,  1.898564E-7 ,  1.794722E-7  ,
         1.694365E-7 ,  1.597780E-7 ,  1.505184E-7  ,
         1.416708E-7 ,  1.332408E-7 ,  1.252289E-7  ,
         1.176305E-7 ,  1.104378E-7 ,  1.036396E-7  ,
         9.722309E-8 ,  9.117350E-8 ,  8.547540E-8  ,
         8.011238E-8 ,  7.506803E-8 ,  7.032591E-8  ,
         6.586982E-8 ,  6.168401E-8 ,  5.775315E-8  ,
         5.406242E-8 ,  5.059781E-8 ,  4.734590E-8  ,
         4.429392E-8 ,  4.142990E-8 ,  3.874255E-8  ,
         3.622122E-8 ,  3.385584E-8 ,  3.163705E-8  ,
         2.955604E-8 ,  2.760456E-8 ,  2.577490E-8  ,
         2.405981E-8 ,  2.245245E-8 ,  2.094643E-8  ,
         1.953575E-8 ,  1.821478E-8 ,  1.697819E-8  ,
         1.582102E-8 ,  1.473855E-8 ,  1.372630E-8  ,
         1.278008E-8 ,  1.189593E-8 ,  1.107009E-8  ,
         1.029902E-8 ,  9.579367E-9 ,  8.907964E-9  ,
         8.281814E-9 ,  7.698076E-9 ,  7.154075E-9  ,
         6.647284E-9 ,  6.175315E-9 ,  5.735914E-9  ,
         5.326960E-9 ,  4.946457E-9 ,  4.592522E-9  ,
         4.263383E-9 ,  3.957381E-9 ,  3.672953E-9  ,
         3.408633E-9 ,  3.163043E-9 ,  2.934899E-9  ,
         2.722996E-9 ,  2.526204E-9 ,  2.343472E-9  ,
         2.173816E-9 ,  2.016320E-9 ,  1.870126E-9  ,
         1.734434E-9 ,  1.608503E-9 ,  1.491638E-9  ,
         1.383194E-9 ,  1.282571E-9 ,  1.189211E-9  ,
         1.102593E-9 ,  1.022235E-9 ,  8.78187E-10  ,
         7.54438E-10 ,  6.48127E-10 ,  5.56796E-10  ,
         4.78336E-10 ,  4.10482E-10 ,  3.52254E-10  ,
         3.02286E-10 ,  2.59406E-10 ,  2.22609E-10  ,
         1.90782E-10 ,  1.63506E-10 ,  1.40129E-10  ,
         1.20095E-10 ,  1.02925E-10 ,  8.81159E-11  ,
         7.54377E-11 ,  6.45835E-11 ,  5.52911E-11  ,
         4.73358E-11 ,  4.04774E-11 ,  3.46128E-11  ,
         2.95978E-11 ,  2.53095E-11 ,  2.16425E-11  ,
         1.84867E-11 ,  1.57910E-11 ,  1.34884E-11  ,
         1.15216E-11 ,  9.84163E-12 ,  8.39862E-12  ,
         7.16718E-12 ,  6.11631E-12 ,  5.21952E-12  ,
         4.45422E-12 ,  3.79766E-12 ,  3.23789E-12  ,
         2.76062E-12 ,  2.35370E-12 ,  2.00677E-12  ,
         1.70943E-12 ,  1.45614E-12 ,  1.24039E-12  ,
         1.05660E-12 ,  9.00051E-13 ,  7.66025E-13  ,
         6.51957E-13 ,  5.54875E-13 ,  4.72250E-13  ,
         4.01928E-13 ,  3.41805E-13 ,  2.90676E-13  ,
         2.47195E-13 ,  2.10218E-13 ,  1.78773E-13  ,
         1.51925E-13 ,  1.29109E-13 ,  1.09720E-13  ,
         9.32431E-14 ,  7.92402E-14 ,  6.73001E-14  ,
         5.71591E-14 ,  4.85462E-14 ,  4.12312E-14  ,
         3.50184E-14 ,  2.97261E-14 ,  2.52336E-14  ,
         2.14201E-14 ,  1.81829E-14 ,  1.54350E-14  ,
         1.30957E-14 ,  1.11109E-14 ,  9.42699E-15  ,
         7.99825E-15 ,  6.78606E-15 ,  5.75458E-15  ,
         4.87990E-15 ,  4.13816E-15 ,  3.50916E-15  ,
         2.97578E-15 ,  2.52208E-15 ,  2.13755E-15  ,
         1.81165E-15 ,  1.53544E-15 ,  1.30134E-15  ,
         1.10227E-15 ,  9.33655E-16 ,  7.90833E-16  ,
         6.69858E-16 ,  5.67389E-16 ,  4.80294E-16  ,
         4.06568E-16 ,  3.44159E-16 ,  2.91330E-16  ,
         2.46611E-16 ,  2.08625E-16 ,  1.76491E-16  ,
			1.49306E-16 , 1.26309E-16 , 1.06854E-16,  0};


  // std::cout<< "EE  "<<Xerg7[1]<<" EEE "<<Yerg7[1]<< "AA "<<Xerg7[642]<< "AAA "<<Yerg7[642]<<std::endl;
  SDef::SrcData D1(1);
  SDef::SrcInfo SI1('A');
  SDef::SrcProb SP1('  ');

 for(unsigned ii=1;ii<=642;ii++) 
   {  
    SI1.addData(Xerg7[ii]);
    SP1.addData(Yerg7[ii]);
    D1.addUnit(SI1);  
    D1.addUnit(SP1);
   }  
  sourceCard.setData("erg",D1);  


  SDef::SrcData D2(2);
  SDef::SrcInfo SI2;
  SDef::SrcProb SP2;

  // SI2.addData(-2.18);
  // SI2.addData(-1.78);
  /////  49  /////////
  SI2.addData(-0.05);
  SI2.addData(0.15);
  ///////////////////////
  SP2.addData(0);
  SP2.addData(1.);
  D2.addUnit(SI2);  
  D2.addUnit(SP2);  
  sourceCard.setData("y",D2);  


  SDef::SrcData D3(3);
  SDef::SrcInfo SI3('A');
  SDef::SrcProb SP3(' ');
  SDef::SrcBias SB3(' ');
  
  SI3.addData(-1);
  SI3.addData(1);
  SP3.addData(1);
  SP3.addData(1);  
  SB3.addData(-31);
  SB3.addData(1);

  // D3.addUnit(SI3);  
  // D3.addUnit(SP3);  
  D3.addUnit(SB3);  
  // std::cout<<" AAA "<<SB3<<std::endl;
  sourceCard.setData("dir",D3);  
 
  sourceCard.setComp("vec",Geometry::Vec3D(0,1,0 ));

  // array limits==
  int nx49(16);
  int nz49(17);
  int nx75(6);
  int nz75(13);
  int nx35(16);
  int nz35(17);
  int nx36(6);
  int nz36(13);

  double nuclearZ(-6.6);
  double nuclearX(2.5);

  SDef::SrcData D4(4);
  //ori  SDef::SrcInfo SI4;
  SDef::SrcInfo SI4('A');
  SDef::SrcProb SP4(' ');
  double hVal=0.0;
  //ori  SP4.addData(hVal);
  for(unsigned iz=0;iz<nz49-1;iz++) 
   {SI4.addData(sinbadSource49[nz49-1-iz][0]+nuclearZ); 
    hVal=0; 
    for(unsigned ix=0;ix<nx49-1;ix++) 
      {
	hVal+=sinbadSource49[nz49-1-iz-1][ix+1]*
        (sinbadSource49[0][ix+1]-sinbadSource49[0][ix])*
        (sinbadSource49[nz49-1-iz-1][0]-sinbadSource49[nz49-1-iz][0]);
      }

    //ori   if(iz<nz36-2)SP4.addData(hVal);
    SP4.addData(1.0);   
   }  
  D4.addUnit(SI4);  
  D4.addUnit(SP4);  
  sourceCard.setData("z",D4);  

  SDef::SrcData D5(5);
  SDef::SrcInfo SI5;
  //

  //  for(unsigned ix=0;ix<15;ix++) SI4.addData(sinbadSource36[0][ix]-2.5); 
  for(unsigned ix=0;ix<nx49-1;ix++) SI4.addData(sinbadSource49[0][ix]); 
  SDef::DSTerm<int>* DS5=D5.getDS<int>(); 
  DS5->setType("z",'s');
  for (unsigned is=6;is<nz49+2;is++) DS5->addData(is);
  
 
  SDef::DSIndex DI6(std::string("z"));
  SDef::SrcInfo SI6('A');
  //  for(unsigned ix=0;ix<15;ix++) SI5.addData(sinbadSource49[0][ix]-2.5); 
  for(unsigned ix=0;ix<nx49;ix++) SI6.addData(sinbadSource49[0][ix]); 

  // starting dist number==6
  int sn(6);
  // ATTENZIONE 20!!!!
  for (unsigned is=sn;is<nz49+sn-2;is++) 
   {
     SDef::SrcProb SP6 (' ');
    //ori    SP6.addData(0.0);
    //ori   for (unsigned ix=1;ix<nx49;ix++)

    for (unsigned ix=0;ix<nx49;ix++)
      {
      //ori  SP6.addData(sinbadSource49[nz49+sn-2-is][ix]*
      //ori (sinbadSource49[0][ix]-sinbadSource49[0][ix-1]));
      SP6.addData(1.0);

       //       if(ix<15)
      //   std::cout<<" S "<<sinbadSource49[nz49-is+sn-2][ix]<<" x "<<sinbadSource49[0][ix-1]<<std::endl;
 
  
     }
       DI6.addData(is,&SI6,0,&SP6);

   }
  D5.addUnit(&DI6);
  sourceCard.setData("x",D5); 
 
 //49//////////////
  int cellNum(41);
/////////////////

  SDef::SrcData D21(21);
  SDef::SrcInfo SI21('L');
  SDef::SrcProb SP21(' ');
   for(unsigned ix=0;ix<nx49-1;ix++)   
    { 
     for(unsigned iz=0;iz<nz49-2;iz++) 
      {
       if(iz==nz49-3)cellNum++;
       SI21.addData(cellNum++);
       SP21.addData(sinbadSource49[nz49-1-iz-1][ix+1]);
      }
    if(ix==nx49-3)cellNum++;
    cellNum++;

   }  

  D21.addUnit(SI21);  
  D21.addUnit(SP21);  
  sourceCard.setData("cel",D21);  

 sourceCard.setComp("eff", 0.00000001);




  return;
}  


} // NAMESPACE SDef
