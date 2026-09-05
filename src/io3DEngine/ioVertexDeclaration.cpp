

#include "stdafx.h"

#include "ioVertexDeclaration.h"
#include "ioVertexBufferManager.h"

ioVertexDeclaration::ioVertexDeclaration()
{
	m_pVertexDeclaration = NULL;
	m_bNeedUpdate = true;
}

ioVertexDeclaration::ioVertexDeclaration( const ioVertexDeclaration &rhs )
	: m_VertexStreamList( rhs.m_VertexStreamList )
{
	m_pVertexDeclaration = rhs.m_pVertexDeclaration;
	m_bNeedUpdate = rhs.m_bNeedUpdate;
}

ioVertexDeclaration::~ioVertexDeclaration()
{
	ClearAllStreamSource();
}

void ioVertexDeclaration::ClearAllStreamSource()
{
	m_VertexStreamList.clear();
}

void ioVertexDeclaration::ParseStreamMaskToComponent( DWORD dwMask, VertexComponentList &kComponentList )
{
	DWORD dwCurMask = 0;
	for( int i=0 ; dwCurMask<IOFVF_END ; i++ )
	{
		dwCurMask = 1<<i;

		if( dwMask & dwCurMask )
		{
			kComponentList.push_back( static_cast<VertexComponent>(dwCurMask) );
		}
	}
}

void ioVertexDeclaration::InsertStreamSource( int iStream, DWORD dwMask )
{
	VertexComponentList kComponentList;
	ParseStreamMaskToComponent( dwMask, kComponentList );

	VertexComponentList::iterator iter;
	for( iter=kComponentList.begin() ; iter!=kComponentList.end() ; ++iter )
	{
		InsertStreamSource( iStream, *iter );
	}
}

void ioVertexDeclaration::InsertStreamSource( int iStream, VertexComponent eComponent )
{
	m_VertexStreamList.insert( ioStreamElement( iStream, eComponent ) );
	m_bNeedUpdate = true;
}

void ioVertexDeclaration::RemoveStreamSource( int iStream, DWORD dwMask )
{
	VertexComponentList kComponentList;
	ParseStreamMaskToComponent( dwMask, kComponentList );

	VertexComponentList::iterator iter;
	for( iter=kComponentList.begin() ; iter!=kComponentList.end() ; ++iter )
	{
		RemoveStreamSource( iStream, *iter );
	}
}

void ioVertexDeclaration::RemoveStreamSource( int iStream, VertexComponent eComponent )
{
	m_VertexStreamList.erase( ioStreamElement( iStream, eComponent ) );
	m_bNeedUpdate = true;
}

void ioVertexDeclaration::ModifyStreamSource( int iPreStream, DWORD dwPreMask,
											  int iNewStream, DWORD dwNewMask )
{
	VertexComponentList kPreComponentList, kNewComponentList;

	ParseStreamMaskToComponent( dwPreMask, kPreComponentList );
	ParseStreamMaskToComponent( dwNewMask, kNewComponentList );

	if( kPreComponentList.size() != kNewComponentList.size() )
		return;

	VertexComponentList::iterator iPreIter, iNewIter;
	iPreIter = kPreComponentList.begin();
	iNewIter = kNewComponentList.begin();

	int iComponentCnt = kPreComponentList.size();
	for( int i=0 ; i<iComponentCnt ; i++, ++iPreIter, ++iNewIter )
	{
		ModifyStreamSource( iPreStream, *iPreIter, iNewStream, *iNewIter );
	}
}

void ioVertexDeclaration::ModifyStreamSource( int iPreStream,
											  VertexComponent ePreComponent,
											  int iNewStream,
											  VertexComponent eNewComponent )
{
	if( m_VertexStreamList.erase( ioStreamElement( iPreStream, ePreComponent ) ) > 0 )
	{
		m_VertexStreamList.insert( ioStreamElement( iNewStream, eNewComponent ) );
		m_bNeedUpdate = true;
	}
}

DWORD ioVertexDeclaration::GetFullStreamMask() const
{
	DWORD dwFullMask = 0;

	StreamElementList::const_iterator iter;
	for( iter=m_VertexStreamList.begin() ; iter!=m_VertexStreamList.end() ; ++iter )
	{
		dwFullMask |= iter->m_Component;
	}

	return dwFullMask;
}

DWORD ioVertexDeclaration::GetStreamMask( int iStream ) const
{
	DWORD dwStreamMask = 0;

	StreamElementList::const_iterator iter;
	for( iter=m_VertexStreamList.begin() ; iter!=m_VertexStreamList.end() ; ++iter )
	{
		if( iter->m_iStream == iStream )
		{
			dwStreamMask |= iter->m_Component;
		}
	}

	return dwStreamMask;
}

void ioVertexDeclaration::RemoveStreamGap()
{
	if( m_VertexStreamList.empty() )
		return;

	StreamElementList kCopyStream( m_VertexStreamList );
	m_VertexStreamList.clear();

	int iTargetStream = 0;

	StreamElementList::iterator iter=kCopyStream.begin();
	int iLastStream = iter->m_iStream;
	
	for( ; iter!=kCopyStream.end() ; ++iter )
	{
		int iElementStream = iter->m_iStream;

		if( iLastStream != iElementStream )
		{
			iTargetStream++;
			iLastStream = iElementStream;
		}

		m_VertexStreamList.insert( ioStreamElement( iTargetStream, iter->m_Component ) );
	}	
}

int ioVertexDeclaration::GetMaxStream() const
{
	int iMaxStream = 0;

	StreamElementList::const_iterator iter;
	for( iter=m_VertexStreamList.begin() ; iter!=m_VertexStreamList.end() ; ++iter )
	{
		if( iter->m_iStream > iMaxStream )
			iMaxStream = iter->m_iStream;
	}

	return iMaxStream;
}

IDirect3DVertexDeclaration9* ioVertexDeclaration::GetD3DDeclaration()
{
	if( m_bNeedUpdate )
	{
		RemoveStreamGap();

		m_bNeedUpdate = false;

		m_pVertexDeclaration = g_VtxBufMgr.GetD3DVertexDeclaration( *this );
	}

	return m_pVertexDeclaration;
}
