

#include "stdafx.h"

#include "ioRandomCreator.h"

/* 주기 매개변수들 */
#define CMATH_MATRIX_A      0x9908b0df
#define CMATH_UPPER_MASK    0x80000000
#define CMATH_LOWER_MASK    0x7fffffff

/* 조절용 매개변수들 */
#define CMATH_TEMPERING_MASK_B       0x9b2c5680
#define CMATH_TEMPERING_MASK_C       0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)   (y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)   (y >> 7)
#define CMATH_TEMPERING_SHIFT_T(y)   (y >> 15)
#define CMATH_TEMPERING_SHIFT_L(y)   (y >> 18)

ioRandomCreator::ioRandomCreator()
{
	m_rseed = 1;
	m_mti = CMATH_N + 1;
}

ioRandomCreator::~ioRandomCreator()
{
}

unsigned int ioRandomCreator::Random( unsigned int uMax )
{
	unsigned long y;
	static unsigned long mag01[2] = {0x0, CMATH_MATRIX_A};

	if( uMax == 0 )
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

	return y % uMax;
}

void ioRandomCreator::SetRandomSeed( unsigned int uSeed )
{
	m_mt[0] = uSeed & 0xffffffff;
	for( m_mti=1; m_mti<CMATH_N; m_mti++ )
	{
		m_mt[m_mti] = (69069 * m_mt[m_mti-1]) & 0xffffffff;
	}

	m_rseed = uSeed;
}

unsigned int ioRandomCreator::GetRandomSeed() const
{
	return m_rseed;
}

void ioRandomCreator::Randomize()
{
	SetRandomSeed( rand() );
}
