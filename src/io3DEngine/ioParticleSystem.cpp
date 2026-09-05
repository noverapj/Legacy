

#include "stdafx.h"

#include "ioCamera.h"

#include "ioStringConverter.h"
#include "ioRenderOperation.h"
#include "ioRenderQueue.h"
#include "ioMovableObject.h"
#include "ioOrientBox.h"

#include "ioFrameTimer.h"

#include "ioParticle.h"
#include "ioParticleEmitter.h"
#include "ioParticleAffector.h"
#include "ioParticleIterator.h"
#include "ioParticleSystem.h"

#include "ioMaterial.h"
#include "ioTextureUnitState.h"

#include "ioEffect.h"
#include "ioEffectFactory.h"
#include "HelpFunc.h"

#include "ioBiped.h"
#include "ioSkeleton.h"

#include "ioMath.h"
#include "ioMaterialManager.h"
#include "ioCPU.h"

ioParticleSystem::CmdQuota			ioParticleSystem::m_QuotaCmd;
ioParticleSystem::CmdCull			ioParticleSystem::m_CullCmd;
ioParticleSystem::CmdFollowParentMode ioParticleSystem::m_FollowParentModeCmd;
ioParticleSystem::CmdDefaultWidth	ioParticleSystem::m_DefaultWidthCmd;
ioParticleSystem::CmdDefaultHeight  ioParticleSystem::m_DefaultHeightCmd;
ioParticleSystem::CmdRenderType		ioParticleSystem::m_RenderTypeCmd;
ioParticleSystem::CmdTexFiltering   ioParticleSystem::m_TexFilteringCmd;
ioParticleSystem::CmdTextureName	ioParticleSystem::m_TextureNameCmd;
ioParticleSystem::CmdBillboardType	ioParticleSystem::m_BillboardTypeCmd;
ioParticleSystem::CmdEndCondition   ioParticleSystem::m_EndConditionCmd;
ioParticleSystem::CmdLimitTime		ioParticleSystem::m_LimitTimeCmd;
ioParticleSystem::CmdCommonDir		ioParticleSystem::m_CommonDirCmd;
ioParticleSystem::CmdCommonDirNoneBillboard ioParticleSystem::m_CommonDirNoneBillboardCmd;
ioParticleSystem::CmdIterationInterval ioParticleSystem::m_IterationIntervalCmd;
ioParticleSystem::CmdStartCreateParticleCnt ioParticleSystem::m_StartCreateParticleCnt;
ioParticleSystem::CmdDepthCheck		ioParticleSystem::m_DepthCheckCmd;
ioParticleSystem::CmdLinkBipedName  ioParticleSystem::m_LinkBipedNameCmd;
ioParticleSystem::CmdLinkPosOffset  ioParticleSystem::m_LinkPosOffsetCmd;
ioParticleSystem::CmdLinkRotOffset  ioParticleSystem::m_LinkRotOffsetCmd;
ioParticleSystem::CmdModulate2X		ioParticleSystem::m_Modulate2XCmd;
ioParticleSystem::CmdRandomSyncIndex ioParticleSystem::m_RandomSyncIndexCmd;

float ioParticleSystem::m_sfDefaultIterationInterval = 0.0f;
bool  ioParticleSystem::m_sbLimitFullParticleQuota = false;
float ioParticleSystem::m_sfLimitQuotaRate = 1.0f;

ioParticleSystem::ioParticleSystem()
: m_VisibleArray( 100, 50 )
{
	m_pParent   = NULL;
	m_pEmitter  = NULL;
	m_pMtl		= NULL;

	m_BillboardType = BBT_POINT;
	m_RenderType	= FRT_ADD;
	m_TexFiltering  = TFO_BILINEAR;
	m_vCommonDir = ioMath::UNIT_Y;

	m_eSystemState = PSS_LIVE;

	m_iParticleQuota = 0;

	m_fBillMinWidth = m_fBillMaxWidth = 100.0f;
	m_fBillMinHeight = m_fBillMaxHeight = 100.0f;

	m_EndCondition = EC_NEVER_END;
	m_fCurTime = 0.0f;
	m_fEndTime = 0.0f;

	m_fCameraSqDist = 0.0f;

	m_vLinkPosOffset  = ioMath::VEC3_ZERO;
	m_qtLinkRotOffset = ioMath::QUAT_IDENTITY;

	m_FollowParentMode = FPT_NONE;
	m_iRandomSyncIndex = -1;	// Not Sync

	m_bAutoExtendPool = false;
	m_bCullIndividual = false;
	m_bDepthCheck = true;
	m_bModulate2X = false;
	m_bCommonDirNoneBillboard = false;

	m_iLimitQuotaRemain = 0;
	m_iLastVisibleFrame = 0;
	m_fUpdateRemainTime = 0.0f;

	m_fIterationInterval = 0.0f;
	m_iStartCreateParticleCnt = 0;

	m_fExtendColorRate = 1.0f;

	m_bIterationIntervalSet = false;
	m_bStartParticleCreated = false;
	m_bEachCulled = false;

	InitParameters();
}

ioParticleSystem::~ioParticleSystem()
{
	ClearActiveParticles();

	SAFEDELETE( m_pEmitter );
	RemoveAllAffector();

	SAFEDELETE( m_pMtl );
}

void ioParticleSystem::InitParameters()
{
	if( CreateParamDictionary( "ParticleSystem" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef( "quota", PT_INT ),
							 &m_QuotaCmd );

		pDict->AddParameter( ioParameterDef( "cull_each", PT_BOOL ),
							 &m_CullCmd );

		pDict->AddParameter( ioParameterDef( "follow_parent", PT_STRING ),
							 &m_FollowParentModeCmd );

		pDict->AddParameter( ioParameterDef( "default_width", PT_VECTOR2 ),
							 &m_DefaultWidthCmd );

		pDict->AddParameter( ioParameterDef( "default_height", PT_VECTOR2 ),
							 &m_DefaultHeightCmd );

		pDict->AddParameter( ioParameterDef( "render_type", PT_STRING ),
							 &m_RenderTypeCmd );

		pDict->AddParameter( ioParameterDef( "tex_filtering", PT_STRING ),
							 &m_TexFilteringCmd );

		pDict->AddParameter( ioParameterDef( "depth_check", PT_BOOL ),
							 &m_DepthCheckCmd );

		pDict->AddParameter( ioParameterDef( "texture", PT_STRING ),
							 &m_TextureNameCmd );

		pDict->AddParameter( ioParameterDef( "billboard_type", PT_STRING ),
							 &m_BillboardTypeCmd );

		pDict->AddParameter( ioParameterDef( "end_condition", PT_STRING ),
							 &m_EndConditionCmd );

		pDict->AddParameter( ioParameterDef( "limit_time", PT_FLOAT ),
							 &m_LimitTimeCmd );

		pDict->AddParameter( ioParameterDef( "common_dir", PT_VECTOR3 ),
							 &m_CommonDirCmd );

		pDict->AddParameter( ioParameterDef( "common_dir_none_billboard", PT_BOOL ),
							 &m_CommonDirNoneBillboardCmd );

		pDict->AddParameter( ioParameterDef( "iteration_interval", PT_FLOAT ),
							 &m_IterationIntervalCmd );

		pDict->AddParameter( ioParameterDef( "start_create_particles", PT_INT ),
							 &m_StartCreateParticleCnt );

		pDict->AddParameter( ioParameterDef( "link_biped_name", PT_STRING ),
							 &m_LinkBipedNameCmd );

		pDict->AddParameter( ioParameterDef( "pos_offset", PT_VECTOR3 ),
							 &m_LinkPosOffsetCmd );

		pDict->AddParameter( ioParameterDef( "rot_offset", PT_QUATERNION ),
							 &m_LinkRotOffsetCmd );

		pDict->AddParameter( ioParameterDef( "modulate2x", PT_BOOL ),
							 &m_Modulate2XCmd );

		pDict->AddParameter( ioParameterDef( "random_sync_index", PT_INT ),
							 &m_RandomSyncIndexCmd );
	}
}

void ioParticleSystem::ClearActiveParticles()
{
	ActiveParticleList::iterator iter=m_ActiveParticle.begin();
	for( ; iter!=m_ActiveParticle.end() ; ++iter )
	{
		delete *iter;
	}
	m_ActiveParticle.clear();
}

void ioParticleSystem::SetIterationInterval( float fInterval )
{
	m_fIterationInterval = fInterval;
	m_bIterationIntervalSet = true;
}

void ioParticleSystem::SetLimitMaxParticleQuota( bool bLimit, float fLimitRate )
{
	m_sbLimitFullParticleQuota = bLimit;
	m_sfLimitQuotaRate = fLimitRate;
}

void ioParticleSystem::Restart()
{
	ClearActiveParticles();

	if( m_pEmitter )
	{
		m_pEmitter->Restart();
	}

	std::for_each( m_AffectorList.begin(),
				   m_AffectorList.end(),
				   std::mem_fun(&ioParticleAffector::Restart) );

	m_eSystemState = PSS_LIVE;
	m_fCurTime = 0.0f;

	m_iLimitQuotaRemain = 0;
	m_iLastVisibleFrame = g_FrameTimer.GetFrameCounter();
	m_fUpdateRemainTime = 0.0f;

	m_bStartParticleCreated = false;
}

void ioParticleSystem::SetParent( ioEffect *pParent )
{
	m_pParent = pParent;

	m_iLastVisibleFrame = g_FrameTimer.GetFrameCounter();
}

ioParticleEmitter* ioParticleSystem::SetEmitter( const ioHashString &kType )
{
	SAFEDELETE( m_pEmitter );

	m_pEmitter = g_EffectFactory.CreateEmitter( kType );

	return m_pEmitter;
}

ioParticleAffector* ioParticleSystem::AddAffector( const ioHashString &kType )
{
	ioParticleAffector *pAff = g_EffectFactory.CreateAffector( kType );
	if( pAff )
	{
		m_AffectorList.push_back( pAff );
	}

	return pAff;
}

void ioParticleSystem::RemoveAffector( int iIdx )
{
	if( COMPARE( iIdx, 0, GetNumAffector() ) )
	{
		ParticleAffectorList::iterator iter = m_AffectorList.begin() + iIdx;

		delete *iter;
		m_AffectorList.erase( iter );
	}
}

void ioParticleSystem::RemoveAllAffector()
{
	ParticleAffectorList::iterator iter=m_AffectorList.begin();
	for( ; iter!=m_AffectorList.end() ; ++iter )
	{
		delete *iter;
	}

	m_AffectorList.clear();
}

ioParticleAffector* ioParticleSystem::GetAffector( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetNumAffector() ) )
		return m_AffectorList[iIdx];

	LOG.PrintTimeAndLog( 0, "ioParticleSystem::GetAffector - overflow(%d/%d)",
							 iIdx, GetNumAffector() );
	return NULL;
}

void ioParticleSystem::CreateStartParticles()
{
	if( m_bStartParticleCreated || m_iStartCreateParticleCnt <= 0 )
		return;

	int iCanEmitCount = GetParticleQuota() - GetNumActiveParticles();
	int iNewParticle = min( m_iStartCreateParticleCnt, iCanEmitCount );

	if( m_sbLimitFullParticleQuota && iNewParticle > 1 )
	{
		iNewParticle *= m_sfLimitQuotaRate;
		iNewParticle = max( 1, iNewParticle );
	}

	for( int i=0 ; i<iNewParticle ; i++ )
	{
		ioParticle *pParticle = AddParticle();
		if( pParticle )
		{
			ApplyStartParticle( pParticle, 0.0f );
		}
	}

	m_bStartParticleCreated = true;
}

void ioParticleSystem::ScaleActualSize( float fScaleRate )
{
	m_fBillMinWidth *= fScaleRate;
	m_fBillMaxWidth *= fScaleRate;

	m_fBillMinHeight *= fScaleRate;
	m_fBillMaxHeight *= fScaleRate;

	if( m_pEmitter )
		m_pEmitter->ScaleActualSize( fScaleRate );

	ParticleAffectorList::iterator iter=m_AffectorList.begin();
	for( ; iter!=m_AffectorList.end() ; ++iter )
	{
		(*iter)->ParentSystemScaled( fScaleRate );
	}
}

void ioParticleSystem::ChangeSystemDirection( const D3DXQUATERNION &qtRotate,
											  bool bTypeChange )
{
	if( bTypeChange && m_BillboardType == BBT_POINT )
		m_BillboardType = BBT_ORIENTED_COMMON;

	m_vCommonDir = qtRotate * m_vCommonDir;
	D3DXVec3Normalize( &m_vCommonDir, &m_vCommonDir );

	if( m_pEmitter )
	{
		m_pEmitter->RotateActualDirection( qtRotate );
	}

	ParticleAffectorList::iterator iter=m_AffectorList.begin();
	for( ; iter!=m_AffectorList.end() ; ++iter )
	{
		(*iter)->ParentSystemDirectionChanged( qtRotate );
	}
}

void ioParticleSystem::UpdateBounds( const D3DXMATRIX &rkInvParent )
{
	m_BoundBox.SetNull();

	if( m_ActiveParticle.empty() )
		return;

	const float INFINITY = std::numeric_limits<float>::infinity();

	D3DXVECTOR3 vMin( INFINITY, INFINITY, INFINITY );
	D3DXVECTOR3 vMax( -INFINITY, -INFINITY, -INFINITY );

	D3DXVECTOR3 vPos;
	ActiveParticleList::iterator iter = m_ActiveParticle.begin();
	for( ; iter!=m_ActiveParticle.end() ; ++iter )
	{
		vPos = (*iter)->m_vPos;

		vMin.x = min( vMin.x, vPos.x );
		vMin.y = min( vMin.y, vPos.y );
		vMin.z = min( vMin.z, vPos.z );

		vMax.x = max( vMax.x, vPos.x );
		vMax.y = max( vMax.y, vPos.y );
		vMax.z = max( vMax.z, vPos.z );
	}

	float fAdjust = max( m_fBillMaxWidth, m_fBillMaxHeight ) * FLOAT05;
	vMin.x -= fAdjust;	vMin.y -= fAdjust;	vMin.z -= fAdjust;
	vMax.x += fAdjust;	vMax.y += fAdjust;	vMax.z += fAdjust;
	m_BoundBox.SetMinMaxPos( vMin, vMax );

	if( m_FollowParentMode == FPT_NONE )
	{
		if( m_pParent->IsAttached() )
		{
			m_BoundBox.Transform( &rkInvParent );
		}
	}
}

ioParticle* ioParticleSystem::AddParticle()
{
	ioParticle *pNewParticle = new ioParticle;
	if( pNewParticle )
	{
		pNewParticle->SetBeginSize( GenBeginWidth(), GenBeginHeight() );
		m_ActiveParticle.push_back( pNewParticle );
	}

	return pNewParticle;
}

void ioParticleSystem::UpdateParentTransform()
{
	if( m_pParent->IsAttached() )
	{
		if( m_FollowParentMode == FPT_POS_ONLY )
		{
			D3DXVECTOR3 vScale = m_pParent->GetDerivedBipedScale( m_LinkBipedName );
			D3DXMatrixScaling( &m_matParentXform, vScale.x, vScale.y, vScale.z );

			D3DXVECTOR3 vPos = m_pParent->GetDerivedBipedPos( m_LinkBipedName, m_vLinkPosOffset );
			m_matParentXform._41 = vPos.x;
			m_matParentXform._42 = vPos.y;
			m_matParentXform._43 = vPos.z;
		}
		else
		{
			m_pParent->GetDerivedBipedTransform( m_LinkBipedName,
												 m_vLinkPosOffset,
												 m_qtLinkRotOffset,
												 &m_matParentXform );
		}
	}
	else
	{
		D3DXMatrixIdentity( &m_matParentXform );
	}
}

void ioParticleSystem::ExpireParticles( float fTime )
{
	ActiveParticleList::iterator iter=m_ActiveParticle.begin();
	while( iter != m_ActiveParticle.end() )
	{
		ioParticle *pParticle = *iter;
		if( pParticle->IsLive() )
		{
			pParticle->m_fCurTime += fTime;
			++iter;
		}
		else
		{
			iter = m_ActiveParticle.erase( iter );
			delete pParticle;
		}
	}
}

void ioParticleSystem::TriggerAffector( float fTime )
{
	float fTimePerSec = fTime * 0.001f;

	ParticleAffectorList::iterator iter;
	for( iter=m_AffectorList.begin() ; iter!=m_AffectorList.end() ; ++iter )
	{
		ioParticleAffector *pAffector = *iter;

		if( pAffector->IsEnable() )
		{
			pAffector->AffectParticle( this, fTimePerSec );
		}
	}
}

void ioParticleSystem::UpdateParticles( float fTime )
{
	float fTimePerSec = fTime * 0.001f;

	if( m_fExtendColorRate == 1.0f )
	{
		ActiveParticleList::iterator iter=m_ActiveParticle.begin();
		for( ; iter!=m_ActiveParticle.end() ; ++iter )
		{
			ioParticle *pParticle = *iter;
			pParticle->m_vPos += pParticle->m_vDir * ( pParticle->m_fMoveSpeed * fTimePerSec );
			pParticle->m_dwColor = m_pEmitter->GenEmissionColor( pParticle->TimeRate() );
		}
	}
	else
	{
		DWORD dwColor, dwAlpha;

		ActiveParticleList::iterator iter=m_ActiveParticle.begin();
		for( ; iter!=m_ActiveParticle.end() ; ++iter )
		{
			ioParticle *pParticle = *iter;

			pParticle->m_vPos += pParticle->m_vDir * ( pParticle->m_fMoveSpeed * fTimePerSec );

			dwColor = m_pEmitter->GenEmissionColor( pParticle->TimeRate() );
			dwAlpha = ( dwColor & 0xff000000 ) * m_fExtendColorRate;

			pParticle->m_dwColor = dwAlpha | ( dwColor & 0xffffff );
		}
	}
}

void ioParticleSystem::SetExtendColorRate( float fRate, bool bMinZero )
{
	if ( !bMinZero )
		m_fExtendColorRate = max( 0.1f, fRate );
	else
		m_fExtendColorRate = max( 0.0f, fRate );
}

int ioParticleSystem::GetNumSimulatedMaxParticles()
{
	if( !m_pEmitter )	return 0;

	float fMaxParticles = 0.0f;
	if( m_EndCondition == EC_TIME_LIMIT )
	{
		fMaxParticles = m_pEmitter->GetEmissionRate() * min( m_pEmitter->GetMaxLiveTime(), m_fEndTime );
	}
	else
	{
		fMaxParticles = m_pEmitter->GetEmissionRate() * m_pEmitter->GetMaxLiveTime();
	}

	fMaxParticles = ( fMaxParticles /FLOAT1000 + 1.0f ) + m_iStartCreateParticleCnt;

	return min( (int)fMaxParticles, m_iParticleQuota );
}

void ioParticleSystem::TriggerEmitter( float fTime )
{
	int iRequest = GetNewParticleCount( fTime );
	if( iRequest <= 0 )	return;

	float fTimePoint = 0.0f;	// 동시생성되는 파티클들의 시간차를 위해
	float fEmitTimeGap = ( fTime * 0.001f ) / (float)iRequest;

	for( int i=0 ; i<iRequest ; i++ )
	{
		ioParticle *pParticle = AddParticle();
		if( pParticle )
		{
			ApplyStartParticle( pParticle, fTimePoint );
		}

		fTimePoint += fEmitTimeGap;
	}
}

void ioParticleSystem::ApplyStartParticle( ioParticle *pParticle, float fTimePoint )
{
	m_pEmitter->InitParticle( pParticle );

	if( m_FollowParentMode == FPT_NONE )
	{
		D3DXVec3TransformCoord( &pParticle->m_vPos, &pParticle->m_vPos, &m_matParentXform );
		D3DXVec3TransformNormal( &pParticle->m_vDir, &pParticle->m_vDir, &m_matParentXform );
	}

	pParticle->m_vPos += pParticle->m_vDir * ( pParticle->m_fMoveSpeed * fTimePoint );

	if( m_BillboardType == BBT_ORIENTED_SELF )
	{
		float fAngle = ioMath::UnitRandom() * m_pEmitter->GetEmitAngle();
		D3DXVECTOR3 vDir = ioMath::RandomDeviant( fAngle, m_pEmitter->GetDirection() );

		if( m_FollowParentMode == FPT_NONE )
		{
			D3DXVec3TransformNormal( &vDir, &vDir, &m_matParentXform );
		}

		ioMath::CalcDirectionQuaternion( pParticle->m_qtRotate, vDir );
	}

	ParticleAffectorList::iterator iter;
	for( iter=m_AffectorList.begin() ; iter!=m_AffectorList.end() ; ++iter )
	{
		ioParticleAffector *pAffector = *iter;

		if( pAffector->IsEnable() )
		{
			pAffector->InitParticle( this, pParticle );
		}
	}
}

void ioParticleSystem::UpdateSystemState( float fTime )
{
	switch( m_eSystemState )
	{
	case PSS_LIVE:
		if( m_EndCondition == EC_TIME_LIMIT )
		{
			m_fCurTime += fTime;

			if( m_fCurTime >= m_fEndTime )
			{
				SetState( PSS_GRACE_CLOSE );
			}
		}
		else	// EC_NEVER_END
		{
			// EC_NEVER_END 인데 시작시만 파티클생성하고 계속 생성안하는 이펙트를 제거하기위한 궁여지책
			if( m_pEmitter->GetEmissionRate() < 0.1f && m_ActiveParticle.empty() )
			{
				SetState( PSS_END );
			}
		}
		break;
	case PSS_GRACE_CLOSE:
		if( m_ActiveParticle.empty() )
		{
			SetState( PSS_END );
		}
		break;
	}
}

int ioParticleSystem::GetNewParticleCount( float fTime )
{
	if( m_eSystemState != PSS_LIVE )
		return 0;

	int iRequest = m_pEmitter->GetCurEmitParticleCount( fTime );
	int iCanEmitCount = GetParticleQuota() - GetNumActiveParticles();
	iRequest = min( iRequest, iCanEmitCount );

	if( !m_sbLimitFullParticleQuota )
		return iRequest;

	m_iLimitQuotaRemain += iRequest;

	if( m_iLimitQuotaRemain > 0 )
	{
		iRequest = m_iLimitQuotaRemain * m_sfLimitQuotaRate;

		if( iRequest == 0 && GetNumActiveParticles() == 0 )	// 한개도 없는 경우
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

void ioParticleSystem::SetParticleQuota( int iQuota )
{
	m_iParticleQuota = iQuota;
}

void ioParticleSystem::SetDepthCheck( bool bCheck )
{
	m_bDepthCheck = bCheck;
}

void ioParticleSystem::SetModulate2X( bool bModulat2X )
{
	m_bModulate2X = bModulat2X;
}

void ioParticleSystem::SetBeginWidth( float fMin, float fMax )
{
	m_fBillMinWidth = min( fMin, fMax );
	m_fBillMaxWidth = max( fMin, fMax );
}

void ioParticleSystem::SetBeginHeight( float fMin, float fMax )
{
	m_fBillMinHeight = min( fMin, fMax );
	m_fBillMaxHeight = max( fMin, fMax );
}

float ioParticleSystem::GenBeginWidth() const
{
	return ioMath::RangeRandom( m_fBillMinWidth, m_fBillMaxWidth );
}

float ioParticleSystem::GenBeginHeight() const
{ 
	return ioMath::RangeRandom( m_fBillMinHeight, m_fBillMaxHeight );
}

void ioParticleSystem::SetRenderType( FXRenderType eType )
{
	m_RenderType = eType;
}

void ioParticleSystem::SetTexFiltering( TextureFilterOption eOption )
{
	m_TexFiltering = eOption;
}

void ioParticleSystem::SetTexture( const ioHashString &name )
{
	m_TextureName = name;
}

void ioParticleSystem::SetLinkBipedName( const ioHashString &name )
{
	m_LinkBipedName = name;
}

bool ioParticleSystem::IsEnableAtNextFrame( float fCurTimeGap )
{
	if( m_fCurTime + fCurTimeGap >= m_fEndTime )
		return false;

	return true;
}

void ioParticleSystem::SetRandomSeed( unsigned int uSeed )
{
	if( m_pEmitter )
	{
		m_pEmitter->SetEmitPosGenSeed( uSeed );
	}
}

unsigned int ioParticleSystem::GetRandomSeed() const
{
	if( m_pEmitter )
		return m_pEmitter->GetEmitPosGenSeed();

	return 0xffffffff;
}

void ioParticleSystem::BuildMaterial()
{
	SAFEDELETE( m_pMtl );

	m_pMtl = g_MaterialMgr.CreateMaterial();

	ioPass *pPass = m_pMtl->CreateTechnique()->CreatePass();
	pPass->SetDepthCheckEnable( m_bDepthCheck );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetAlphaTestSetting( D3DCMP_GREATER, 1 );

	switch( m_RenderType )
	{
	case FRT_ADD:
		pPass->SetSceneBlending( BLEND_ADD );
		break;
	case FRT_SRCALPHA:
		pPass->SetSceneBlending( BLEND_SRCALPHA );
		break;
	case FRT_SCREEN:
		pPass->SetSceneBlending( BLEND_SCREEN );
		break;
	case FRT_LIGHTEN:
		pPass->SetSceneBlending( BLEND_LIGHTEN );
		break;
	case FRT_DARKEN:
		pPass->SetSceneBlending( BLEND_DARKEN );
		break;
	case FRT_REV_SUBTRACT:
		pPass->SetSceneBlending( BLEND_REV_SUBTRACT );
		break;
	case FRT_SUBTRACT:
		pPass->SetSceneBlending( BLEND_SUBTRACT );
		break;
	}

	ioTextureUnitState *pState = pPass->CreateTextureUnitState( m_TextureName );
	pState->SetTextureAddressMode( TAM_CLAMP );
	pState->SetTextureFiltering( m_TexFiltering );

	if( m_bModulate2X )
		pState->SetColorStageBlendOp( D3DTOP_MODULATE2X );
	else
		pState->SetColorStageBlendOp( D3DTOP_MODULATE );

	m_pMtl->Load( EP_MANUAL );
}

void ioParticleSystem::BuildColorTable()
{
	if( m_pEmitter )
	{
		m_pEmitter->BuildColorTable();
	}
}

void ioParticleSystem::UpdateColorTable()
{
	if( m_pEmitter )
	{
		m_pEmitter->UpdateColorTable();
	}
}

bool ioParticleSystem::IsVisibleParticle( ioCamera *pCamera, ioParticle *pParticle )
{
	ioSphere kSphere;
	if( m_FollowParentMode == FPT_NONE )
	{
		kSphere.SetCenter( pParticle->m_vPos );
	}
	else
	{
		D3DXVECTOR3 vPos;
		D3DXVec3TransformCoord( &vPos, &pParticle->m_vPos, &m_matParentXform );
		kSphere.SetCenter( vPos );
	}

	kSphere.SetRadius( max( pParticle->m_fWidth * FLOAT05, pParticle->m_fHeight * FLOAT05 ) );

	return pCamera->IsVisible( kSphere );
}

void ioParticleSystem::GenBillboardAxis( const D3DXVECTOR3 &vCamDir,
									     D3DXVECTOR3 *pX,
									     D3DXVECTOR3 *pY,
										 ioParticle *pParticle )
{
	switch( m_BillboardType )
	{
	case BBT_POINT:
		pX->x = m_matViewInv._11;
		pX->y = m_matViewInv._12;
		pX->z = m_matViewInv._13;

		pY->x = m_matViewInv._21;
		pY->y = m_matViewInv._22;
		pY->z = m_matViewInv._23;
		break;
	case BBT_ORIENTED_COMMON:
		*pY = m_vCommonDir;

		if( !m_bCommonDirNoneBillboard )
		{
			D3DXVec3Cross( pX, pY, &vCamDir );
			D3DXVec3Normalize( pX, pX );
		}
		else
		{
			D3DXVECTOR3 vUp = ioMath::UNIT_Y;
			if( fabsf( D3DXVec3Dot( &m_vCommonDir, &vUp ) ) > 0.99f )
			{
				vUp = ioMath::UNIT_Z;
				D3DXVec3Cross( pX, &m_vCommonDir, &vUp );
			}
			else
			{
				D3DXVec3Cross( pX, &vUp, &m_vCommonDir );
			}

			D3DXVec3Normalize( pX, pX );
		}
		break;
	case BBT_ORIENTED_DIRECTION:
		*pX = pParticle->m_vDir;
		D3DXVec3Cross( pY, pX, &vCamDir );
		break;
	case BBT_ORIENTED_SELF:
		*pX = pParticle->m_qtRotate * ioMath::UNIT_X;
		*pY = pParticle->m_qtRotate * ioMath::UNIT_Y;
		break;
	}
}

void ioParticleSystem::GenVertex( const ioParticle *pParticle, ioVtxColorTex *pVtx )
{
	float fCosRotHalf = cosf( pParticle->m_fTexRotation ) * FLOAT05;
	float fSinRotHalf = sinf( pParticle->m_fTexRotation ) * FLOAT05;

	D3DXVECTOR3 vLeft, vTop;
	vLeft   = ( -pParticle->m_fWidth * FLOAT05 ) * m_vCameraX;
	vTop	= ( -pParticle->m_fHeight * FLOAT05 ) * m_vCameraY;

	pVtx[0].vPos    = pParticle->m_vPos + vTop + vLeft;
	pVtx[0].dwColor = pParticle->m_dwColor;
	pVtx[0].u = -fCosRotHalf - fSinRotHalf + FLOAT05;
	pVtx[0].v = -fSinRotHalf + fCosRotHalf + FLOAT05;

	pVtx[1].vPos    = pParticle->m_vPos + vTop - vLeft;
	pVtx[1].dwColor = pParticle->m_dwColor;
	pVtx[1].u =  fCosRotHalf - fSinRotHalf + FLOAT05;
	pVtx[1].v =  fSinRotHalf + fCosRotHalf + FLOAT05;

	pVtx[2].vPos    = pParticle->m_vPos - vTop + vLeft;
	pVtx[2].dwColor = pParticle->m_dwColor;
	pVtx[2].u = -fCosRotHalf + fSinRotHalf + FLOAT05;
	pVtx[2].v = -fSinRotHalf - fCosRotHalf + FLOAT05;

	pVtx[3].vPos    = pParticle->m_vPos - vTop - vLeft;
	pVtx[3].dwColor = pParticle->m_dwColor;
	pVtx[3].u =  fCosRotHalf + fSinRotHalf + FLOAT05;
	pVtx[3].v =  fSinRotHalf - fCosRotHalf + FLOAT05;
}

void ioParticleSystem::GenVertexParentXform( const ioParticle *pParticle, ioVtxColorTex *pVtx )
{
	float fCosRotHalf = cosf( pParticle->m_fTexRotation ) * FLOAT05;
	float fSinRotHalf = sinf( pParticle->m_fTexRotation ) * FLOAT05;

	D3DXVECTOR3 vLeft, vTop;
	vLeft   = ( -pParticle->m_fWidth * FLOAT05 ) * m_vCameraX;
	vTop	= ( -pParticle->m_fHeight * FLOAT05 ) * m_vCameraY;

	D3DXVECTOR3 vLocalPos;

	vLocalPos = pParticle->m_vPos + vTop + vLeft;
	D3DXVec3TransformCoord( &pVtx[0].vPos, &vLocalPos, &m_matParentXform );
	pVtx[0].dwColor = pParticle->m_dwColor;
	pVtx[0].u = -fCosRotHalf - fSinRotHalf + FLOAT05;
	pVtx[0].v = -fSinRotHalf + fCosRotHalf + FLOAT05;

	vLocalPos = pParticle->m_vPos + vTop - vLeft;
	D3DXVec3TransformCoord( &pVtx[1].vPos, &vLocalPos, &m_matParentXform );
	pVtx[1].dwColor = pParticle->m_dwColor;
	pVtx[1].u =  fCosRotHalf - fSinRotHalf + FLOAT05;
	pVtx[1].v =  fSinRotHalf + fCosRotHalf + FLOAT05;

	vLocalPos = pParticle->m_vPos - vTop + vLeft;
	D3DXVec3TransformCoord( &pVtx[2].vPos, &vLocalPos, &m_matParentXform );
	pVtx[2].dwColor = pParticle->m_dwColor;
	pVtx[2].u = -fCosRotHalf + fSinRotHalf + FLOAT05;
	pVtx[2].v = -fSinRotHalf - fCosRotHalf + FLOAT05;

	vLocalPos = pParticle->m_vPos - vTop - vLeft;
	D3DXVec3TransformCoord( &pVtx[3].vPos, &vLocalPos, &m_matParentXform );
	pVtx[3].dwColor = pParticle->m_dwColor;
	pVtx[3].u =  fCosRotHalf + fSinRotHalf + FLOAT05;
	pVtx[3].v =  fSinRotHalf - fCosRotHalf + FLOAT05;
}

void ioParticleSystem::Update( float fTime )
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

void ioParticleSystem::NotifyCurrentCamera( ioCamera *pCamera )
{
	m_iLastVisibleFrame = g_FrameTimer.GetFrameCounter();

	if( m_FollowParentMode == FPT_NONE )
	{
		m_matViewInv = *pCamera->GetViewInverseMatrix();
	}
	else
	{
		D3DXMatrixMultiply( &m_matViewInv,
							&m_matParentXform,
							pCamera->GetViewMatrix() );

		D3DXMatrixInverse( &m_matViewInv, NULL, &m_matViewInv );
	}

	m_VisibleArray.Clear();
	m_vCameraDir = pCamera->GetDirection();

	if( IsEachCulled() || m_ActiveParticle.empty() )
		return;

	m_VisibleArray.CheckMaxCount( m_ActiveParticle.size() );

	if( !m_bCullIndividual )
	{
		ActiveParticleList::iterator iter=m_ActiveParticle.begin();
		for( ; iter!=m_ActiveParticle.end() ; ++iter )
		{
			m_VisibleArray.Add( *iter );
		}
	}
	else
	{
		ActiveParticleList::iterator iter=m_ActiveParticle.begin();
		for( ; iter!=m_ActiveParticle.end() ; ++iter )
		{
			if( IsVisibleParticle( pCamera, *iter ) )
			{
				m_VisibleArray.Add( *iter );
			}
		}
	}
}

int ioParticleSystem::FillParticleBuffer( ioVtxColorTex *pDest )
{
	D3DXVECTOR3 vParentScale( 1.0f, 1.0f, 1.0f );

	ioNode *pNode = m_pParent->GetParentNode();
	if( pNode )
	{
		vParentScale = pNode->GetDerivedScale();
	}

	int iCurCount = m_VisibleArray.GetCount();
	ioParticle **pBase = m_VisibleArray.GetPtr();

	if( m_BillboardType == BBT_POINT ||	m_BillboardType == BBT_ORIENTED_COMMON )
	{
		GenBillboardAxis( m_vCameraDir, &m_vCameraX, &m_vCameraY );
		m_vCameraX = m_vCameraX * vParentScale;
		m_vCameraY = m_vCameraY * vParentScale;

		if( m_FollowParentMode == FPT_NONE )
		{
			for( int i=0 ; i<iCurCount ; i++ )
			{
				GenVertex( pBase[i], pDest );
				pDest += 4;
			}
		}
		else
		{
			for( int i=0 ; i<iCurCount ; i++ )
			{
				GenVertexParentXform( pBase[i], pDest );
				pDest += 4;
			}
		}
	}
	else
	{
		if( m_FollowParentMode == FPT_NONE )
		{
			for( int i=0 ; i<iCurCount ; i++ )
			{
				GenBillboardAxis( m_vCameraDir, &m_vCameraX, &m_vCameraY, pBase[i] );
				m_vCameraX = m_vCameraX * vParentScale;
				m_vCameraY = m_vCameraY * vParentScale;

				GenVertex( pBase[i], pDest );
				pDest += 4;
			}
		}
		else
		{
			for( int i=0 ; i<iCurCount ; i++ )
			{
				GenBillboardAxis( m_vCameraDir, &m_vCameraX, &m_vCameraY, pBase[i] );
				m_vCameraX = m_vCameraX * vParentScale;
				m_vCameraY = m_vCameraY * vParentScale;

				GenVertexParentXform( pBase[i], pDest );
				pDest += 4;
			}
		}
	}

	return GetNumVisibleParticles() * 4;
}

D3DXVECTOR3 ioParticleSystem::GetEmitPosition() const
{
	D3DXVECTOR3 vEmitPos = m_pEmitter->GetPosition();
	if( m_FollowParentMode == FPT_NONE )
	{
		D3DXVec3TransformCoord( &vEmitPos, &vEmitPos, &m_matParentXform );
	}

	return vEmitPos;
}

D3DXVECTOR3 ioParticleSystem::GetTrailBipedPosition( const ioHashString &rkName ) const
{
	if( m_pParent->IsAttached() )
	{
		ioSkeleton *pSkel = m_pParent->GetSkeleton();
		if( pSkel )
		{
			ioBiped *pBiped = pSkel->GetBiped( rkName );
			if( pBiped )
			{
				D3DXVECTOR3 vPos = pBiped->GetDerivedPosition();
				D3DXVec3TransformCoord( &vPos, &vPos, m_pParent->GetTransform() );
				return vPos;
			}
		}
	}

	return GetEmitPosition();
}

RenderGroupID ioParticleSystem::GetRenderQueueGroup() const
{
	if( m_pParent )
		return m_pParent->GetRenderQueueGroup();

	return RENDER_MAIN;
}

ioParticleIterator ioParticleSystem::GetIterator()
{
	return ioParticleIterator( m_ActiveParticle.begin(), m_ActiveParticle.end() );
}

bool ioParticleSystem::HasSameMaterialProperty( ioParticleSystem *pRhs )
{
	if( m_RenderType != pRhs->m_RenderType )
		return false;

	if( m_TexFiltering != pRhs->m_TexFiltering )
		return false;

	if( m_TextureName != pRhs->m_TextureName )
		return false;

	if( m_bDepthCheck != pRhs->m_bDepthCheck )
		return false;

	if( m_bModulate2X != pRhs->m_bModulate2X )
		return false;

	return true;
}

ioParticleSystem& ioParticleSystem::operator=( const ioParticleSystem &rhs )
{
	SAFEDELETE(m_pEmitter);
	RemoveAllAffector();

	ioParticleEmitter *pRhsEmit = rhs.GetEmitter();
	ioParticleEmitter *pNewEmit = SetEmitter( pRhsEmit->GetGeneratorType() );
	pNewEmit->ParametersCopy( pRhsEmit );
	pNewEmit->SetColorTable( pRhsEmit->GetColorTable() );

	int iNumAff = rhs.GetNumAffector();
	m_AffectorList.reserve( iNumAff );

	ioParticleAffector *pRhsAff, *pNewAff;
	for( int i=0 ; i<iNumAff ; i++ )
	{
		pRhsAff = rhs.GetAffector( i );

		if( pRhsAff )
		{
			pNewAff = AddAffector( pRhsAff->GetType() );
			pNewAff->ParametersCopy( pRhsAff );
		}
	}

	m_BillboardType = rhs.m_BillboardType;
	m_RenderType    = rhs.m_RenderType;
	m_TexFiltering  = rhs.m_TexFiltering;
	m_vCommonDir    = rhs.m_vCommonDir;

	m_fBillMinWidth	 = rhs.m_fBillMinWidth;
	m_fBillMaxWidth	 = rhs.m_fBillMaxWidth;
	m_fBillMinHeight = rhs.m_fBillMinHeight;
	m_fBillMaxHeight = rhs.m_fBillMaxHeight;

	m_EndCondition = rhs.m_EndCondition;
	m_fEndTime = rhs.m_fEndTime;

	m_LinkBipedName = rhs.m_LinkBipedName;
	m_vLinkPosOffset  = rhs.m_vLinkPosOffset;
	m_qtLinkRotOffset = rhs.m_qtLinkRotOffset;

	m_FollowParentMode = rhs.m_FollowParentMode;
	m_iRandomSyncIndex = rhs.m_iRandomSyncIndex;

	m_bAutoExtendPool = rhs.m_bAutoExtendPool;
	m_bCullIndividual = rhs.m_bCullIndividual;
	m_bDepthCheck = rhs.m_bDepthCheck;
	m_bModulate2X = rhs.m_bModulate2X;

	m_bCommonDirNoneBillboard = rhs.m_bCommonDirNoneBillboard;

	SetParticleQuota( rhs.GetParticleQuota() );
	SetTexture( rhs.GetTextureName() );

	m_fIterationInterval = rhs.m_fIterationInterval;
	m_iStartCreateParticleCnt = rhs.m_iStartCreateParticleCnt;

	m_bIterationIntervalSet = rhs.m_bIterationIntervalSet;

	return *this;
}

bool ioParticleSystem::operator==( const ioParticleSystem &rhs ) const
{
	if( !m_pEmitter || !rhs.m_pEmitter )
		return false;

	if( *m_pEmitter != *rhs.m_pEmitter )
		return false;

	if( m_AffectorList.size() != rhs.m_AffectorList.size() )
		return false;

	ParticleAffectorList::const_iterator iter, iRhs;
	iter = m_AffectorList.begin();
	iRhs = rhs.m_AffectorList.begin();
	for( ; iter!=m_AffectorList.end() ; ++iter, ++iRhs )
	{
		if( !(*iter)->IsSameAffector( *iRhs ) )
			return false;
	}

	if( m_TextureName != rhs.m_TextureName )
		return false;

	if( m_BillboardType != rhs.m_BillboardType )
		return false;

	if( m_RenderType != rhs.m_RenderType )
		return false;

	if( m_TexFiltering != rhs.m_TexFiltering )
		return false;

	if( !ioMath::IsEqual( m_vCommonDir, rhs.m_vCommonDir ) )
		return false;

	if( m_iParticleQuota != rhs.m_iParticleQuota )
		return false;

	if( m_fBillMinWidth != rhs.m_fBillMinWidth )
		return false;

	if( m_fBillMaxWidth != rhs.m_fBillMaxWidth )
		return false;

	if( m_fBillMinHeight != rhs.m_fBillMinHeight )
		return false;

	if( m_fBillMaxHeight != rhs.m_fBillMaxHeight )
		return false;

	if( m_EndCondition != rhs.m_EndCondition )
		return false;

	if( m_fEndTime != rhs.m_fEndTime )
		return false;

	if( m_LinkBipedName != rhs.m_LinkBipedName )
		return false;

	if( !ioMath::IsEqual( m_vLinkPosOffset, rhs.m_vLinkPosOffset ) )
		return false;

	if( !ioMath::IsEqual( m_qtLinkRotOffset, rhs.m_qtLinkRotOffset ) )
		return false;

	if( m_FollowParentMode != rhs.m_FollowParentMode )
		return false;

	if( m_iRandomSyncIndex != rhs.m_iRandomSyncIndex )
		return false;

	if( m_bAutoExtendPool != rhs.m_bAutoExtendPool )
		return false;

	if( m_bCullIndividual != rhs.m_bCullIndividual )
		return false;

	if( m_bDepthCheck != rhs.m_bDepthCheck )
		return false;

	if( m_bModulate2X != rhs.m_bModulate2X )
		return false;

	if( m_fIterationInterval != rhs.m_fIterationInterval )
		return false;

	if( m_iStartCreateParticleCnt != rhs.m_iStartCreateParticleCnt )
		return false;

	return true;
}

bool ioParticleSystem::operator!=( const ioParticleSystem &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioParticleSystem::CmdQuota::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetParticleQuota() );
}

void ioParticleSystem::CmdQuota::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetParticleQuota( ioStringConverter::ParseInt( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdCull::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetCullIndividual() );
}

void ioParticleSystem::CmdCull::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetCullIndividual( ioStringConverter::ParseBool( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdFollowParentMode::Get( const void *pTarget  ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >( pTarget );

	switch( pSys->GetFollowParentMode() )
	{
	case FPT_NONE:
		return "none";
	case FPT_POS_ONLY:
		return "pos_only";
	case FPT_ALL:
		return "all";
	}

	return "none";
}

void ioParticleSystem::CmdFollowParentMode::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem::FollowParentType eType = ioParticleSystem::FPT_NONE;
	if( kVal == "none" )
	{
		eType = ioParticleSystem::FPT_NONE;
	}
	else if( kVal == "pos_only" )
	{
		eType = ioParticleSystem::FPT_POS_ONLY;
	}
	else if( kVal == "all" )
	{
		eType = ioParticleSystem::FPT_ALL;
	}

	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);
	pSys->SetFollowParentMode( eType );
}

std::string ioParticleSystem::CmdDefaultWidth::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	D3DXVECTOR2 vSize;
	vSize.x = pSys->GetBillMinWidth();
	vSize.y = pSys->GetBillMaxWidth();

	return ioStringConverter::toString( vSize );
}

void ioParticleSystem::CmdDefaultWidth::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	D3DXVECTOR2 vSize = ioStringConverter::ParseVector2( kVal.c_str() );
	pSys->SetBeginWidth( vSize.x, vSize.y );
}

std::string ioParticleSystem::CmdDefaultHeight::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	D3DXVECTOR2 vSize;
	vSize.x = pSys->GetBillMinHeight();
	vSize.y = pSys->GetBillMaxHeight();

	return ioStringConverter::toString( vSize );
}

void ioParticleSystem::CmdDefaultHeight::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	D3DXVECTOR2 vSize = ioStringConverter::ParseVector2( kVal.c_str() );
	pSys->SetBeginHeight( vSize.x, vSize.y );
}

std::string ioParticleSystem::CmdTextureName::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return pSys->GetTextureName().c_str();
}

void ioParticleSystem::CmdTextureName::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetTexture( kVal.c_str() );
}

std::string ioParticleSystem::CmdBillboardType::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	switch( pSys->GetBillboardType() )
	{
	case ioParticleSystem::BBT_POINT:
		return "point";
	case ioParticleSystem::BBT_ORIENTED_COMMON:
		return "common";
	case ioParticleSystem::BBT_ORIENTED_DIRECTION:
		return "direction";
	case ioParticleSystem::BBT_ORIENTED_SELF:
		return "self";
	}

	return "point";
}

void ioParticleSystem::CmdBillboardType::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem::BillBoardType eType = ioParticleSystem::BBT_POINT;
	if( kVal == "point" )
	{
		eType = ioParticleSystem::BBT_POINT;
	}
	else if( kVal == "common" )
	{
		eType = ioParticleSystem::BBT_ORIENTED_COMMON;
	}
	else if( kVal == "direction" )
	{
		eType = ioParticleSystem::BBT_ORIENTED_DIRECTION;
	}
	else if( kVal == "self" )
	{
		eType = ioParticleSystem::BBT_ORIENTED_SELF;
	}

	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);
	pSys->SetBillboardType( eType );
}

std::string ioParticleSystem::CmdRenderType::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast<const ioParticleSystem* >(pTarget);

	switch( pSys->GetRenderType() )
	{
	case FRT_ADD:
		return "add";
	case FRT_SRCALPHA:
		return "src_alpha";
	case FRT_SCREEN:
		return "screen";
	case FRT_LIGHTEN:
		return "lighten";
	case FRT_DARKEN:
		return "darken";
	case FRT_REV_SUBTRACT:
		return "rev_subtract";
	case FRT_SUBTRACT:
		return "subtract";
	}

	return "add";
}

void ioParticleSystem::CmdRenderType::Set( void *pTarget, const std::string &kVal )
{
	FXRenderType eType = ioStringConverter::ParseFXRenderType( kVal.c_str() );

	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);
	pSys->SetRenderType( eType );
}

std::string ioParticleSystem::CmdEndCondition::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast<const ioParticleSystem* >(pTarget);
	
	static char szEncNeverEnd[10] = {'n','e','v','e','r','_','e','n','d',0};
	static char szEncTimeLimit[11] = {'t','i','m','e','_','l','i','m','i','t',0};

	switch( pSys->GetEndCondition() )
	{
	case ioParticleSystem::EC_NEVER_END:
		//UJ 140120, 보안강화: 인니 핵 대응
		//return "never_end";
		return szEncNeverEnd;
	case ioParticleSystem::EC_TIME_LIMIT:
		//UJ 140120, 보안강화: 인니 핵 대응
		//return "time_limit";
		return szEncTimeLimit;
	}

	//return "never_end";
	return szEncNeverEnd;
}

void ioParticleSystem::CmdEndCondition::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	//UJ 140120, 보안강화: 인니 핵 대응
	//if( kVal == "time_limit" )
	static char szEncTimeLimit[11] = {'t','i','m','e','_','l','i','m','i','t',0};
	if( strcmp( kVal.c_str(), szEncTimeLimit ) == 0 )
	{
		pSys->SetEndCondition( ioParticleSystem::EC_TIME_LIMIT );
	}
	else
	{
		pSys->SetEndCondition( ioParticleSystem::EC_NEVER_END );
	}
}

std::string ioParticleSystem::CmdLimitTime::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast<const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetEndTime() );
}

void ioParticleSystem::CmdLimitTime::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetEndTime( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdCommonDir::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetCommonDirection() );
}

void ioParticleSystem::CmdCommonDir::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetCommonDirection( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdCommonDirNoneBillboard::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetCommonDirNoneBillboard() );
}

void ioParticleSystem::CmdCommonDirNoneBillboard::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetCommonDirNoneBillboard( ioStringConverter::ParseBool( kVal.c_str() ) );
}

bool ioParticleSystem::CmdIterationInterval::IsSkipSave( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);
	if( pSys->GetIterationInterval() == 0.0f )
		return true;

	return false;
}

std::string ioParticleSystem::CmdIterationInterval::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetIterationInterval() );
}

void ioParticleSystem::CmdIterationInterval::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetIterationInterval( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdStartCreateParticleCnt::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetStartCreateParticleCnt() );
}

void ioParticleSystem::CmdStartCreateParticleCnt::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetStartCreateParticleCnt( ioStringConverter::ParseInt( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdDepthCheck::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetDepthCheck() );
}

void ioParticleSystem::CmdDepthCheck::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetDepthCheck( ioStringConverter::ParseBool( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdLinkBipedName::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	if( pSys->GetLinkBipedName().IsEmpty() )
		return "none";

	return pSys->GetLinkBipedName().c_str();
}

void ioParticleSystem::CmdLinkBipedName::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	if( kVal != "none" )
	{
		pSys->SetLinkBipedName( kVal.c_str() );
	}
	else
	{
		pSys->SetLinkBipedName( "" );
	}
}

std::string ioParticleSystem::CmdLinkPosOffset::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetLinkPosOffset() );
}

void ioParticleSystem::CmdLinkPosOffset::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);
	pSys->SetLinkPosOffset( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdLinkRotOffset::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetLinkRotOffset() );
}

void ioParticleSystem::CmdLinkRotOffset::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);
	pSys->SetLinkRotOffset( ioStringConverter::ParseQuaternion( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdModulate2X::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetModulate2X() );
}

void ioParticleSystem::CmdModulate2X::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetModulate2X( ioStringConverter::ParseBool( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdRandomSyncIndex::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	return ioStringConverter::toString( pSys->GetRandomSyncIndex() );
}

void ioParticleSystem::CmdRandomSyncIndex::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetRandomSyncIndex( ioStringConverter::ParseInt( kVal.c_str() ) );
}

std::string ioParticleSystem::CmdTexFiltering::Get( const void *pTarget ) const
{
	const ioParticleSystem *pSys = static_cast< const ioParticleSystem* >(pTarget);

	switch( pSys->GetTexFiltering() )
	{
	case TFO_NEAREST:
		return "none";
	case TFO_BILINEAR:
		return "bilinear";
	case TFO_TRILINEAR:
		return "trilinear";
	case TFO_ANISOTROPIC:
		return "anisotropic";
	}

	return "bilinear";
}

void ioParticleSystem::CmdTexFiltering::Set( void *pTarget, const std::string &kVal )
{
	ioParticleSystem *pSys = static_cast< ioParticleSystem* >(pTarget);

	pSys->SetTexFiltering( ioStringConverter::ParseTextureFilterOption( kVal.c_str() ) );
}

void ioParticleSystem::TranslateParticle( const D3DXVECTOR3 &vMove )
{
	if( m_FollowParentMode == FPT_NONE )
	{
		ActiveParticleList::iterator iter=m_ActiveParticle.begin();
		for( ; iter!=m_ActiveParticle.end() ; ++iter )
		{
			ioParticle *pParticle = *iter;
			pParticle->m_vPos += vMove;
		}
	}
}
