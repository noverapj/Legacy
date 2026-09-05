

#include "stdafx.h"

#include "ioRcAnimation.h"
#include "ioAnimationManager.h"

#include "ioEntityGroup.h"

#include "ioSkeleton.h"
#include "ioAniController.h"
#include "ioAnimation.h"
#include "ioEntityGroupBuilder.h"

#include "ioNode.h"
#include "ioSceneNode.h"
#include "ioCamera.h"

#include "ioEntity.h"
#include "ioMeshManager.h"
#include "ioOpcodeShape.h"
#include "ioOpcodeManager.h"
#include "ioShaderParameter.h"

#include "ioStringInterface.h"
#include "ioEffect.h"
#include "ioEffectFactory.h"
#include "ioLight.h"
#include "ioMeshTrailer.h"

#include "HelpFunc.h"

#include "ioDecal.h"
#include "ioRenderQueue.h"
#include "ioEdgeRender.h"
#include "ioSkeletonManager.h"
#include "ioResourceLoader.h"

#include "ioFrameTimer.h"
#include "ErrorReport.h"

ioEntityGroup::ioEntityGroup()
{
	m_pAniController = NULL;
	m_pSkeleton = NULL;
	m_pEdgeRender = NULL;

	m_pOpcodeShape = NULL;

	m_fCameraDistSq = 0.0f;
	m_MeshColor.r = m_MeshColor.g = m_MeshColor.b = m_MeshColor.a = 1.0f;

	m_LightReceiveType = LRT_ALL_REAL_TIME;
	m_dwLightApplyMask = 0x1;

	m_dwLastBipedCachedFrame = 0;

	m_bDestroyDeadEffect = true;
	m_bSoftwareVertexBlend = false;
	m_bEnableEachEntityCull = false;

	m_pParentEvent = NULL;

	m_iCurAlphaRate = MAX_ALPHA_RATE;
}

ioEntityGroup::~ioEntityGroup()
{
	RcAnimationList::iterator iter=m_RcAnimationList.begin();
	for( ; iter!=m_RcAnimationList.end() ; ++iter )
	{
		ioAnimationManager::GetSingleton().Destroy( *iter );
	}
	m_RcAnimationList.clear();

	DestroyAllDecal();
	DestroyAllEffect();
	DestroyAllTrailer();

	RemoveEdgeRender();

	ClearAllRopeSpringCurve();

	SAFEDELETE(m_pSkeleton);
	SAFEDELETE(m_pAniController);

	if( m_pOpcodeShape )
	{
		g_OpcodeMgr.Destroy( m_pOpcodeShape );
		m_pOpcodeShape = NULL;
	}

	DestroyAllEntity();
}

bool ioEntityGroup::LoadGroup( const char *szFileName, bool bThread )
{
	ioEntityGroupBuilder kBuilder;
	return kBuilder.BuildGroup( szFileName, this, bThread );
}

bool ioEntityGroup::HasSkeleton() const
{
	return ( m_pSkeleton != NULL );
}

void ioEntityGroup::SetSkeletonName( const ioHashString &name )
{
	SAFEDELETE(m_pSkeleton);

	m_pSkeleton = ioSkeletonManager::GetSingleton().CreateInstance( name );
	if( m_pSkeleton )
	{
		if( !m_pAniController )
		{
			m_pAniController = new ioAniController( this );
		}

		RopeSpringBackUp::iterator iter = m_RopeSpringBackUp.begin();
		for( ; iter!=m_RopeSpringBackUp.end() ; ++iter )
		{
			m_pSkeleton->AddRopeSpringCurve( iter->first, iter->second );
		}        
	}
	else
	{
		ClearAllRopeSpringCurve();
		SAFEDELETE( m_pAniController );
	}
}

bool ioEntityGroup::SetAniEventHandler( ioAniEventHandler *pHandler )
{
	if( m_pAniController )
	{
		m_pAniController->SetAniEventHandler( pHandler );
		return true;
	}

	return false;
}

void ioEntityGroup::HideMeshExceptionCheck( const ioHashString &szMeshName )
{
	if( szMeshName.IsEmpty() ) return;

	if( m_pAniController )
	{
		m_pAniController->HideMeshExceptionCheck( szMeshName );
	}
}

ioHashString ioEntityGroup::ConvertCustomAniName( const ioHashString &rkAniName )
{
	ioHashString kName = rkAniName;
	if( m_szCustomSkeletonAniName.IsEmpty() )
		return kName;

	char szSplitName[MAX_PATH] = "";
	_splitpath( rkAniName.c_str(), NULL, NULL, szSplitName, NULL );

	char szFileName[MAX_PATH] = "";
	sprintf( szFileName, "resource\\animation\\%s%s.ani", szSplitName, m_szCustomSkeletonAniName.c_str() );
	if( !g_ResourceLoader.IsFileExist( szFileName ) )
	{
		LOG.PrintTimeAndLog( 0, "Animation File Not Exist : %s", szFileName );
		return kName;
	}

	sprintf( szFileName, "%s%s.ani", szSplitName, m_szCustomSkeletonAniName.c_str() );
	return szFileName;
}

bool ioEntityGroup::AddAnimation( const ioHashString &name )
{
	if( name.IsEmpty() )
		return false;
	
	RcAnimationList::iterator iter;
	for( iter=m_RcAnimationList.begin(); iter!=m_RcAnimationList.end() ; ++iter )
	{
		if( (*iter)->GetName() == name )
			return true;
	}

	ioRcAnimation *pRcAnimation = ioAnimationManager::GetSingleton().CreateAnimation( name );
	if( !pRcAnimation )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::AddAnimation - %s Cannot Load", name.c_str() );
		return false;
	}

	m_RcAnimationList.push_back( pRcAnimation );

	return true;
}

void ioEntityGroup::AddDecal( ioDecal *pDecal )
{
	DecalList::iterator iter = m_DecalList.find( pDecal->GetName() );
	if( iter == m_DecalList.end() )
	{
		m_DecalList.insert( DecalList::value_type( pDecal->GetName(), pDecal ) );
	}
	else
	{
		delete iter->second;
		iter->second = pDecal;
	}
	
	pDecal->NotifyAttached( this );
}

void ioEntityGroup::DestroyDecal( const ioHashString &rkName )
{
	DecalList::iterator iter = m_DecalList.find( rkName );
	if( iter != m_DecalList.end() )
	{
		delete iter->second;
		m_DecalList.erase( iter );
	}
}

void ioEntityGroup::DestroyAllDecal()
{
	DecalList::iterator iter;
	for( iter=m_DecalList.begin() ; iter!=m_DecalList.end() ; ++iter )
	{
		delete iter->second;
	}

	m_DecalList.clear();
}

ioDecal* ioEntityGroup::GetDecal( const ioHashString &rkName ) const
{
	DecalList::const_iterator iter = m_DecalList.find( rkName );
	if( iter != m_DecalList.end() )
		return iter->second;

	return NULL;
}

ioDecal* ioEntityGroup::GetDecal( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetDecalCnt() ) )
	{
		DecalList::const_iterator iter = m_DecalList.begin();
		std::advance( iter, iIndex );
		return iter->second;
	}

	LOG.PrintTimeAndLog( 0, "ioEntityGroup::GetDecal - overflow(%d/%d)",
							iIndex, GetDecalCnt() );

	return NULL;
}

int ioEntityGroup::GetDecalCnt() const
{
	return m_DecalList.size();
}

void ioEntityGroup::AddEdgeRender( ioEdgeRender *pEdge )
{
	RemoveEdgeRender();

	m_pEdgeRender = pEdge;
	m_pEdgeRender->SetOwner( this );
}

void ioEntityGroup::EnableEdgeRender( bool bEnable )
{
	if( m_pEdgeRender )
	{
		m_pEdgeRender->Enable( bEnable );
	}
}

void ioEntityGroup::RemoveEdgeRender()
{
	SAFEDELETE( m_pEdgeRender );
}

bool ioEntityGroup::AddRopeSpringCurve( const ioHashString &rkBiped,
									    const RopeSpringParam *pParam )
{
	if( !m_pSkeleton )
		return false;

	if( !m_pSkeleton->AddRopeSpringCurve( rkBiped, pParam ) )
		return false;

	RopeSpringParam *pSaveParam = new RopeSpringParam;
	*pSaveParam = *pParam;

	RopeSpringBackUp::iterator iter = m_RopeSpringBackUp.find( rkBiped );
	if( iter != m_RopeSpringBackUp.end() )	// 기존에 같은게 존재한다.
	{
		delete iter->second;
		iter->second = pSaveParam;
	}
	else
	{
		m_RopeSpringBackUp.insert( RopeSpringBackUp::value_type( rkBiped, pSaveParam ) );
	}

	return true;
}

bool ioEntityGroup::RemoveRopeSpringCurve( const ioHashString &rkBiped )
{
	if( !m_pSkeleton )
		return false;

	RopeSpringBackUp::iterator iter = m_RopeSpringBackUp.find( rkBiped );
	if( iter != m_RopeSpringBackUp.end() )
	{
		delete iter->second;
		m_RopeSpringBackUp.erase( iter );
	}

	return m_pSkeleton->RemoveRopeSpringCurve( rkBiped );
}

void ioEntityGroup::ClearAllRopeSpringCurve()
{
	RopeSpringBackUp::iterator iRope = m_RopeSpringBackUp.begin();
	for( ; iRope != m_RopeSpringBackUp.end() ; ++iRope )
	{
		delete iRope->second;
	}

	if( m_pSkeleton )
	{
		m_pSkeleton->ClearAllRopeSpringCurve();
	}
}

void ioEntityGroup::AddWorldForce( const D3DXVECTOR3 &vMoveAmt )
{
	if( !IsAttached() )	return;
	if( !m_pSkeleton || !m_pSkeleton->HasRopeSpring() )
		return;
	if( !GetParentSceneNode() )
		return;

	D3DXMATRIX matInvXform;
	D3DXMatrixInverse( &matInvXform, NULL, GetParentSceneNode()->GetFullTransform() );

	D3DXVECTOR3 vLocalMove;
	D3DXVec3TransformNormal( &vLocalMove, &vMoveAmt, &matInvXform );
	m_pSkeleton->AddWorldForce( vLocalMove );
}

ioMeshTrailer* ioEntityGroup::CreateTrailer()
{
	ioMeshTrailer *pTrailer = new ioMeshTrailer( this );
	if( pTrailer )
	{
		m_MeshTrailerList.push_back( pTrailer );
	}

	return pTrailer;
}

void ioEntityGroup::SetGraceCloseAllTrailer()
{
	MeshTrailerList::iterator iter;
	for( iter=m_MeshTrailerList.begin() ; iter!=m_MeshTrailerList.end() ; ++iter )
	{
		(*iter)->SetGraceClose();
	}
}

void ioEntityGroup::DestroyTrailer( ioMeshTrailer *pTrail )
{
	if( pTrail )
	{
		m_MeshTrailerList.remove( pTrail );
		delete pTrail;
	}
}

void ioEntityGroup::DestroyAllTrailer()
{
	MeshTrailerList::iterator iter;
	for( iter=m_MeshTrailerList.begin() ; iter!=m_MeshTrailerList.end() ; ++iter )
	{
		delete *iter;
	}
	m_MeshTrailerList.clear();
}

void ioEntityGroup::SetAlphaRate( int iAlphaRate )
{
	m_iCurAlphaRate = iAlphaRate;

	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		(*iter)->SetAlphaRate( iAlphaRate );
	}
}

void ioEntityGroup::SetAttachEffectAlphaRate( int iAlphaRate, bool bMinZero )
{
	float fAlphaRate = iAlphaRate/(float)MAX_ALPHA_RATE;

	AttachedEffectList::iterator iter = m_AttachedEffectList.begin();
	while( iter != m_AttachedEffectList.end() )
	{
		ioEffect *pEffect = *iter;
		if( pEffect )
			pEffect->SetExtendColorRate( fAlphaRate, bMinZero );

		++iter;
	}
}

void ioEntityGroup::SetLightReceiveType( LightReceiveType eType )
{
	m_LightReceiveType = eType;
}

void ioEntityGroup::UpdateApplyLightList()
{
	const ioLightList &rkList = GetParentSceneNode()->GetLightList();

	ioLightList vRecvEnableList;
	vRecvEnableList.reserve( rkList.size() );

	ioLightList::const_iterator iter;
	for( iter=rkList.begin() ; iter!=rkList.end() ; ++iter )
	{
		ioLight *pLight = *iter;

		if( pLight->IsValidLightByReceiveType( m_LightReceiveType ) &&
			pLight->HasLightApplyMask( m_dwLightApplyMask ) )
		{
			vRecvEnableList.push_back( pLight );
		}
	}

	if( HasSkeleton() )
	{
		EntityList::iterator iEntity;
		for( iEntity=m_EntityList.begin() ; iEntity!=m_EntityList.end() ; ++iEntity )
		{
			ioEntity *pEntity = *iEntity;

			if( !pEntity->IsCulled() )
			{
				pEntity->SetLightList( vRecvEnableList );
			}
		}
	}
	else
	{
		D3DXQUATERNION qtParent  = GetParentSceneNode()->GetDerivedOrientation();
		D3DXVECTOR3 vParentScale = GetParentSceneNode()->GetDerivedScale();
		D3DXVECTOR3 vParentPos   = GetParentSceneNode()->GetDerivedPosition();

		EntityList::iterator iEntity;
		for( iEntity=m_EntityList.begin() ; iEntity!=m_EntityList.end() ; ++iEntity )
		{
			ioEntity *pEntity = *iEntity;

			if( !pEntity->IsCulled() )
			{
				pEntity->UpdateLightList( vRecvEnableList,
										  qtParent,
										  vParentPos,
										  vParentScale );
			}
		}
	}
}

void ioEntityGroup::CacheBipedMatrices()
{
	if( !HasSkeleton() )	return;
	if( m_dwLastBipedCachedFrame == g_FrameTimer.GetFrameCounter() )
		return;

	EntityList::iterator iter;
	for( iter=m_EntityList.begin(); iter!=m_EntityList.end(); ++iter )
	{
		ioEntity *pEntity = *iter;

		if( !pEntity->IsCulled() )
		{
			pEntity->CacheBipedMatrices( m_pSkeleton );

			if( m_bSoftwareVertexBlend )
			{
				pEntity->SoftwareVertexBlending();
			}
		}
	}

	m_dwLastBipedCachedFrame = g_FrameTimer.GetFrameCounter();
}

void ioEntityGroup::AddEntityEvent( ioEntity *pEntity )
{
	if( pEntity == NULL ) return;

	if( m_pParentEvent )
	{
		m_pParentEvent->AddEntityEvent( pEntity );
	}
}

ioEntity* ioEntityGroup::AddEntity( const ioHashString &rkMeshName, bool bThread )
{
	ioMesh *pMesh = ioMeshManager::GetSingleton().CreateMesh( rkMeshName, bThread );
	if( !pMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::AddEntity - %s Load Failed", rkMeshName.c_str() );
		return NULL;
	}

	ioEntity *pEntity = new ioEntity( this, pMesh );

	bool bBuildDone = false;
	if( HasSkeleton() )
	{
		bBuildDone = pEntity->BuildEntity( m_pSkeleton, m_bSoftwareVertexBlend );
	}
	else
	{
		bBuildDone = pEntity->BuildEntity( NULL, false );
	}

	m_EntityList.push_back( pEntity );	
	if( bBuildDone )
	{
		UpdateBounds( true );
	}
	else
	{
		m_NotBuildEntityList.push_back( pEntity );
	}
	AddEntityEvent( pEntity );
	return pEntity;
}

ioEntity* ioEntityGroup::GetEntity( int iIndex )
{
	if( COMPARE( iIndex, 0, GetNumEntities() ) )
		return m_EntityList[iIndex];

	return NULL;
}

ioEntity* ioEntityGroup::GetEntity( const ioHashString &rkMeshName )
{
	EntityList::iterator iter;
	for( iter=m_EntityList.begin(); iter!=m_EntityList.end(); ++iter )
	{
		ioEntity *pEntity = *iter;

		if( pEntity->GetMeshName() == rkMeshName )
			return pEntity;
	}

	return NULL;
}

bool ioEntityGroup::DestroyEntity( int iIndex )
{
	if( COMPARE( iIndex, 0, GetNumEntities() ) )
	{
		EntityList::iterator iter = m_EntityList.begin();
		std::advance( iter, iIndex );

		ioEntity *pEntity = *iter;
		EraseNotBuildEntity( pEntity );
		m_EntityList.erase( iter );
		delete pEntity;
		return true;
	}

	return false;
}

bool ioEntityGroup::DestroyEntity( const ioHashString &rkMeshName )
{
	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		ioEntity *pEntity = *iter;
		if( !pEntity )	continue;

		if( pEntity->GetMeshName() == rkMeshName )
		{
			EraseNotBuildEntity( pEntity );
			m_EntityList.erase( iter );
			delete pEntity;
			return true;
		}
	}

	return false;
}

void ioEntityGroup::DestroyAllEntity()
{
	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		delete *iter;
	}

	m_NotBuildEntityList.clear();
	m_EntityList.clear();
}

void ioEntityGroup::SetEntityVisible( const ioHashString &szMeshName, bool bVisible )
{
	ioEntity *pEntity = GetEntity( szMeshName );
	if( pEntity )
	{
		pEntity->SetVisible( bVisible );
	}
}

bool ioEntityGroup::SetActionAni( int id,
								  float fFadeIn,
								  float fFadeOut,
								  float fWeight,
								  float fTimeFactor,
								  float fWaitTime,
								  float fPreDelay,
								  bool bHoldLastFrame,
								  bool bUseFX )
{
	if( !HasSkeleton() )
		return false;

	if( !COMPARE( id, 0, GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetActionAni - over flow(%d/%d)",
								id, GetAnimationCnt() );
		return false;
	}

	m_pAniController->ExecuteAction( id,
									 fFadeIn,
									 fFadeOut,
									 fWeight,
									 fTimeFactor,
									 fWaitTime,
									 fPreDelay,
									 bHoldLastFrame,
									 bUseFX );

	return true;
}

bool ioEntityGroup::SetActionAni( const ioHashString &rkName,
								  float fFadeIn,
								  float fFadeOut,
								  float fWeight,
								  float fTimeFactor,
								  float fWaitTime,
								  float fPreDelay,
								  bool bHoldLastFrame,
								  bool bUseFX )
{
	if( !HasSkeleton() )
		return false;

	int iAniID = GetAnimationIdx( rkName );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetActionAni - %s Not Exist Ani", rkName.c_str() );
		return false;
	}

	SetActionAni( iAniID,
				  fFadeIn,
				  fFadeOut,
				  fWeight,
				  fTimeFactor,
				  fWaitTime,
				  fPreDelay,
				  bHoldLastFrame,
				  bUseFX );

	return true;
}

bool ioEntityGroup::SetActionAniForTime( int id,
									     float fFadeIn,
									     float fFadeOut,
									     float fPlayTime,
									     float fWeight,
									     float fWaitTime )
{
	if( !HasSkeleton() || fPlayTime <= 0.0f )
		return false;

	if( !COMPARE( id, 0, GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetActionAniForTime - over flow(%d/%d)",
								id, GetAnimationCnt() );
		return false;
	}

	float fTimeFactor = GetAnimationFullTime( id ) / fPlayTime;
	m_pAniController->ExecuteAction( id,
									 fFadeIn,
									 fFadeOut,
									 fWeight,
									 fTimeFactor,
									 fWaitTime,
									 0.0f,
									 true );

	return true;
}

bool ioEntityGroup::SetActionAniForTime( const ioHashString &rkName,
									     float fFadeIn,
									     float fFadeOut,
									     float fPlayTime,
									     float fWeight,
									     float fWaitTime )
{
	if( !HasSkeleton() || fPlayTime <= 0.0f )
		return false;

	int iAniID = GetAnimationIdx( rkName );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetActionAniForTime - %s Not Exist", rkName.c_str() );
		return false;
	}
		
	SetActionAniForTime( iAniID, fFadeIn, fFadeOut, fPlayTime, fWeight, fWaitTime );
	return true;
}

bool ioEntityGroup::SetPlayActionCurTime( int iId, float fCurTime )
{
	ioAnimation *pCurAni = GetActionInPlay( iId );
	if( pCurAni )
	{
		pCurAni->SetCurTime( fCurTime );
		return true;
	}

	return false;
}

bool ioEntityGroup::SetPlayActionCurTime( const ioHashString &rkName, float fCurTime )
{
	ioAnimation *pCurAni = GetActionInPlay( rkName );
	if( pCurAni )
	{
		pCurAni->SetCurTime( fCurTime );
		return true;
	}

	return false;
}

bool ioEntityGroup::ClearActionAni( int id, float fFadeOut )
{
	if( !HasSkeleton() )	return false;
	if( !COMPARE( id, 0, GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::ClearActionAni - overflow(%d/%d)",
							    id, GetAnimationCnt() );
		return false;
	}

	m_pAniController->ClearAction( id, fFadeOut );
	return true;
}

void ioEntityGroup::ClearAllActionAni( float fFadeOut, bool bWaitAlso )
{
	if( m_pAniController )
	{
		m_pAniController->ClearAllAction( fFadeOut, bWaitAlso );
	}
}

void ioEntityGroup::StopAllPlayingAnimation()
{
	if( m_pAniController )
	{
		m_pAniController->StopAllPlayingAnimation();
	}
}

void ioEntityGroup::ResumeAllPlayingAnimation()
{
	if( m_pAniController )
	{
		m_pAniController->ResumeAllPlayingAnimation();
	}
}

ioAnimation* ioEntityGroup::GetActionInPlay( const ioHashString &kName )
{
	if( m_pAniController )
	{
		ioAniController::ioAniList vAniList;
		m_pAniController->FindActionAniInPlay( kName, vAniList );
		if( !vAniList.empty() )
		{
			return vAniList.front();
		}
	}

	return NULL;
}

ioAnimation* ioEntityGroup::GetActionInPlay( int iIdx )
{
	if( COMPARE( iIdx, 0, GetAnimationCnt() ) )
		return GetActionInPlay( GetAnimationName( iIdx ) );

	return NULL;
}

ioAnimation* ioEntityGroup::GetActionInPlayByOrder( int iOrder )
{
	if( m_pAniController )
		return m_pAniController->GetPlayingActionByOrder( iOrder );

	return NULL;
}

int ioEntityGroup::GetCurPlayingActionCnt() const
{
	if( m_pAniController )
		return m_pAniController->GetPlayingActionCnt();

	return 0;
}

ioAnimation* ioEntityGroup::GetLoopInPlayByOrder( int iOrder )
{
	if( m_pAniController )
		return m_pAniController->GetPlayingLoopByOrder( iOrder );

	return NULL;
}

int ioEntityGroup::GetCurPlayingLoopCnt() const
{
	if( m_pAniController )
		return m_pAniController->GetPlayingLoopCnt();

	return 0;
}

bool ioEntityGroup::SetLoopAni( int id,
								float fFadeIn,
								float fWeight,
								float fTimeFactor,
								float fWaitTime )
{
	if( !HasSkeleton() )	return false;
	if( !COMPARE( id, 0, GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetLoopAni - over flow(%d/%d)",
							   id, GetAnimationCnt() );
		return false;
	}

	m_pAniController->ExecuteLoop( id, fFadeIn, fWeight, fTimeFactor, fWaitTime );
	return true;
}

bool ioEntityGroup::SetLoopAni( const ioHashString &rkName,
								float fFadeIn,
								float fWeight,
								float fTimeFactor,
								float fWaitTime )
{
	if( !HasSkeleton() )	return false;

	int iAniID = GetAnimationIdx( rkName );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetLoopAni - %s Not Exist", rkName.c_str() );
		return false;
	}
	
	SetLoopAni( iAniID, fFadeIn, fWeight, fTimeFactor, fWaitTime );
	return true;
}

bool ioEntityGroup::SetLoopAniInMid( int id,
								     float fStartTime,
								     float fFadeIn,
								     float fWeight,
								     float fTimeFactor )
{
	if( !HasSkeleton() )	return false;
	if( !COMPARE( id, 0, GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetLoopAniInMid - over flow(%d/%d)",
								id, GetAnimationCnt() );
		return false;
	}
		
	m_pAniController->ExecuteLoopInMid( id, fStartTime, fFadeIn, fWeight, fTimeFactor );
	return true;
}

bool ioEntityGroup::SetLoopAniInMid( const ioHashString &rkName,
								     float fStartTime,
								     float fFadeIn,
								     float fWeight,
								     float fTimeFactor )
{
	if( !HasSkeleton() )	return false;

	int iAniID = GetAnimationIdx( rkName );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::SetLoopAniInMid - %s Not Exist", rkName.c_str() );
		return false;
	}

	SetLoopAniInMid( iAniID, fStartTime, fFadeIn, fWeight, fTimeFactor );
	return true;
}

bool ioEntityGroup::ClearLoopAni( int id, float fFadeOut, bool bAlsoWait )
{
	if( !HasSkeleton() )	return false;
	if( !COMPARE( id, 0, GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroup::ClearLoopAni - over flow(%d/%d)",
								id, GetAnimationCnt() );
		return false;
	}

	m_pAniController->ClearLoop( id , fFadeOut, bAlsoWait );
	return true;
}

void ioEntityGroup::ClearAniEvent()
{
	if( HasSkeleton() )
	{
		m_pAniController->ClearAniEvent();
	}
}

void ioEntityGroup::ClearAniEffect()
{
	if( HasSkeleton() )
	{
		m_pAniController->ClearAniEffect();
	}
}

void ioEntityGroup::ClearAllLoopAni( float fFadeOut, bool bWaitAlso )
{
	if( HasSkeleton() )
	{
		m_pAniController->ClearAllLoop( fFadeOut, bWaitAlso );
	}
}

bool ioEntityGroup::ClearAllLoopAniExceptOne( int iExceptID, float fFadeOut, bool bWaitAlso )
{
	if( HasSkeleton() )
	{
		return m_pAniController->ClearAllLoopExceptOne( iExceptID, fFadeOut, bWaitAlso );
	}

	return false;
}

ioAnimation* ioEntityGroup::GetLoopInPlay( const ioHashString &rkName )
{
	if( m_pAniController )
		return m_pAniController->FindLoopAniInPlay( rkName );

	return NULL;
}

ioAnimation* ioEntityGroup::GetLoopInPlay( int iIdx )
{
	if( m_pAniController )
		return m_pAniController->FindLoopAniInPlay( iIdx );

	return NULL;
}

ioHashString ioEntityGroup::GetAnimationName( int id ) const
{
	if( COMPARE( id, 0, GetAnimationCnt() ) )
		return m_RcAnimationList[ id ]->GetName();

	return ioHashString();
}

int ioEntityGroup::GetAnimationIdx( const ioHashString &kName )
{
	if( kName.IsEmpty() )
		return -1;

	int iIdx = 0;
	ioHashString cName = ConvertCustomAniName( kName );
	RcAnimationList::iterator iter=m_RcAnimationList.begin();
	for( ; iter!=m_RcAnimationList.end() ; ++iter, ++iIdx )
	{
		if( (*iter)->GetName() == cName )
			return iIdx;
	}

	if( AddAnimation( cName ) )
		return ( m_RcAnimationList.size() - 1 );

	LOG.PrintTimeAndLog( 0, "ioEntityGroup::GetAnimationIdx - Not Find : %s", kName.c_str() );
	return -1;
}

int ioEntityGroup::GetCurPlayingAniCnt() const
{
	if( HasSkeleton() )
	{
		return m_pAniController->GetPlayingActionCnt() + m_pAniController->GetPlayingLoopCnt();
	}

	return 0;
}

float ioEntityGroup::GetAnimationFullTime( int id )
{
	if( COMPARE( id, 0, GetAnimationCnt() ) )
		return m_RcAnimationList[id]->GetFullTime();

	return 0.0f;
}

float ioEntityGroup::GetAnimationEventTime( int id, const ioHashString &szEventType )
{
	if( COMPARE( id, 0, GetAnimationCnt() ) )
		return m_RcAnimationList[id]->GetEventTime( szEventType );

	return 0.0f;
}

int ioEntityGroup::GetAnimationEventCnt( int id, const ioHashString &szEventType )
{
	if( COMPARE( id, 0, GetAnimationCnt() ) )
		return m_RcAnimationList[id]->GetEventCnt( szEventType );

	return 0;
}

int ioEntityGroup::GetAnimationEventTimeList( int id,
											  const ioHashString &szEventType,
											  std::list<float> &vTimeList )
{
	if( COMPARE( id, 0, GetAnimationCnt() ) )
		return m_RcAnimationList[id]->GetEventTimeList( szEventType, vTimeList );

	return 0;
}

int ioEntityGroup::GetAnimationEventConstPtrList( int id,
												  const ioHashString &szEventType,
												  AniEventConstPtrList &vEventList )
{
	if( COMPARE( id, 0, GetAnimationCnt() ) )
		return m_RcAnimationList[id]->GetEventConstPtrList( szEventType, vEventList );

	return 0;
}

bool ioEntityGroup::IsActionAniPlaying( int id )
{
	if( HasSkeleton() )
	{
		ioAniController::ioAniList vAniList;
		m_pAniController->FindActionAniInPlay( id, vAniList );
		return !vAniList.empty();
	}

	return false;
}

bool ioEntityGroup::IsLoopAniPlaying( int id )
{
	if( HasSkeleton() )
	{
		if( m_pAniController->FindLoopAniInPlay(id) )
			return true;
	}

	return false;
}

bool ioEntityGroup::SetCollisionShape( const ioHashString &kFileName )
{
	if( m_pOpcodeShape )
	{
		g_OpcodeMgr.Destroy( m_pOpcodeShape );
		m_pOpcodeShape = NULL;
	}

	m_pOpcodeShape = g_OpcodeMgr.CreateShape( kFileName );
	if( !m_pOpcodeShape )
		return false;

	return true;
}

void ioEntityGroup::SetExtraRotate( const ioHashString &kBipedName,
								    const D3DXQUATERNION &qtRot,
									bool bLocal )
{
	if( m_pSkeleton )
		m_pSkeleton->SetExtraRotate( kBipedName, qtRot, bLocal );
}

void ioEntityGroup::ReleaseExtraRotate( const ioHashString &kBipedName )
{
	if( m_pSkeleton )
		m_pSkeleton->ReleaseExtraRotate( kBipedName );
}

void ioEntityGroup::ReleaseAllExtraRotate()
{
	if( m_pSkeleton )
		m_pSkeleton->ReleaseAllExtraRotate();
}

ioBiped* ioEntityGroup::GetBiped( const ioHashString &kName )
{
	if( m_pSkeleton )
		return m_pSkeleton->GetBiped( kName );

	return NULL;
}

ioBiped* ioEntityGroup::GetBiped( int iIndex )
{
	if( m_pSkeleton )
		return m_pSkeleton->GetBiped( iIndex );

	return NULL;
}

ioEffect* ioEntityGroup::AttachEffect( ioEffect *pEffect, ioEntityEvent *pEntityEventOwner,
									   const D3DXQUATERNION &qtRot,
									   const D3DXVECTOR3 &vPos,
									   bool bDependence )
{
	ioSceneNode *pChild = GetParentSceneNode()->CreateChildSceneNode();
	pChild->SetPosition( vPos );
	pChild->SetOrientation( qtRot );
	pChild->AttachObject( pEffect );

	if( bDependence )
		pEffect->SetVisible( m_bVisible );
	else
		pEffect->SetVisible( true );

	pEffect->SetWorldEffectEventOwner( pEntityEventOwner );
	pEffect->SetAttachedEntityGrp( this );
	pEffect->Update( 0.1f );

	m_AttachedEffectList.push_back( pEffect );

	return pEffect;
}

ioEffect* ioEntityGroup::AttachEffect( const ioHashString &rkFileName, ioEntityEvent *pEntityEventOwner,
									   const D3DXQUATERNION &qtRot,
									   const D3DXVECTOR3 &vPos,
									   bool bDependence )
{
	ioEffect *pEff = g_EffectFactory.CreateEffect( rkFileName );
	if( pEff )
	{
		AttachEffect( pEff, pEntityEventOwner, qtRot, vPos, bDependence );
	}

	return pEff;
}

void ioEntityGroup::EndEffect( const ioHashString &rkFileName, bool bGrace, bool bAllEnd )
{
	AttachedEffectList::iterator iter=m_AttachedEffectList.begin();
	for( ; iter!=m_AttachedEffectList.end() ; ++iter )
	{
		ioEffect *pEffect = *iter;
		if( pEffect->GetFileName() == rkFileName )
		{
			if( bGrace )
			{
				pEffect->EndEffectGrace();
			}
			else
			{
				pEffect->EndEffectForce();
			}

			if( !bAllEnd )
				break;
		}
	}
}

void ioEntityGroup::EndEffect( int iIndex, bool bGrace )
{
	if( !COMPARE( iIndex, 0, GetAttachedEffectCnt() ) )
		return;

	AttachedEffectList::iterator iter = m_AttachedEffectList.begin();
	std::advance( iter, iIndex );

	if( bGrace )
	{
		(*iter)->EndEffectGrace();
	}
	else
	{
		(*iter)->EndEffectForce();
	}
}

void ioEntityGroup::EndEffect( UniqueObjID eObjID, bool bGrace )
{
	AttachedEffectList::iterator iter;
	for( iter=m_AttachedEffectList.begin() ; iter!=m_AttachedEffectList.end() ; ++iter )
	{
		if( (*iter)->GetUniqueID() == eObjID )
		{
			if( bGrace )
			{
				(*iter)->EndEffectGrace();
			}
			else
			{
				(*iter)->EndEffectForce();
			}

			return;
		}
	}
}

ioEffect* ioEntityGroup::GetAttachedEffect( int iIndex )
{
	if( !COMPARE( iIndex, 0, GetAttachedEffectCnt() ) )
		return NULL;

	AttachedEffectList::iterator iter = m_AttachedEffectList.begin();
	std::advance( iter, iIndex );

	return *iter;
}

ioEffect* ioEntityGroup::GetAttachedEffectByObjID( UniqueObjID eObjID )
{
	AttachedEffectList::iterator iter;
	for( iter=m_AttachedEffectList.begin() ; iter!=m_AttachedEffectList.end() ; ++iter )
	{
		if( (*iter)->GetUniqueID() == eObjID )
			return *iter;
	}

	return NULL;
}

ioEffect* ioEntityGroup::GetAttachedEffectByFileName( const ioHashString &rkFileName )
{
	ioEffect *pEffect = NULL;

	AttachedEffectList::iterator iter;
	for( iter=m_AttachedEffectList.begin() ; iter!=m_AttachedEffectList.end() ; ++iter )
	{
		pEffect = *iter;

		if( pEffect->GetFileName() == rkFileName )
			return pEffect;
	}

	return NULL;
}

int ioEntityGroup::GetAttachedEffectCnt() const
{
	return m_AttachedEffectList.size();
}

void ioEntityGroup::EnableDestroyDeadEffect( bool bDestroy )
{
	m_bDestroyDeadEffect = bDestroy;
}

void ioEntityGroup::DetachEffect( ioEffect *pEff )
{
	if( pEff )
	{
		DestroyEffectSceneNode( pEff );
		m_AttachedEffectList.remove( pEff );

		pEff->SetAttachedEntityGrp( NULL );
	}
}

void ioEntityGroup::DestroyDeadEffect()
{
	if( !m_bDestroyDeadEffect )	return;

	AttachedEffectList::iterator iter = m_AttachedEffectList.begin();
	while( iter != m_AttachedEffectList.end() )
	{
		ioEffect *pEffect = *iter;
		if( pEffect->IsLive() )
		{
			++iter;
		}
		else
		{
			iter = m_AttachedEffectList.erase( iter );

			DestroyEffectSceneNode( pEffect );
			g_EffectFactory.DestroyEffect( pEffect );
		}
	}
}

void ioEntityGroup::DestroyEffectSceneNode( ioEffect *pEffect )
{
	ioSceneNode *pEffNode = pEffect->GetParentSceneNode();
	if( pEffNode != GetParentSceneNode() )
	{
		ioSceneNode *pEntityNode = GetParentSceneNode();
		if( pEntityNode )
		{
			pEntityNode->RemoveAndDestroyChild( pEffNode->GetUniqueID() );
		}
	}
}

void ioEntityGroup::DestroyAllEffect()
{
	AttachedEffectList::iterator iter=m_AttachedEffectList.begin();
	for( ; iter!=m_AttachedEffectList.end() ; ++iter )
	{
		ioEffect *pEffect = *iter;
		if( pEffect )
		{
			DestroyEffectSceneNode( pEffect );
			g_EffectFactory.DestroyEffect( pEffect );
		}
	}

	m_AttachedEffectList.clear();
}

void ioEntityGroup::SetSoftwareVertexBlending( bool bSoftware )
{
	if( m_bSoftwareVertexBlend == bSoftware )
		return;

	m_bSoftwareVertexBlend = bSoftware;

	ReBuildAnimateEntityMeshData();
}

void ioEntityGroup::ReBuildAnimateEntityMeshData()
{
	if( HasSkeleton() )
	{
		EntityList::iterator iter;
		for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
		{
			(*iter)->BuildMeshData( m_bSoftwareVertexBlend );
		}
	}
}

void ioEntityGroup::GetEntityTransform( D3DXMATRIX *pMatWorld ) const
{
	*pMatWorld = *GetTransform();
}

float ioEntityGroup::GetSquaredViewDepth( const ioCamera *pCamera )
{
	if( IsAttached() )
		return GetParentNode()->GetSquaredViewDepth( pCamera );

	return 0.0f;
}

bool ioEntityGroup::IsShadowRecvRender() const
{
	return ioMovableObject::IsShadowRecvEnable();
}

void ioEntityGroup::Update( float fTime )
{
	if( m_pAniController )
	{
		m_pAniController->Update( fTime );
	}

	CheckNotBuildEntity();
}

void ioEntityGroup::UpdateAttachedObjects( float fTime )
{
	if( fTime > 0.0f )
	{
		AttachedEffectList::iterator iEffect = m_AttachedEffectList.begin();
		for( ; iEffect != m_AttachedEffectList.end() ; ++iEffect )
		{
			(*iEffect)->Update( fTime );
		}

		MeshTrailerList::iterator iTrail = m_MeshTrailerList.begin();
		while( iTrail != m_MeshTrailerList.end() )
		{
			ioMeshTrailer *pTrailer = *iTrail;
			
			if( pTrailer->UpdateCtrlPoints( fTime ) )
			{
				++iTrail;
			}
			else
			{
				iTrail = m_MeshTrailerList.erase( iTrail );
				delete pTrailer;
			}
		}
	}

	DestroyDeadEffect();
}

void ioEntityGroup::CheckEachEntityCulled( ioCamera *pCamera )
{
	if( !m_bEnableEachEntityCull )
		return;

	ioAxisAlignBox kWorldBound;

	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		ioEntity *pEntity = *iter;

		kWorldBound = pEntity->GetBoundBox();
		kWorldBound.Transform( GetTransform() );

		if( pCamera->IsVisible( kWorldBound ) )
			pEntity->SetCulled( false );
		else
			pEntity->SetCulled( true );
	}
}

void ioEntityGroup::CheckNotBuildEntity()
{
	if( m_NotBuildEntityList.empty() )
		return;

	if( HasSkeleton() )
	{
		NotBuiledList::iterator iter = m_NotBuildEntityList.begin();
		while( iter != m_NotBuildEntityList.end() )
		{
			ioEntity *pEntity = *iter;
			if( pEntity->BuildEntity( m_pSkeleton, m_bSoftwareVertexBlend ) )
			{
				iter = m_NotBuildEntityList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		NotBuiledList::iterator iter = m_NotBuildEntityList.begin();
		while( iter != m_NotBuildEntityList.end() )
		{
			ioEntity *pEntity = *iter;
			if( pEntity->BuildEntity( NULL, false ) )
			{
				iter = m_NotBuildEntityList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	UpdateBounds( true );
}

void ioEntityGroup::EraseNotBuildEntity( ioEntity *pEntity )
{
	NotBuiledList::iterator iter = m_NotBuildEntityList.begin();
	for( ; iter!=m_NotBuildEntityList.end() ; ++iter )
	{
		if( (*iter) == pEntity )
		{
			m_NotBuildEntityList.erase( iter );
			break;
		}
	}
}

void ioEntityGroup::NotifyCurrentCamera( ioCamera *pCamera )
{
	CheckEachEntityCulled( pCamera );

	DecalList::iterator iDecal;
	for( iDecal = m_DecalList.begin() ; iDecal!=m_DecalList.end() ; ++iDecal )
	{
		iDecal->second->NotifyCurrentCamera( pCamera );
	}

	MeshTrailerList::iterator iTrail;
	for(iTrail=m_MeshTrailerList.begin() ; iTrail!=m_MeshTrailerList.end() ; ++iTrail )
	{
		(*iTrail)->NotifyCurrentCamera( pCamera );
	}

	if( m_pEdgeRender )
	{
		if( m_pEdgeRender->GetLoadState() == RLS_FAILED )
		{
			RemoveEdgeRender();
		}
		else
		{
			if( m_pEdgeRender->IsEnable() )
			{
				m_pEdgeRender->NotifyCurrentCamera( pCamera );
			}
		}
	}

	UpdateApplyLightList();
}

void ioEntityGroup::UpdateRenderQueue( ioRenderQueue &queue )
{
	CacheBipedMatrices();

	EntityList::iterator iter=m_EntityList.begin();
	for( ; iter!=m_EntityList.end() ; ++iter )
	{
		ioEntity *pEntity = *iter;
		if( pEntity->IsVisible() && !pEntity->IsCulled() )
		{
			pEntity->UpdateRenderQueue( queue, m_RenderQueueID );
		}
	}

	DecalList::iterator iDecal = m_DecalList.begin();
	for( ; iDecal!=m_DecalList.end() ; ++iDecal )
	{
		ioDecal *pDecal = iDecal->second;
		if( pDecal->IsVisible() )
		{
			queue.AddRenderable( pDecal, m_RenderQueueID );
		}
	}

	MeshTrailerList::iterator iTrail=m_MeshTrailerList.begin();
	for( ; iTrail!=m_MeshTrailerList.end() ; ++iTrail )
	{
		ioMeshTrailer *pTrailer = *iTrail;
		if( pTrailer->IsUpdateBufferOK() )
		{
			queue.AddRenderable( pTrailer, m_RenderQueueID );
		}
	}

	if( m_pEdgeRender && m_pEdgeRender->IsLoaded() )
	{
		if( m_pEdgeRender->IsEnable() && m_pEdgeRender->IsUpdateBufferOK() )
		{
			queue.AddRenderable( m_pEdgeRender, m_RenderQueueID, RENDERABLE_DEFAULT_PRIORITY + 1 );
		}
	}	
}

void ioEntityGroup::UpdateGenShadowMapToQueue( ioRenderQueue &queue )
{
	CacheBipedMatrices();

	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		ioEntity *pEntity = *iter;
		if( pEntity->IsVisible() && !pEntity->IsCulled() )
		{
			pEntity->UpdateGenShadowMapToQueue( queue, m_RenderQueueID );
		}
	}
}

void ioEntityGroup::UpdateBounds( bool bForceUpdate )
{
	if( !bForceUpdate )	return;

	m_BoundBox.SetNull();

	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		m_BoundBox.Merge( (*iter)->GetBoundBox() );
	}

	if( IsAttached() )
	{
		GetParentNode()->NeedUpdate();
	}
}

const ioAxisAlignBox& ioEntityGroup::GetBoundBox() const
{
	return m_BoundBox;
}

float ioEntityGroup::GetBoundRadius() const
{
	return m_BoundBox.GetHalfDiagonal();
}

void ioEntityGroup::SetDiffuseRate( const D3DCOLORVALUE &rkColor )
{
	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		(*iter)->SetDiffuseRate( rkColor );
	}
}

void ioEntityGroup::SetFileName( const ioHashString &rkName )
{
	m_FileName = rkName;
}

void ioEntityGroup::SetMeshColor( float fRed, float fGreen, float fBlue )
{
	m_MeshColor.r = fRed;
	m_MeshColor.g = fGreen;
	m_MeshColor.b = fBlue;

	ioShaderCustomArg kArg( "MeshColor" );
	kArg.SetValue( m_MeshColor );

	EntityList::iterator iter;
	for( iter=m_EntityList.begin() ; iter!=m_EntityList.end() ; ++iter )
	{
		(*iter)->SetCustomParameter( kArg );
	}
}

void ioEntityGroup::SetEachEntityCull( bool bEachCull )
{
	m_bEnableEachEntityCull = bEachCull;
}

void ioEntityGroup::GuaranteeAtLeastOneAnimation( bool bEnable )
{
	ioAniController::m_bGuaranteeAtLeastOneAnimation = bEnable;
}

void ioEntityGroup::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	AttachedEffectList::iterator iter = m_AttachedEffectList.begin();
	while( iter != m_AttachedEffectList.end() )
	{
		ioEffect *pEffect = *iter;
		if( pEffect && pEffect->IsLive() )
		{
			pEffect->SetVisible( bVisible );
		}

		++iter;
	}
}

void ioEntityGroup::SetVisibleNow( bool bVisible, bool bVisibleChangePass )
{
	EntityList::iterator iEntity;
	for( iEntity=m_EntityList.begin() ; iEntity!=m_EntityList.end() ; ++iEntity )
	{
		ioEntity *pEntity = *iEntity;
		if( bVisibleChangePass && pEntity->IsVisibleChangePass() )
			continue;

		pEntity->SetVisible( bVisible );
	}

	AttachedEffectList::iterator iter = m_AttachedEffectList.begin();
	while( iter != m_AttachedEffectList.end() )
	{
		ioEffect *pEffect = *iter;
		if( pEffect && pEffect->IsLive() )
		{
			pEffect->SetVisible( bVisible );
		}

		++iter;
	}
}

void ioEntityGroup::SetVisibleAllMesh( bool bVisible )
{
	EntityList::iterator iEntity;
	for( iEntity=m_EntityList.begin() ; iEntity!=m_EntityList.end() ; ++iEntity )
	{
		ioEntity *pEntity = *iEntity;
		pEntity->SetVisible( bVisible );
	}
}

