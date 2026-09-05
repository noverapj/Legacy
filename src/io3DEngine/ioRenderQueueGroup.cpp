

#include "stdafx.h"

#include "ioRenderable.h"
#include "ioRenderQueueGroup.h"

#include "ioMaterial.h"
#include "ioTechnique.h"
#include "ErrorReport.h"

ioRenderPriorityGroup::ioRenderPriorityGroup( WORD wPriority )
{
	m_OpaquePass.Initialize( 1000, 200 );
	m_TransparentPass.Initialize( 600, 200 );
	m_GenerateShadowMapPass.Initialize( 200, 100 );

	m_wPriority = wPriority;
}

ioRenderPriorityGroup::~ioRenderPriorityGroup()
{
	ClearQueue();
}

void ioRenderPriorityGroup::AddRenderable( ioRenderable *pRend )
{
	ioTechnique *pTech = pRend->GetTechnique();
	if( !pTech )	return;
	if( !pTech->IsLoadDone() )	return;

	int iAlphaRate = pRend->GetAlphaRate();
	if( iAlphaRate > 0 )
	{
		if( iAlphaRate == MAX_ALPHA_RATE && !pTech->IsTransparent() )
		{
			if( pTech->IsDepthWriteEnabled() && pTech->IsDepthCheckEnabled() )
			{
				AddRenderableToList( pTech, pRend, m_OpaquePass );
			}
			else
			{
				AddRenderableToList( pTech, pRend, m_TransparentPass );
			}
		}
		else
		{
			AddRenderableToList( pTech, pRend, m_TransparentPass );
		}
	}
}

void ioRenderPriorityGroup::AddGenerateShadowMapRenderable( ioRenderable *pRend )
{
	ioTechnique *pTech = pRend->GetTechnique();
	if( !pTech || pTech->IsTransparent() )	return;

	if( pRend->GetAlphaRate() < MAX_ALPHA_RATE )
		return;

	int iPassCnt = pTech->GetNumPasses();
	for( int i=0 ; i<iPassCnt ; i++ )
	{
		ioPass *pCurPass = pTech->GetPass(i);

		if( pCurPass && pCurPass->IsPassEnable() && pCurPass->IsCanCastShadow() )
		{
			RenderablePass kPass( pCurPass, pRend );
			kPass.pShader = kPass.pPass->BindActiveShader( NULL, true, false );
			if( kPass.pShader )
			{
				m_GenerateShadowMapPass.Add( kPass );
				break;	// ÇÑ¹ø¸¸..
			}
		}
	}
}

void ioRenderPriorityGroup::AddRenderableToList( ioTechnique *pTech,
												 ioRenderable *pRend,
												 ioRenderableList &rkList )
{
	ioTechnique::PassIterator iter = pTech->GetPassIterator();

	ioPass *pPass = NULL;
	while( iter.HasMoreElements() )
	{
		pPass = iter.Next();
		if( pPass->IsPassEnable() )
		{
			BindRenderablePassShader( pPass, pRend, rkList );
		}
	}
}

void ioRenderPriorityGroup::BindRenderablePassShader( ioPass *pPass,
													  ioRenderable *pRend,
													  ioRenderableList &rkList )
{
	bool bRecvShadow = pRend->IsShadowRecvRender();

	RenderablePass kPass( pPass, pRend );

	if( pPass->IsLightingEnable() )
	{
		const ioLightList &rkLightList = pRend->GetLights();

		if( pPass->HasProperty( ioPass::PP_LIGHT_ITERATE_PASS ) )
		{
			int iLightCnt = rkLightList.size();
			for( int i=0 ; i<iLightCnt ; i++ )
			{
				ioLight *pCurLight = rkLightList[i];
				if( pPass->IsLightAvailable( pCurLight ) )
				{
					kPass.pShader = pPass->BindActiveShader( pCurLight, false, bRecvShadow );
					kPass.iLightIndex = i;
					rkList.Add( kPass );
				}
			}
		}
		else
		{
			kPass.pShader = pPass->BindActiveShader( rkLightList, false, bRecvShadow );
			rkList.Add( kPass );
		}
	}
	else
	{
		kPass.pShader = pPass->BindActiveShader( NULL, false, bRecvShadow );
		rkList.Add( kPass );
	}
}

void ioRenderPriorityGroup::SortQueue( const ioCamera *pCamera )
{
	if( m_OpaquePass.GetCount() > 0 )
	{
		std::sort( m_OpaquePass.GetPtr(),
				   m_OpaquePass.GetPtr() + m_OpaquePass.GetCount(),
				   OpaquePassLess() );
	}

	if( m_GenerateShadowMapPass.GetCount() > 0 )
	{
		std::sort( m_GenerateShadowMapPass.GetPtr(),
				   m_GenerateShadowMapPass.GetPtr() + m_GenerateShadowMapPass.GetCount(),
				   ShadowPassLess() );
	}

	if( m_TransparentPass.GetCount() > 0 )
	{
		TransparentPassLess transFunctor;
		transFunctor.pCamera = pCamera;

		std::stable_sort( m_TransparentPass.GetPtr(),
						  m_TransparentPass.GetPtr() + m_TransparentPass.GetCount(),
						  transFunctor );
	}
}

void ioRenderPriorityGroup::ClearQueue()
{
	m_OpaquePass.Clear();
	m_TransparentPass.Clear();
	m_GenerateShadowMapPass.Clear();
}

//------------------------------------------------------------------------------------------------

ioRenderQueueGroup::ioRenderQueueGroup( RenderGroupID eGroupID )
{
	m_GroupID = eGroupID;
	
	m_pCachedGroup = new ioRenderPriorityGroup( RENDERABLE_DEFAULT_PRIORITY );
	if( m_pCachedGroup )
	{
		m_PriorityGroups.insert( PriorityMap::value_type( RENDERABLE_DEFAULT_PRIORITY, m_pCachedGroup ) );
	}
}

ioRenderQueueGroup::~ioRenderQueueGroup()
{
	PriorityMap::iterator iter;
	for( iter=m_PriorityGroups.begin() ; iter!=m_PriorityGroups.end() ; ++iter )
	{
		delete iter->second;
	}
	m_PriorityGroups.clear();
}

ioRenderQueueGroup::PriorityMapIterator ioRenderQueueGroup::GetIterator()
{
	return PriorityMapIterator( m_PriorityGroups.begin(), m_PriorityGroups.end() );
}

void ioRenderQueueGroup::AddRenderable( ioRenderable *pRend, WORD wPriority )
{
	ioRenderPriorityGroup *pPriorityGrp = GetPriorityGroup( wPriority );
	if( pPriorityGrp )
	{
		pPriorityGrp->AddRenderable( pRend );
	}
}

void ioRenderQueueGroup::AddGenerateShadowMapRenderable( ioRenderable *pRend, WORD wPriority )
{
	ioRenderPriorityGroup *pPriorityGrp = GetPriorityGroup( wPriority );
	if( pPriorityGrp )
	{
		pPriorityGrp->AddGenerateShadowMapRenderable( pRend );
	}
}

ioRenderPriorityGroup* ioRenderQueueGroup::GetPriorityGroup( WORD wPriority )
{
	if( m_pCachedGroup && m_pCachedGroup->GetPriority() == wPriority )
		return m_pCachedGroup;

	PriorityMap::iterator iter = m_PriorityGroups.find( wPriority );
	if( iter != m_PriorityGroups.end() )
	{
		m_pCachedGroup = iter->second;
	}
	else
	{
		m_pCachedGroup = new ioRenderPriorityGroup( wPriority );
		if( m_pCachedGroup )
		{
			m_PriorityGroups.insert( PriorityMap::value_type( wPriority, m_pCachedGroup ) );
		}
	}

	return m_pCachedGroup;
}

void ioRenderQueueGroup::SortGroup( const ioCamera *pCamera )
{
	PriorityMap::iterator iter;
	for( iter=m_PriorityGroups.begin() ; iter!=m_PriorityGroups.end() ; ++iter )
	{
		iter->second->SortQueue( pCamera );
	}
}

void ioRenderQueueGroup::Clear( bool bDestroy )
{
	if( !bDestroy )
	{
		PriorityMap::iterator iter=m_PriorityGroups.begin();
		for( ; iter!=m_PriorityGroups.end() ; ++iter )
		{
			iter->second->ClearQueue();
		}
	}
	else
	{
		PriorityMap::iterator iter=m_PriorityGroups.begin();
		for( ; iter!=m_PriorityGroups.end() ; ++iter )
		{
			delete iter->second;
		}

		m_PriorityGroups.clear();
	}
}
