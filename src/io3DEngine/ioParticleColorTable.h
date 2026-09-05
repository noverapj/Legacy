

#ifndef _ioParticleColorTable_h_
#define _ioParticleColorTable_h_

#define MAX_COLOR_TABLE 128
#define FLOAT_COLOR_TABLE_RATE 127.0f

#include "ioSharedPtr.h"
#include "ioMemoryPool.h"

class ioParticleColorTable : public ioPoolObject< ioParticleColorTable, 300 >
{
private:
	DWORD m_aColorTable[MAX_COLOR_TABLE];

public:
	inline void SetColor( int iIndex, DWORD dwColor )
	{
		m_aColorTable[iIndex] = dwColor;
	}

	inline DWORD GetColor( float fTimeRate ) const
	{
		int iIndex = (int)(FLOAT_COLOR_TABLE_RATE * fTimeRate);
		if( iIndex < 0 )
			iIndex = 0;
		else if( iIndex >= MAX_COLOR_TABLE )
			iIndex = MAX_COLOR_TABLE - 1;

		return m_aColorTable[iIndex];
	}

	inline DWORD GetStartColor() const
	{
		return m_aColorTable[0];
	}

public:
	ioParticleColorTable& operator=( const ioParticleColorTable &rhs );

public:
	ioParticleColorTable();
	~ioParticleColorTable();
};

typedef ioSharedPtr<ioParticleColorTable> ioParticleColorTablePtr;

#endif