

#ifndef _ioRenderQueue_h_
#define _ioRenderQueue_h_

class ioRenderQueueGroup;
class ioCamera;

#include "ioRenderable.h"
#include "ioPass.h"
#include "ioTechnique.h"
#include "ioMapIterator.h"

class __EX ioRenderQueue
{
public:
	typedef std::map< RenderGroupID, ioRenderQueueGroup* > RenderQueueGroupMap;
	typedef ioMapIterator< RenderQueueGroupMap > QueueGroupIterator;

protected:
	RenderQueueGroupMap m_QueueGroups;
	ioRenderQueueGroup *m_pCacheGroup;

	RenderGroupID m_DefaultGroupID;
	WORD m_wDefaultRenderablePriority;

public:
	void AddRenderable( ioRenderable *pRend );
	void AddRenderable( ioRenderable *pRend, RenderGroupID eGroupID );
	void AddRenderable( ioRenderable *pRend, RenderGroupID eGroupID, WORD wPriority );

	void AddGenerateShadowMapRenderable( ioRenderable *pRend );
	void AddGenerateShadowMapRenderable( ioRenderable *pRend,
										 RenderGroupID eGroupID );
	void AddGenerateShadowMapRenderable( ioRenderable *pRend,
										 RenderGroupID eGroupID,
										 WORD wPriority );

	void Clear( bool bDestoryPassMaps = false );
	void SortQueue( const ioCamera *pCamera );

public:
	void SetDefaultGroup( RenderGroupID eGroupID );
	void SetDefaultRenderablePriority( WORD wPriority );

	RenderGroupID GetDefaultGroup() const;
	WORD GetDefaultRenderablePriority() const;

public:
	ioRenderQueueGroup* GetQueueGroup( RenderGroupID eGroupID );
	QueueGroupIterator GetIterator();

public:
	ioRenderQueue();
	virtual ~ioRenderQueue();
};

#endif
