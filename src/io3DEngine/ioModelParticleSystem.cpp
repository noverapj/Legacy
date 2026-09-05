

#include "stdafx.h"

#include "ErrorReport.h"
#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioOrientBox.h"
#include "ioMovableObject.h"
#include "ioModelParticle.h"
#include "ioModelParticleIterator.h"
#include "ioModelParticleAffector.h"
#include "ioModelScaleAffector.h"
#include "ioModelEmitter.h"
#include "ioModelParticleSystem.h"

#include "ioMesh.h"
#include "ioMeshManager.h"
#include "ioEntity.h"

#include "ioFrameTimer.h"

#include "ioEffect.h"
#include "ioEffectFactory.h"
#include "HelpFunc.h"

#include "ioBiped.h"
#include "ioSkeleton.h"
#include "ioMath.h"

ioModelParticleSystem::CmdQuota				ioModelParticleSystem::m_QuotaCmd;
ioModelParticleSystem::CmdCull				ioModelParticleSystem::m_CullCmd;
ioModelParticleSystem::CmdMeshName			ioModelParticleSystem::m_MeshNameCmd;
ioModelParticleSystem::CmdEndCondition		ioModelParticleSystem::m_EndConditionCmd;
ioModelParticleSystem::CmdLimitTime			ioModelParticleSystem::m_LimitTimeCmd;
ioModelParticleSystem::CmdIterationInterval ioModelParticleSystem::m_IterationIntervalCmd;
ioModelParticleSystem::CmdStartCreateParticleCnt  ioModelParticleSystem::m_StartCreateParticleCntCmd;
ioModelParticleSystem::CmdFollowParentMode		  ioModelParticleSystem::m_FollowParentModeCmd;
ioModelParticleSystem::CmdLinkBipedName			  ioModelParticleSystem::m_LinkBipedNameCmd;
ioModelParticleSystem::CmdLinkPosOffset			  ioModelParticleSystem::m_LinkPosOffsetCmd;
ioModelParticleSystem::CmdLinkRotOffset			  ioModelParticleSystem::m_LinkRotOffsetCmd;
ioModelParticleSystem::CmdRandomSyncIndex		  ioModelParticleSystem::m_RandomSyncIndexCmd;

float ioModelParticleSystem::m_sfDefaultIterationInterval = 0.0f;
bool  ioModelParticleSystem::m_sbLimitFullParticleQuota = false;
float ioModelParticleSystem::m_sfLimitQuotaRate = 1.0f;

ioModelParticleSystem::ioModelParticleSystem()
{
	m_pEmitter = NULL;
	m_pParent  = NULL;

	m_iRandomSyncIndex = -1;
	m_EndCondition = EC_NEVER_END;
	m_fCurTime	= 0.0f;
	m_fEndTime	= 0.0f;

	m_FollowMode = FPT_NONE;
	m_vLinkPosOffset  = ioMath::VEC3_ZERO;
	m_qtLinkRotOffset = ioMath::QUAT_IDENTITY;

	m_iParticleQuota  = 0;
	m_eSystemState = MPSS_LIVE;

	m_iLimitQuotaRemain = 0;
	m_iLastVisibleFrame = 0;
	m_fUpdateRemainTime = 0.0f;

	m_fIterationInterval = 0.0f;
	m_iStartCreateParticleCnt = 0;

	m_fExtendAlphaRate = 1.0f;

	m_bCullIndividual = false;
	m_bIterationIntervalSet = false;
	m_bStartParticleCreated = false;

	m_bEachCulled = false;

	InitParameters();
}

ioModelParticleSystem::~ioModelParticleSystem()
{
	ClearActiveParticles();

	SAFEDELETE(m_pEmitter);
	RemoveAllAffector();
}

void ioModelParticleSystem::InitParameters()
{
	if( CreateParamDictionary( "ioModelParticleSystem" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		if( pDict )
		{
			pDict->AddParameter( ioParameterDef( "mesh_file", PT_STRING ),
								 &m_MeshNameCmd );

			pDict->AddParameter( ioParameterDef( "quota", PT_INT ),
								 &m_QuotaCmd );

			pDict->AddParameter( ioParameterDef( "cull_each", PT_BOOL ),
								 &m_CullCmd );
			
			pDict->AddParameter( ioParameterDef( "follow_parent", PT_STRING ),
								 &m_FollowParentModeCmd );

			pDict->AddParameter( ioParameterDef( "end_condition", PT_STRING ),
								 &m_EndConditionCmd );

			pDict->AddParameter( ioParameterDef( "limit_time", PT_FLOAT ),
								 &m_LimitTimeCmd );

			pDict->AddParameter( ioParameterDef( "iteration_interval", PT_FLOAT ),
								 &m_IterationIntervalCmd );

			pDict->AddParameter( ioParameterDef( "start_create_particles", PT_INT ),
								 &m_StartCreateParticleCntCmd );

			pDict->AddParameter( ioParameterDef( "link_biped_name", PT_STRING ),
								 &m_LinkBipedNameCmd );

			pDict->AddParameter( ioParameterDef( "pos_offset", PT_VECTOR3 ),
								 &m_LinkPosOffsetCmd );

			pDict->AddParameter( ioParameterDef( "rot_offset", PT_QUATERNION ),
								 &m_LinkRotOffsetCmd );

			pDict->AddParameter( ioParameterDef( "random_sync_index", PT_INT ),
								 &m_RandomSyncIndexCmd );
		}
	}
}

void ioModelParticleSystem::ClearActiveParticles()
{
	ActiveModelList::iterator iter;
	for( iter=m_ActiveModelList.begin() ; iter!=m_ActiveModelList.end() ; ++iter )
	{
		delete *iter;
	}
	m_ActiveModelList.clear();
}

void ioModelParticleSystem::SetIterationInterval( float fInterval )
{
	m_fIterationInterval = fInterval;
	m_bIterationIntervalSet = true;
}

void ioModelParticleSystem::SetLimitMaxParticleQuota( bool bLimit, float fLimitRate )
{
	m_sbLimitFullParticleQuota = bLimit;
	m_sfLimitQuotaRate = fLimitRate;
}

void ioModelParticleSystem::Restart()
{
	ClearActiveParticles();

	if( m_pEmitter )
	{
		m_pEmitter->Restart();
	}

	std::for_each( m_AffectorList.begin(),
				   m_AffectorList.end(),
				   std::mem_fun(&ioModelParticleAffector::Restart) );

	m_fCurTime = 0.0f;
	m_eSystemState = MPSS_LIVE;

	m_iLimitQuotaRemain = 0;
	m_iLastVisibleFrame = g_FrameTimer.GetFrameCounter();
	m_fUpdateRemainTime = 0.0f;

	m_bStartParticleCreated = false;
}

void ioModelParticleSystem::SetParent( ioEffect *pParent )
{
	m_pParent = pParent;

	m_iLastVisibleFrame = g_FrameTimer.GetFrameCounter();
}

ioModelEmitter* ioModelParticleSystem::SetEmitter( const ioHashString &kType )
{
	SAFEDELETE(m_pEmitter);

	m_pEmitter = g_EffectFactory.CreateModelEmitter( kType );

	return m_pEmitter;
}

void ioModelParticleSystem::SetFollowParentMode( FollowParentType eType )
{
	m_FollowMode = eType;
}

void ioModelParticleSystem::SetParticleQuota( int iQuota )
{
	m_iParticleQuota = iQuota;
}

void ioModelParticleSystem::SetMeshName( const ioHashString &rkName )
{
	m_MeshName = rkName;
}

ioModelParticle* ioModelParticleSystem::AddModelParticle()
{
	ioModelParticle *pNewParticle = new ioModelParticle;
	if( pNewParticle )
	{
		m_ActiveModelList.push_back( pNewParticle );
		
		pNewParticle->SetParent( this );
		pNewParticle->SetEntity( m_MeshName );

		if( m_pParent )
			m_pParent->AddEntityEvent( pNewParticle->GetEntity() );
	}

	return pNewParticle;
}

void ioModelParticleSystem::Update( float fTime )
{
	UpdateParentTransform();

	CreateStartParticles();

	if( !m_pParent->IsAlwaysUpdateSystem() )
	{
		int iFrameDiff = g_FrameTimer.GetFrameCounter() - m_iLastVisibleFrame;
		if( iFrameDiff > 1 || iFrameDiff < 0 )	// Now Not Visible
		{
			ExpireParticles( fTime );
			UpdateSystemState( fTime );
			return;
		}
	}

	float fIterationInterval = m_sfDefaultIterationInterval;
	if( m_bIterationIntervalSet )
	{
		fIterationInterval = m_fIterationInterval;
	}

	if( fIterationInterval > 0.0f )
	{
		m_fUpdateRemainTime += fTime;

		while( m_fUpdateRemainTime >= fIterationInterval )
		{
			ExpireParticles( fIterationInterval );
			TriggerAffector( fIterationInterval );
			UpdateParticles( fIterationInterval );
			TriggerEmitter( fIterationInterval );
			UpdateSystemState( fIterationInterval );

			m_fUpdateRemainTime -= fIterationInterval;
		}
	}
	else
	{
		ExpireParticles( fTime );
		TriggerAffector( fTime );
		UpdateParticles( fTime );
		TriggerEmitter( fTime );
		UpdateSystemState( fTime );
	}
}

void ioModelParticleSystem::UpdateSystemState( float fTime )
{
	switch( m_eSystemState )
	{
	case MPSS_LIVE:
		if( m_EndCondition == EC_TIME_LIMIT )
		{
			m_fCurTime += fTime;

			if( m_fCurTime >= m_fEndTime )
			{
				SetState( MPSS_GRACE_CLOSE );
			}
		}
		else
		{
			// EC_NEVER_END 인데 시작시만 파티클생성하고 계속 생성안하는 이펙트를 제거하기위한 궁여지책
			if( m_pEmitter->GetEmissionRate() < 0.1f && m_ActiveModelList.empty() )
			{
				SetState( MPSS_END );
			}
		}
		break;
	case MPSS_GRACE_CLOSE:
		if( m_ActiveModelList.empty() )
		{
			SetState( MPSS_END );
		}
		break;
	}
}

void ioModelParticleSystem::UpdateParentTransform()
{
	if( m_FollowMode != FPT_POS_ONLY )
	{
		m_pParent->GetDerivedBipedTransform( m_LinkBipedName,
											 m_vLinkPosOffset,
											 m_qtLinkRotOffset,
											 &m_matStartXform );
	}

	if( m_FollowMode == FPT_ROT_ONLY )
	{
		m_qtStartRotation = m_pParent->GetDerivedBipedOrientation( m_LinkBipedName,
																   m_qtLinkRotOffset );
	}

	if( m_pParent->IsAttached() )
	{
		if( m_FollowMode == FPT_POS_ONLY )
		{
			D3DXVECTOR3 vScale = m_pParent->GetDerivedBipedScale( m_LinkBipedName );
			D3DXMatrixScaling( &m_matParentXform, vScale.x, vScale.y, vScale.z );

			D3DXVECTOR3 vPos = m_pParent->GetDerivedBipedPos( m_LinkBipedName, m_vLinkPosOffset );
			m_matParentXform._41 = vPos.x;
			m_matParentXform._42 = vPos.y;
			m_matParentXform._43 = vPos.z;
		}
		else if( m_FollowMode == FPT_ALL )
		{
			m_matParentXform = m_matStartXform;
		}
		else
		{
			D3DXMatrixIdentity( &m_matParentXform );
		}
	}
	else
	{
		D3DXMatrixIdentity( &m_matParentXform );
	}
}

void ioModelParticleSystem::ExpireParticles( float fTime )
{
	ActiveModelList::iterator iter;
	for( iter=m_ActiveModelList.begin() ; iter!=m_ActiveModelList.end() ; )
	{
		ioModelParticle *pMdl = *iter;

		if( pMdl->IsLive() )
		{
			pMdl->m_fCurTime += fTime;
			++iter;
		}
		else
		{
			iter = m_ActiveModelList.erase( iter );
			delete pMdl;
		}
	}
}

void ioModelParticleSystem::TriggerAffector( float fTime )
{
	float fTimePerSec = fTime /FLOAT1000;

	ModelAffectorList::iterator iter;
	for( iter=m_AffectorList.begin() ; iter!=m_AffectorList.end() ; ++iter )
	{
		ioModelParticleAffector *pAffector = *iter;

		if( pAffector->IsEnable() )
		{
			pAffector->AffectModel( this, fTimePerSec );
		}
	}
}

void ioModelParticleSystem::UpdateParticles( float fTime )
{
	float fTimePerSec = fTime /FLOAT1000;

	if( m_fExtendAlphaRate == 1.0f )
	{
		ActiveModelList::iterator iter;
		for( iter=m_ActiveModelList.begin() ; iter!=m_ActiveModelList.end() ; ++iter )
		{
			ioModelParticle *pMdl = *iter;

			pMdl->m_vPosition += pMdl->m_vDir * ( pMdl->m_fMoveSpeed * fTimePerSec );
			pMdl->SetAlphaRate( m_pEmitter->GetCurAlphaRate( pMdl->TimeRate() ) );
		}
	}
	else
	{
		ActiveModelList::iterator iter;
		for( iter=m_ActiveModelList.begin() ; iter!=m_ActiveModelList.end() ; ++iter )
		{
			ioModelParticle *pMdl = *iter;

			pMdl->m_vPosition += pMdl->m_vDir * ( pMdl->m_fMoveSpeed * fTimePerSec );

			int iAlphaRate = m_pEmitter->GetCurAlphaRate( pMdl->TimeRate() );
			pMdl->SetAlphaRate( (int)(iAlphaRate * m_fExtendAlphaRate) );
		}
	}
}

void ioModelParticleSystem::SetExtendAlphaRate( float fRate )
{
	m_fExtendAlphaRate = max( 0.0f, min( fRate, 1.0f ) );
}

void ioModelParticleSystem::TriggerEmitter( float fTime )
{
	int iRequest = GetNewParticleCount( fTime );
	if( iRequest <= 0 )	return;

	float fTimePoint = 0.0f;
	float fEmitTimeGap = ( fTime/FLOAT1000 ) / (float)iRequest;

	for( int i=0 ; i<iRequest ; i++ )
	{
		ioModelParticle *pMdl = AddModelParticle();
		if( pMdl )
		{
			ApplyStartParticle( pMdl, fTimePoint );
		}

		fTimePoint += fEmitTimeGap;
	}
}

void ioModelParticleSystem::ApplyStartParticle( ioModelParticle *pMdl, float fTimePoint )
{
	m_pEmitter->InitParticle( pMdl );

	int iAlphaRate = m_pEmitter->GetCurAlphaRate( pMdl->TimeRate() );
	pMdl->SetAlphaRate( (int)(iAlphaRate * m_fExtendAlphaRate) );

	if( m_FollowMode == FPT_NONE )
	{
		D3DXVec3TransformCoord( &pMdl->m_vPosition, &pMdl->m_vPosition, &m_matStartXform );
		D3DXVec3TransformNormal( &pMdl->m_vDir, &pMdl->m_vDir, &m_matStartXform );
	}
	else if( m_FollowMode == FPT_ROT_ONLY )
	{
		D3DXVec3TransformCoord( &pMdl->m_vPosition, &pMdl->m_vPosition, &m_matStartXform );
		D3DXVec3TransformNormal( &pMdl->m_vDir, &pMdl->m_vDir, &m_matStartXform );
		pMdl->m_qtStartParentOrientation = m_qtStartRotation;
	}

	pMdl->m_vPosition += pMdl->m_vDir * ( pMdl->m_fMoveSpeed * fTimePoint );

	ModelAffectorList::iterator iter;
	for( iter=m_AffectorList.begin() ; iter!=m_AffectorList.end() ; ++iter )
	{
		ioModelParticleAffector *pAffector = *iter;
		if( pAffector->IsEnable() )
		{
			pAffector->InitModel( pMdl );
		}
	}
}

int ioModelParticleSystem::GetNewParticleCount( float fTime )
{
	if( m_eSystemState != MPSS_LIVE )
		return 0;

	int iRequest = m_pEmitter->GetCurEmitModelCnt( fTime );
	int iCanEmitCount = GetParticleQuota() - GetNumCurParticles();
	iRequest = min( iRequest, iCanEmitCount );

	if( !m_sbLimitFullParticleQuota )
		return iRequest;

	m_iLimitQuotaRemain += iRequest;

	if( m_iLimitQuotaRemain > 0 )
	{
		iRequest = m_iLimitQuotaRemain * m_sfLimitQuotaRate;

		if( iRequest == 0 && GetNumCurParticles() == 0 )
		{
			iRequest = m_iLimitQuotaRemain;
			m_iLimitQuotaRemain = 0;
		}
		else
		{
			m_iLimitQuotaRemain -= iRequest / m_sfLimitQuotaRate;
			m_iLimitQuotaRemain = max( 0, m_iLimitQuotaRemain );
		}
	}

	return iRequest;
}

void ioModelParticleSystem::UpdateBounds( const D3DXMATRIX &rkInvParent )
{
	if( m_ActiveModelList.empty() )
	{
		m_BoundBox.SetNull();
		return;
	}

	float fMaxFloat = ioMath::FLOAT_INFINITY;
	D3DXVECTOR3 vMin( fMaxFloat, fMaxFloat, fMaxFloat );
	D3DXVECTOR3 vMax( -fMaxFloat, -fMaxFloat, -fMaxFloat );
	D3DXVECTOR3 vCurMin, vCurMax;

	bool bBoundUpdated = false;
	ActiveModelList::iterator iter = m_ActiveModelList.begin();
	for( ; iter!=m_ActiveModelList.end(); ++iter )
	{
		ioModelParticle *pMdl = *iter;

		pMdl->BuildResource();
		pMdl->UpdateWorldTransform();

		if( pMdl->GetWorldBoundPos( vCurMin, vCurMax ) )
		{
			vMin.x = min( vMin.x, vCurMin.x );
			vMin.y = min( vMin.y, vCurMin.y );
			vMin.z = min( vMin.z, vCurMin.z );

			vMax.x = max( vMax.x, vCurMax.x );
			vMax.y = max( vMax.y, vCurMax.y );
			vMax.z = max( vMax.z, vCurMax.z );

			bBoundUpdated = true;
		}
	}

	if( bBoundUpdated )
	{
		m_BoundBox.SetMinMaxPos( vMin, vMax );
		m_BoundBox.Transform( &rkInvParent );
	}
	else
	{
		m_BoundBox.SetNull();
	}
}

void ioModelParticleSystem::NotifyCurrentCamera( ioCamera *pCamera )
{
	m_iLastVisibleFrame = g_FrameTimer.GetFrameCounter();
}

void ioModelParticleSystem::UpdateRenderQueue( ioRenderQueue &queue )
{
	RenderGroupID eGroupID = m_pParent->GetRenderQueueGroup();

	ActiveModelList::iterator iter;
	for( iter = m_ActiveModelList.begin() ; iter!=m_ActiveModelList.end() ; ++iter )
	{
		(*iter)->UpdateRenderQueue( queue, eGroupID );
	}
}

void ioModelParticleSystem::CreateStartParticles()
{
	if( m_bStartParticleCreated || m_iStartCreateParticleCnt <= 0 )	return;

	int iCanEmitCount = GetParticleQuota() - GetNumCurParticles();
	int iNewParticle = min( m_iStartCreateParticleCnt, iCanEmitCount );

	if( m_sbLimitFullParticleQuota && iNewParticle > 1 )
	{
		iNewParticle *= m_sfLimitQuotaRate;
		iNewParticle = max( 1, iNewParticle );
	}

	for( int i=0 ; i<iNewParticle ; i++ )
	{
		ioModelParticle *pMdl = AddModelParticle();
		if( pMdl )
		{
			ApplyStartParticle( pMdl, 0.0f );
		}
	}

	m_bStartParticleCreated = true;
}

void ioModelParticleSystem::ScaleActualSize( float fScaleRate )
{
	if( m_pEmitter )
	{
		m_pEmitter->ScaleActualSize( fScaleRate );
	}

	bool bHasScaleAffector = false;
	ModelAffectorList::iterator iter;
	for( iter=m_AffectorList.begin() ; iter!=m_AffectorList.end() ; ++iter )
	{
		ioModelParticleAffector *pAff = *iter;

		pAff->ParentSystemScaled( fScaleRate );
		if( pAff->GetType() == "Scale" )
		{
			bHasScaleAffector = true;
			break;
		}
	}

	if( !bHasScaleAffector )
	{
		ioModelScaleAffector *pScale = (ioModelScaleAffector*)AddAffector( "Scale" );
		if( pScale )
		{
			pScale->AddScaleRate( 0.0f, fScaleRate, fScaleRate, fScaleRate );
		}
	}	
}

void ioModelParticleSystem::ChangeSystemDirection( const D3DXQUATERNION &qtRotate )
{
	if( m_pEmitter )
	{
		m_pEmitter->RotateActualDirection( qtRotate );
	}

	ModelAffectorList::iterator iter;
	for( iter=m_AffectorList.begin() ; iter!=m_AffectorList.end() ; ++iter )
	{
		(*iter)->ParentSystemDirectionChanged( qtRotate );
	}
}

int ioModelParticleSystem::GetNumSimulatedMaxParticles()
{
	if( !m_pEmitter )	return 0;

	int iMaxParticles = 0;
	if( m_EndCondition == EC_TIME_LIMIT )
	{
		iMaxParticles = m_pEmitter->GetEmissionRate() * min( m_pEmitter->GetMaxLiveTime(), m_fEndTime );
	}
	else
	{
		iMaxParticles = m_pEmitter->GetEmissionRate() * m_pEmitter->GetMaxLiveTime();
	}

	iMaxParticles = iMaxParticles / 1000 + m_iStartCreateParticleCnt;

	return min( iMaxParticles, m_iParticleQuota );
}

int ioModelParticleSystem::GetNumSimulatedMaxFaceCount()
{
	int iFaceCount = 0;

	ioMesh *pMesh = ioMeshManager::GetSingleton().CreateMesh( m_MeshName );
	if( pMesh )
	{
		iFaceCount = GetNumSimulatedMaxParticles() * pMesh->GetFaceCount();
		
		ioMeshManager::GetSingleton().Destroy( pMesh );
		pMesh = NULL;
	}

	return iFaceCount;
}

ioModelParticleAffector* ioModelParticleSystem::AddAffector( const ioHashString &kType )
{
	ioModelParticleAffector *pAff = g_EffectFactory.CreateModelAffector( kType );
	if( pAff )
	{
		m_AffectorList.push_back( pAff );
	}

	return pAff;
}

void ioModelParticleSystem::RemoveAffector( int iIdx )
{
	if( COMPARE( iIdx, 0, GetNumAffector() ) )
	{
		ModelAffectorList::iterator iter = m_AffectorList.begin() + iIdx;

		delete *iter;
		m_AffectorList.erase( iter );
	}
}

void ioModelParticleSystem::RemoveAllAffector()
{
	ModelAffectorList::iterator iter;
	for( iter = m_AffectorList.begin() ; iter!=m_AffectorList.end(); ++iter )
	{
		delete *iter;
	}

	m_AffectorList.erase( m_AffectorList.begin(), m_AffectorList.end() );
}

ioModelParticleAffector* ioModelParticleSystem::GetAffector( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetNumAffector() ) )
		return m_AffectorList[iIdx];

	return NULL;
}

bool ioModelParticleSystem::IsEnableAtNextFrame( float fCurTimeGap )
{
	if( m_fCurTime + fCurTimeGap >= m_fEndTime )
		return false;

	return true;
}

void ioModelParticleSystem::SetRandomSeed( unsigned int uSeed )
{
	if( m_pEmitter )
	{
		m_pEmitter->SetEmitPosGenSeed( uSeed );
	}
}

unsigned int ioModelParticleSystem::GetRandomSeed() const
{
	if( m_pEmitter )
		return m_pEmitter->GetEmitPosGenSeed();

	return 0xffffffff;
}

ioModelParticleIterator ioModelParticleSystem::GetIterator()
{
	return ioModelParticleIterator( m_ActiveModelList.begin(), m_ActiveModelList.end() );
}

ioModelParticleSystem& ioModelParticleSystem::operator=( const ioModelParticleSystem &rhs )
{
	SAFEDELETE(m_pEmitter);
	RemoveAllAffector();

	ioModelEmitter *pRhsEmit = rhs.GetEmitter();
	ioModelEmitter *pNewEmit = SetEmitter( pRhsEmit->GetGeneratorType() );
	pNewEmit->ParametersCopy( pRhsEmit );

	m_AffectorList.reserve( rhs.GetNumAffector() );
	ioModelParticleAffector *pRhsAff, *pNewAff;
	int iNumAff = rhs.GetNumAffector();
	for( int i=0 ; i<iNumAff ; i++ )
	{
		pRhsAff = rhs.GetAffector(i);
		pNewAff = AddAffector( pRhsAff->GetType() );
		pNewAff->ParametersCopy( pRhsAff );
	}

	m_iRandomSyncIndex = rhs.m_iRandomSyncIndex;

	m_LinkBipedName   = rhs.m_LinkBipedName;
	m_vLinkPosOffset  = rhs.m_vLinkPosOffset;
	m_qtLinkRotOffset = rhs.m_qtLinkRotOffset;

	SetMeshName( rhs.m_MeshName );

	m_EndCondition = rhs.m_EndCondition;
	m_fEndTime = rhs.m_fEndTime;

	m_FollowMode = rhs.m_FollowMode;
	m_bCullIndividual = rhs.m_bCullIndividual;

	SetParticleQuota( rhs.GetParticleQuota() );

	m_fIterationInterval = rhs.m_fIterationInterval;
	m_iStartCreateParticleCnt = rhs.m_iStartCreateParticleCnt;

	m_bIterationIntervalSet = rhs.m_bIterationIntervalSet;

	return *this;
}

bool ioModelParticleSystem::operator==( const ioModelParticleSystem &rhs )
{
	if( !m_pEmitter || !rhs.m_pEmitter )
		return false;

	if( *m_pEmitter != *rhs.m_pEmitter )
		return false;

	if( m_AffectorList.size() != rhs.m_AffectorList.size() )
		return false;

	ModelAffectorList::const_iterator iter, iRhs;
	iter = m_AffectorList.begin();
	iRhs = rhs.m_AffectorList.begin();
	for( ; iter!=m_AffectorList.end() ; ++iter, ++iRhs )
	{
		if( !(*iter)->IsSameAffector( *iRhs ) )
			return false;
	}

	if( m_iRandomSyncIndex != rhs.m_iRandomSyncIndex )
		return false;

	if( m_LinkBipedName != rhs.m_LinkBipedName )
		return false;

	if( !ioMath::IsEqual( m_vLinkPosOffset, rhs.m_vLinkPosOffset ) )
		return false;

	if( !ioMath::IsEqual( m_qtLinkRotOffset, rhs.m_qtLinkRotOffset ) )
		return false;

	if( m_MeshName != rhs.m_MeshName )
		return false;

	if( m_EndCondition != rhs.m_EndCondition )
		return false;

	if( m_fEndTime != rhs.m_fEndTime )
		return false;

	if( m_FollowMode != rhs.m_FollowMode )
		return false;

	if( m_iParticleQuota != rhs.m_iParticleQuota )
		return false;

	if( m_bCullIndividual != rhs.m_bCullIndividual )
		return false;

	if( m_fIterationInterval != rhs.m_fIterationInterval )
		return false;

	if( m_iStartCreateParticleCnt != rhs.m_iStartCreateParticleCnt )
		return false;

	return true;
}

bool ioModelParticleSystem::operator!=( const ioModelParticleSystem &rhs )
{
	if( *this == rhs )
		return false;

	return true;
}

D3DXVECTOR3 ioModelParticleSystem::GetTrailBipedPosition( const ioHashString &rkName ) const
{
	if( m_pParent->IsAttached() )
	{
		ioSkeleton *pSkel = m_pParent->GetSkeleton();
		if( pSkel )
		{
			ioBiped *pBiped = pSkel->GetBiped( rkName );
			if( pBiped )
			{
				D3DXVECTOR3 vPos;
				D3DXVec3TransformCoord( &vPos,
										&pBiped->GetDerivedPosition(),
										m_pParent->GetTransform() );
				return vPos;
			}
		}
	}

	return GetEmitPosition();
}

D3DXVECTOR3 ioModelParticleSystem::GetEmitPosition() const
{
	D3DXVECTOR3 vEmitPos = m_pEmitter->GetPosition();
	if( m_FollowMode == FPT_NONE || m_FollowMode == FPT_ROT_ONLY )
	{
		D3DXVec3TransformCoord( &vEmitPos, &vEmitPos, &m_matStartXform );
	}

	return vEmitPos;
}

D3DXVECTOR3 ioModelParticleSystem::GetParentScale() const
{
	ioNode *pNode = m_pParent->GetParentNode();
	if( pNode )
		return pNode->GetDerivedScale();

	return ioMath::UNIT_ALL;
}

RenderGroupID ioModelParticleSystem::GetRenderQueueGroup() const
{
	if( m_pParent )
		return m_pParent->GetRenderQueueGroup();

	return RENDER_MAIN;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioModelParticleSystem::CmdQuota::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetParticleQuota() );
}

void ioModelParticleSystem::CmdQuota::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetParticleQuota( ioStringConverter::ParseInt( kVal.c_str() ) );
}

std::string ioModelParticleSystem::CmdCull::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->IsCullIndividual() );
}

void ioModelParticleSystem::CmdCull::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetCullIndividual( ioStringConverter::ParseBool( kVal.c_str() ) );
}

std::string ioModelParticleSystem::CmdMeshName::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return pSys->GetMeshName().c_str();
}

void ioModelParticleSystem::CmdMeshName::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetMeshName( kVal.c_str() );
}

std::string ioModelParticleSystem::CmdEndCondition::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast<const ioModelParticleSystem* >(pTarget);

	switch( pSys->GetEndCondition() )
	{
	case ioModelParticleSystem::EC_NEVER_END:
		{
			//UJ 140120, 보안강화: 인니 핵 대응
			//return "never_end";
			static char szEncNeverEnd[10] = {'n','e','v','e','r','_','e','n','d',0};
			return szEncNeverEnd;
		}
		break;
	case ioModelParticleSystem::EC_TIME_LIMIT:
		{
			//UJ 140120, 보안강화: 인니 핵 대응
			//return "time_limit";
			static char szEncTimeLimit[11] = {'t','i','m','e','_','l','i','m','i','t',0};
			return szEncTimeLimit;
		}
		break;
	}

	return "unknown";
}

void ioModelParticleSystem::CmdEndCondition::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	//UJ 140120, 보안강화: 인니 핵 대응
	//if( kVal == "time_limit" )
	static char szEncTimeLimit[11] = {'t','i','m','e','_','l','i','m','i','t',0};
	if( strcmp( kVal.c_str(), szEncTimeLimit ) == 0 )
	{
		pSys->SetEndCondition( ioModelParticleSystem::EC_TIME_LIMIT );
	}
	else
	{
		pSys->SetEndCondition( ioModelParticleSystem::EC_NEVER_END );
	}
}

std::string ioModelParticleSystem::CmdLimitTime::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast<const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetEndTime() );
}

void ioModelParticleSystem::CmdLimitTime::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetEndTime( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

bool ioModelParticleSystem::CmdIterationInterval::IsSkipSave( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);
	if( pSys->GetIterationInterval() == 0.0f )
		return true;

	return false;
}

std::string ioModelParticleSystem::CmdIterationInterval::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetIterationInterval() );
}

void ioModelParticleSystem::CmdIterationInterval::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetIterationInterval( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

std::string ioModelParticleSystem::CmdStartCreateParticleCnt::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetStartCreateParticleCnt() );
}

void ioModelParticleSystem::CmdStartCreateParticleCnt::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetStartCreateParticleCnt( ioStringConverter::ParseInt( kVal.c_str() ) );
}

std::string ioModelParticleSystem::CmdFollowParentMode::Get( const void *pTarget  ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >( pTarget );

	switch( pSys->GetFollowParentMode() )
	{
	case FPT_NONE:
		return "none";
	case FPT_POS_ONLY:
		return "pos_only";
	case FPT_ROT_ONLY:
		return "rot_only";
	case FPT_ALL:
		return "all";
	}

	return "none";
}

void ioModelParticleSystem::CmdFollowParentMode::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem::FollowParentType eType = ioModelParticleSystem::FPT_NONE;
	if( kVal == "none" )
	{
		eType = ioModelParticleSystem::FPT_NONE;
	}
	else if( kVal == "pos_only" )
	{
		eType = ioModelParticleSystem::FPT_POS_ONLY;
	}
	else if( kVal == "rot_only" )
	{
		eType = ioModelParticleSystem::FPT_ROT_ONLY;
	}
	else if( kVal == "all" )
	{
		eType = ioModelParticleSystem::FPT_ALL;
	}

	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);
	pSys->SetFollowParentMode( eType );
}

std::string ioModelParticleSystem::CmdLinkBipedName::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	if( pSys->GetLinkBipedName().IsEmpty() )
		return "none";

	return pSys->GetLinkBipedName().c_str();
}

void ioModelParticleSystem::CmdLinkBipedName::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	if( kVal != "none" )
	{
		pSys->SetLinkBipedName( kVal.c_str() );
	}
	else
	{
		pSys->SetLinkBipedName( "" );
	}
}

std::string ioModelParticleSystem::CmdLinkPosOffset::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetLinkPosOffset() );
}

void ioModelParticleSystem::CmdLinkPosOffset::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);
	pSys->SetLinkPosOffset( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioModelParticleSystem::CmdLinkRotOffset::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetLinkRotOffset() );
}

void ioModelParticleSystem::CmdLinkRotOffset::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);
	pSys->SetLinkRotOffset( ioStringConverter::ParseQuaternion( kVal.c_str() ) );
}

std::string ioModelParticleSystem::CmdRandomSyncIndex::Get( const void *pTarget ) const
{
	const ioModelParticleSystem *pSys = static_cast< const ioModelParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetRandomSyncIndex() );
}

void ioModelParticleSystem::CmdRandomSyncIndex::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleSystem *pSys = static_cast< ioModelParticleSystem* >(pTarget);

	pSys->SetRandomSyncIndex( ioStringConverter::ParseInt( kVal.c_str() ) );
}

void ioModelParticleSystem::TranslateModel( const D3DXVECTOR3 &vMove )
{
	if( m_FollowMode == FPT_NONE )
	{
		ActiveModelList::iterator iter = m_ActiveModelList.begin();
		for( ; iter!=m_ActiveModelList.end(); ++iter )
		{
			ioModelParticle *pMdl = *iter;
			pMdl->m_vPosition += vMove;
		}
	}
}
