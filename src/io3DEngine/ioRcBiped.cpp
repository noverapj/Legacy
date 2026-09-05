

#include "StdAfx.h"

#include "ioRcBiped.h"

ioRcBiped::ioRcBiped()
{
	m_fLinkOffsetRate = 1.0f;
}

ioRcBiped::ioRcBiped( const ioHashString &name ) : m_Name( name )
{
	m_fLinkOffsetRate = 1.0f;
}

ioRcBiped::~ioRcBiped()
{
}

void ioRcBiped::SetName( const char *szName )
{
	m_Name = szName;
}

void ioRcBiped::SetParentName( const char *szName )
{
	m_ParentName = szName;
}

void ioRcBiped::SetTranslation( const D3DXVECTOR3 &vTrans )
{
	m_vTrans = vTrans;
}

void ioRcBiped::SetRotation( const D3DXQUATERNION &qtRot )
{
	m_qtRotation = qtRot;
}

void ioRcBiped::SetBipedSpaceTM( const D3DXMATRIX &kBipedSpaceTM )
{
	m_BipedSpaceTM = kBipedSpaceTM;
}

void ioRcBiped::SetObjectTM( const D3DXMATRIX &rkObjectTM )
{
	m_ObjectTM = rkObjectTM;
}

void ioRcBiped::MakeOffsetTM( const ioRcBiped *pLinked )
{
	D3DXMATRIX matSrcInv;
	D3DXMatrixInverse( &matSrcInv, NULL, &pLinked->GetObjectTM() );
	D3DXMatrixMultiply( &m_ObjectOffsetTM, &matSrcInv, &m_ObjectTM );

	float fLinkLength = D3DXVec3Length( &pLinked->GetTranslation() );
	float fOwnLength = D3DXVec3Length( &GetTranslation() );
	m_fLinkOffsetRate = fOwnLength / fLinkLength;
}

void ioRcBiped::ReserveChildCount( int iCount )
{
	m_vChildList.reserve( iCount );
}

void ioRcBiped::AddChild( const ioHashString &name )
{
	m_vChildList.push_back( name );
}

const ioHashString& ioRcBiped::GetChildName( int id )  const
{
	assert( COMPARE( id, 0, GetChildCnt() ) );

	return m_vChildList[id];
}
