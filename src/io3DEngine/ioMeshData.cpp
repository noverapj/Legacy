

#include "stdafx.h"

#include "ioVertexDeclaration.h"
#include "ioVertexBufferBinder.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"

#include "ioMeshData.h"

ioMeshData::ioMeshData()
{
	m_pVertexDeclaration	= new ioVertexDeclaration;
	m_pVertexBufferBinder	= new ioVertexBufferBinder;
	m_pIndexBufferInstance	= NULL;

	m_iVertexCount = 0;
	m_iIndexCount  = 0;
}

ioMeshData::~ioMeshData()
{
	SAFEDELETE( m_pVertexDeclaration );
	SAFEDELETE( m_pVertexBufferBinder );
	m_pIndexBufferInstance = NULL;
}

void ioMeshData::InsertStreamSource( int iStream, DWORD dwMask )
{
	m_pVertexDeclaration->InsertStreamSource( iStream, dwMask );
}

void ioMeshData::InsertStreamSource( int iStream, VertexComponent eComponent )
{
	m_pVertexDeclaration->InsertStreamSource( iStream, eComponent );
}

void ioMeshData::SetBinding( UINT uIndex, ioVertexBufferInstance *pInstance )
{
	m_pVertexBufferBinder->SetBinding( uIndex, pInstance );
}

ioVertexBufferInstance* ioMeshData::GetVtxInstance( UINT uIndex )
{
	return m_pVertexBufferBinder->GetBufferInstance( uIndex );
}

ioVertexBufferInstance* ioMeshData::GetVtxInstanceByMask( DWORD dwMask )
{
	return m_pVertexBufferBinder->GetBufferInstanceByMask( dwMask );
}

void ioMeshData::DeleteAllBuffers()
{
	DeleteAllBindingBuffers();
	if( m_pIndexBufferInstance )
	{
		g_IdxBufMgr.DestroyInstance( m_pIndexBufferInstance );
		m_pIndexBufferInstance = NULL;
	}
}

void ioMeshData::DeleteAllBindingBuffers()
{
	const ioVertexBufferBinder::VertexBufferBindList &rkBindList = m_pVertexBufferBinder->GetBufferList();

	ioVertexBufferBinder::VertexBufferBindList::const_iterator iter;
	for( iter=rkBindList.begin() ; iter!=rkBindList.end() ; ++iter )
	{
		g_VtxBufMgr.DestroyInstance( iter->m_pInstance );
	}

	m_pVertexBufferBinder->UnSetAllBindings();
}

