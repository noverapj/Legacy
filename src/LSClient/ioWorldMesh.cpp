

#include "stdafx.h"

#include "../io3DEngine/ioLight.h"

#include "../io3DEngine/ioDecal.h"
#include "../io3DEngine/ioDecalMaker.h"

#include "ioWorldMesh.h"

ioWorldMesh::ioWorldMesh( ioGameSceneManager *pSceneMgr )
{
	m_pCreator = pSceneMgr;
}

ioWorldMesh::~ioWorldMesh()
{
	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;

		ioSceneNode *pScene = rkGroupData.m_pGroup->GetParentSceneNode();
		if( pScene )
		{
			pScene->RemoveAndDestroyAllChildren();
			m_pCreator->DestroySceneNode( pScene->GetUniqueID() );
		}

		m_pCreator->DestroyEntityGrp( rkGroupData.m_pGroup );
		rkGroupData.m_pGroup = NULL;
	}
	m_GroupList.clear();
}

void ioWorldMesh::ShadowMapEnable( bool bEnable )
{
	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;

		rkGroupData.m_pGroup->SetShadowRecvEnable( bEnable );
		int iEntityCnt = rkGroupData.m_pGroup->GetNumEntities();
		for( int i=0 ; i<iEntityCnt ; i++ )
		{
			ioEntity *pEntity = rkGroupData.m_pGroup->GetEntity( i );

			int iSubEntityCnt = pEntity->GetNumSubEntity();
			for( int j=0 ; j<iSubEntityCnt ; j++ )
			{
				ioTechnique *pTech = pEntity->GetSubEntity( j )->GetTechnique();
				if( !pTech )	continue;

				ioPass *pPass = pTech->GetPass( 0 );
				if( pPass && pPass->HasRenderShadowShader() )
				{
					pPass->AddPassProperty( ioPass::PP_SHADOW_RECV_PASS );
				}
			}
		}
	}	
}

void ioWorldMesh::MapEdgeEnable( bool bEnable )
{
	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;

		rkGroupData.m_pGroup ->EnableEdgeRender( bEnable );
	}	
}

void ioWorldMesh::StopAllTextureEffect( bool bStop )
{
	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;

		int iEntityCnt = rkGroupData.m_pGroup->GetNumEntities();
		for( int i=0 ; i<iEntityCnt ; i++ )
		{
			ioEntity *pEntity = rkGroupData.m_pGroup->GetEntity( i );

			int iSubEntityCnt = pEntity->GetNumSubEntity();
			for( int j=0 ; j<iSubEntityCnt ; j++ )
			{
				ioMaterial *pMaterial = pEntity->GetSubEntity( j )->GetMaterial();
				if( pMaterial )
				{
					pMaterial->StopAllTextureEffect( bStop );
				}
			}
		}
	}		
}

bool ioWorldMesh::LoadWorldMesh( ioINILoader &rkLoader )
{
	if( m_pCreator == NULL ) return false;

	if( m_pCreator->IsMultiMap() )
	{
		rkLoader.SetTitle( "map" );

		for(int i = 0;i < m_pCreator->GetMaxMultiMapCount();i++)
		{
			char szKey[MAX_PATH];
			sprintf_e( szKey, "mesh_name%d", i + 1 );

			char szMeshName[MAX_PATH];
			rkLoader.LoadString( szKey, "", szMeshName, MAX_PATH );
			if( !strcmp( szMeshName, "" ) )
			{
				LOG.PrintTimeAndLog( 0, "ioWorldMesh::LoadWorldMesh %s mesh_name Emtpy Multi(%d)", rkLoader.GetFileName(), i + 1 );
				return false;
			}

			EntityGroupData kEntityGroup;
			kEntityGroup.m_iMapIndex = i;
			kEntityGroup.m_pGroup    = m_pCreator->CreateEntityGrp();

			ioSceneNode *pSNode = m_pCreator->GetRootSceneNode()->CreateChildSceneNode();
			pSNode->AttachObject( kEntityGroup.m_pGroup );

			kEntityGroup.m_pGroup->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );
			kEntityGroup.m_pGroup->SetEachEntityCull( true );
			kEntityGroup.m_pGroup->LoadGroup( szMeshName, false );

			kEntityGroup.m_pGroup->SetLightReceiveType( LRT_ALL_REAL_TIME );
			kEntityGroup.m_pGroup->SetRenderQueueGroup( RENDER_PRE_WORLD );

			if( i == 0 )
			{	// 첫번째 충돌 영역만 유지
				m_WorldAABB.SetByOrientBox( kEntityGroup.m_pGroup->GetCollisionBox() );
			}
			m_GroupList.push_back( kEntityGroup );
		}
		SetMultiMapCurrentIndex( m_pCreator->GetMultiMapCurrentIndex() );
	}
	else
	{
		rkLoader.SetTitle( "map" );

		char szMeshName[MAX_PATH];
		rkLoader.LoadString_e( "mesh_name", "", szMeshName, MAX_PATH );
		if( !strcmp( szMeshName, "" ) )
		{
			LOG.PrintTimeAndLog( 0, "ioWorldMesh::LoadWorldMesh %s mesh_name Emtpy No Multi", rkLoader.GetFileName() );
			return false;
		}

		EntityGroupData kEntityGroup;
		kEntityGroup.m_iMapIndex = 0;
		kEntityGroup.m_pGroup    = m_pCreator->CreateEntityGrp();

		ioSceneNode *pSNode = m_pCreator->GetRootSceneNode()->CreateChildSceneNode();
		pSNode->AttachObject( kEntityGroup.m_pGroup );

		kEntityGroup.m_pGroup->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );
		kEntityGroup.m_pGroup->SetEachEntityCull( true );
		kEntityGroup.m_pGroup->LoadGroup( szMeshName, false );

		kEntityGroup.m_pGroup->SetLightReceiveType( LRT_ALL_REAL_TIME );
		kEntityGroup.m_pGroup->SetRenderQueueGroup( RENDER_PRE_WORLD );

		m_WorldAABB.SetByOrientBox( kEntityGroup.m_pGroup->GetCollisionBox() );		
		m_GroupList.push_back( kEntityGroup );
	}
	return true;
}

void ioWorldMesh::LoadLightDecalTextureList( ioINILoader &rkLoader )
{
	m_LightDecalTextureList.clear();

	rkLoader.SetTitle_e( "decal_texture" );

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	int iTextureCnt = rkLoader.LoadInt_e( "texture_cnt", 0 );
	for( int i=0 ; i<iTextureCnt ; i++ )
	{
		wsprintf_e( szKey, "decal_tex%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_LightDecalTextureList.push_back( ioHashString(szBuf) );
	}
}

void ioWorldMesh::Update()
{
	if( m_pCreator == NULL ) return;

	int iCurrentMapIndex = m_pCreator->GetMultiMapCurrentIndex();

	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;
		if( !COMPARE( rkGroupData.m_iMapIndex, iCurrentMapIndex - 1, iCurrentMapIndex + 2 ) ) continue;

		rkGroupData.m_pGroup->Update( 0.0f );
	}	
}

ioEntityGroup *ioWorldMesh::GetEntityGroup( int iMapIndex )
{
	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;

		if( rkGroupData.m_iMapIndex == iMapIndex )
		{
			return rkGroupData.m_pGroup;
		}
	}	
	return NULL;
}

ioDecal* ioWorldMesh::CreateDecal( int iMapIndex, const ioHashString &rkName, const ioOrientBox &rkBound, float fEpsilon /* = -FLOAT1  */ )
{
	ioEntityGroup *pGroup = GetEntityGroup( iMapIndex );
	if( !pGroup )
	{
		LOG.PrintTimeAndLog( 0, "ioWorldMesh::CreateDecal - Not Initialized - %d", iMapIndex + 1 );
		return NULL;
	}

	ioDecalMaker kMaker;
	kMaker.Initialize( rkBound, fEpsilon );

	ioAxisAlignBox kBoundAABB;
	kBoundAABB.SetByOrientBox( rkBound );

	ioEntity *pEntity;
	int iEntityCnt = pGroup->GetNumEntities();
	for( int i=0 ; i<iEntityCnt ; i++ )
	{
		pEntity = pGroup->GetEntity( i );

		if( ioMath::TestIntersection( kBoundAABB, pEntity->GetBoundBox() ) )
		{
			kMaker.AddDecal( pEntity );
		}
	}

	return kMaker.BuildDecal( rkName );
}

void ioWorldMesh::AddOmniLightDecal( int iMapIndex, ioLight *pLight )
{
	if( pLight->GetType() != LT_POINT )
		return;

	if( !pLight->GetDecalTexture() )
	{
		if( !m_LightDecalTextureList.empty() )
		{
			int iRandIdx = rand() % m_LightDecalTextureList.size();
			pLight->SetDecalTexture( m_LightDecalTextureList[iRandIdx] );
		}
	}

	BuildOmniLightDecal( iMapIndex, pLight );
}

void ioWorldMesh::BuildOmniLightDecal( int iMapIndex, ioLight *pLight )
{
	ioEntityGroup *pGroup = GetEntityGroup( iMapIndex );
	if( !pGroup ) return;
	if( !strcmp( pLight->GetDecalTextureName(), "" ) ) return;

	ioOpcodeShape *pShape = GetCollisionShape();
	if( !pShape )	return;

	float fRange = pLight->GetAttenuationRange();
	D3DXVECTOR3 vLightPos = pLight->GetDerivedPosition();

	ioSphere kLightSphere( vLightPos, fRange );
	if( !pShape->TestIntersection( NULL, kLightSphere ) )
		return;

	D3DXVECTOR3 vColPoint;
	ioRay kDownRay( vLightPos, D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	if( !pShape->TestIntersection( NULL, kDownRay, NULL, &vColPoint ) )
		return;

	float fDist = D3DXVec3Length( &(vLightPos - vColPoint) );
	if( fDist > fRange )	return;

	fDist = sqrt( fRange*fRange - fDist*fDist );

	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vLightPos;
	vMin.x -= fDist;
	vMin.y -= fRange;
	vMin.z -= fDist;

	vMax.x += fDist;
	vMax.y += fRange;
	vMax.z += fDist;

	ioOrientBox kBound;
	kBound.SetBoxByMinMax( vMin, vMax );
	ioDecal *pDecal = CreateDecal( iMapIndex, pLight->GetName(), kBound, -0.3f );
	if( !pDecal )	return;

	ioMaterial *pMtl = pDecal->GetMaterial();
	pMtl->SetDiffuse( pLight->GetDiffuseColor() );

	ioPass *pPass = pMtl->CreateTechnique()->CreatePass();
	pPass->SetShaderGroup( "DecalToon" );
	pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE );
	pPass->SetDepthCheckEnable( true );
	pPass->SetDepthWriteEnable( false );
	pPass->SetDepthBias( -0.001f, 0.0f );
	pPass->SetLightingEnable( false );

	ioTextureUnitState *pState = pPass->CreateTextureUnitState( pLight->GetDecalTextureName() );
	pState->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pState->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pState->SetTextureAddressMode( TAM_CLAMP );

	pMtl->Load( EP_MANUAL );

	pGroup->AddDecal( pDecal );
	m_DecalLightMap[pLight->GetName()] = pLight->GetDerivedPosition();
}

void ioWorldMesh::UpdateEffectLightDecal( int iMapIndex, ioLight *pLight )
{
	ioEntityGroup *pGroup = GetEntityGroup( iMapIndex );
	if( !pGroup ) return;

	ioDecal *pDecal = pGroup->GetDecal( pLight->GetName() );
	if( !pDecal )	return;

	DecalLightMap::iterator iter = m_DecalLightMap.find( pLight->GetName() );
	if( iter == m_DecalLightMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "ioWorldMesh::UpdateEffectLightDecal - Not Exist Decal" );
		return;
	}

	if( !ioMath::IsEqual( iter->second, pLight->GetDerivedPosition() ) )
	{
		pGroup->DestroyDecal( pLight->GetName() );
		AddOmniLightDecal( iMapIndex, pLight );
	}
	else
	{
		ioMaterial *pMaterial = pDecal->GetMaterial();
		if( pMaterial )
		{
			pMaterial->SetDiffuse( pLight->GetRatedDiffuseColor() );
		}
	}
}

bool ioWorldMesh::HasLightDecal( int iMapIndex, const ioHashString &rkName )
{
	ioEntityGroup *pGroup = GetEntityGroup( iMapIndex );
	if( !pGroup ) return false;

	if( pGroup->GetDecal( rkName ) )
		return true;

	return false;
}

void ioWorldMesh::DestroyLightDecal( int iMapIndex, const ioHashString &rkName )
{
	ioEntityGroup *pGroup = GetEntityGroup( iMapIndex );
	if( pGroup )
	{
		pGroup->DestroyDecal( rkName );
		m_DecalLightMap.erase( rkName );
	}
}

void ioWorldMesh::DestroyAllLightDecal( int iMapIndex )
{
	ioEntityGroup *pGroup = GetEntityGroup( iMapIndex );
	if( pGroup )
	{
		pGroup->DestroyAllDecal();
		m_DecalLightMap.clear();
	}
}

ioOpcodeShape* ioWorldMesh::GetCollisionShape()
{
	if( m_pCreator == NULL )
		return NULL;

	const ioEntityGroup *pGroup = GetEntityGroup( 0 );//GetEntityGroup( m_pCreator->GetMultiMapCurrentIndex() );
	if( pGroup )
		return pGroup->GetOpcodeShape();

	return NULL;
}

float ioWorldMesh::GetHeight( float fXPos, float fZPos )
{
	if( m_pCreator == NULL )
		return 0.0f;

	const ioEntityGroup *pGroup = GetEntityGroup( 0 );//GetEntityGroup( m_pCreator->GetMultiMapCurrentIndex() );
	if( !pGroup )
		return 0.0f;

	ioOpcodeShape *pShape = pGroup->GetOpcodeShape();
	if( pShape )
	{
		ioRay kYRay( D3DXVECTOR3(fXPos, 100000.0f, fZPos), -ioMath::UNIT_Y );
		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );

		if( pShape->TestIntersection( NULL, kYRay, NULL, &vColPoint ) )
			return vColPoint.y;
	}

	return 0.0f;
}

float ioWorldMesh::GetHeight( const D3DXVECTOR3 &vPos )
{
	if( m_pCreator == NULL )
		return 0.0f;

	const ioEntityGroup *pGroup = GetEntityGroup( 0 );//GetEntityGroup( m_pCreator->GetMultiMapCurrentIndex() );
	if( !pGroup )
		return 0.0f;

	ioOpcodeShape *pShape = pGroup->GetOpcodeShape();
	if( pShape )
	{
		ioRay kYRay( vPos, -ioMath::UNIT_Y );
		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );

		if( pShape->TestIntersection( NULL, kYRay, NULL, &vColPoint ) )
			return vColPoint.y;
	}

	return 0.0f;
}

void ioWorldMesh::SetMultiMapCurrentIndex( int iMapIndex )
{
	EntityGroupDataList::iterator iter = m_GroupList.begin();
	for(;iter != m_GroupList.end();++iter)
	{
		EntityGroupData &rkGroupData = *iter;
		if( rkGroupData.m_pGroup == NULL ) continue;

		if( rkGroupData.m_iMapIndex == iMapIndex )
		{
			rkGroupData.m_pGroup->SetVisible( true );
		}
		else
		{
			rkGroupData.m_pGroup->SetVisible( false );
		}
	}	
}