

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioChargingFireSkill5.h"

#include "WeaponDefine.h"

ioChargingFireSkill5::ioChargingFireSkill5()
{
	ClearData();
}

ioChargingFireSkill5::ioChargingFireSkill5( const ioChargingFireSkill5 &rhs )
: ioNormalSkill( rhs ),
 m_bDirection( rhs.m_bDirection ),
 m_szNormalDelayAnimation( rhs.m_szNormalDelayAnimation ),
 m_szNormalMoveAnimation( rhs.m_szNormalMoveAnimation ),
 m_dwNormalDuration( rhs.m_dwNormalDuration ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_iTotalChargingCount( rhs.m_iTotalChargingCount ),
 m_ChargingInfoList( rhs.m_ChargingInfoList ),
 m_szLoopMotion( rhs.m_szLoopMotion ),
 m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_fLoopMotionRate( rhs.m_fLoopMotionRate )
{
	ClearData();
}

ioChargingFireSkill5::~ioChargingFireSkill5()
{
}

void ioChargingFireSkill5::ClearData()
{
	m_State = SS_PREDELAY;

	m_bSetHandMesh = false;

	m_dwNormalStartTime		= 0;
	m_dwAttackEndTime		= 0;
	m_dwFireStartTime		= 0;
	m_dwFireEndTime			= 0;
	m_dwMotionEndTime		= 0;
	m_dwCurSkillProtectTime	= 0;

	m_iCurChargingPhase = 0;
	m_fTotalReduceGauge	= 0.0f;
	m_fLimitReduceGauge	= 0.0f;

	m_bSetMoveAni = false;
}

void ioChargingFireSkill5::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	// 애니메이션
	m_bDirection = rkLoader.LoadBool_e( "direction", false );
	rkLoader.LoadString_e( "normal_delay_animation", "", szBuf, MAX_PATH );	m_szNormalDelayAnimation	= szBuf;
	rkLoader.LoadString_e( "normal_move_animation", "", szBuf, MAX_PATH );	m_szNormalMoveAnimation		= szBuf;
	m_dwNormalDuration		= (DWORD)rkLoader.LoadInt_e( "normal_duration", 0 );
	m_dwSkillProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_ChargingInfoList.clear();
	m_iTotalChargingCount = rkLoader.LoadInt_e( "charging_time_cnt", 0 );
	for( int i=0; i<m_iTotalChargingCount; ++i )
	{
		CHARGING_INFO kInfo;
		wsprintf_e( szKey, "charging_time%d", i+1 );						kInfo.dwChargingTime = (DWORD)rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "charging_time%d_reduce_gauge_rate", i+1 );	kInfo.fReduceGaugeRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		wsprintf_e( szKey, "charging_time%d_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );				kInfo.szEffectName = szBuf;
		wsprintf_e( szKey, "charging_time%d_weapon_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );				kInfo.szWeaponEffectName = szBuf;
		wsprintf_e( szKey, "charging_time%d_attack_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );				kInfo.szAttackEffectName = szBuf;
		wsprintf_e( szKey, "charging_time%d_attack", i+1 );
		kInfo.AttackAttribute.Init();
		LoadAttackAttribute( szKey, kInfo.AttackAttribute, rkLoader );
		m_ChargingInfoList.push_back( kInfo );
	}

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szLoopMotion = szBuf;
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_fLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
}

ioSkill* ioChargingFireSkill5::Clone()
{
	return new ioChargingFireSkill5( *this );
}

bool ioChargingFireSkill5::OnSkillStart( ioBaseChar *pChar )
{
	for( int i=0; i<m_iTotalChargingCount; ++i )
	{
		if( m_ChargingInfoList[i].AttackAttribute.m_AttackAnimation.IsEmpty() )
			return false;
	}

	ClearData();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	m_dwSkillStartTime = FRAMEGETTIME();

	// 스킬게이지 최대 감소량을 정한다.
	if( GetOwnerItem() )
	{
		float fTotalReduce = 0.0f;
		for( int i=0; i<m_iTotalChargingCount; ++i )
		{
			fTotalReduce += m_ChargingInfoList[i].fReduceGaugeRate;
			m_ChargingInfoList[i].dwWeaponEffectID = -1;	// 이펙트 ID 초기화
			m_ChargingInfoList[i].dwAttackEffectID = -1;	// 이펙트 ID 초기화
		}
		float fMaxGauge = GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() );
		m_fLimitReduceGauge = fMaxGauge / 100 * fTotalReduce;
	}

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar );
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioChargingFireSkill5::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	//DestroyMapEffect
	for( int i=0; i<m_iTotalChargingCount; ++i )
	{
		if( m_ChargingInfoList[i].dwWeaponEffectID != -1 )
		{
			pChar->EndEffect( m_ChargingInfoList[i].dwWeaponEffectID, true );
			m_ChargingInfoList[i].dwWeaponEffectID = -1;
		}
	}

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	ioBombMissileWeapon2 *pBombM = ToBombMissileWeapon2( pWeapon );
	if( pBombM && pBombM->IsLive() )
	{
		pBombM->SetWeaponDead();
	}

	ClearData();

	pChar->SetSkillMoveEnable( false, false );
}

void ioChargingFireSkill5::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_PREDELAY:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckNormalMoveAni( pChar );
			CheckCurChargingPhase( pChar, pStage );

			/*if( m_dwNormalDuration > 0 && m_dwNormalStartTime+m_dwNormalDuration < dwCurTime )
			{
				SetActionState( pChar, pStage, false );
				m_dwNormalDuration = 0;
				return;
			}*/

			if( pChar->IsNeedProcess() && pChar->IsAttackKey())
			{
				SetActionState( pChar, pStage, true );
				return;
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime &&
				m_ChargingInfoList[ m_iCurChargingPhase-1 ].dwAttackEffectID == -1 )
			{
				ioEffect *pEffect = pChar->AttachEffect( m_ChargingInfoList[ m_iCurChargingPhase-1 ].szAttackEffectName );
				if( pEffect )
				{
					m_ChargingInfoList[ m_iCurChargingPhase-1 ].dwAttackEffectID = pEffect->GetUniqueID();
				}
			}

			if( m_dwFireEndTime > 0 && m_dwFireEndTime < dwCurTime )
			{
				m_dwFireEndTime = 0;
				SetLoopAfterActionState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP_AFTER_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar );
			}
			else if( !CheckControlWeapon( pChar ) )
			{
				SetEndState( pChar );
			}
		}
		break;
	}
}


void ioChargingFireSkill5::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioBaseChar *pChar = pWeapon->GetOwner();

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

void ioChargingFireSkill5::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_ACTION:
		{
			rkPacket >> m_iCurChargingPhase;
			SetActionState( pChar, pStage, false );
		}
		break;
	}
}


bool ioChargingFireSkill5::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioChargingFireSkill5::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	switch( m_State )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
		break;
	}

	return false;
}

bool ioChargingFireSkill5::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	if( m_State == SS_END )
		return true;

	return false;
}

bool ioChargingFireSkill5::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioChargingFireSkill5::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioChargingFireSkill5::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;

	pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() ) );
	m_bReduceGauge = true;
}

void ioChargingFireSkill5::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	EndPreDelayEffect( pChar );

	m_bSetMoveAni = false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_LOOP;
	m_dwNormalStartTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szNormalDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	pChar->SetSkillMoveEnable( true, m_bDirection );
}

void ioChargingFireSkill5::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();

	if( m_State == SS_ACTION || pGrp == NULL || ((int)m_ChargingInfoList.size() <= m_iCurChargingPhase-1) )
		return;

	SetNormalSkill( pChar );

	DWORD dwCurTime = FRAMEGETTIME();
	m_State = SS_ACTION;

	AttackAttribute	CurrentAttribute;
	CurrentAttribute.Init();
	CurrentAttribute = m_ChargingInfoList[m_iCurChargingPhase-1].AttackAttribute;

	int iAniID = pGrp->GetAnimationIdx( CurrentAttribute.m_AttackAnimation );
	float fAniRate = CurrentAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = CurrentAttribute.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( iAniID != -1 )
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	// Attack
	if( !CurrentAttribute.m_vWeaponInfoList.empty() )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											CurrentAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fAniRate,
											dwPreDelay );
	}

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	float fFireStart = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" );
	if( fFireStart > 0.0f )
	{
		m_dwFireStartTime = dwCurTime + fFireStart + dwPreDelay;
	}

	float fFireEnd = pGrp->GetAnimationEventTime_e( iAniID, "fire_e" ) * fAniRate;
	if( fFireEnd > 0.0f )
	{
		m_dwFireEndTime = dwCurTime + fFireEnd + dwPreDelay;
	}

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fAniRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd + dwPreDelay;
	else
		m_dwAttackEndTime = 0;

	pChar->SetReservedSliding( CurrentAttribute.m_vForceInfoList, iAniID, fAniRate, dwPreDelay );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << m_iCurChargingPhase;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pChar->SetSkillMoveEnable( false, false );
}

void ioChargingFireSkill5::SetLoopAfterActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;
	if( m_szLoopMotion.IsEmpty() )	return;

	m_State = SS_LOOP_AFTER_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopMotion );

	if( iAniID == -1 )
		return;

	float fAniRate = m_fLoopMotionRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

    pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + m_dwLoopDuration * fAniRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;
}

void ioChargingFireSkill5::SetEndState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	m_State = SS_END;

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

void ioChargingFireSkill5::CheckCurChargingPhase( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD CurTime = dwCurTime - m_dwNormalStartTime;

	for( int i=0; i<m_iTotalChargingCount; ++i )
	{
		DWORD dwPreTime = 0;
		DWORD dwAfterTime = 0;

		if( i != 0 )
			dwPreTime = m_ChargingInfoList[i-1].dwChargingTime;

		dwAfterTime = m_ChargingInfoList[i].dwChargingTime;

		if( COMPARE( CurTime, dwPreTime, dwAfterTime ) )
		{
			if( m_iCurChargingPhase != i+1 )
			{
				pChar->AttachEffect( m_ChargingInfoList[i].szEffectName );
			}
			m_iCurChargingPhase = i+1;
			if( m_iCurChargingPhase >= m_iTotalChargingCount )
				m_iCurChargingPhase = m_iTotalChargingCount;
		}
	}

	// 게이지 감소량 체크
	float	fMaxGauge = GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() );
	int		iCurPhase = m_iCurChargingPhase-1;
	DWORD	dwGapTime = m_ChargingInfoList[iCurPhase].dwChargingTime;
	if( iCurPhase > 0 )
		dwGapTime -= m_ChargingInfoList[iCurPhase-1].dwChargingTime;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = ( fTimePerSec * 1000 ) / dwGapTime;
	float fReduce =  ( fMaxGauge / 100 * m_ChargingInfoList[iCurPhase].fReduceGaugeRate ) * fRate;
	m_fTotalReduceGauge += fReduce;

	if( m_fTotalReduceGauge < m_fLimitReduceGauge )
	{
		pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fReduce );
	}

	for( int i=0; i<m_iTotalChargingCount; ++i )
	{
		D3DXVECTOR3	vPos = pChar->GetWorldPosition();
		D3DXVECTOR3	vScale = pChar->GetWorldScale();
		if( i == iCurPhase )
		{
			if( m_ChargingInfoList[i].dwWeaponEffectID == -1 )
			{
				// 이펙트 생성
				
				ioEffect *pEffect = pChar->AttachEffect( m_ChargingInfoList[i].szWeaponEffectName );
				if( pEffect )
				{
					m_ChargingInfoList[i].dwWeaponEffectID = pEffect->GetUniqueID();
				}
			}
		}
		else
		{
			if( m_ChargingInfoList[i].dwWeaponEffectID != -1 )
			{
				pChar->EndEffect( m_ChargingInfoList[i].dwWeaponEffectID, false );
				m_ChargingInfoList[i].dwWeaponEffectID = -1;
			}
		}
	}
}

void ioChargingFireSkill5::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{
		if( !m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_szNormalMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_szNormalDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

bool ioChargingFireSkill5::CheckControlWeapon( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToBombMissileWeapon2( pWeapon ) )
		return true;

	return false;
}

bool ioChargingFireSkill5::IsWeaponControl( ioBaseChar *pChar )
{
	switch( m_State )
	{
	case SS_ACTION:
	case SS_LOOP_AFTER_ACTION:
	case SS_END:
		if( CheckControlWeapon( pChar ) )
			return true;
		break;
	}

	return false;
}

const D3DXVECTOR3& ioChargingFireSkill5::GetWeaponPos( ioBaseChar *pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	if( m_dwWeaponBaseIndex < 0 )
		return pChar->GetWorldPosition();

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	ioBombMissileWeapon2 *pBombM = ToBombMissileWeapon2( pWeapon );

	if( pBombM )
	{
		return pBombM->GetPosition();
	}

	return pChar->GetWorldPosition();
}