

#include "stdafx.h"

#include "ioSegment.h"
#include "ioMath.h"

ioSegment::ioSegment()
{
	m_vOrigin = m_vDirection = ioMath::VEC3_ZERO;
}

ioSegment::ioSegment( const ioSegment &rkSegment )
					: m_vOrigin( rkSegment.m_vOrigin ), m_vDirection( rkSegment.m_vDirection )
{
}

ioSegment::ioSegment( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd )
{
	m_vOrigin = vStart;
	m_vDirection = vEnd - vStart;
}

ioSegment::~ioSegment()
{
}
