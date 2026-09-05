

#include "stdafx.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferBinder.h"

ioVertexBufferBinder::ioVertexBufferBinder()
{
}

ioVertexBufferBinder::~ioVertexBufferBinder()
{
	UnSetAllBindings();
}

void ioVertexBufferBinder::SetBinding( UINT uIndex, ioVertexBufferInstance *pInstance )
{
	VertexBufferBindList::iterator iter;
	for( iter=m_BindStreamList.begin() ; iter!=m_BindStreamList.end() ; ++iter )
	{
		if( iter->m_uStream == uIndex )
		{
			iter->m_pInstance = pInstance;
			return;
		}
	}

	// New Instance..
	m_BindStreamList.push_back( VertexBufferStream( uIndex, pInstance ) );

	if( m_BindStreamList.size() > 1 )
	{
		std::sort( m_BindStreamList.begin(), m_BindStreamList.end() );
	}
}

ioVertexBufferInstance* ioVertexBufferBinder::UnSetBinding( UINT uIndex )
{
	ioVertexBufferInstance *pInstance = NULL;

	VertexBufferBindList::iterator iter;
	for( iter=m_BindStreamList.begin() ; iter!=m_BindStreamList.end() ; ++iter )
	{
		if( iter->m_uStream == uIndex )
		{
            pInstance = iter->m_pInstance;
			m_BindStreamList.erase( iter );
			return pInstance;
		}
	}

	return NULL;
}

ioVertexBufferInstance* ioVertexBufferBinder::UnSetBindingByMask( DWORD dwMask )
{
	ioVertexBufferInstance *pInstance = NULL;

	VertexBufferBindList::iterator iter;
	for( iter=m_BindStreamList.begin() ; iter!=m_BindStreamList.end() ; ++iter )
	{
		pInstance = iter->m_pInstance;
		if( pInstance->GetVertexMask() == dwMask )
		{
			m_BindStreamList.erase( iter );
			return pInstance;
		}
	}

	return NULL;
}

void ioVertexBufferBinder::UnSetAllBindings()
{
	m_BindStreamList.clear();
}

ioVertexBufferInstance* ioVertexBufferBinder::GetBufferInstance( UINT uIndex ) const
{
	VertexBufferBindList::const_iterator iter;
	for( iter=m_BindStreamList.begin() ; iter!=m_BindStreamList.end() ; ++iter )
	{
		if( iter->m_uStream == uIndex )
			return iter->m_pInstance;
	}

	return NULL;
}

ioVertexBufferInstance* ioVertexBufferBinder::GetBufferInstanceByMask( DWORD dwMask ) const
{
	VertexBufferBindList::const_iterator iter;
	for( iter=m_BindStreamList.begin() ; iter!=m_BindStreamList.end() ; ++iter )
	{
		if( iter->m_pInstance->GetVertexMask() == dwMask )
			return iter->m_pInstance;
	}

	return NULL;
}
