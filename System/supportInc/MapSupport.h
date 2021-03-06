/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/MapSupport.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef MapSupport_h
#define MapSupport_h

/*!
  \namespace MapSupport
  \brief Holds stuff for manipulating maps
  \author S. Ansell
  \date 2/9/05
  \version 1.0

  Allows maps to be created with cloned objects
*/

namespace MapSupport
{
  /*!
    \class PFirst
    \author S. Ansell
    \date Aug 2005
    \version 1.1
    \brief Class to access the first object in index pair. 
    
    Added support to unary_function for use of the bind functions
    and put into MapSupport from mathSupport
  */

   //Note this needs to be non-constant unary_function return
  template<typename T,typename U>
  class PFirst  : public  std::unary_function<std::pair<T,U>, T >  
    {
      public:
      
      /// Functional to the first object
      T operator()(const std::pair<T,U>& A) {  return A.first; }
      
    };

  /*!
    \class PSecond
    \author S. Ansell
    \date June 2006
    \version 1.0
    \brief Class to access the second object in index pair. 
  */
  
  template<typename MapTYPE>
  class PSecond :
  public  std::unary_function<typename MapTYPE::value_type, 
                              typename MapTYPE::mapped_type> 
    {
      public:
           
      /// Functional to the first object
      typename MapTYPE::mapped_type operator()
             (const typename MapTYPE::value_type& A) 
	{  return A.second; }
    };
  
  /*!
    \class valEqual
    \brief Functor using second value as equal
    \author S. Ansell
    \date 2/9/05
    \version 1.0

    Functor allows searching a map for second value
    equal.
  */
  template<typename KeyPart,typename NumPart>
  class valEqual
    {
      private:
      
      const NumPart value;   ///< Value to check against map
   
      public:

      /// Set Value to check
      valEqual(const NumPart& V) :
        value(V)
	{ }

      /// Equality operator vs Map second object
      bool
      operator()(const std::pair<KeyPart,NumPart>& A) const
	{
	  return A.second==value;
	}
    };
  /*!
    \class mapClone
    \brief Functor for coping map elements with clone functions
    \author S. Ansell
    \date 2/9/05
    \version 1.0

    This functor takes a map of key,Pointer, where the 
    Pointer is associa<ted with a class that can provide the 
    clone function. The clone function must return a Pointer
    of the same type.
    It is used for coping maps using a deep copy.
  */

  template<typename KeyPart,typename PtrPart>
  class mapClone
    {
      public:

        /// clone function to return insert object
        std::pair<KeyPart,PtrPart> 
	operator()(const std::pair<KeyPart,PtrPart>& A) const
	  {
	    return std::pair<KeyPart,PtrPart>(A.first,A.second->clone());
	  }
    };

  /*!
    \class mapDelete
    \brief Functor for deleting the second component of a map
    \author S. Ansell
    \date 2/9/05
    \version 1.1

    This functor takes a map of key,Pointer and 
    deletes the memory associated with the pointer.
  */

  template<typename MapTYPE>
  class mapDelete
    {
      public:

        /// deletion of the map:second ptr
        void
	operator()(typename MapTYPE::value_type& A)
	  {
	    delete A.second;
	    A.second=0;
	  }
    };

  /*!
    \class mapSwap
    \brief Functor for reversing a map
    \author S. Ansell
    \date August 2007
    \version 1.0

    This functor swaps the key:map components in a map.
  */
  
  template<typename KeyPart,typename BodyPart>
  class mapSwap :
    public std::unary_function<std::pair<const BodyPart,KeyPart>,
			       std::pair<const KeyPart,BodyPart> >
    {
      public:

      /// Swap operator [exchange Key and Body]
      std::pair<BodyPart,KeyPart> 
      operator()(const std::pair<const KeyPart,BodyPart>& A) const
	{
	  return std::pair<BodyPart,KeyPart>(A.second,A.first);
	}
    };


  /*!
    \class mapWrite
    \brief Functor quick write out of a map
    \author S. Ansell
    \date February 2014
    \version 2.0

    This functor takes a map of key,value and write the 
    key,value to the stream
  */
  template<typename MapTYPE>
  class mapWrite
    {
      private:

      std::ostream& OX;

      public:
      
       mapWrite(std::ostream& OutStream) : OX(OutStream) {}


        /// Write both the key and object 
        void 
	  operator()(const typename MapTYPE::value_type& A) const
	  {
	    OX<<A.first<<" "<<A.second<<std::endl;
	  }
    };
  /*!
    \class sndValue
    \brief Functor to get second point in a map
    \author S. Ansell
    \date September 2005
    \version 1.0

    This functor takes a map of key,value 
    returns just the value. It is built so that
    the useage is 
     - sndValue<a,b>(MapInstance)
    This avoids needing to call bind2nd each occasion.
    
    Additionally it does not throw any exceptions on 
    key not found.
  */

  template<typename MapTYPE>
  class sndValue :  
  public std::unary_function<MapTYPE, typename MapTYPE::mapped_type > 
    {
      private:
      
      const MapTYPE& MRef;   ///< Map begin accessd
      
      public:

      /// Create with Map
       sndValue(const MapTYPE& Mr) :
         MRef(Mr)
	 { }

       /// Access via key
      typename MapTYPE::mapped_type
      operator()(const typename MapTYPE::key_type& Ky) const
	{
	  typename MapTYPE::const_iterator vc; 
	  vc=MRef.find(Ky);
	  return vc->second;
	}
    };

} // NAMESPACE MapSupport


#endif
