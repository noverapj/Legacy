

#include "stdafx.h"

#include "ioRenderQueue.h"
#include "ioRenderQueueGroup.h"

ioRenderQueue::ioRenderQueue()
{
	m_pCacheGroup = new ioRenderQueueGroup( RENDER_MAIN );
	if( m_pCacheGroup )
	{
		m_QueueGroups.insert( RenderQueueGroupMap::value_type( RENDER_MAIN, m_pCacheGroup ) );
	}

	m_DefaultGroupID = RENDER_MAIN;
	m_wDefaultRenderablePriority = RENDERABLE_DEFAULT_PRIORITY;
}

ioRenderQueue::~ioRenderQueue()
{
	RenderQueueGroupMap::iterator iter;
	for( iter=m_QueueGroups.begin() ; iter!=m_QueueGroups.end() ; ++iter )
	{
		delete iter->second;
	}
	m_QueueGroups.clear();

	m_pCacheGroup = NULL;
}

void ioRenderQueue::AddRenderable( ioRenderable *pRend )
{
	AddRenderable( pRend, m_DefaultGroupID, m_wDefaultRenderablePriority );
}

void ioRenderQueue::AddRenderable( ioRenderable *pRend, RenderGroupID eGroupID )
{
	AddRenderable( pRend, eGroupID, m_wDefaultRenderablePriority );
}

void ioRenderQueue::AddRenderable( ioRenderable *pRend, RenderGroupID eGroupID, WORD wPriority )
{
	ioRenderQueueGroup *pGrp = GetQueueGroup( eGroupID );
	if( pGrp )
	{
		// touch material;

		pGrp->AddRenderable( pRend, wPriority );
	}
}

void ioRenderQueue::AddGenerateShadowMapRenderable( ioRenderable *pRend )
{
	AddGenerateShadowMapRenderable( pRend, m_DefaultGroupID, m_wDefaultRenderablePriority );
}

void ioRenderQueue::AddGenerateShadowMapRenderable( ioRenderable *pRend,
													RenderGroupID eGroupID )
{
	AddGenerateShadowMapRenderable( pRend, eGroupID, m_wDefaultRenderablePriority );
}

void ioRenderQueue::AddGenerateShadowMapRenderable( ioRenderable *pRend,
													RenderGroupID eGroupID,
													WORD wPriority )
{
	ioRenderQueueGroup *pGrp = GetQueueGroup( eGroupID );
	if( pGrp )
	{
		// touch material

		pGrp->AddGenerateShadowMapRenderable( pRend, wPriority );
	}
}

void ioRenderQueue::Clear( bool bDestoryPassMaps )
{
	RenderQueueGroupMap::iterator iter;
	for( iter=m_QueueGroups.begin() ; iter!=m_QueueGroups.end() ; ++iter )
	{
		iter->second->Clear( bDestoryPassMaps );
	}
}

void ioRenderQueue::SortQueue( const ioCamera *pCamera )
{
	RenderQueueGroupMap::iterator iter;
	for( iter=m_QueueGroups.begin() ; iter!=m_QueueGroups.end() ; ++iter )
	{
		iter->second->SortGroup( pCamera );
	}
}

void ioRenderQueue::SetDefaultGroup( RenderGroupID eGroupID )
{
	m_DefaultGroupID = eGroupID;
}

void ioRenderQueue::SetDefaultRenderablePriority( WORD wPriority )
{
	m_wDefaultRenderablePriority = wPriority;
}

RenderGroupID ioRenderQueue::GetDefaultGroup() const
{
	return m_DefaultGroupID;
}

WORD ioRenderQueue::GetDefaultRenderablePriority() const
{
	return m_wDefaultRenderablePriority;
}

ioRenderQueueGroup* ioRenderQueue::GetQueueGroup( RenderGroupID eGroupID )
{
	if( m_pCacheGroup && m_pCacheGroup->GetGroupID() == eGroupID )
		return m_pCacheGroup;

	RenderQueueGroupMap::iterator iter = m_QueueGroups.find( eGroupID );
	if( iter != m_QueueGroups.end() )
	{
		m_pCacheGroup = iter->second;
	}
	else
	{
		m_pCacheGroup = new ioRenderQueueGroup( eGroupID );
		if( m_pCacheGroup )
		{
			m_QueueGroups.insert( RenderQueueGroupMap::value_type( eGroupID, m_pCacheGroup ) );
		}
	}

	return m_pCacheGroup;
}

ioRenderQueue::QueueGroupIterator ioRenderQueue::GetIterator()
{
	return QueueGroupIterator( m_QueueGroups.begin(), m_QueueGroups.end() );
}