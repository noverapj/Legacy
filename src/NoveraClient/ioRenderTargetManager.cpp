#include "stdafx.h"

#include "ioRenderTargetManager.h"

template<> ioRenderTargetManager* Singleton< ioRenderTargetManager >::ms_Singleton = 0;

ioRenderTargetManager::ioRenderTargetManager() : m_RenderTargetIndex( 0 )
{	
}

ioRenderTargetManager::~ioRenderTargetManager()
{
}

ioRenderTargetManager& ioRenderTargetManager::GetSingleton()
{
	return Singleton< ioRenderTargetManager >::GetSingleton();
}

int ioRenderTargetManager::AddRenderTarget( ioRenderTarget* pTarget )
{
	m_RenderTargetMap.insert( RenderTargetMap::value_type( ++m_RenderTargetIndex, pTarget ) );
	return m_RenderTargetIndex;
}

void ioRenderTargetManager::RemoveRenderTarget( int Index )
{	
	RenderTargetMap::iterator iter = m_RenderTargetMap.find( Index );
	if( iter != m_RenderTargetMap.end() )
	{
		m_RenderTargetMap.erase( iter );
	}
}

void ioRenderTargetManager::OnRender()
{
	for( RenderTargetMap::iterator iter = m_RenderTargetMap.begin(); iter != m_RenderTargetMap.end(); ++iter )
	{
		ioRenderTarget* pRender = iter->second;
		if( pRender )
		{
			pRender->OnRenderTarget();
		}
	}
}