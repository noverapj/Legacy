
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioConsecution5Skill.h"

ioConsecution5Skill::ioConsecution5Skill()
{
	Init();
}

ioConsecution5Skill::ioConsecution5Skill( const ioConsecution5Skill &rhs )
	: ioAttackSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_bDisableGrowth( rhs.m_bDisableGrowth ),
	  m_fGaugePerTic( rhs.m_fGaugePerTic ),
	  m_dwTicTime( rhs.m_dwTicTime ),
	  m_szTargetMarker( rhs.m_szTargetMarker ),
	  m_szResultMarker( rhs.m_szResultMarker ),
	  m_fTargetMarkerOffset( rhs.m_fTargetMarkerOffset ),
	  m_fTargetMarkerMaxOffset( rhs.m_fTargetMarkerMaxOffset ),
	  m_fTargetMarkerMoveSpeed( rhs.m_fTargetMarkerMoveSpeed ),
	  m_szSkillLoopMotion( rhs.m_szSkillLoopMotion ), 
	  m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
	  m_SkillMotionFr( rhs.m_SkillMotionFr ),
	  m_SkillMotionBk( rhs.m_SkillMotionBk ),
	  m_SkillMotionLt( rhs.m_SkillMotionLt ),
	  m_SkillMotionRt( rhs.m_SkillMotionRt ),
	  m_fSkillMotionRate( rhs.m_fSkillMotionRate ),
	  m_nMaxAttackCnt( rhs.m_nMaxAttackCnt ),
	  m_vAttackList( rhs.m_vAttackList ),
	  m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
	  m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
	  m_bSetUseCount( rhs.m_bSetUseCount )
{
	Init();
}

ioConsecution5Skill::~ioConsecution5Skill()
{
}

void ioConsecution5Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGaugePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "target_marker", "", szBuf, MAX_PATH );
	m_szTargetMarker = szBuf;

	m_fTargetMarkerOffset = rkLoader.LoadFloat_e( "target_marker_offset", 0.0f );
	m_fTargetMarkerMaxOffset = rkLoader.LoadFloat_e( "target_marker_max_offset", 0.0f );
	m_fTargetMarkerMoveSpeed = rkLoader.LoadFloat_e( "target_marker_move_speed", 0.0f );

	rkLoader.LoadString_e( "result_target_marker", "", szBuf, MAX_PATH );
	m_szResultMarker = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szSkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_motion_fr", "", szBuf, MAX_PATH );
	m_SkillMotionFr = szBuf;

	rkLoader.LoadString_e( "skill_motion_bk", "", szBuf, MAX_PATH );
	m_SkillMotionBk = szBuf;

	rkLoader.LoadString_e( "skill_motion_lt", "", szBuf, MAX_PATH );
	m_SkillMotionLt = szBuf;

	rkLoader.LoadString_e( "skill_motion_rt", "", szBuf, MAX_PATH );
	m_SkillMotionRt = szBuf;

	m_fSkillMotionRate = rkLoader.LoadFloat_e( "skill_motion_rate", FLOAT1 );

	m_vAttackList.clear();
	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 0 );
	for ( int i=0; i<m_nMaxAttackCnt; i++ )
	{
		char szBuf[MAX_PATH] = "";
		wsprintf( szBuf, "attack%d_info", i+1 );

		AttackAttribute sAttackInfo;
		LoadAttackAttribute( szBuf, sAttackInfo, rkLoader );
		m_vAttackList.push_back( sAttackInfo );
	}

	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_bSetUseCount = rkLoader.LoadBool_e( "set_use_count", false );
}

ioSkill* ioConsecution5Skill::Clone()
{
	return new ioConsecution5Skill( *this );
}

bool ioConsecution5Skill::OnSkillStart( ioBaseChar *pChar )
{
	Init();

	m_bUseActiveCnt = m_bSetUseCount;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;
			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue( pStage );

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue( pStage );			
		}
	}

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	return true;
}

void ioConsecution5Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = m_fGaugePerTic / m_fTicGaugeRateByGrowth;
	int nCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_None:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetMoveState( pChar, pStage );
			}
		}
		break;
	case SS_Move:
		{
			if ( pChar->IsNeedProcess() )
			{
				if( m_dwCheckGaugeTime + nCurTicTime <= dwCurTime )
				{
					m_dwCheckGaugeTime = dwCurTime;

					if( fCurTicGauge != 0.0f )
					{
						if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
						{
							SetEndState( pChar );
							return;
						}
					}
				}
			}

			CheckMoveAni( pChar, false );
			UpdateTargetMarker( pChar, pStage );
			CheckInputKey( pChar, pStage );
		}
		break;
	case SS_Action:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
					SetMoveState( pChar, pStage );
				else
					SetEndState( pChar );
				return;
			}
		}
		break;
	}
}

void ioConsecution5Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;
	
	pChar->SetSkillMoveEnable( false, false );

	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
		DestroyEffect( pStage, m_dwTargetMarkerID );

	m_bUseActiveCnt = false;

	ioItem *pOwnerItem = GetOwnerItem();
	if ( pOwnerItem )
		pChar->SetCurItemSkillGauge( pOwnerItem->GetName(), 0.0f );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioConsecution5Skill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}
	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwPreDelayStartTime == 0 )
	{		
		if( m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioConsecution5Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pChar ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_None:
	case SS_Move:
	case SS_Action:
		return false;
	case SS_End:
		if( m_fSkillEndNextJumpAmt == 0.0f )
			return true;
		else
			return false;
	}

	if( m_dwMotionEndTime + dwActionStop >= FRAMEGETTIME() )
		return false;

	return true;
}

void ioConsecution5Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage ) 
		return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case SS_Action:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pChar->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_fCurTargetMarkerOffset;
			rkPacket >> m_nCurAttackCnt;

			rkPacket >> m_vCurWeaponMoveDir;
			rkPacket >> m_fCurWeaponMoveSpeed;
			rkPacket >> m_fCurWeaponFloatPower;

			if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
				SetAttackState( pChar, pStage, m_vAttackList[m_nCurAttackCnt] );
		}
		break;
	case SS_End:
		m_SkillState = SS_End;
		break;
	}
}

void ioConsecution5Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) 
		return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	pWeapon->SetMoveDir( m_vCurWeaponMoveDir );
	pWeapon->SetMoveSpeed( m_fCurWeaponMoveSpeed );
	pWeapon->SetFloatPower( m_fCurWeaponFloatPower );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioConsecution5Skill::Init()
{
	m_dwCurSkillProtectTime = 0;
	m_dwCheckGaugeTime = 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_bSetHandMesh = false;

	m_SkillState = SS_None;	
	m_StartDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetDelay = false;

	m_dwTargetMarkerID = -1;
	m_fCurTargetMarkerOffset = 0.0f;
	m_nCurAttackCnt = 0;

	m_vCurWeaponMoveDir = ioMath::VEC3_ZERO;
	m_fCurWeaponFloatPower = 0;
	m_fCurWeaponMoveSpeed = 0;
	m_bUseActiveCnt = false;
}

void ioConsecution5Skill::SetMoveState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )	
		return;
		
	pChar->ChangeDirectionExplicitIfDirChanged( true );
	pChar->SetSkillMoveEnable( true, false );
	m_StartDirKey = pChar->GetCurDirKey();
	m_dwCheckGaugeTime = FRAMEGETTIME();
	m_SkillState = SS_Move;
	m_bSetDelay = false;
	m_fCurTargetMarkerOffset = m_fTargetMarkerOffset;

	CheckMoveAni( pChar, true );

	if( pChar->IsNeedProcess() && !m_szTargetMarker.IsEmpty() && m_dwTargetMarkerID == -1 )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * m_fCurTargetMarkerOffset );
		vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );

		m_dwTargetMarkerID = CreateEffect( pStage, m_szTargetMarker, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	}
}

void ioConsecution5Skill::SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage, const AttackAttribute &sAttribute )
{
	if( !pChar || !pStage )	
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sAttribute.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = sAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = sAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pChar->RefreshFireTimeList( nAniID, sAttribute.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( sAttribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	DestroyEffect( pStage, m_dwTargetMarkerID );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * m_fCurTargetMarkerOffset );
	vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
	CreateEffect( pStage, m_szResultMarker, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	pChar->SetSkillMoveEnable( false, false );
	m_SkillState = SS_Action;
	
	if ( pChar->IsNeedProcess())
		SetWeaponFloatValue( pChar, pStage, vPos );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_Action;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << m_fCurTargetMarkerOffset;
		kPacket << m_nCurAttackCnt;
		kPacket << m_vCurWeaponMoveDir;
		kPacket << m_fCurWeaponMoveSpeed;
		kPacket << m_fCurWeaponFloatPower;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	

	m_nCurAttackCnt++;
}

void ioConsecution5Skill::SetEndState( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	m_SkillState = SS_End;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioConsecution5Skill::CheckMoveAni( ioBaseChar *pChar, bool bFirst )
{
	if( !pChar ) 
		return;

	if( pChar->IsSettedDirection() )
	{
		if( pChar->IsChangeDirection() || bFirst )
		{	
			ioEntityGroup *pGrp = pChar->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			m_bSetDelay = false;

			ioUserKeyInput::DirKeyInput eXformDir = pChar->GetXformDir( m_StartDirKey );
			float fRate = m_fSkillMotionRate;

			switch( eXformDir )
			{
			case ioUserKeyInput::DKI_UP:
				pGrp->SetLoopAni( m_SkillMotionFr, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_RIGHTUP:
			case ioUserKeyInput::DKI_RIGHT:
			case ioUserKeyInput::DKI_RIGHTDOWN:
				pGrp->SetLoopAni( m_SkillMotionRt, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_DOWN:
				pGrp->SetLoopAni( m_SkillMotionBk, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			case ioUserKeyInput::DKI_LEFTDOWN:
			case ioUserKeyInput::DKI_LEFT:
			case ioUserKeyInput::DKI_LEFTUP:
				pGrp->SetLoopAni( m_SkillMotionLt, FLOAT100, FLOAT1, FLOAT1/fRate );
				break;
			}
		}
	}
	else
	{		
		if( !m_bSetDelay )
		{
			ioEntityGroup *pGrp = pChar->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szSkillLoopMotion, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopMotionRate );

			m_bSetDelay = true;
		}
	}
}

void ioConsecution5Skill::CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	if ( pChar->IsAttackKeyDown() )
	{
		m_fCurTargetMarkerOffset += (m_fTargetMarkerMoveSpeed * g_FrameTimer.GetSecPerFrame());
		m_fCurTargetMarkerOffset = min( m_fCurTargetMarkerOffset, m_fTargetMarkerMaxOffset );
	}
	else if ( pChar->IsAttackKeyRelease() )
	{
		if (COMPARE( m_nCurAttackCnt, 0, m_nMaxAttackCnt ) )
			SetAttackState( pChar, pStage, m_vAttackList[m_nCurAttackCnt] );
		else
			SetEndState( pChar );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioConsecution5Skill::CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pStage || szEffect.IsEmpty() )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioConsecution5Skill::DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID )
{
	if ( !pStage || dwEffectID == -1 )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioConsecution5Skill::UpdateTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage || m_dwTargetMarkerID == -1 )
		return;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( pChar->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * m_fCurTargetMarkerOffset );
	vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTargetMarkerID );
	if( pMapEffect )
		pMapEffect->SetWorldPosition( vPos );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioConsecution5Skill::SetWeaponFloatValue( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vTargetPos )
{
	if ( !pChar || !pStage )
		return;

	D3DXVECTOR3 vDir = vTargetPos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );
	m_vCurWeaponMoveDir = vDir;

	m_fCurWeaponFloatPower = 0.0f;
	m_fCurWeaponMoveSpeed = 0.0f;

	if( fRange > 0.0f )
	{
		float fRate = fRange / m_fTargetMarkerMaxOffset;

		m_fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		m_fCurWeaponFloatPower = max( m_fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fWeaponMoveSpeed = 0.0f;
		float fFlightDuration = GetFlightDuration( pStage->GetRoomGravity(), m_fCurWeaponFloatPower );
		if( fFlightDuration > 0.0f )
			fWeaponMoveSpeed = m_fTargetMarkerMaxOffset / (fFlightDuration / FLOAT1000);

		m_fCurWeaponMoveSpeed = fWeaponMoveSpeed * fRate;
	}
}

float ioConsecution5Skill::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower )
{
	float fTopTime = 0.0f; 
	float fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}
	return 2 * fTopTime * FLOAT1000;
}

bool ioConsecution5Skill::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioConsecution5Skill::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	return m_nMaxAttackCnt;
}

int ioConsecution5Skill::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	return m_nCurAttackCnt;
}