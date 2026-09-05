

#ifndef _ioRandomCreator_h_
#define _ioRandomCreator_h_

#include "io3DCommon.h"

#define CMATH_N             624
#define CMATH_M             397

class __EX ioRandomCreator
{
protected:
	unsigned int  m_rseed;
	unsigned long m_mt[CMATH_N];
	int	m_mti;

public:
	void Randomize();
	unsigned int Random( unsigned int uMax );

public:
	// 0.0f ~ 1.0f
	inline float UnitRandom()
	{
		return (float)Random( RAND_MAX )/(float)RAND_MAX;
	}

	// fMin ~ fMax
	float RangeRandom( float fMin, float fMax )
	{
		return (fMax-fMin)*UnitRandom() + fMin;
	}

	// -1.0f ~ 1.0f
	float SymmetricRandom()
	{
		return 2.0f * UnitRandom() - 1.0f;
	}

public:
	void SetRandomSeed( unsigned int uSeed );
	unsigned int GetRandomSeed() const;

public:
	ioRandomCreator();
	virtual ~ioRandomCreator();
};

#endif
