/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: The SOFA Team (see Authors.txt)                                    *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
/*
 * RandomGenerator.h
 *
 *  Created on: 25 mai 2009
 *      Author: froy
 */

#ifndef RANDOMGENERATOR_H_
#define RANDOMGENERATOR_H_

#include <cstdlib>
#include <sofa/helper/helper.h>
#include <limits>

#define RAND48_SEED_0   (0x330e)
#define RAND48_SEED_1   (0xabcd)
#define RAND48_SEED_2   (0x1234)
#define RAND48_MULT_0   (0xe66d)
#define RAND48_MULT_1   (0xdeec)
#define RAND48_MULT_2   (0x0005)
#define RAND48_ADD      (0x000b)

namespace sofa
{

namespace helper
{


/// Generate Random number
/// based on random functions from OpenBSD
class SOFA_HELPER_API RandomGenerator
{
private:

    unsigned short __rand48_seed[3];
    unsigned short __rand48_mult[3];
    unsigned short __rand48_add;

    long seed;

    void __dorand48(unsigned short xseed[3]);

public:
    /// integer between [0, 2^32-1)
    unsigned long int randomBase();

    RandomGenerator();
    RandomGenerator(long seed);
    virtual ~RandomGenerator();

    void initSeed(long seed);



    /// @deprecated for backward compatibility
    /// use random<long>(min,max)
    long int randomInteger(long min, long max);
    /// @deprecated for backward compatibility
    /// use random<double>(min,max)
    double randomDouble(double min, double max);




    /// number between [min, max)  (max has less chance to appear)
    /// note that "only" 2^32 different values can be generated
    /// @warning min < max
    /// @warning for floating types a too large range can generate inf
    template<class T> T random( T min, T max )
    {
        return (T)random<long>( (long)min, (long)max );  // default implementation for integer types. Specialization for floating types in .cpp
    }


    /// number between [T::min, T::max)  (max has less chance to appear)
    /// note that "only" 2^32 different values can be generated
    /// @warning for floating types, min = -(2^32-1) & max = (2^32-1)
    template<class T> T random()
    {
        return random<T>( std::numeric_limits<T>::min(), std::numeric_limits<T>::max() );
    }

private:
	static const unsigned long RANDOM_BASE_MAX;

};

///////////////////////

// specialization for long
template<>
inline long RandomGenerator::random( long min, long max )
{
    SReal randomReal = min + ((max - min)*(SReal)randomBase())/(SReal)RANDOM_BASE_MAX;
	return (long)(randomReal);
}

// specialization for double
template<>
inline double RandomGenerator::random( double min, double max )
{
	return min + (max - min)*((double)randomBase()/(double)RANDOM_BASE_MAX);
}

// specialization for float
template<>
inline float RandomGenerator::random( float min, float max )
{
	return min + (max - min)*((float)randomBase()/(float)RANDOM_BASE_MAX);
}

// specialization for bool
template<>
inline bool RandomGenerator::random( bool, bool )
{
	return random<long>( 0, 2 ) != 0;
}


// specialization for double with limited range
template<>
inline double RandomGenerator::random()
{
	return random<double>( -(double)RANDOM_BASE_MAX, (double)RANDOM_BASE_MAX );
}

// specialization for float with limited range
template<>
inline float RandomGenerator::random()
{
	return random<float>( -(float)RANDOM_BASE_MAX, (float)RANDOM_BASE_MAX );
}

/** Pseudo-random number between -vmax and +vmax. Uses the seed defined using the standard srand(unsigned int) function of the stdlib
*/
inline SReal symrand(SReal vmax,int seed = 0){
    RandomGenerator randomgenerator(seed);
    SReal randomReal = -vmax + ((2*vmax)*(SReal)randomgenerator.randomBase())/(SReal)4294967295U;
    return (SReal)(randomReal);
}

}

}

#endif /* RANDOMGENERATOR_H_ */
