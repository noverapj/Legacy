

#include "stdafx.h"

#include "ioNode.h"
#include "ioSceneNode.h"

#include "ioCamera.h"
#include "ioMath.h"

#include "ioTextureEffectManager.h"
#include "ioSceneShadowBox.h"

#include "ioRenderOperation.h"
#include "ioRenderSystem.h"
#include "ioTextureUnitState.h"
#include "ioRenderTexture.h"
#include "ioTextureManager.h"

#include "ioEntityGroup.h"
#include "ioSceneManager.h"

#include "ioGlowPostProcess.h"
#include "ioPostFilter.h"
#include "ioPostOveray.h"

#include "ioMaterialModifier.h"
#include "ioMaterial.h"

#include "ioShader.h"
#include "HelpFunc.h"
#include "ioEffectBufferManager.h"
#include "ErrorReport.h"

bool ioSceneManager::m_bSoftwareVertexBlending = false;

ioSceneManager::ioSceneManager( ioRenderSystem *pRenderSystem )
{
	m_pSceneRoot = new ioSceneNode( this );

	m_pRenderSystem = pRenderSystem;

	m_ShadowMapType = SMT_NOT_ENABLE;
	m_pShadowMapRT = NULL;
	
	m_vShadowLightDir = D3DXVECTOR3( -1.0f, -1.0f, FLOAT05 );
	D3DXVec3Normalize( &m_vShadowLightDir, &m_vShadowLightDir );

	m_iShadowMapSize = 1024;

	m_pPostTarget1 = NULL;
	m_pPostTarget2 = NULL;

	m_pPostTarget3 = NULL;
	m_pPostTarget4 = NULL;

	m_pPostGlow = NULL;
	m_pPostGlowSecond = NULL;
	m_pPostFilter  = NULL;
	m_pPostOveray  = NULL;

	m_iGlowAlphaValue = 51;
	m_iGlowSecondAlphaValue = 0;
	m_iFilerAlphaValue = 102;

	m_bDrawAllTransObjForGlow = false;

	for( int i=0 ; i<2 ; i++ )
	{
		m_TextureFilterOpt[i]  = TFO_BILINEAR;
		m_TextureMipMapBias[i] = 0.0f;
	}

	m_bNowFog  = false;
}

ioSceneManager::~ioSceneManager()
{
	ClearScene();
	ClearPostProcess();
	ClearShadowMapResource();

	SAFEDELETE( m_pSceneRoot );
}

void ioSceneManager::UpdateSceneGraph( ioCamera *pCamera, const D3DXMATRIX *pExtraProj, bool bResetVolume )
{
	ErrorReport::SetEnginePos( 1, 1 );
	if( ioTextureEffectManager::GetSingletonPtr() )
	{
		ioTextureEffectManager::GetSingleton().UpdateAllEffect();
	}
	ErrorReport::SetEnginePos( 1, 2 );

	m_pSceneRoot->Update( true, false, bResetVolume );
	ErrorReport::SetEnginePos( 1, 3 );

	UpdatePickRay( pCamera );
	ErrorReport::SetEnginePos( 1, 4 );

	m_pRenderSystem->SetViewMatrix( pCamera->GetViewMatrix() );
	ErrorReport::SetEnginePos( 1, 5 );

	if( !pExtraProj )
	{
		m_pRenderSystem->SetProjMatrix( pCamera->GetProjMatrix() );
		ErrorReport::SetEnginePos( 1, 6 );
	}
	else
	{
		D3DXMATRIX matProj;
		D3DXMatrixMultiply( &matProj, pCamera->GetProjMatrix(), pExtraProj );
		m_pRenderSystem->SetProjMatrix( &matProj );
		ErrorReport::SetEnginePos( 1, 7 );
	}

	m_AutoShaderParamSource.SetCurrentCamera( pCamera );
	m_AutoShaderParamSource.SetAmbientLightColor( m_pRenderSystem->GetAmbientColor() );
	m_AutoShaderParamSource.SetFogFactor( m_pRenderSystem->GetFogFactor() );
	m_AutoShaderParamSource.SetViewPort( m_pRenderSystem->GetViewPort() );
	m_AutoShaderParamSource.SetExtraProjectionMatrix( pExtraProj );
	ErrorReport::SetEnginePos( 1, 8 );

	m_RenderQueue.Clear();
	g_EffectBuffer.ClearRenderableLists();
	ErrorReport::SetEnginePos( 1, 9 );

	FindVisibleObjects( pCamera );
	ErrorReport::SetEnginePos( 1, 10 );

	if( GetShadowMapType() != SMT_NOT_ENABLE )
	{
		FindShadowCasterObjects( pCamera );
		ErrorReport::SetEnginePos( 1, 11 );
		UpdateShadowMapMatrix( pCamera );
		ErrorReport::SetEnginePos( 1, 12 );
	}

	g_EffectBuffer.UpdateRenderQueue( m_RenderQueue );
	ErrorReport::SetEnginePos( 1, 13 );

	m_RenderQueue.SortQueue( pCamera );
}

ioEntityGroup* ioSceneManager::FindPickEntityGroup()
{
	if( m_EntityGroupMap.empty() )
		return NULL;

	float fMinDist = ioMath::FLOAT_INFINITY;
	ioEntityGroup *pPickObj = NULL;

	EntityGroupMap::iterator iter;
	for( iter=m_EntityGroupMap.begin() ; iter!=m_EntityGroupMap.end() ; ++iter )
	{
		ioEntityGroup *pCurObj = iter->second;

		if( !ioMath::TestIntersection( m_PickRay, pCurObj->GetWorldBoundBox() ) )
			continue;

		D3DXVECTOR3 vDiff = m_PickRay.GetOrigin() - pCurObj->GetParentNode()->GetDerivedPosition();
		float fCurDist = D3DXVec3LengthSq( &vDiff );

		if( fMinDist > fCurDist )
		{
			pPickObj = pCurObj;
			fMinDist = fCurDist;
		}
	}

	return pPickObj;
}

void ioSceneManager::FindVisibleObjects( ioCamera *pCamera )
{
	m_pSceneRoot->FindVisibleObjects( pCamera, m_RenderQueue );
}

void ioSceneManager::FindShadowCasterObjects( ioCamera *pCamera )
{
	ioSceneShadowBox kSceneBox( pCamera, m_vShadowLightDir );
	m_pSceneRoot->FindShadowCasterObjects( m_RenderQueue, kSceneBox );

	kSceneBox.CreateShadowFrustum( m_vShadowAreaList );
}

void ioSceneManager::RenderVisibleObject()
{
	if( m_pRenderSystem )
	{
		m_pRenderSystem->ClearRenderStateDesc();
	}

	m_AutoShaderParamSource.SetCurrentPass( NULL );
	m_AutoShaderParamSource.SetCurrentRenderable( NULL );

	RenderQueueFlush( m_RenderQueue );

	if( m_pRenderSystem )
		m_pRenderSystem->SetSoftwareVertexProcessing( true );

	m_RenderQueue.Clear();

	g_EffectBuffer.ClearRenderableLists();
}


void ioSceneManager::RenderVisibleGlowObject()
{
	if( m_pRenderSystem )
	{
		m_pRenderSystem->ClearRenderStateDesc();
	}

	m_AutoShaderParamSource.SetCurrentPass( NULL );
	m_AutoShaderParamSource.SetCurrentRenderable( NULL );

	RenderQueueFlushGlow( m_RenderQueue );

	m_pRenderSystem->SetSoftwareVertexProcessing( true );

	//m_RenderQueue.Clear();

	//g_EffectBuffer.ClearRenderableLists();
}


void ioSceneManager::RenderQueueFlushGlow( ioRenderQueue &rkQueue )
{
	ioRenderQueue::QueueGroupIterator queueIter = rkQueue.GetIterator();
	while( queueIter.HasMoreElements() )
	{
		ioRenderQueueGroup *pGrp = queueIter.Next();
		if( pGrp )
		{
			RenderQueueGroupObjectsGlow( pGrp );
		}
	}
}

void ioSceneManager::RenderQueueFlush( ioRenderQueue &rkQueue )
{
	ioRenderQueue::QueueGroupIterator queueIter = rkQueue.GetIterator();
	while( queueIter.HasMoreElements() )
	{
		ioRenderQueueGroup *pGrp = queueIter.Next();
		if( pGrp )
		{
			RenderQueueGroupObjects( pGrp );
		}
	}
}

void ioSceneManager::RenderQueueGroupObjects( ioRenderQueueGroup *pGrp )
{
	ioRenderQueueGroup::PriorityMapIterator groupIter = pGrp->GetIterator();
	while( groupIter.HasMoreElements() )
	{
		ioRenderPriorityGroup *pPriorityGrp = groupIter.Next();
		if( pPriorityGrp )
		{
			m_pRenderSystem->SetFogEnable( m_bNowFog );
			RenderOpaqueObjects( pPriorityGrp->GetOpaquePasses() );

			m_pRenderSystem->SetFogEnable( false );
			RenderTransparentObjects( pPriorityGrp->GetTransparentPasses() );
		}
	}
}


void ioSceneManager::RenderQueueGroupObjectsGlow( ioRenderQueueGroup *pGrp )
{
	ioRenderQueueGroup::PriorityMapIterator groupIter = pGrp->GetIterator();
	while( groupIter.HasMoreElements() )
	{
		ioRenderPriorityGroup *pPriorityGrp = groupIter.Next();
		if( pPriorityGrp )
		{
			m_pRenderSystem->SetFogEnable( m_bNowFog );
			RenderOpaqueObjectsLocalGlow( pPriorityGrp->GetOpaquePasses() );

			m_pRenderSystem->SetFogEnable( false );
			if( m_bDrawAllTransObjForGlow )
				RenderTransparentObjects( pPriorityGrp->GetTransparentPasses() );
			else
				RenderTransparentObjectsLocalGlow( pPriorityGrp->GetTransparentPasses() );
		}
	}
}

void ioSceneManager::ClearScene()
{
	DestroyAllEntityGrp();
	DestroyAllLights();

	m_pSceneRoot->RemoveAllChildren();
	m_pSceneRoot->DetachAllObjects();

	SceneNodeMap::iterator iter = m_SceneNodeMap.begin();
	for( ; iter != m_SceneNodeMap.end() ; ++iter )
	{
		SAFEDELETE( iter->second );
	}

	m_SceneNodeMap.clear();
}

void ioSceneManager::UpdatePickRay( ioCamera *pCamera )
{
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( m_pRenderSystem->GetHWND(), &ptCursor );

	m_PickRay = Calc2DTo3DRay( pCamera, ptCursor );
}

ioRay ioSceneManager::Calc2DTo3DRay( ioCamera *pCamera, const POINT &ptCursor )
{
	D3DXMATRIX matProj;
	matProj = *( pCamera->GetProjMatrix() );

	D3DVIEWPORT9 vp;
	vp = m_pRenderSystem->GetViewPort();

	D3DXVECTOR3	v;
	v.x = ((  ( (ptCursor.x-vp.X)*2.0f/vp.Width  - 1.0f) ) - matProj._31 ) / matProj._11;
	v.y = (( -( (ptCursor.y-vp.Y)*2.0f/vp.Height - 1.0f) ) - matProj._32 ) / matProj._22;
	v.z = 1.0f;
	D3DXVec3Normalize( &v, &v );

	const D3DXMATRIX *pViewInv = pCamera->GetViewInverseMatrix();

	// Transform the screen space pick ray into 3D space
	D3DXVECTOR3 vPickRayDir, vPickRayOrig;
	D3DXVec3TransformNormal( &vPickRayDir, &v, pViewInv );

	vPickRayOrig.x = pViewInv->_41;
	vPickRayOrig.y = pViewInv->_42;
	vPickRayOrig.z = pViewInv->_43;

	ioRay kRay;
	kRay.SetOrigin( vPickRayOrig );
	kRay.SetDirection( vPickRayDir );

	return kRay;
}

void ioSceneManager::SetViewPortAutoAndSystem( const D3DVIEWPORT9 &rkViewPort )
{
	m_AutoShaderParamSource.SetViewPort( rkViewPort );
	m_pRenderSystem->SetViewPort( rkViewPort );
}

void ioSceneManager::RenderOpaqueObjects( const ioRenderableList &rkPassList )
{
	ioShader *pActiveShader = NULL;
	m_pRenderSystem->DisableShader();

	int iRenderCount = rkPassList.GetCount();
	for( int i=0 ; i<iRenderCount ; i++ )
	{
		const RenderablePass &rkPass = rkPassList.GetAt( i );

		if( pActiveShader != rkPass.pShader )
		{
			if( pActiveShader )
			{
				pActiveShader->EndPass();
				pActiveShader->End();
			}

			if( rkPass.pShader )
			{
				pActiveShader = rkPass.pShader;

				pActiveShader->Begin();
				pActiveShader->BeginPass( 0 );
			}
			else
			{
				m_pRenderSystem->DisableShader();
				pActiveShader = NULL;
			}
		}

		SetRenderPass( rkPass );

		RenderSingleObject( rkPass );
	}

	if( pActiveShader )
	{
		pActiveShader->EndPass();
		pActiveShader->End();
	}
}


void ioSceneManager::RenderOpaqueObjectsLocalGlow( const ioRenderableList &rkPassList )
{
	ioShader *pActiveShader = NULL;
	m_pRenderSystem->DisableShader();

	int iRenderCount = rkPassList.GetCount();
	for( int i=0 ; i<iRenderCount ; i++ )
	{
		const RenderablePass &rkPass = rkPassList.GetAt( i );

		if( rkPass.pRend && rkPass.pRend->GetMaterial() && rkPass.pRend->GetMaterial()->GetLocalGlow() )
		{
			if( pActiveShader != rkPass.pShader )
			{
				if( pActiveShader )
				{
					pActiveShader->EndPass();
					pActiveShader->End();
				}

				if( rkPass.pShader )
				{
					pActiveShader = rkPass.pShader;

					pActiveShader->Begin();
					pActiveShader->BeginPass( 0 );
				}
				else
				{
					m_pRenderSystem->DisableShader();
					pActiveShader = NULL;
				}
			}
			SetRenderPass( rkPass );
			RenderSingleObject( rkPass );
		}
		else
		{
			if( pActiveShader != rkPass.pShader )
			{
				if( pActiveShader )
				{
					pActiveShader->EndPass();
					pActiveShader->End();
				}

				if( rkPass.pShader )
				{
					pActiveShader = rkPass.pShader;

					pActiveShader->Begin();
					pActiveShader->BeginPass( 0 );
				}
				else
				{
					m_pRenderSystem->DisableShader();
					pActiveShader = NULL;
				}
			}

			SetRenderPassLocalGlow( rkPass );
			RenderSingleObjectLocalGlow( rkPass );
		}
	}

	if( pActiveShader )
	{
		pActiveShader->EndPass();
		pActiveShader->End();
	}
}



void ioSceneManager::RenderTransparentObjects( const ioRenderableList &rkPassList )
{
	ioShader *pActiveShader = NULL;
	m_pRenderSystem->DisableShader();

	int iRenderCount = rkPassList.GetCount();
	for( int i=0 ; i<iRenderCount ; i++ )
	{
		const RenderablePass &rkPass = rkPassList.GetAt( i );

		if( pActiveShader != rkPass.pShader )
		{
			if( pActiveShader )
			{
				pActiveShader->EndPass();
				pActiveShader->End();
			}

			if( rkPass.pShader )
			{
				pActiveShader = rkPass.pShader;

				pActiveShader->Begin();
				pActiveShader->BeginPass( 0 );
			}
			else
			{
				m_pRenderSystem->DisableShader();
				pActiveShader = NULL;
			}
		}

		if( rkPass.pRend->GetAlphaRate() != MAX_ALPHA_RATE )
		{
			SetRenderPassAlpha( rkPass );
		}
		else
		{
			SetRenderPass( rkPass );
		}

		RenderSingleObject( rkPass );
	}

	if( pActiveShader )
	{
		pActiveShader->EndPass();
		pActiveShader->End();
	}
}


void ioSceneManager::RenderTransparentObjectsLocalGlow( const ioRenderableList &rkPassList )
{
	ioShader *pActiveShader = NULL;
	m_pRenderSystem->DisableShader();

	int iRenderCount = rkPassList.GetCount();
	for( int i=0 ; i<iRenderCount ; i++ )
	{
		const RenderablePass &rkPass = rkPassList.GetAt( i );

		if( rkPass.pRend && rkPass.pRend->GetMaterial() && rkPass.pRend->GetMaterial()->GetLocalGlow() )
		{
			if( pActiveShader != rkPass.pShader )
			{
				if( pActiveShader )
				{
					pActiveShader->EndPass();
					pActiveShader->End();
				}

				if( rkPass.pShader )
				{
					pActiveShader = rkPass.pShader;

					pActiveShader->Begin();
					pActiveShader->BeginPass( 0 );
				}
				else
				{
					m_pRenderSystem->DisableShader();
					pActiveShader = NULL;
				}
			}

			if( rkPass.pRend->GetAlphaRate() != MAX_ALPHA_RATE )
			{
				SetRenderPassAlpha( rkPass );
			}
			else
			{
				SetRenderPass( rkPass );
			}

			RenderSingleObject( rkPass );
		}
		/*else
		{
			if( pActiveShader != rkPass.pShader )
			{
				if( pActiveShader )
				{
					pActiveShader->EndPass();
					pActiveShader->End();
				}

				if( rkPass.pShader )
				{
					pActiveShader = rkPass.pShader;

					pActiveShader->Begin();
					pActiveShader->BeginPass( 0 );
				}
				else
				{
					m_pRenderSystem->DisableShader();
					pActiveShader = NULL;
				}
			}

			if( rkPass.pRend->GetAlphaRate() != MAX_ALPHA_RATE )
			{
				SetRenderPassAlpha( rkPass );
			}
			else
			{
				SetRenderPassLocalGlow( rkPass );
				//SetRenderPass( rkPass );
			}

			RenderSingleObjectLocalGlow( rkPass );
		}*/
	}

	if( pActiveShader )
	{
		pActiveShader->EndPass();
		pActiveShader->End();
	}
}

void ioSceneManager::RenderToShadowMap()
{
	ioRenderQueue::QueueGroupIterator queueIter = m_RenderQueue.GetIterator();
	while( queueIter.HasMoreElements() )
	{
		ioRenderQueueGroup *pGrp = queueIter.Next();
		if( pGrp )
			RenderToShadowMapQueueGroup( pGrp );
	}
}

void ioSceneManager::RenderToShadowMapQueueGroup( ioRenderQueueGroup *pGrp )
{
	ioShader *pActiveShader = NULL;
	m_pRenderSystem->DisableShader();

	ioRenderQueueGroup::PriorityMapIterator groupIter = pGrp->GetIterator();
	while( groupIter.HasMoreElements() )
	{
		ioRenderPriorityGroup *pPriorityGrp = groupIter.Next();

		const ioRenderableList &rkList = pPriorityGrp->GetGenerateShadowMapPasses();

		int iRenderCount = rkList.GetCount();
		for( int i=0 ; i<iRenderCount ; i++ )
		{
			const RenderablePass &rkPass = rkList.GetAt( i );

			if( pActiveShader != rkPass.pShader )
			{
				if( pActiveShader )
				{
					pActiveShader->EndPass();
					pActiveShader->End();
				}

				if( rkPass.pShader )
				{
					pActiveShader = rkPass.pShader;

					pActiveShader->Begin();
					pActiveShader->BeginPass(0);
				}
				else
				{
					m_pRenderSystem->DisableShader();
					pActiveShader = NULL;
				}
			}

			SetRenderPass( rkPass, true );
			RenderToShadowMapSingleObject( rkPass );
		}
	}

	if( pActiveShader )
	{
		pActiveShader->EndPass();
		pActiveShader->End();
	}
}

void ioSceneManager::RenderToShadowMapSingleObject( const RenderablePass &rkPass )
{
	m_AutoShaderParamSource.SetCurrentRenderable( rkPass.pRend );

	ioRenderOperation kRo;
	rkPass.pRend->GetRenderOperation( kRo );
	FinalFlushSingleObject( rkPass, kRo );
}

void ioSceneManager::SetRenderPass( const RenderablePass &rkPass, bool bGenShadow )
{
	ioPass *pPass = rkPass.pPass;
	ioRenderable *pRend = rkPass.pRend;

	m_pRenderSystem->SetSceneBlend( pPass->GetBlendOp(),
									pPass->GetSourceBlendFactor(),
									pPass->GetDestBlendFactor() );

	if( pPass->IsUseTextureFactor() )
	{
		m_pRenderSystem->SetTextureFactor( pPass->GetTextureFactor() );
	}

	m_pRenderSystem->SetDepthFunction( pPass->GetDepthFunction(),
									   pPass->GetDepthWriteEnable(),
									   pPass->GetDepthCheckEnable() );

	m_pRenderSystem->SetCullingMode( pPass->GetCullingMode() );
	m_pRenderSystem->SetShadingType( pPass->GetShadeType() );
	m_pRenderSystem->SetFillMode( pPass->GetFillMode() );

	int iAlphaTestRef = ( pPass->GetAlphaTestRef() * pRend->GetAlphaRate() )/MAX_ALPHA_RATE;
	m_pRenderSystem->SetAlphaTest( pPass->GetAlphaTestFunction(), iAlphaTestRef );

	int iTexUnit = 0;
	ioPass::TextureUnitStateIterator texIter = pPass->GetTextureUnitStateIterator();
	while( texIter.HasMoreElements() )
	{
		m_pRenderSystem->SetTextureUnitSetting( iTexUnit, texIter.Next() );
		++iTexUnit;
	}
 
	if( !bGenShadow && m_ShadowMapType != SMT_NOT_ENABLE )
	{
		if( pPass->HasProperty( ioPass::PP_SHADOW_RECV_PASS ) && pRend->IsShadowRecvRender() )
		{
			if( m_ShadowMapType == SMT_DEPTH_BUF )
			{
				m_pRenderSystem->SetTexture( iTexUnit, m_pShadowMapRT->GetDepthTexture() );
			}
			else if( m_ShadowMapType == SMT_R32F_TEX )
			{
				m_pRenderSystem->SetTexture( iTexUnit, m_pShadowMapRT->GetTexture() );
			}

			m_pRenderSystem->SetTextureAddressMode( iTexUnit, TAM_CLAMP );
			iTexUnit++;
		}
	}

	m_pRenderSystem->DisableTextureUnitsFrom( iTexUnit );

	if( bGenShadow && m_ShadowMapType == SMT_DEPTH_BUF )
	{
		m_pRenderSystem->SetColorWriteEnable( 0x0 );
	}
	else
	{
		if( pPass->GetColorWriteEnable() )
			m_pRenderSystem->SetColorWriteEnable( 0xf );
		else
			m_pRenderSystem->SetColorWriteEnable( 0x0 );

		m_pRenderSystem->SetDepthBias( pPass->GetDepthBias(), pPass->GetBiasSlope() );
	}

	m_AutoShaderParamSource.SetCurrentPass( pPass );
}


void ioSceneManager::SetRenderPassLocalGlow( const RenderablePass &rkPass, bool bGenShadow )
{
	ioPass *pPass = rkPass.pPass;
	ioRenderable *pRend = rkPass.pRend;

	m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD,
		D3DBLEND_ZERO,
		D3DBLEND_ZERO );

	if( pPass->IsUseTextureFactor() )
	{
		m_pRenderSystem->SetTextureFactor( pPass->GetTextureFactor() );
	}

	m_pRenderSystem->SetDepthFunction( pPass->GetDepthFunction(),
		pPass->GetDepthWriteEnable(),
		pPass->GetDepthCheckEnable() );

	m_pRenderSystem->SetCullingMode( pPass->GetCullingMode() );
	m_pRenderSystem->SetShadingType( pPass->GetShadeType() );
	m_pRenderSystem->SetFillMode( pPass->GetFillMode() );

	int iAlphaTestRef = ( pPass->GetAlphaTestRef() * pRend->GetAlphaRate() )/MAX_ALPHA_RATE;
	m_pRenderSystem->SetAlphaTest( pPass->GetAlphaTestFunction(), iAlphaTestRef );

	int iTexUnit = 0;
	ioPass::TextureUnitStateIterator texIter = pPass->GetTextureUnitStateIterator();
	while( texIter.HasMoreElements() )
	{
		m_pRenderSystem->SetTextureUnitSetting( iTexUnit, texIter.Next() );
		++iTexUnit;
	}

	if( !bGenShadow && m_ShadowMapType != SMT_NOT_ENABLE )
	{
		if( pPass->HasProperty( ioPass::PP_SHADOW_RECV_PASS ) && pRend->IsShadowRecvRender() )
		{
			if( m_ShadowMapType == SMT_DEPTH_BUF )
			{
				m_pRenderSystem->SetTexture( iTexUnit, m_pShadowMapRT->GetDepthTexture() );
			}
			else if( m_ShadowMapType == SMT_R32F_TEX )
			{
				m_pRenderSystem->SetTexture( iTexUnit, m_pShadowMapRT->GetTexture() );
			}

			m_pRenderSystem->SetTextureAddressMode( iTexUnit, TAM_CLAMP );
			iTexUnit++;
		}
	}

	m_pRenderSystem->DisableTextureUnitsFrom( iTexUnit );

	if( bGenShadow && m_ShadowMapType == SMT_DEPTH_BUF )
	{
		m_pRenderSystem->SetColorWriteEnable( 0x0 );
	}
	else
	{
		if( pPass->GetColorWriteEnable() )
			m_pRenderSystem->SetColorWriteEnable( 0xf );
		else
			m_pRenderSystem->SetColorWriteEnable( 0x0 );

		m_pRenderSystem->SetDepthBias( pPass->GetDepthBias(), pPass->GetBiasSlope() );
	}

	m_AutoShaderParamSource.SetCurrentPass( pPass );
}

void ioSceneManager::SetRenderPassAlpha( const RenderablePass &rkPass )
{
	ioPass *pPass = rkPass.pPass;
	ioRenderable *pRend = rkPass.pRend;

	m_pRenderSystem->SetSceneBlend( pPass->GetAlphaBlendOp(),
									pPass->GetAlphaSourceBlendFactor(),
									pPass->GetAlphaDestBlendFactor() );

	if( pPass->IsUseTextureFactor() )
	{
		m_pRenderSystem->SetTextureFactor( pPass->GetTextureFactor() );
	}

	m_pRenderSystem->SetDepthFunction( pPass->GetDepthFunction(),
									   pPass->GetDepthWriteEnable(),
									   pPass->GetDepthCheckEnable() );
	m_pRenderSystem->SetDepthBias( pPass->GetDepthBias(), pPass->GetBiasSlope() );

	int iAlphaTestRef = ( pPass->GetAlphaTestRef() * pRend->GetAlphaRate() )/MAX_ALPHA_RATE;
	m_pRenderSystem->SetAlphaTest( pPass->GetAlphaTestFunction(), iAlphaTestRef );

	m_pRenderSystem->SetCullingMode( pPass->GetCullingMode() );
	m_pRenderSystem->SetShadingType( pPass->GetShadeType() );
	m_pRenderSystem->SetFillMode( pPass->GetFillMode() );

	if( pPass->GetColorWriteEnable() )
		m_pRenderSystem->SetColorWriteEnable( 0xf );
	else
		m_pRenderSystem->SetColorWriteEnable( 0x0 );

	int iUnit = 0;
	ioPass::TextureUnitStateIterator texIter = pPass->GetTextureUnitStateIterator();
	while( texIter.HasMoreElements() )
	{
		m_pRenderSystem->SetTextureUnitSetting( iUnit, texIter.Next() );
		++iUnit;
	}

	m_pRenderSystem->DisableTextureUnitsFrom( iUnit );

	m_AutoShaderParamSource.SetCurrentPass( pPass );
}


void ioSceneManager::SetPassLightTexture( ioPass *pPass, const ioLight *pLight )
{
	if( !pLight )	return;

	int iUnit = 0;
	ioPass::TextureUnitStateIterator texIter = pPass->GetTextureUnitStateIterator();
	while( texIter.HasMoreElements() )
	{
		ioTextureUnitState *pState = texIter.Next();
		if( pState->IsUseLightTexture() )
		{
			m_pRenderSystem->SetioTexture( iUnit, pLight->GetLightTexture() );
		}

		++iUnit;
	}
}

void ioSceneManager::RenderSingleObject( const RenderablePass &rkPass )
{
	ioPass *pPass = rkPass.pPass;
	ioRenderable *pRend = rkPass.pRend;

	D3DVIEWPORT9 kOldViewPort;
	bool bRestoreRenderTarget = false;

	if( pPass->HasRenderTarget() )
	{
		ioRenderTexture *pTarget = dynamic_cast< ioRenderTexture* >
									( g_TextureMgr.GetTextureByName( pPass->GetRenderTarget() ) );
		if( pTarget )
		{
			kOldViewPort = m_pRenderSystem->GetViewPort();

			D3DVIEWPORT9 kNewViewPort;
			kNewViewPort.X = 0;
			kNewViewPort.Y = 0;
			kNewViewPort.Width  = pPass->GetRenderTargetWidth();
			kNewViewPort.Height = pPass->GetRenderTargetHeight();
			kNewViewPort.MinZ = 0.0f;
			kNewViewPort.MaxZ = 1.0f;

			m_pRenderSystem->SetRenderTarget( pTarget );
			SetViewPortAutoAndSystem( kNewViewPort );

			m_pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000 );
			bRestoreRenderTarget = true;
		}
	}

	ioRenderOperation kRo;
	pRend->GetRenderOperation( kRo );
	m_AutoShaderParamSource.SetCurrentRenderable( pRend );

	if( pPass->IsLightingEnable() )
	{
		if( !rkPass.pShader )
		{
			D3DMATERIAL9 kMaterial = pPass->GetDXMaterial();
			kMaterial.Diffuse.a = (float)pRend->GetAlphaRate() / 255.0f;
			m_pRenderSystem->SetMaterial( kMaterial );
		}

		const ioLightList &kLightList = rkPass.pRend->GetLights();

		if( pPass->HasProperty( ioPass::PP_LIGHT_ITERATE_PASS ) )
		{
			int iLightIndex = rkPass.iLightIndex;
			if( iLightIndex == 0 )
			{
				m_AutoShaderParamSource.SetIgnoreAmbient( false );
			}
			else
			{
				m_AutoShaderParamSource.SetIgnoreAmbient( true );

				if( !pPass->IsTransparent() )
				{
					m_pRenderSystem->SetSceneBlend( pPass->GetAlphaBlendOp(),
													D3DBLEND_ONE,
													D3DBLEND_ONE );
				}

				m_pRenderSystem->SetDepthFunction( pPass->GetDepthFunction(),
												   false,
												   pPass->GetDepthCheckEnable() );
			}

			int iLightCount = kLightList.size();
			if( COMPARE( iLightIndex, 0, iLightCount ) )
			{
				ioLightList kLocalLightList;
				kLocalLightList.push_back( kLightList[iLightIndex] );

				if( rkPass.pShader )
				{
					m_pRenderSystem->SetLightEnable( false );
					m_AutoShaderParamSource.SetApplyLightList( &kLocalLightList );
				}
				else
				{
					m_pRenderSystem->SetLightEnable( true );
					m_pRenderSystem->SetLightsList( kLightList );
					m_AutoShaderParamSource.SetApplyLightList( NULL );
				}

				SetPassLightTexture( pPass, kLightList[iLightIndex] );
				FinalFlushSingleObject( rkPass, kRo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioSceneManager::RenderSingleObject - Light Over(%d/%d)",
										iLightIndex, iLightCount );
			}

			m_AutoShaderParamSource.SetIgnoreAmbient( false );
		}
		else
		{
			if( rkPass.pShader )
			{
				m_pRenderSystem->SetLightEnable( false );
				m_AutoShaderParamSource.SetApplyLightList( &kLightList );
			}
			else
			{
				m_pRenderSystem->SetLightEnable( true );
				m_pRenderSystem->SetLightsList( kLightList );
				m_AutoShaderParamSource.SetApplyLightList( NULL );
			}

			SetPassLightTexture( pPass, m_AutoShaderParamSource.GetApplyLight(0) );
			FinalFlushSingleObject( rkPass, kRo );
		}

		m_AutoShaderParamSource.SetApplyLightList( NULL );
	}
	else
	{
		m_pRenderSystem->SetLightEnable( false );
		m_AutoShaderParamSource.SetApplyLightList( NULL );

		FinalFlushSingleObject( rkPass, kRo );
	}

	// restore rendertarget..
	if( bRestoreRenderTarget )
	{
		m_pRenderSystem->RestoreRenderTarget();
		SetViewPortAutoAndSystem( kOldViewPort );
	}
}


void ioSceneManager::RenderSingleObjectLocalGlow( const RenderablePass &rkPass )
{
	ioPass *pPass = rkPass.pPass;
	ioRenderable *pRend = rkPass.pRend;

	D3DVIEWPORT9 kOldViewPort;
	bool bRestoreRenderTarget = false;

	if( pPass->HasRenderTarget() )
	{
		ioRenderTexture *pTarget = dynamic_cast< ioRenderTexture* >
			( g_TextureMgr.GetTextureByName( pPass->GetRenderTarget() ) );
		if( pTarget )
		{
			kOldViewPort = m_pRenderSystem->GetViewPort();

			D3DVIEWPORT9 kNewViewPort;
			kNewViewPort.X = 0;
			kNewViewPort.Y = 0;
			kNewViewPort.Width  = pPass->GetRenderTargetWidth();
			kNewViewPort.Height = pPass->GetRenderTargetHeight();
			kNewViewPort.MinZ = 0.0f;
			kNewViewPort.MaxZ = 1.0f;

			m_pRenderSystem->SetRenderTarget( pTarget );
			SetViewPortAutoAndSystem( kNewViewPort );

			m_pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000 );
			bRestoreRenderTarget = true;
		}
	}

	ioRenderOperation kRo;
	pRend->GetRenderOperation( kRo );
	m_AutoShaderParamSource.SetCurrentRenderable( pRend );

	/*if( pPass->IsLightingEnable() )
	{
		if( !rkPass.pShader )
		{
			D3DMATERIAL9 kMaterial = pPass->GetDXMaterial();
			kMaterial.Diffuse.a = (float)pRend->GetAlphaRate() / 255.0f;
			m_pRenderSystem->SetMaterial( kMaterial );
		}

		const ioLightList &kLightList = rkPass.pRend->GetLights();

		if( pPass->HasProperty( ioPass::PP_LIGHT_ITERATE_PASS ) )
		{
			int iLightIndex = rkPass.iLightIndex;
			if( iLightIndex == 0 )
			{
				m_AutoShaderParamSource.SetIgnoreAmbient( false );
			}
			else
			{
				m_AutoShaderParamSource.SetIgnoreAmbient( true );

				if( !pPass->IsTransparent() )
				{
					m_pRenderSystem->SetSceneBlend( pPass->GetAlphaBlendOp(),
						D3DBLEND_ONE,
						D3DBLEND_ONE );
				}

				m_pRenderSystem->SetDepthFunction( pPass->GetDepthFunction(),
					false,
					pPass->GetDepthCheckEnable() );
			}

			int iLightCount = kLightList.size();
			if( COMPARE( iLightIndex, 0, iLightCount ) )
			{
				ioLightList kLocalLightList;
				kLocalLightList.push_back( kLightList[iLightIndex] );

				if( rkPass.pShader )
				{
					m_pRenderSystem->SetLightEnable( false );
					m_AutoShaderParamSource.SetApplyLightList( &kLocalLightList );
				}
				else
				{
					m_pRenderSystem->SetLightEnable( true );
					m_pRenderSystem->SetLightsList( kLightList );
					m_AutoShaderParamSource.SetApplyLightList( NULL );
				}

				SetPassLightTexture( pPass, kLightList[iLightIndex] );
				FinalFlushSingleObject( rkPass, kRo );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioSceneManager::RenderSingleObject - Light Over(%d/%d)",
					iLightIndex, iLightCount );
			}

			m_AutoShaderParamSource.SetIgnoreAmbient( false );
		}
		else
		{
			if( rkPass.pShader )
			{
				m_pRenderSystem->SetLightEnable( false );
				m_AutoShaderParamSource.SetApplyLightList( &kLightList );
			}
			else
			{
				m_pRenderSystem->SetLightEnable( true );
				m_pRenderSystem->SetLightsList( kLightList );
				m_AutoShaderParamSource.SetApplyLightList( NULL );
			}

			SetPassLightTexture( pPass, m_AutoShaderParamSource.GetApplyLight(0) );
			FinalFlushSingleObject( rkPass, kRo );
		}

		m_AutoShaderParamSource.SetApplyLightList( NULL );
	}
	else*/
	{
		m_pRenderSystem->SetLightEnable( false );
		m_AutoShaderParamSource.SetApplyLightList( NULL );

		FinalFlushSingleObject( rkPass, kRo );
	}

	// restore rendertarget..
	if( bRestoreRenderTarget )
	{
		m_pRenderSystem->RestoreRenderTarget();
		SetViewPortAutoAndSystem( kOldViewPort );
	}
}

void ioSceneManager::FinalFlushSingleObject( const RenderablePass &rkPass, ioRenderOperation &rkRo )
{
	if( rkPass.pShader )
	{
		m_pRenderSystem->SetSoftwareVertexProcessing( true );

		rkPass.pShader->BindAutoShaderParamNoLights( m_AutoShaderParamSource );
		rkPass.pShader->BindAutoShaderParamOnlyLights( m_AutoShaderParamSource );
		rkPass.pShader->BindCustomShaderParam( rkPass.pPass->GetCustomParameters() );
	}
	else
	{
		m_pRenderSystem->SetSoftwareVertexProcessing( false );

		D3DXMATRIX matWorld;
		rkPass.pRend->GetWorldTransform( &matWorld );
		m_pRenderSystem->SetModelMatrix( &matWorld );
	}

	if( !rkPass.pPass->UseQuadBuffer() )
	{
		m_pRenderSystem->Render( rkRo, rkPass.pShader );
	}
	else
	{
		m_pRenderSystem->RenderQuad();
	}
}

ioSceneNode* ioSceneManager::GetRootSceneNode() const
{
	return m_pSceneRoot;
}

ioSceneNode* ioSceneManager::GetSceneNode( UniqueNodeID eNodeID ) const
{
	SceneNodeMap::const_iterator iter = m_SceneNodeMap.find( eNodeID );
	if( iter != m_SceneNodeMap.end() )
		return iter->second;

	return NULL;
}

ioSceneNode* ioSceneManager::CreateSceneNode()
{
	ioSceneNode *pNode = new ioSceneNode( this );
	m_SceneNodeMap.insert( SceneNodeMap::value_type( pNode->GetUniqueID(), pNode ) );

	return pNode;
}

void ioSceneManager::DestroySceneNode( UniqueNodeID eNodeID )
{
	SceneNodeMap::iterator iter = m_SceneNodeMap.find( eNodeID );

	if( iter != m_SceneNodeMap.end() )
	{
		ioSceneNode *pSNode = iter->second;

		ioNode *pParent = pSNode->GetParent();
		if( pParent )
		{
			pParent->RemoveChild( pSNode );
		}

		m_SceneNodeMap.erase( iter );
		delete pSNode;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSceneManager::DestroySceneNode - %d not found", eNodeID );
	}
}

ioEntityGroup* ioSceneManager::CreateEntityGrp()
{
	ioEntityGroup *pEntityGrp = new ioEntityGroup;
	m_EntityGroupMap.insert( EntityGroupMap::value_type( pEntityGrp->GetUniqueID(), pEntityGrp ) );
	pEntityGrp->SetSoftwareVertexBlending( m_bSoftwareVertexBlending );

	return pEntityGrp;
}

ioSceneManager::EntityGrpIterator ioSceneManager::GetEntityGrpIterator()
{
	return EntityGrpIterator( m_EntityGroupMap.begin(), m_EntityGroupMap.end() );
}

int ioSceneManager::GetEntityGrpCnt() const
{
	return m_EntityGroupMap.size();
}

ioEntityGroup* ioSceneManager::GetEntityGrp( UniqueObjID eObjID )
{
	EntityGroupMap::iterator iter = m_EntityGroupMap.find( eObjID );
	if( iter != m_EntityGroupMap.end() )
		return iter->second;

	return NULL;
}

void ioSceneManager::DestroyEntityGrp( ioEntityGroup *pGrp )
{
	EntityGroupMap::iterator iter;
	for( iter=m_EntityGroupMap.begin() ; iter!=m_EntityGroupMap.end() ; ++iter )
	{
		if( iter->second == pGrp )
		{
			delete iter->second;
			m_EntityGroupMap.erase( iter );
			break;
		}
	}
}

void ioSceneManager::DestroyEntityGrp( UniqueObjID eObjID )
{
	EntityGroupMap::iterator iter = m_EntityGroupMap.find( eObjID );
	if( iter != m_EntityGroupMap.end() )
	{
		delete iter->second;
		m_EntityGroupMap.erase( iter );
	}
}

void ioSceneManager::DestroyAllEntityGrp()
{
	EntityGroupMap::iterator iter;
	for( iter=m_EntityGroupMap.begin() ; iter!=m_EntityGroupMap.end() ; ++iter )
	{
		delete iter->second;
	}
	m_EntityGroupMap.clear();
}

void ioSceneManager::ModifyEntityGroupMaterial( ioEntityGrpMaterialModifier *pModifier )
{
	EntityGroupMap::iterator iter;
	for( iter=m_EntityGroupMap.begin() ; iter!=m_EntityGroupMap.end() ; ++iter )
	{
		pModifier->ModifyMaterial( iter->second );
	}
}

ioLight* ioSceneManager::CreateLight( const ioHashString &kName )
{
	if( kName.IsEmpty() )
		return NULL;

	SceneLightList::iterator iter = m_SceneLightList.find( kName );
	if( iter != m_SceneLightList.end() )
	{
		LOG.PrintTimeAndLog( 0, "ioSceneManager::CreateLight - %s Already Exist", kName.c_str() );
		return NULL;
	}

	ioLight *pLight = new ioLight( kName );
	if( pLight )
	{
		m_SceneLightList.insert( SceneLightList::value_type( kName, pLight ) );
	}

	return pLight;
}

ioLight* ioSceneManager::GetLight( const ioHashString &kName )
{
	if( kName.IsEmpty() )
		return NULL;

	SceneLightList::iterator iter = m_SceneLightList.find( kName );
	if( iter != m_SceneLightList.end() )
		return iter->second;

	return NULL;
}

void ioSceneManager::DestroyLight( const ioHashString &kName )
{
	if( kName.IsEmpty() )
		return;

	SceneLightList::iterator iter = m_SceneLightList.find( kName );
	if( iter != m_SceneLightList.end() )
	{
		delete iter->second;
		m_SceneLightList.erase( iter );
	}
}

void ioSceneManager::DestroyLight( ioLight *pLight )
{
	SceneLightList::iterator iter = m_SceneLightList.begin();
	for( ; iter!=m_SceneLightList.end() ; ++iter )
	{
		if( iter->second == pLight )
		{
			delete iter->second;
			m_SceneLightList.erase( iter );
			break;
		}
	}
}

void ioSceneManager::DestroyAllLights()
{
	SceneLightList::iterator iter;
	for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
	{
		delete iter->second;
	}

	m_SceneLightList.clear();
}

void ioSceneManager::PopulateLightList( const D3DXVECTOR3 &vPos,
										float fRadius,
										ioLightList &rkDestList )
{
	rkDestList.clear();

	float fRadiusSq = fRadius * fRadius;

	SceneLightList::iterator iter;
	for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
	{
		ioLight *pLight = iter->second;

		if( !pLight->IsVisible() )
			continue;

		if( pLight->GetType() == LT_DIRECTIONAL )
		{
			pLight->m_fTempSquareDist = 0.0f;
			rkDestList.push_back( pLight );
		}
		else
		{
			D3DXVECTOR3 vDiff = pLight->GetDerivedPosition() - vPos;
			pLight->m_fTempSquareDist = D3DXVec3LengthSq( &vDiff );

			if( pLight->m_fTempSquareDist - fRadiusSq < pLight->GetSquareRange() )
			{
				rkDestList.push_back( pLight );
			}
		}
	}

	if( rkDestList.size() > 1 )
	{
		std::sort( rkDestList.begin(), rkDestList.end(), LightLess() );
	}
}

void ioSceneManager::PopulateLightList( const D3DXVECTOR3 vPos,
										float fRadius,
										LightReceiveType eType,
										ioLightList &rkDestList )
{
	rkDestList.clear();

	float fRadiusSq = fRadius * fRadius;

	SceneLightList::iterator iter;
	for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
	{
		ioLight *pLight = iter->second;

		if( !pLight->IsVisible() ||
			!pLight->IsValidLightByReceiveType( eType ) )
			continue;

		if( pLight->GetType() == LT_DIRECTIONAL )
		{
			pLight->m_fTempSquareDist = 0.0f;
			rkDestList.push_back( pLight );
		}
		else
		{
			D3DXVECTOR3 vDiff = pLight->GetDerivedPosition() - vPos;
			pLight->m_fTempSquareDist = D3DXVec3LengthSq( &vDiff );

			if( pLight->m_fTempSquareDist - fRadiusSq < pLight->GetSquareRange() )
			{
				rkDestList.push_back( pLight );
			}
		}
	}
	
	if( rkDestList.size() > 1 )
	{
		std::sort( rkDestList.begin(), rkDestList.end(), LightLess() );
	}
}

void ioSceneManager::SetShadowMapEnable( bool bEnable )
{
	ShadowMapType eMapType = m_pRenderSystem->GetShadowMapType();
	if( !bEnable || eMapType == SMT_NOT_ENABLE )
	{
		m_ShadowMapType = SMT_NOT_ENABLE;
		ClearShadowMapResource();
		return;
	}

	m_ShadowMapType = eMapType;

	bool bNeedCreateShadowMap = true;
	if( m_pShadowMapRT )
	{
		if( m_pShadowMapRT->GetWidth() != m_iShadowMapSize )
		{
			ClearShadowMapResource();
		}
		else
		{
			bNeedCreateShadowMap = false;	// 기존에 생성된것과 사이즈가 똑같아서 새로 생성X
		}
	}

	if( bNeedCreateShadowMap )
	{
		if( m_ShadowMapType == SMT_R32F_TEX )
		{
			m_pShadowMapRT = g_TextureMgr.CreateRTT( "_ShadowMapRT",
													 m_iShadowMapSize,
													 m_iShadowMapSize,
													 D3DFMT_R32F,
													 D3DFMT_D16,
													 false );
		}
		else
		{
			m_pShadowMapRT = g_TextureMgr.CreateRTT( "_ShadowMapRT",
													 m_iShadowMapSize,
													 m_iShadowMapSize,
													 D3DFMT_R5G6B5,
													 D3DFMT_D16,
													 true );
		}
	}

	D3DXMATRIX matShadowTex;
	D3DXMatrixIdentity( &matShadowTex );

	float fTexOffset = FLOAT05 + ( FLOAT05 / (float)m_iShadowMapSize );
	matShadowTex._11 =  FLOAT05;
	matShadowTex._22 = -FLOAT05;
	matShadowTex._33 =  1.0f;
	matShadowTex._41 = fTexOffset;
	matShadowTex._42 = fTexOffset;

	m_AutoShaderParamSource.SetShadowMapTexMatrix( matShadowTex );
}

void ioSceneManager::SetShadowColor( float fAlpha, float fRed, float fGreen, float fBlue )
{
	D3DCOLORVALUE kColor;
	kColor.a = fAlpha;
	kColor.r = fRed;
	kColor.g = fGreen;
	kColor.b = fBlue;

	m_AutoShaderParamSource.SetShadowColor( kColor );
}

void ioSceneManager::SetShadowLightDir( const D3DXVECTOR3 &vDir )
{
	D3DXVec3Normalize( &m_vShadowLightDir, &vDir );
}

void ioSceneManager::ClearShadowMapResource()
{
	if( m_pShadowMapRT )
	{
		g_TextureMgr.DestroyRTTIfNoRef( m_pShadowMapRT );
		m_pShadowMapRT = NULL;
	}
}

ShadowMapType ioSceneManager::GetShadowMapType() const
{
	return m_ShadowMapType;
}

//Light Space Perspective Shadow Map
//( http://www.cg.tuwien.ac.at/research/vr/lispsm/ )
void ioSceneManager::UpdateShadowMapMatrix( ioCamera *pCamera )
{
	if( m_vShadowAreaList.empty() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		m_AutoShaderParamSource.SetLightViewProjMatrix( matIdentity );
		return;
	}

	D3DXVECTOR3 vEyePos, vEyeDir;
	vEyePos = pCamera->GetPosition();
	vEyeDir = pCamera->GetDirection();

	D3DXVECTOR3 vLightLookAt;
	vLightLookAt = vEyePos + m_vShadowLightDir;

	D3DXMATRIX matLightView;
	D3DXMatrixLookAtLH( &matLightView, &vEyePos,
									   &vLightLookAt,
									   &vEyeDir );

	D3DXVECTOR3 vMin, vMax;
	ioAxisAlignBox kLightSpaceBox;
	kLightSpaceBox.SetMinMaxPos( m_vShadowAreaList, &matLightView );
	vMin = kLightSpaceBox.GetMinPos();
	vMax = kLightSpaceBox.GetMaxPos();

	float fDotProd = D3DXVec3Dot( &vEyeDir, &m_vShadowLightDir );
	float fSinGamma = sqrt( 1.0f - fDotProd * fDotProd );

	float fZn = pCamera->GetNearP() / fSinGamma;
	float fDist = fabs( vMax.y - vMin.y );
	float fZf = fZn + fDist * fSinGamma;
	float fN  = ( fZn + sqrt( fZf*fZn ) ) / fSinGamma;
	float fF  = fN + fDist;

	D3DXVECTOR3 vLightPos;
	vLightPos = vEyePos - vEyeDir * ( fN - pCamera->GetNearP() );
	vLightLookAt = vLightPos + m_vShadowLightDir;
	D3DXMatrixLookAtLH( &matLightView, &vLightPos,
									   &vLightLookAt,
									   &vEyeDir );

	D3DXMATRIX matLisp;
	D3DXMatrixIdentity( &matLisp );
	matLisp._22 = (fF+fN)/(fF-fN);
	matLisp._24 = 1.0f;
	matLisp._42 = -2.0f * fF * fN / ( fF-fN );
	matLisp._44 = 0.0f;

	D3DXMATRIX matLightProj;
	D3DXMatrixMultiply( &matLightProj, &matLightView, &matLisp );

	kLightSpaceBox.SetMinMaxPos( m_vShadowAreaList, &matLightProj );
	vMin = kLightSpaceBox.GetMinPos();
	vMax = kLightSpaceBox.GetMaxPos();

	D3DXMATRIX matScaleToFit;
	D3DXMatrixIdentity( &matScaleToFit );
	matScaleToFit._11 = 2.0f / ( vMax.x - vMin.x );
	matScaleToFit._41 = -( vMax.x + vMin.x ) / ( vMax.x - vMin.x );

	matScaleToFit._22 = 2.0f / ( vMax.y - vMin.y );
	matScaleToFit._42 = -( vMax.y + vMin.y ) / ( vMax.y - vMin.y );

	matScaleToFit._33 = 1.0f / ( vMax.z - vMin.z );
	matScaleToFit._43 = -vMin.z / ( vMax.z - vMin.z );

	D3DXMatrixMultiply( &matLightProj, &matLisp, &matScaleToFit );

	D3DXMATRIX matLightViewProj;
	D3DXMatrixMultiply( &matLightViewProj, &matLightView, &matLightProj );
	m_AutoShaderParamSource.SetLightViewProjMatrix( matLightViewProj );
}

void ioSceneManager::GenerateShadowMap()
{
	if( !m_pShadowMapRT || m_ShadowMapType == SMT_NOT_ENABLE )
		return;

	D3DVIEWPORT9 kOldViewPort, kNewViewPort;
	kNewViewPort.X = 0;
	kNewViewPort.Y = 0;
	kNewViewPort.Width  = m_iShadowMapSize;
	kNewViewPort.Height = m_iShadowMapSize;
	kNewViewPort.MinZ = 0.0f;
	kNewViewPort.MaxZ = 1.0f;

	kOldViewPort = m_pRenderSystem->GetViewPort();
	
	m_pRenderSystem->SetRenderTarget( m_pShadowMapRT );
	SetViewPortAutoAndSystem( kNewViewPort );

	m_pRenderSystem->SetFogEnable( false );
	m_pRenderSystem->ClearStreamDesc();

	switch( m_ShadowMapType )
	{
	case SMT_DEPTH_BUF:
		GenerateShadowMapD16();
		break;
	case SMT_R32F_TEX:
		GenerateShadowMapR32F();
		break;
	}

	m_pRenderSystem->SetFogEnable( IsNowFog() );

	m_pRenderSystem->RestoreRenderTarget();
	SetViewPortAutoAndSystem( kOldViewPort );
}

void ioSceneManager::GenerateShadowMapR32F()
{
	m_pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );

	if( m_pRenderSystem->BeginScene() )
	{
		RenderToShadowMap();
		m_pRenderSystem->EndScene();
	}
}

void ioSceneManager::GenerateShadowMapD16()
{
	float fDepthBias;
	float fBiasSlope;

	fDepthBias = 8000.0f / 16777215.0f;
	fBiasSlope = 1.0f;

	m_pRenderSystem->SetDepthBias( fDepthBias, fBiasSlope );

	m_pRenderSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );
	if( m_pRenderSystem->BeginScene() )
	{
		RenderToShadowMap();
		m_pRenderSystem->EndScene();
	}

	fDepthBias = 0.0f;
	fBiasSlope = 0.0f;

	m_pRenderSystem->SetDepthBias( fDepthBias, fBiasSlope );
}

void ioSceneManager::EnableGlowProcess( bool bEnable, const ioHashString &szBlur, const ioHashString &szFilter )
{
	SAFEDELETE( m_pPostGlow );

	if( bEnable )
	{
		m_pPostGlow = new ioGlowPostProcess( m_pRenderSystem );
		if( !m_pPostGlow->InitProcess( szBlur, szFilter ) )
		{
			SAFEDELETE( m_pPostGlow );
		}

		SetGlowAlphaValue( m_iGlowAlphaValue );
		SetGlowSecondAlphaValue( m_iGlowSecondAlphaValue );
	}
}


void ioSceneManager::EnableGlowProcessSecond( bool bEnable, const ioHashString &szBlur, const ioHashString &szFilter )
{
	SAFEDELETE( m_pPostGlowSecond );

	if( bEnable )
	{
		m_pPostGlowSecond = new ioGlowPostProcess( m_pRenderSystem );
		if( !m_pPostGlowSecond->InitProcess( szBlur, szFilter ) )
		{
			SAFEDELETE( m_pPostGlowSecond );
		}
	}
}


void ioSceneManager::EnableDrawAllTransObjForGlow( bool b)
{
	m_bDrawAllTransObjForGlow = b;
}


void ioSceneManager::EnableFilterProcess( bool bEnable, const ioHashString &szShader )
{
	SAFEDELETE( m_pPostFilter );

	if( !bEnable || szShader.IsEmpty() )
		return;

	m_pPostFilter = new ioPostFilter( m_pRenderSystem );
	if( !m_pPostFilter->InitFilter( szShader ) )
	{
		SAFEDELETE( m_pPostFilter );
	}

	SetFilterAlphaValue( m_iFilerAlphaValue );
}

void ioSceneManager::EnableOverayProcess( bool bEnable,
										  const ioHashString &szTex,
										  int iRed,
										  int iGreen,
										  int iBlue )
{
	SAFEDELETE( m_pPostOveray );

	if( !bEnable || szTex.IsEmpty() )
		return;

	m_pPostOveray = new ioPostOveray( m_pRenderSystem );
	if( !m_pPostOveray->InitOveray( szTex ) )
	{
		SAFEDELETE( m_pPostOveray );
	}
	else
	{
		m_pPostOveray->SetColor( iRed, iGreen, iBlue );
	}
}


void ioSceneManager::SetGlownessAndTexcelInc( float fGlowness, float fTexcel)
{
	if( m_pPostGlowSecond )
	{
		m_pPostGlowSecond->SetGlownessAndTexcelInc( fGlowness, fTexcel );
	}
}


void ioSceneManager::SetGlowAlphaValue( int iAlpha )
{
	m_iGlowAlphaValue = iAlpha;

	if( m_pPostGlow )
	{
		m_pPostGlow->SetAlphaValue( m_iGlowAlphaValue );
	}
}

void ioSceneManager::SetGlowSecondAlphaValue( int iAlpha )
{
	m_iGlowSecondAlphaValue = iAlpha;

	if( m_pPostGlow )
	{
		m_pPostGlow->SetSecondAlphaValue( m_iGlowSecondAlphaValue );
	}
}

void ioSceneManager::SetFilterAlphaValue( int iAlpha )
{
	m_iFilerAlphaValue = iAlpha;

	if( m_pPostFilter )
	{
		m_pPostFilter->SetAlphaValue( m_iFilerAlphaValue );
	}
}

void ioSceneManager::RenderPostProcess()
{
	if( CheckPostRenderTexture() )
	{
		if( m_pPostGlow )
			m_pPostGlow->RenderProcess( m_pPostTarget1, m_pPostTarget2 );

		if( m_pPostFilter )
			m_pPostFilter->RenderFilter( m_pPostTarget1 );
	}

	if( m_pPostOveray )
	{
		m_pPostOveray->RenderOveray();
	}
}


void ioSceneManager::RenderLocalGlowProcess()
{
	if( !m_pPostGlowSecond || !m_pPostTarget3 || !m_pPostTarget4 )
	{
		DESTROY_TEXTURE( m_pPostTarget3 );
		DESTROY_TEXTURE( m_pPostTarget4 );
		return;
	}

	m_pPostGlowSecond->RenderLocalGlow(m_pPostTarget3, m_pPostTarget4 );
}


void ioSceneManager::CalcLocalGlowProcess()
{
	if( CheckSecondRenderTexture() )
	{
		if( m_pPostGlowSecond )
			m_pPostGlowSecond->CalcLocalGlow( m_pPostTarget3, m_pPostTarget4 );
	}
}


bool ioSceneManager::CheckSecondRenderTexture()
{
	if( !m_pPostGlowSecond )
	{
		DESTROY_TEXTURE( m_pPostTarget3 );
		DESTROY_TEXTURE( m_pPostTarget4 );
		return false;
	}

	int iNewWidth  = GetTwoPowAlignSize( m_pRenderSystem->GetWindowWidth() );
	int iNewHeight = GetTwoPowAlignSize( m_pRenderSystem->GetWindowHeight() );

	if( m_pPostGlowSecond )	// 1,2 둘다 체크
	{
		if( CheckEnoughSecondRenderTexture( m_pPostTarget3, 1, iNewWidth, iNewHeight ) )
		{
			if( CheckEnoughSecondRenderTexture( m_pPostTarget4, 2, iNewWidth, iNewHeight ) )
				return true;
		}

		return false;
	}

	return false;
}

bool ioSceneManager::CheckPostRenderTexture()
{
	if( !m_pPostGlow && !m_pPostFilter )
	{
		DESTROY_TEXTURE( m_pPostTarget1 );
		DESTROY_TEXTURE( m_pPostTarget2 );
		return false;
	}

	int iNewWidth  = GetTwoPowAlignSize( m_pRenderSystem->GetWindowWidth() );
	int iNewHeight = GetTwoPowAlignSize( m_pRenderSystem->GetWindowHeight() );

	if( m_pPostGlow )	// 1,2 둘다 체크
	{
		if( CheckEnoughPostRenderTexture( m_pPostTarget1, 1, iNewWidth, iNewHeight ) )
		{
			if( CheckEnoughPostRenderTexture( m_pPostTarget2, 2, iNewWidth, iNewHeight ) )
				return true;
		}

		return false;
	}
	else if( m_pPostFilter )	// 1만 체크
	{
		DESTROY_TEXTURE( m_pPostTarget2 );

		if( CheckEnoughPostRenderTexture( m_pPostTarget1, 1, iNewWidth, iNewHeight ) )
			return true;
	}

	return false;
}


bool ioSceneManager::CheckEnoughSecondRenderTexture( ioRenderTexture* &pTarget, 
													int iIndex, 
													int iNeedWidth, 
													int iNeedHeight )
{
	if( pTarget )
	{
		if( pTarget->GetWidth() == iNeedWidth &&
			pTarget->GetHeight() == iNeedHeight )
			return true;
	}

	DESTROY_TEXTURE( pTarget );

	if( iIndex == 1 )
	{
		pTarget = g_TextureMgr.CreateRTT( "_SecoundTextureFirst",
			iNeedWidth,
			iNeedHeight,
			ioRenderSystem::m_BackSurfaceDesc.Format,
			D3DFMT_UNKNOWN,
			false );
	}
	else
	{
		pTarget = g_TextureMgr.CreateRTT( "_SecoundTextureSecond",
			iNeedWidth,
			iNeedHeight,
			ioRenderSystem::m_BackSurfaceDesc.Format,
			D3DFMT_UNKNOWN,
			false );
	}

	if( pTarget )
	{
		pTarget->SetErasePolicy( EP_AUTOMATIC );

		LOG.PrintTimeAndLog( 0, "%s - (%d/%d) - Second", pTarget->GetName().c_str(), iNeedWidth, iNeedHeight );
		return true;
	}

	return false;
}

bool ioSceneManager::CheckEnoughPostRenderTexture( ioRenderTexture* &pTarget,
												   int iIndex,
												   int iNeedWidth,
												   int iNeedHeight )
{
	if( pTarget )
	{
		if( pTarget->GetWidth() == iNeedWidth &&
			pTarget->GetHeight() == iNeedHeight )
			return true;
	}

	DESTROY_TEXTURE( pTarget );

	if( iIndex == 1 )
	{
		pTarget = g_TextureMgr.CreateRTT( "_PostTextureFirst",
										  iNeedWidth,
										  iNeedHeight,
										  ioRenderSystem::m_BackSurfaceDesc.Format,
										  D3DFMT_UNKNOWN,
										  false );
	}
	else
	{
		pTarget = g_TextureMgr.CreateRTT( "_PostTextureSecond",
										  iNeedWidth,
										  iNeedHeight,
										  ioRenderSystem::m_BackSurfaceDesc.Format,
										  D3DFMT_UNKNOWN,
										  false );
	}

	if( pTarget )
	{
		pTarget->SetErasePolicy( EP_AUTOMATIC );

		LOG.PrintTimeAndLog( 0, "%s - (%d/%d)", pTarget->GetName().c_str(), iNeedWidth, iNeedHeight );
		return true;
	}

	return false;
}

void ioSceneManager::ClearPostProcess()
{
	DESTROY_TEXTURE( m_pPostTarget1 );
	DESTROY_TEXTURE( m_pPostTarget2 );

	DESTROY_TEXTURE( m_pPostTarget3 );
	DESTROY_TEXTURE( m_pPostTarget4 );

	SAFEDELETE( m_pPostGlow );
	SAFEDELETE( m_pPostGlowSecond )
	SAFEDELETE( m_pPostFilter );
	SAFEDELETE( m_pPostOveray );
}

void ioSceneManager::SetSoftwareVertexBlending( bool bSoftware )
{
	if( m_bSoftwareVertexBlending == bSoftware )
		return;

	EntityGroupMap::iterator iter;
	for( iter=m_EntityGroupMap.begin() ; iter!=m_EntityGroupMap.end() ; ++iter )
	{
		ioEntityGroup *pGrp = iter->second;
		if( pGrp->HasSkeleton() )
		{
			pGrp->SetSoftwareVertexBlending( bSoftware );
		}
	}

	m_bSoftwareVertexBlending = bSoftware;
}

void ioSceneManager::SetTextureFilterOpt( int iStage, TextureFilterOption filter )
{
	if( iStage < 2 )
	{
		m_TextureFilterOpt[iStage] = filter;
	}
}

TextureFilterOption ioSceneManager::GetTextureFilterOpt( int iStage ) const
{
	if( iStage < 2 )
	{
		return m_TextureFilterOpt[iStage];
	}

	return TFO_NEAREST;
}

void ioSceneManager::SetTextureMipMapBias( int iStage, float fBias )
{
	if( iStage < 2 )
	{
		m_TextureMipMapBias[iStage] = fBias;
	}
}

float ioSceneManager::GetTextureMipMapBias( int iStage ) const
{
	if( iStage < 2 )
	{
		return m_TextureMipMapBias[iStage];
	}

	return 0.0f;
}

void ioSceneManager::SetFogEnable( bool bEnable )
{
	m_bNowFog = bEnable;
	m_pRenderSystem->SetFogEnable( bEnable );
}

void ioSceneManager::SetSceneAmbient( float fRed, float fGreen, float fBlue )
{
	if( m_pRenderSystem )
	{
		m_pRenderSystem->SetAmbientLight( fRed, fGreen, fBlue );
	}
}

void ioSceneManager::ClearBack( DWORD dwFlags )
{
	if( m_pRenderSystem )
	{
		m_pRenderSystem->ClearBack( dwFlags );
	}
}

D3DCOLORVALUE ioSceneManager::GetSceneAmbient() const
{
	if( m_pRenderSystem )
		return m_pRenderSystem->GetAmbientColor();

	return D3DCOLORVALUE();
}
