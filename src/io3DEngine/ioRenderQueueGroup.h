

#ifndef _ioRenderQueueGroup_h_
#define _ioRenderQueueGroup_h_

#include "ioRenderable.h"
#include "ioPass.h"
#include "ioTechnique.h"
#include "ioMapIterator.h"
#include "ioRenderableList.h"

class ioCamera;

class ioRenderPriorityGroup
{
private:
	struct OpaquePassLess
	{
		bool operator()( const RenderablePass &a, const RenderablePass &b ) const
		{
			if( a.pPass->GetPassIndex() < b.pPass->GetPassIndex() )
				return true;
			else if( a.pPass->GetPassIndex() == b.pPass->GetPassIndex() )
			{
				if( a.pShader < b.pShader )
					return true;
				else if( a.pShader == b.pShader )
				{
					return ( a.pRend->GetMeshHash() < b.pRend->GetMeshHash() );
				}
			}

			return false;
		}
	};

	struct TransparentPassLess
	{
		const ioCamera *pCamera;

		bool operator()( const RenderablePass &a, const RenderablePass &b ) const
		{
			if( a.pRend != b.pRend )
			{
				float fDepthA = a.pRend->GetSquaredViewDepth( pCamera );
				float fDepthB = b.pRend->GetSquaredViewDepth( pCamera );

				return ( fDepthA > fDepthB );
			}

			return ( a.pPass->GetHash() < b.pPass->GetHash() );
		}
	};

	struct ShadowPassLess
	{
		bool operator()( const RenderablePass &a, const RenderablePass &b ) const
		{
			if( a.pShader < b.pShader )
				return true;
			else if( a.pShader == b.pShader )
			{
				return ( a.pRend->GetMeshHash() < b.pRend->GetMeshHash() );
			}

			return false;
		}
	};

private:
	ioRenderableList m_OpaquePass;
	ioRenderableList m_TransparentPass;
	ioRenderableList m_GenerateShadowMapPass;
	WORD m_wPriority;

public:
	void AddRenderable( ioRenderable *pRend );
	void AddGenerateShadowMapRenderable( ioRenderable *pRend );

private:
	void AddRenderableToList( ioTechnique *pTech, ioRenderable *pRend, ioRenderableList &rkList );
	void BindRenderablePassShader( ioPass *pPass, ioRenderable *pRend, ioRenderableList &rkList );

public:
	void SortQueue( const ioCamera *pCamera );
	void ClearQueue();

	inline const ioRenderableList& GetOpaquePasses() const
	{
		return m_OpaquePass;
	}
	inline const ioRenderableList& GetTransparentPasses() const
	{
		return m_TransparentPass;
	}
	inline const ioRenderableList& GetGenerateShadowMapPasses() const
	{
		return m_GenerateShadowMapPass;
	}

public:
	inline WORD GetPriority() const { return m_wPriority; }

public:
	ioRenderPriorityGroup( WORD wPriority );
	~ioRenderPriorityGroup();
};

class ioRenderQueueGroup
{
public:
	typedef std::map< WORD, ioRenderPriorityGroup*, std::less<WORD> > PriorityMap;
	typedef ioMapIterator< PriorityMap > PriorityMapIterator;

private:
	RenderGroupID	m_GroupID;

	PriorityMap		m_PriorityGroups;
	ioRenderPriorityGroup *m_pCachedGroup;

public:
	void AddRenderable( ioRenderable *pRend, WORD wPriority );
	void AddGenerateShadowMapRenderable( ioRenderable *pRend, WORD wPriority );

	void SortGroup( const ioCamera *pCamera );
	void Clear( bool bDestroy = false );

private:
	ioRenderPriorityGroup* GetPriorityGroup( WORD wPriority );

public:
	inline RenderGroupID GetGroupID() const { return m_GroupID; }
	PriorityMapIterator GetIterator();

public:
	ioRenderQueueGroup( RenderGroupID eGroupID );
	~ioRenderQueueGroup();
};

#endif
