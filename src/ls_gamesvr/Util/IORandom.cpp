// IORandom.cpp: implementation of the IORandom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IORandom.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IORandom::IORandom()
{
	m_rseed = 1;
	m_mti   = CMATH_N + 1;
}

IORandom::~IORandom()
{

}

unsigned int IORandom::Random( unsigned int nMin, unsigned int nMax)
{
	unsigned int nDiff = max(0, nMax - nMin );
	unsigned int nRnd = Random( nDiff );
	return nRnd + nMin;
}

unsigned int IORandom::Random( unsigned int n )
{
	unsigned long y;
	static unsigned long mag01[2] = {0x0, CMATH_MATRIX_A};

	if( n == 0 )
		return 0;

	if( m_mti >= CMATH_N )
	{
		int kk;

		if( m_mti == CMATH_N+1 )
			SetRandomSeed( 4357 );

		for(kk = 0;kk < CMATH_N - CMATH_M;kk++)
		{
			y = (m_mt[kk] & CMATH_UPPER_MASK) | (m_mt[kk+1] &CMATH_LOWER_MASK);
			m_mt[kk] = m_mt[kk+CMATH_M] ^ (y >> 1) ^ mag01[y & 0x1];
		}

		for(;kk < CMATH_N-1;kk++)
		{
			y = (m_mt[kk] & CMATH_UPPER_MASK) | (m_mt[kk+1]&CMATH_LOWER_MASK);
			m_mt[kk] = m_mt[kk+(CMATH_M-CMATH_N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}

		y = (m_mt[CMATH_N-1] & CMATH_UPPER_MASK) | (m_mt[0] & CMATH_LOWER_MASK);
		m_mt[CMATH_N-1] = m_mt[CMATH_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

		m_mti = 0;
	}

	y = m_mt[m_mti++];
	y ^= CMATH_TEMPERING_SHIFT_U(y);
	y ^= CMATH_TEMPERING_SHIFT_S(y) & CMATH_TEMPERING_MASK_B;
	y ^= CMATH_TEMPERING_SHIFT_T(y) & CMATH_TEMPERING_MASK_C;
	y ^= CMATH_TEMPERING_SHIFT_L(y);

	return y % n;
}

void IORandom::SetRandomSeed( unsigned int seed )
{
	m_mt[0] = seed & 0xffffffff;
	for(m_mti = 1;m_mti < CMATH_N;m_mti++)
		m_mt[m_mti] = (69069 * m_mt[m_mti-1]) & 0xffffffff;

	m_rseed = seed;
}

unsigned int IORandom::GetRandomSeed()
{
	return m_rseed;
}

void IORandom::Randomize()
{
	SetRandomSeed( timeGetTime() );
}