

#include "stdafx.h"

#include "ErrorReport.h"

#include "ioRenderable.h"
#include "ioStringConverter.h"
#include "ioLightSystem.h"

#include "ioNode.h"
#include "ioSceneNode.h"
#include "ioSceneManager.h"
#include "ioCamera.h"

#include "ioBiped.h"
#include "ioSkeleton.h"
#include "ioAnimation.h"
#include "HelpFunc.h"

#include "ioEffect.h"
#include "ioMath.h"
#include "ioEffectBufferManager.h"
#include "ioEntityGroup.h"

#include "ioAnimationManager.h"
#include "ioSkeletonManager.h"

#include "ioFrameTimer.h"

ioEffect::CmdCollisionBox		ioEffect::m_CollisionBoxCmd;
ioEffect::CmdSkeletonName		ioEffect::m_SkeletonCmd;
ioEffect::CmdAnimationName		ioEffect::m_AnimationCmd;
ioEffect::CmdEffectDirection	ioEffect::m_EffectDirectionCmd;
ioEffect::CmdCullEachSystem		ioEffect::m_CullEachSystemCmd;
ioEffect::CmdUpdateSystemAlways ioEffect::m_UpdateSystemAlwaysCmd;

bool ioEffect::m_sbEnableEffectLightCreation = true;

using namespace std;

ioEffect::ioEffect()
{
	m_vEffectDirection = ioMath::UNIT_Y;

	m_pAttachedGrp = NULL;
	m_pSkeleton = NULL;
	m_pAnimation = NULL;
	m_pEntityEventOwner = NULL;
	m_bAnimationLoop  = true;
	m_bCullEachSystem = false;
	m_bAlwaysUpdateSystem = false;

	m_fEffectStopWaitTime  = 0.0f;
	m_fEffectStopDuration  = 0.0f;
	m_fEffectStopLooseRate = 0.0f;

	m_fSquaredViewDepth = 0.0f;

	m_pLightSys = NULL;
	m_dwLightCheckTime = 0;

	m_fWaitTime	 = 0.0f;
	m_fRemainWaitTime = 0.0f;

	m_bLive	= true;
	m_bTryedLightCreation = false;

	InitParameters();
}

void ioEffect::InitParameters()
{
	if( CreateParamDictionary( "Effect" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef( "collision_box", PT_VECTOR_LIST ),
							 &m_CollisionBoxCmd );

		pDict->AddParameter( ioParameterDef( "skeleton_name", PT_STRING ),
							 &m_SkeletonCmd );

		pDict->AddParameter( ioParameterDef( "animation_name", PT_STRING ),
							 &m_AnimationCmd );

		pDict->AddParameter( ioParameterDef( "direction", PT_VECTOR3 ),
							 &m_EffectDirectionCmd );

		pDict->AddParameter( ioParameterDef( "cull_each_system", PT_BOOL ),
							 &m_CullEachSystemCmd );

		pDict->AddParameter( ioParameterDef( "update_always", PT_BOOL ),
							 &m_UpdateSystemAlwaysCmd );
	}
}

ioEffect::~ioEffect()
{
	SAFEDELETE( m_pAnimation );
	SAFEDELETE( m_pSkeleton );

	DestroyAllSystem();
}

void ioEffect::DestroyAllSystem()
{
	DestroyLightSystem();

	ParticleSystemList::iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		delete *iParticle;
	}
	m_ParticleGrp.clear();

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		delete *iModel;
	}
	m_ModelGrp.clear();
}

void ioEffect::ChangeParticleSysIndex( int iSrc, int iDest )
{
	if( iSrc == iDest )
		return;

	int iSysCnt = GetParticleSystemCnt();

	if( !COMPARE( iSrc, 0, iSysCnt ) || !COMPARE( iDest, 0, iSysCnt ) )
		return;

	ioParticleSystem *pTempSys = m_ParticleGrp[iSrc];
	m_ParticleGrp[iSrc] = m_ParticleGrp[iDest];
	m_ParticleGrp[iDest] = pTempSys;	
}

void ioEffect::ChangeModelSysIndex( int iSrc, int iDest )
{
	if( iSrc == iDest )
		return;

	int iSysCnt = GetModelSystemCnt();
	if( !COMPARE( iSrc, 0, iSysCnt ) || !COMPARE( iDest, 0, iSysCnt ) )
		return;

	ioModelParticleSystem *pTempSys = m_ModelGrp[iSrc];
	m_ModelGrp[iSrc] = m_ModelGrp[iDest];
	m_ModelGrp[iDest] = pTempSys;
}

int ioEffect::GetNumSimulatedMaxFaces()
{
	int iMaxFaces = 0;

	ParticleSystemList::iterator iParticle = m_ParticleGrp.begin();
	for( ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		iMaxFaces += (*iParticle)->GetNumSimulatedMaxParticles() * 2;
	}

	ModelSystemList::iterator iModel = m_ModelGrp.begin();
	for( ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		iMaxFaces += (*iModel)->GetNumSimulatedMaxFaceCount();
	}

	return iMaxFaces;
}

void ioEffect::InitResource()
{
	std::for_each( m_ParticleGrp.begin(),
				   m_ParticleGrp.end(),
				   std::mem_fun(&ioParticleSystem::BuildMaterial) );

	InitializeRandomSeed();
}

void ioEffect::Restart()
{
	for_each( m_ParticleGrp.begin(),
			  m_ParticleGrp.end(),
			  mem_fun( &ioParticleSystem::Restart ) );

	for_each( m_ModelGrp.begin(),
			  m_ModelGrp.end(),
			  mem_fun( &ioModelParticleSystem::Restart ) );

	if( m_pSkeleton && m_pAnimation )
	{
		SetAnimation( m_pAnimation->GetName(), m_bAnimationLoop );
	}

	m_fEffectStopWaitTime  = 0.0f;
	m_fEffectStopDuration  = 0.0f;
	m_fEffectStopLooseRate = 0.0f;

	if( m_pLightSys )
	{
		DestroyCreatedLight();
		m_dwLightCheckTime = 0;
	}

	m_fRemainWaitTime = m_fWaitTime;
	m_bLive = true;

	InitializeRandomSeed();
}

void ioEffect::InitializeRandomSeed()
{
	ParticleSystemList::iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		(*iParticle)->SetRandomSeed( rand() );
	}

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		(*iModel)->SetRandomSeed( rand() );
	}

	int iSysCnt = GetParticleSystemCnt();
	for( int i=0 ; i<iSysCnt ; i++ )
	{
		ioParticleSystem *pSys = GetParticleSys(i);

		int iSyncIndex = pSys->GetRandomSyncIndex();
		if( iSyncIndex != -1 )
		{
			ioParticleSystem *pOrgSys = GetParticleSys( iSyncIndex );
			if( pOrgSys )
			{
				pSys->SetRandomSeed( pOrgSys->GetRandomSeed() );
			}
		}
	}

	iSysCnt = GetModelSystemCnt();
	for( i=0 ; i<iSysCnt ; i++ )
	{
		ioModelParticleSystem *pSys = GetModelSys( i );

		int iSyncIndex = pSys->GetRandomSyncIndex();
		if( iSyncIndex != -1 )
		{
			ioModelParticleSystem *pOrgSys = GetModelSys( iSyncIndex );
			if( pOrgSys )
			{
				pSys->SetRandomSeed( pOrgSys->GetRandomSeed() );
			}
		}
	}
}

void ioEffect::AddParticleSys( ioParticleSystem *pSys )
{
	if( pSys )
	{
		m_ParticleGrp.push_back( pSys );
		pSys->SetParent( this );
	}
}

void ioEffect::DestroyParticleSys( int iIdx )
{
	if( COMPARE( iIdx, 0, GetParticleSystemCnt() ) )
	{
		delete m_ParticleGrp[iIdx];
		m_ParticleGrp.erase( m_ParticleGrp.begin() + iIdx );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioEffect::DestroyParticleSys - over (%d/%d)",
								 iIdx, GetParticleSystemCnt() );
	}
}

ioParticleSystem* ioEffect::GetParticleSys( int iIdx )
{
	if( COMPARE( iIdx, 0, GetParticleSystemCnt() ) )
	{
		return m_ParticleGrp[iIdx];
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioEffect::GetParticleSys - over (%d/%d)",
								 iIdx, GetParticleSystemCnt() );
	}

	return NULL;
}

void ioEffect::AddModelSystem( ioModelParticleSystem *pSys )
{
	if( pSys )
	{
		m_ModelGrp.push_back( pSys );
		pSys->SetParent( this );
	}
}

void ioEffect::DestroyModelSystem( int iIdx )
{
	if( COMPARE( iIdx, 0, GetModelSystemCnt() ) )
	{
		delete m_ModelGrp[iIdx];
		m_ModelGrp.erase( m_ModelGrp.begin() + iIdx );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioEffect::DestroyModelSystem - over (%d/%d)",
								 iIdx, GetModelSystemCnt() );
	}
}

ioModelParticleSystem* ioEffect::GetModelSys( int iIdx )
{
	if( COMPARE( iIdx, 0, GetModelSystemCnt() ) )
	{
		return m_ModelGrp[iIdx];
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioEffect::GetModelSys - over (%d/%d)",
								 iIdx, GetModelSystemCnt() );
	}

	return NULL;
}

void ioEffect::AddEntityEvent( ioEntity *pEntity )
{
	if( m_pEntityEventOwner )
		m_pEntityEventOwner->AddEntityEvent( pEntity );
	else if( m_pAttachedGrp )
		m_pAttachedGrp->AddEntityEvent( pEntity );
}

bool ioEffect::HasTimeLimit() const
{
	ParticleSystemList::const_iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		if( (*iParticle)->GetEndCondition() == ioParticleSystem::EC_NEVER_END )
			return false;
	}

	ModelSystemList::const_iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		if( (*iModel)->GetEndCondition() == ioModelParticleSystem::EC_NEVER_END )
			return false;
	}

	return true;
}

bool ioEffect::IsLiveAtNextFrame( float fCurTimeGap )
{
	ParticleSystemList::const_iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		if( (*iParticle)->IsEnableAtNextFrame( fCurTimeGap ) )
			return true;
	}

	ModelSystemList::const_iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		if( (*iModel)->IsEnableAtNextFrame( fCurTimeGap ) )
			return true;
	}

	return false;
}

void ioEffect::SetExtendColorRate( float fRate, bool bMinZero )
{
	ParticleSystemList::iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		ioParticleSystem *pPartSys = *iParticle;
		if( pPartSys && pPartSys->IsEnable() )
		{
			pPartSys->SetExtendColorRate( fRate, bMinZero );
		}
	}

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pModelSys = *iModel;
		if( pModelSys && pModelSys->IsEnable() )
		{
			pModelSys->SetExtendAlphaRate( fRate );
		}
	}
}

void ioEffect::Update( float fTime )
{
	if( m_fRemainWaitTime == 0.0f )
	{
		m_bLive = false;

		fTime = CheckEffectStop( fTime );
		if( fTime == 0.0f )
		{
			m_bLive = true;
			return;	// No Update..
		}

		UpdateSkeleton( fTime );

		ParticleSystemList::iterator iParticle;
		for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
		{
			ioParticleSystem *pPartSys = *iParticle;
			if( pPartSys->IsEnable() )
			{
				pPartSys->Update( fTime );
				m_bLive = true;
			}
		}

		ModelSystemList::iterator iModel;
		for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
		{
			ioModelParticleSystem *pModelSys = *iModel;
			if( pModelSys->IsEnable() )
			{
				pModelSys->Update( fTime );
				m_bLive = true;
			}
		}

		CheckCreateLightState( fTime );
	}
	else
	{
		m_fRemainWaitTime -= fTime;
		if( m_fRemainWaitTime <= 0.0f )
		{
			m_fRemainWaitTime = 0.0f;
		}
	}

	if( IsAttached() )
	{
		GetParentNode()->NeedUpdate();
	}
}

void ioEffect::UpdateSkeleton( float fTime )
{
	if( !m_pSkeleton || !m_pAnimation )
		return;

	if( m_pAnimation->GetState() != ioAnimation::AS_END )
	{
		m_pAnimation->Update( fTime );

		m_pSkeleton->ClearBlend();
		m_pAnimation->BlendSkeleton( m_pSkeleton );
		m_pSkeleton->LockBlend();
		m_pSkeleton->UpdateSkeleton( fTime );
	}
}

float ioEffect::CheckEffectStop( float fTime )
{
	if( m_fEffectStopWaitTime == 0.0f && m_fEffectStopDuration == 0.0f )
		return fTime;

	if( m_fEffectStopWaitTime > 0.0f )
	{
		if( m_fEffectStopWaitTime > fTime )
		{
			m_fEffectStopWaitTime -= fTime;
			return fTime;
		}
		else
		{
			fTime -= m_fEffectStopWaitTime;
			m_fEffectStopWaitTime = 0.0f;
		}
	}

	m_fEffectStopDuration -= fTime;
	if( m_fEffectStopDuration > 0.0f )
	{
		return fTime * m_fEffectStopLooseRate;
	}

	fTime = -m_fEffectStopDuration;
	m_fEffectStopDuration = 0.0f;
	return fTime;
}

void ioEffect::CheckCreateLightState( float fTime )
{
	if( !m_pLightSys || !IsAttached() )
		return;

	m_dwLightCheckTime += (DWORD)fTime;

	if( !m_bTryedLightCreation )
	{
		if( COMPARE( m_dwLightCheckTime, m_pLightSys->dwLight_s, m_pLightSys->dwLight_e ) )
		{
			m_bTryedLightCreation = true;

			if( m_sbEnableEffectLightCreation )
			{
				MakeLight();
			}
		}
	}
	else
	{
		if( m_dwLightCheckTime > m_pLightSys->dwLight_e )
		{
			DestroyCreatedLight();
		}
	}
}

void ioEffect::MakeLight()
{
	if( !m_pLightSys )	return;

	ioHashString szLightName = ioStringConverter::toString( (DWORD)m_pLightSys ).c_str();

	ioLight *pLight = GetParentSceneNode()->GetSceneMgr()->CreateLight( szLightName );
	if( !pLight )
	{
		m_pLightSys->szName.Clear();
		return;
	}

	m_pLightSys->szName = szLightName;

	pLight->SetLightSystem( m_pLightSys );
	pLight->SetLightTimeDuration( m_pLightSys->dwLight_e - m_dwLightCheckTime );

	if( m_pAttachedGrp && m_pAttachedGrp->HasSkeleton() )
	{
		ioHashString szBiped;
		D3DXVECTOR3 vPosOffset;
		D3DXQUATERNION qtRotOffset;

		if( GetFirstLinkOffset( szBiped, vPosOffset, qtRotOffset ) )
		{
			ioBiped *pBiped = m_pAttachedGrp->GetBiped( szBiped );
			if( pBiped )
			{
				D3DXQuaternionMultiply( &qtRotOffset, &qtRotOffset, &pBiped->GetDerivedOrientation() );
				vPosOffset = pBiped->GetDerivedOrientation() * vPosOffset + pBiped->GetDerivedPosition();

				D3DXVECTOR3 vNewDirection = qtRotOffset * pLight->GetDirection();
				pLight->SetDirection( vNewDirection );

				D3DXVECTOR3 vNewPosition = qtRotOffset * pLight->GetPosition() + vPosOffset;
				pLight->SetPosition( vNewPosition );
			}
		}
	}

	pLight->NotifyAttached( GetParentNode() );
}

void ioEffect::DestroyCreatedLight()
{
	if( !m_pLightSys || !m_bTryedLightCreation )
		return;

	ioSceneNode *pSceneNode = GetParentSceneNode();
	if( pSceneNode )
	{
		pSceneNode->GetSceneMgr()->DestroyLight( m_pLightSys->szName );
	}

	m_bTryedLightCreation = false;
}

void ioEffect::EnableEffectLightCreation( bool bEnable )
{
	m_sbEnableEffectLightCreation = bEnable;
}

ioLightSystem* ioEffect::CreateLightSystem()
{
	DestroyLightSystem();

	m_pLightSys = new ioLightSystem;
	return m_pLightSys;
}

void ioEffect::DestroyLightSystem()
{
	if( m_pLightSys )
	{
		DestroyCreatedLight();
		SAFEDELETE( m_pLightSys );
	}
}

void ioEffect::UpdateBounds( bool bForceUpdate )
{
	m_BoundBox.SetNull();

	D3DXMATRIX matInv;
	D3DXMatrixInverse( &matInv, NULL, GetTransform() );

	ParticleSystemList::iterator iParticle=m_ParticleGrp.begin();
	for( ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		ioParticleSystem *pPartSys = *iParticle;
		if( pPartSys->IsEnable() )
		{
			pPartSys->UpdateBounds( matInv );
			m_BoundBox.Merge( pPartSys->GetBoundBox() );
		}
	}

	ModelSystemList::iterator iModel=m_ModelGrp.begin();
	for( ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pModelSys = *iModel;
		if( pModelSys->IsEnable() )
		{
			pModelSys->UpdateBounds( matInv );
			m_BoundBox.Merge( pModelSys->GetBoundBox() );
		}
	}

	if( IsAttached() )
	{
		m_BoundBox.AddPoint( ioMath::VEC3_ZERO );
	}
}

void ioEffect::CheckCullEachSystem( ioCamera *pCamera )
{
	if( !m_bCullEachSystem || !IsAttached() )
		return;

	ioAxisAlignBox kBoundBox;
	D3DXMATRIX *pXform = GetTransform();

	ParticleSystemList::iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		ioParticleSystem *pPartSys = *iParticle;

		kBoundBox = pPartSys->GetBoundBox();
		kBoundBox.Transform( pXform );

		if( pCamera->IsVisible( kBoundBox ) )
			pPartSys->SetEachCulled( false );
		else
			pPartSys->SetEachCulled( true );
	}

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pModelSys = *iModel;

		kBoundBox = pModelSys->GetBoundBox();
		kBoundBox.Transform( pXform );

		if( pCamera->IsVisible( kBoundBox ) )
			pModelSys->SetEachCulled( false );
		else
			pModelSys->SetEachCulled( true );
	}
}

void ioEffect::NotifyCurrentCamera( ioCamera *pCamera )
{
	CheckCullEachSystem( pCamera );

	m_fSquaredViewDepth = 0.0f;

	if( IsAttached() )
	{
		m_fSquaredViewDepth = GetParentNode()->GetSquaredViewDepth( pCamera );
	}

	ParticleSystemList::iterator iParticle=m_ParticleGrp.begin();
	for( ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		ioParticleSystem *pPartSys = *iParticle;
		if( pPartSys && pPartSys->IsEnable() )
		{
			pPartSys->NotifyCurrentCamera( pCamera );
		}
	}

	ModelSystemList::iterator iModel=m_ModelGrp.begin();
	for( ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pModelSys = *iModel;
		if( pModelSys && pModelSys->IsEnable() )
		{
			pModelSys->NotifyCurrentCamera( pCamera );
		}
	}
}

void ioEffect::UpdateRenderQueue( ioRenderQueue &queue )
{
	g_EffectBuffer.NotifyParticleSystemList( m_ParticleGrp, m_fSquaredViewDepth );

	if( !IsVisible() )
		return;

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pModelSys = *iModel;
		if( pModelSys->IsEnable() && !pModelSys->IsEachCulled() )
		{
			pModelSys->UpdateRenderQueue( queue );
		}
	}
}

void ioEffect::NotifyAttached( ioNode *pParent )
{
	if( IsAttached() && pParent == NULL )	// Detaching...
	{
		DestroyCreatedLight();
	}

	ioMovableObject::NotifyAttached( pParent );
}

void ioEffect::SetWaitTime( float fWaitTime ) 
{
	m_fWaitTime = fWaitTime;
	m_fRemainWaitTime = fWaitTime;
}

void ioEffect::ReserveEffectStop( float fStopWaitTime,
								  float fStopDuration,
								  float fLooseRate )
{
	m_fEffectStopWaitTime  = fStopWaitTime;
	m_fEffectStopDuration  = fStopDuration;
	m_fEffectStopLooseRate = fLooseRate;
}

void ioEffect::EndEffectForce()
{
	ParticleSystemList::iterator iPart = m_ParticleGrp.begin();
	for( ; iPart!=m_ParticleGrp.end() ; ++iPart )
	{
		if( (*iPart)->GetState() != ioParticleSystem::PSS_END )
		{
			(*iPart)->SetState( ioParticleSystem::PSS_END );
		}
	}

	ModelSystemList::iterator iModel = m_ModelGrp.begin();
	for( ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		if( (*iModel)->GetState() != ioModelParticleSystem::MPSS_END )
		{
			(*iModel)->SetState( ioModelParticleSystem::MPSS_END );
		}
	}
}

void ioEffect::EndEffectGrace()
{
	ParticleSystemList::iterator iPart = m_ParticleGrp.begin();
	for( ; iPart!=m_ParticleGrp.end() ; ++iPart )
	{
		if( (*iPart)->GetState() == ioParticleSystem::PSS_LIVE )
		{
			(*iPart)->SetState( ioParticleSystem::PSS_GRACE_CLOSE );
		}
	}

	ModelSystemList::iterator iModel = m_ModelGrp.begin();
	for( ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		if( (*iModel)->GetState() == ioModelParticleSystem::MPSS_LIVE )
		{
			(*iModel)->SetState( ioModelParticleSystem::MPSS_GRACE_CLOSE );
		}
	}
}

void ioEffect::ScaleActualSize( float fScaleRate )
{
	fScaleRate = max( fScaleRate, 0.1f );

	for_each( m_ParticleGrp.begin(),
			  m_ParticleGrp.end(),
			  bind2nd( mem_fun( &ioParticleSystem::ScaleActualSize ), fScaleRate ) );

	for_each( m_ModelGrp.begin(),
			  m_ModelGrp.end(),
			  bind2nd( mem_fun( &ioModelParticleSystem::ScaleActualSize ), fScaleRate ) );
}

void ioEffect::ChangeEffectDirection( const D3DXVECTOR3 &vDirection, bool bTypeChange )
{
	D3DXQUATERNION qtRotate = ioMath::QuaternionFromDirections( m_vEffectDirection,
															    vDirection );

	m_vEffectDirection = vDirection;

	ParticleSystemList::iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		(*iParticle)->ChangeSystemDirection( qtRotate, bTypeChange );
	}

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		(*iModel)->ChangeSystemDirection( qtRotate );
	}
}

const ioAxisAlignBox& ioEffect::GetBoundBox() const
{
	return m_BoundBox;
}

float ioEffect::GetBoundRadius() const
{
	return m_BoundBox.GetHalfDiagonal();
}

void ioEffect::SetFileName( const ioHashString &kFileName )
{
	m_FileName = kFileName;
}

void ioEffect::SetCollisionBox( const ioOrientBox &rkBox )
{
	m_CollisionBox = rkBox;
}

void ioEffect::SetEffectDirection( const D3DXVECTOR3 &vDirection )
{
	m_vEffectDirection = vDirection;
}

void ioEffect::SetCullEachSystem( bool bCullEach )
{
	m_bCullEachSystem = bCullEach;
}

void ioEffect::SetAlwaysUpdateSystem( bool bAlways )
{
	m_bAlwaysUpdateSystem = bAlways;
}

void ioEffect::SetSkeleton( const ioHashString &rkName )
{
	SAFEDELETE( m_pSkeleton );

	m_pSkeleton = ioSkeletonManager::GetSingleton().CreateInstance( rkName );
}

void ioEffect::SetAnimation( const ioHashString &rkName, bool bLoop )
{
	if( !m_pSkeleton )
	{
		LOG.PrintTimeAndLog( 0, "ioEffect::SetAnimation - %s SetSkeleton First", rkName.c_str() );
		return;
	}

	ClearAnimation();

	m_pAnimation = g_AnimationMgr.CreateInstance( rkName );
	if( m_pAnimation )
	{
		m_pAnimation->BuildAniTrackList( m_pSkeleton );
		
		if( bLoop )
			m_pAnimation->StartLoop( 1.0f, 0.0f, 1.0f, 0.0f );
		else
			m_pAnimation->ExecuteAni( 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, false );

		m_bAnimationLoop = bLoop;
	}
}

void ioEffect::ClearSkeletonAndAnimation()
{
	SAFEDELETE( m_pAnimation );
	SAFEDELETE( m_pSkeleton );
}

void ioEffect::ClearAnimation()
{
	SAFEDELETE( m_pAnimation );
}

ioSkeleton* ioEffect::GetSkeleton() const
{
	if( m_pSkeleton )
		return m_pSkeleton;

	if( m_pAttachedGrp )
		return m_pAttachedGrp->GetSkeleton();

	return NULL;
}

ioHashString ioEffect::GetSkeletonName() const
{
	if( m_pSkeleton )
		return m_pSkeleton->GetName();

	return ioHashString();
}

ioHashString ioEffect::GetAnimationName() const
{
	if( m_pAnimation )
		return m_pAnimation->GetName();

	return ioHashString();
}

bool ioEffect::HasLinkedSystem() const
{
	ParticleSystemList::const_iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		if( !(*iParticle)->GetLinkBipedName().IsEmpty() )
			return true;
	}

	ModelSystemList::const_iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		if( !(*iModel)->GetLinkBipedName().IsEmpty() )
			return true;
	}

	return false;
}

bool ioEffect::GetFirstLinkOffset( ioHashString &szBiped,
								   D3DXVECTOR3 &vPosOffset,
								   D3DXQUATERNION &qtRotOffset )
{
	ParticleSystemList::iterator iParticle;
	for( iParticle=m_ParticleGrp.begin() ; iParticle!=m_ParticleGrp.end() ; ++iParticle )
	{
		ioParticleSystem *pPartSys = *iParticle;

		if( !pPartSys->GetLinkBipedName().IsEmpty() )
		{
			szBiped = pPartSys->GetLinkBipedName();
			vPosOffset  = pPartSys->GetLinkPosOffset();
			qtRotOffset = pPartSys->GetLinkRotOffset();
			return true;
		}
	}

	ModelSystemList::iterator iModel;
	for( iModel=m_ModelGrp.begin() ; iModel!=m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pModelSys = *iModel;

		if( !pModelSys->GetLinkBipedName().IsEmpty() )
		{
			szBiped = pModelSys->GetLinkBipedName();
			vPosOffset = pModelSys->GetLinkPosOffset();
			qtRotOffset = pModelSys->GetLinkRotOffset();
			return true;
		}
	}

	return false;
}

D3DXVECTOR3 ioEffect::GetDerivedBipedScale( const ioHashString &rkBiped ) const
{
	D3DXVECTOR3 vDerivedScale( 1.0f, 1.0f, 1.0f );
	
	ioNode *pNode = GetParentNode();
	if( pNode )
	{
		vDerivedScale = pNode->GetDerivedScale();
	}

	ioSkeleton *pSkeleton = GetSkeleton();
	if( !pSkeleton || rkBiped.IsEmpty() )
		return vDerivedScale;

	ioBiped *pBiped = pSkeleton->GetBiped( rkBiped );
	if( pBiped )
	{
		return pBiped->GetDerivedScale() * vDerivedScale;
	}

	return vDerivedScale;
}

D3DXVECTOR3 ioEffect::GetDerivedBipedPos( const ioHashString &rkBiped,
										  const D3DXVECTOR3 &vPosOffset ) const
{
	D3DXVECTOR3 vDerivedPos( 0.0f, 0.0f, 0.0f );

	ioNode *pNode = GetParentNode();
	if( pNode )
	{
		vDerivedPos = pNode->GetDerivedPosition();
	}

	ioSkeleton *pSkeleton = GetSkeleton();
	if( !pSkeleton || rkBiped.IsEmpty() )
		return vDerivedPos;

	ioBiped *pBiped = pSkeleton->GetBiped( rkBiped );
	if( pBiped )
	{
		vDerivedPos = pBiped->GetDerivedOrientation() * ( vPosOffset * pBiped->GetDerivedScale() )
					  + pBiped->GetDerivedPosition();

		vDerivedPos = pNode->GetDerivedOrientation() * ( vDerivedPos * pNode->GetDerivedScale() );
	}

	return vDerivedPos;
}

D3DXQUATERNION ioEffect::GetDerivedBipedOrientation( const ioHashString &rkBiped,
													 const D3DXQUATERNION &qtRotOffset ) const
{
	D3DXQUATERNION qtRotate( 0.0f, 0.0f, 0.0f, 1.0f );

	ioNode *pNode = GetParentNode();
	if( pNode )
	{
		qtRotate = pNode->GetDerivedOrientation();
	}

	ioSkeleton *pSkeleton = GetSkeleton();
	if( !pSkeleton || rkBiped.IsEmpty() )
		return qtRotate;

	ioBiped *pBiped = pSkeleton->GetBiped( rkBiped );
	if( pBiped )
	{
		D3DXQuaternionMultiply( &qtRotate, &pBiped->GetDerivedOrientation(), &qtRotate );
		D3DXQuaternionMultiply( &qtRotate, &qtRotOffset, &qtRotate );
		return qtRotate;
	}

	return qtRotate;
}

bool ioEffect::GetDerivedBipedTransform( const ioHashString &rkBiped,
										 const D3DXVECTOR3 &vPosOffset,
										 const D3DXQUATERNION &qtRotOffset,
										 D3DXMATRIX *pMat ) const
{
	*pMat = *ioMovableObject::GetTransform();

	ioSkeleton *pSkeleton = GetSkeleton();
	if( !pSkeleton || rkBiped.IsEmpty() )
		return false;

	ioBiped *pBiped = pSkeleton->GetBiped( rkBiped );
	if( pBiped )
	{
		D3DXMATRIX matOffset;
		D3DXMatrixRotationQuaternion( &matOffset, &qtRotOffset );
		matOffset._41 = vPosOffset.x;
		matOffset._42 = vPosOffset.y;
		matOffset._43 = vPosOffset.z;

		D3DXMatrixMultiply( &matOffset, &matOffset, pBiped->GetFullTransform() );
		D3DXMatrixMultiply( pMat, &matOffset, pMat );
		return true;
	}

	return false;
}

ioEffect& ioEffect::operator=( const ioEffect &rhs )
{
	SAFEDELETE( m_pAnimation );
	SAFEDELETE( m_pSkeleton );
	DestroyAllSystem();

	m_ParticleGrp.reserve( rhs.m_ParticleGrp.size() );

	ParticleSystemList::const_iterator iter;
	for( iter=rhs.m_ParticleGrp.begin() ; iter!=rhs.m_ParticleGrp.end() ; ++iter )
	{
		ioParticleSystem *pSys = new ioParticleSystem;
		if( pSys )
		{
			*pSys = *(*iter);
			AddParticleSys( pSys );
		}
	}

	m_ModelGrp.reserve( rhs.m_ModelGrp.size() );

	ModelSystemList::const_iterator iModel;
	for( iModel=rhs.m_ModelGrp.begin() ; iModel!=rhs.m_ModelGrp.end() ; ++iModel )
	{
		ioModelParticleSystem *pSys = new ioModelParticleSystem;
		if( pSys )
		{
			*pSys = *(*iModel);
			AddModelSystem( pSys );
		}
	}

	m_FileName = rhs.m_FileName;
	m_vEffectDirection = rhs.m_vEffectDirection;

	if( rhs.m_pSkeleton )
	{
		SetSkeleton( rhs.m_pSkeleton->GetName() );
	}

	if( rhs.m_pAnimation )
	{
		SetAnimation( rhs.m_pAnimation->GetName(), rhs.m_bAnimationLoop );
	}

	m_bCullEachSystem = rhs.m_bCullEachSystem;
	m_bAlwaysUpdateSystem = rhs.m_bAlwaysUpdateSystem;

	m_CollisionBox = rhs.m_CollisionBox;

	if( rhs.m_pLightSys )
	{
		CreateLightSystem();
		*m_pLightSys = *rhs.m_pLightSys;
	}

	m_dwLightCheckTime = 0;

	SetWaitTime( rhs.m_fWaitTime );

	return *this;
}

bool ioEffect::operator==( const ioEffect &rhs ) const
{
	if( m_FileName != rhs.m_FileName )
		return false;

	if( !ioMath::IsEqual( m_vEffectDirection, rhs.m_vEffectDirection ) )
		return false;

	if( m_pSkeleton && rhs.m_pSkeleton )
	{
		if( m_pSkeleton->GetName() != rhs.m_pSkeleton->GetName() )
			return false;
	}
	else
	{
		if( m_pSkeleton && !rhs.m_pSkeleton )
			return false;
		if( !m_pSkeleton && rhs.m_pSkeleton )
			return false;
	}

	if( m_pAnimation && rhs.m_pAnimation )
	{
		if( m_pAnimation->GetName() != rhs.m_pAnimation->GetName() )
			return false;
	}
	else
	{
		if( m_pAnimation && !rhs.m_pAnimation )
			return false;
		if( !m_pAnimation && rhs.m_pAnimation )
			return false;
	}

	if( m_bAnimationLoop != rhs.m_bAnimationLoop )
		return false;

	if( m_bCullEachSystem != rhs.m_bCullEachSystem )
		return false;

	if( m_bAlwaysUpdateSystem != rhs.m_bAlwaysUpdateSystem )
		return false;

	if( m_CollisionBox != rhs.m_CollisionBox )
		return false;

	if( m_pLightSys && rhs.m_pLightSys )
	{
		if( *m_pLightSys != *rhs.m_pLightSys )
			return false;
	}
	else
	{
		if( m_pLightSys && !rhs.m_pLightSys )
			return false;
		if( !m_pLightSys && rhs.m_pLightSys )
			return false;
	}

	if( m_fWaitTime != rhs.m_fWaitTime )
		return false;

	if( m_ParticleGrp.size() != rhs.m_ParticleGrp.size() )
		return false;

	int iGrpCnt = m_ParticleGrp.size();
	for( int i=0 ; i<iGrpCnt ; i++ )
	{
		if( *m_ParticleGrp[i] != *rhs.m_ParticleGrp[i] )
			return false;
	}

	if( m_ModelGrp.size() != rhs.m_ModelGrp.size() )
		return false;

	iGrpCnt = m_ModelGrp.size();
	for( i=0 ; i<iGrpCnt ; i++ )
	{
		if( *m_ModelGrp[i] != *rhs.m_ModelGrp[i] )
			return false;
	}

	return true;
}

bool ioEffect::operator!=( const ioEffect &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

//------------------------------------------------------------------------------------

std::string ioEffect::CmdCollisionBox::Get( const void *pTarget ) const
{
	const ioEffect *pEff = static_cast< const ioEffect* >( pTarget );

	Vector3Vec vVtxList;
	const ioOrientBox &rkBox = pEff->GetCollisionBox();

	D3DXVECTOR3 vVtx;
	vVtx = rkBox.GetCenter();
	vVtx.x -= rkBox.GetExtents(0);
	vVtx.y -= rkBox.GetExtents(1);
	vVtx.z -= rkBox.GetExtents(2);
	vVtxList.push_back( vVtx );

	vVtx = rkBox.GetCenter();
	vVtx.x += rkBox.GetExtents(0);
	vVtx.y += rkBox.GetExtents(1);
	vVtx.z += rkBox.GetExtents(2);
	vVtxList.push_back( vVtx );

	return ioStringConverter::toString( vVtxList );
}

void ioEffect::CmdCollisionBox::Set( void *pTarget, const std::string &kVal )
{
	ioEffect *pEff = static_cast< ioEffect* >(pTarget);

	Vector3Vec kBoxPosList;
	ioStringConverter::ParseVector3Vec( kVal.c_str(), kBoxPosList );

	if( kBoxPosList.size() == 2 )
	{
		ioOrientBox kOBB;
		kOBB.SetBoxByMinMax( kBoxPosList[0], kBoxPosList[1] );
		pEff->SetCollisionBox( kOBB );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioEffect::CmdCollisionBox::Set size wrong %d",
								kBoxPosList.size() );
	}
}

std::string ioEffect::CmdSkeletonName::Get( const void *pTarget ) const
{
	const ioEffect *pEff = static_cast< const ioEffect* >( pTarget );

	return pEff->GetSkeletonName().c_str();
}

void ioEffect::CmdSkeletonName::Set( void *pTarget, const std::string &kVal )
{
	ioEffect *pEff = static_cast< ioEffect* >(pTarget);

	pEff->SetSkeleton( kVal.c_str() );
}

std::string ioEffect::CmdAnimationName::Get( const void *pTarget ) const
{
	const ioEffect *pEff = static_cast< const ioEffect* >( pTarget );

	ioHashString szName = pEff->GetAnimationName();
	if( szName.IsEmpty() )
		return szName.c_str();

	char szBuf[MAX_PATH];
	if( pEff->IsAnimationLoop() )
		wsprintf( szBuf, "%s true", szName.c_str() );
	else
		wsprintf( szBuf, "%s false", szName.c_str() );

	return szBuf;
}

void ioEffect::CmdAnimationName::Set( void *pTarget, const std::string &kVal )
{
	ioEffect *pEff = static_cast< ioEffect* >(pTarget);

	const StringVector &vParam = ioStringConverter::Split( kVal.c_str(), " " );

	if( vParam.size() == 2 )
	{
		pEff->SetAnimation( vParam[0].c_str(),
							ioStringConverter::ParseBool( vParam[1].c_str() ) );
	}
}

std::string ioEffect::CmdEffectDirection::Get( const void *pTarget ) const
{
	const ioEffect *pEff = static_cast< const ioEffect* >( pTarget );
	return ioStringConverter::toString( pEff->GetEffectDirection() );
}

void ioEffect::CmdEffectDirection::Set( void *pTarget, const std::string &kVal )
{
	ioEffect *pEff = static_cast< ioEffect* >(pTarget);
	pEff->SetEffectDirection( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioEffect::CmdCullEachSystem::Get( const void *pTarget ) const
{
	const ioEffect *pEff = static_cast< const ioEffect* >( pTarget );
	return ioStringConverter::toString( pEff->IsCullEachSystem() );
}

void ioEffect::CmdCullEachSystem::Set( void *pTarget, const std::string &kVal )
{
	ioEffect *pEff = static_cast< ioEffect* >(pTarget);
	pEff->SetCullEachSystem( ioStringConverter::ParseBool( kVal.c_str() ) );
}

std::string ioEffect::CmdUpdateSystemAlways::Get( const void *pTarget ) const
{
	const ioEffect *pEff = static_cast< const ioEffect* >( pTarget );
	return ioStringConverter::toString( pEff->IsAlwaysUpdateSystem() );
}

void ioEffect::CmdUpdateSystemAlways::Set( void *pTarget, const std::string &kVal )
{
	ioEffect *pEff = static_cast< ioEffect* >(pTarget);
	pEff->SetAlwaysUpdateSystem( ioStringConverter::ParseBool( kVal.c_str() ) );
}

void ioEffect::TranslateParticle( const D3DXVECTOR3 &vMove )
{
	int iSysCnt = GetParticleSystemCnt();
	for( int i=0 ; i<iSysCnt ; i++ )
	{
		ioParticleSystem *pSys = GetParticleSys(i);
		if( pSys )
		{
			pSys->TranslateParticle( vMove );
		}
	}

	iSysCnt = GetModelSystemCnt();
	for( i=0 ; i<iSysCnt ; i++ )
	{
		ioModelParticleSystem *pSys = GetModelSys( i );
		if( pSys )
		{
			pSys->TranslateModel( vMove );
		}
	}
}