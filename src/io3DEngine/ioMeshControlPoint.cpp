

#include "stdafx.h"

#include "ioMeshControlPoint.h"
#include "ioStringConverter.h"

ioMeshControlPoint::ioMeshControlPoint()
{
	m_iIndex = 0;
}

ioMeshControlPoint::~ioMeshControlPoint()
{
}

void ioMeshControlPoint::SetTypeIndex( const char *szBuf )
{
	const StringVector &vecparams = ioStringConverter::Split( szBuf, "_", 1 );

	if( vecparams.size() == 2 )
	{
		m_Type = vecparams[0].c_str();
		m_iIndex = ioStringConverter::ParseInt( vecparams[1].c_str() );
	}
}

void ioMeshControlPoint::SetTypeIndex( const char *szType, int iIndex )
{
	m_Type = szType;
	m_iIndex = iIndex;
}

void ioMeshControlPoint::SetLinkedBipedName( const char *szName )
{
	m_LinkedBiped = szName;
}

void ioMeshControlPoint::SetExtraInfo( const char *szInfo )
{
	m_ExtraInfo = szInfo;
}

void ioMeshControlPoint::SetPoint( const D3DXVECTOR3 &vPoint )
{
	m_vPoint = vPoint;
}