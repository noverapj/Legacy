// IORandom.h: interface for the IORandom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IORANDOM_H__368A70A8_75E8_4BB6_8B18_0311D483030C__INCLUDED_)
#define AFX_IORANDOM_H__368A70A8_75E8_4BB6_8B18_0311D483030C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* 주기 매개변수들 */
#define CMATH_N             624
#define CMATH_M             397
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

class IORandom  
{
	unsigned int	m_rseed;
	unsigned long	m_mt[CMATH_N];
	int             m_mti;

	public:
	void Randomize();
	unsigned int Random( unsigned int n );
	unsigned int Random(unsigned int nMin, unsigned int nMax);
	
	void SetRandomSeed( unsigned int seed );
	unsigned int GetRandomSeed();

	public:
	IORandom();
	virtual ~IORandom();

};

#endif // !defined(AFX_IORANDOM_H__368A70A8_75E8_4BB6_8B18_0311D483030C__INCLUDED_)
