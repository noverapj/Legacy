

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingAttackSkill.h"

ioGrapplingAttackSkill::ioGrapplingAttackSkill()
{
	ClearData();
}

ioGrapplingAttackSkill::ioGrapplingAttackSkill( const ioGrapplingAttackSkill &rhs )
: ioAttackSkill( rhs ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_fTargetGapRange( rhs.m_fTargetGapRange ),
m_fDownTargetGapRange( rhs.m_fDownTargetGapRange ),
m_fCharHeightRate( rhs.m_fCharHeightRate ),
m_szStandAttack_lt( rhs.m_szStandAttack_lt ),
m_szStandAttack_rt( rhs.m_szStandAttack_rt ),
m_szDownAttack_lt( rhs.m_szDownAttack_lt ),
m_szDownAttack_rt( rhs.m_szDownAttack_rt ),
m_vStandAttack( rhs.m_vStandAttack ),
m_vStandFinishAttack( rhs.m_vStandFinishAttack ),
m_vDownAttack( rhs.m_vDownAttack ),
m_vDownFinishAttack( rhs.m_vDownFinishAttack ),
m_dwFastValue( rhs.m_dwFastValue ),
m_dwGrapplingMaxValue( rhs.m_dwGrapplingMaxValue ),
m_dwEnableGrapplingCancelTime( rhs.m_dwEnableGrapplingCancelTime ),
m_szStandCancelMotion( rhs.m_szStandCancelMotion ),
m_szTargetStandCancelMotion( rhs.m_szTargetStandCancelMotion ),
m_szGrapplingLoopStandMotion( rhs.m_szGrapplingLoopStandMotion ),
m_szGrapplingLoopDownMotion( rhs.m_szGrapplingLoopDownMotion ),
m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
m_iMaxCombo( rhs.m_iMaxCombo ),
m_dwRunLoopDuration( rhs.m_dwRunLoopDuration ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_vCancelForce( rhs.m_vCancelForce ),
m_vTargetCancelForce( rhs.m_vTargetCancelForce )
{
	ClearData();
}

ioGrapplingAttackSkill::~ioGrapplingAttackSkill()
{
}

void ioGrapplingAttackSkill::ClearData()
{
	m_SkillState			= SS_NONE;
	m_GrapplingTargetState	= GTS_NONE;
	m_bCancelGrappling = false;

	m_dwCheckKeyTime		= 0;
	m_dwGrapplingStartTime	= 0;
	m_dwGrapplingOrigStartTime	= 0;
	m_dwMotionStartTime		= 0;
	m_dwActionMotionEndtime	= 0;

	m_dwCurGrapplingValue = 0;

	m_iCurCombo = 0;

	m_bSetMotion = false;

	m_bRepeatInput	= false;
	m_bPreLeft		= false;
}

void ioGrapplingAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	// 타겟 위치 보정
	m_fTargetGapRange		= rkLoader.LoadFloat_e( "skill_target_gap_range", 0.0f );
	m_fDownTargetGapRange	= rkLoader.LoadFloat_e( "skill_down_target_gap_range", 0.0f );
	m_fCharHeightRate		= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );

	// 기립잡기 타격정보
	m_vStandAttack.Init();
	m_vStandFinishAttack.Init();
	LoadAttackAttribute_e( "grappling_attack", m_vStandAttack, rkLoader );	
	LoadAttackAttribute_e( "grappling_finish_attack", m_vStandFinishAttack, rkLoader );
	rkLoader.LoadString_e( "grappling_attack_lt", "", szBuf, MAX_PATH );
	m_szStandAttack_lt = szBuf;
	rkLoader.LoadString_e( "grappling_attack_rt", "", szBuf, MAX_PATH );
	m_szStandAttack_rt = szBuf;
	rkLoader.LoadString_e( "grappling_down_attack_lt", "", szBuf, MAX_PATH );
	m_szDownAttack_lt = szBuf;
	rkLoader.LoadString_e( "grappling_down_attack_rt", "", szBuf, MAX_PATH );
	m_szDownAttack_rt = szBuf;

	// 다운잡기 타격정보
	m_vDownAttack.Init();
	m_vDownFinishAttack.Init();
	LoadAttackAttribute_e( "grappling_down_attack", m_vDownAttack, rkLoader );
	LoadAttackAttribute_e( "grappling_down_finish_attack", m_vDownFinishAttack, rkLoader );

	// 상태별 잡기캔슬 모션
	m_dwFastValue = (DWORD)rkLoader.LoadInt_e( "cancel_fast_value", 0 );
	m_dwGrapplingMaxValue = (DWORD)rkLoader.LoadInt_e( "grappling_cancel_max_value", 0 );
	m_dwEnableGrapplingCancelTime = (DWORD)rkLoader.LoadInt_e( "enable_grappling_cancel_time", 0 );

	rkLoader.LoadString_e( "grappling_cancel_motion_stand", "", szBuf, MAX_PATH );
	m_szStandCancelMotion = szBuf;
	rkLoader.LoadString_e( "target_grappling_cancel_motion_stand", "", szBuf, MAX_PATH );
	m_szTargetStandCancelMotion = szBuf;

	rkLoader.LoadString_e( "wound_grappling_loop_stand_motion", "", szBuf, MAX_PATH );
	m_szGrapplingLoopStandMotion = szBuf;
	rkLoader.LoadString_e( "wound_grappling_loop_down_motion", "", szBuf, MAX_PATH );
	m_szGrapplingLoopDownMotion = szBuf;

	_ENCSTR("grappling_cancel", szEncGrapplingCancel);
	_ENCSTR("target_grappling_cancel", szEncTargetGrapplingCancel);
	LoadForceInfo( szEncGrapplingCancel, m_vCancelForce, rkLoader);
	LoadForceInfo( szEncTargetGrapplingCancel, m_vTargetCancelForce, rkLoader);

	// 키 연타.
	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );

	// 콤보
	m_iMaxCombo = rkLoader.LoadInt_e( "max_combo", 1 );

	// 대쉬
	m_dwRunLoopDuration	= (DWORD)rkLoader.LoadInt_e( "run_duration", 0 );
	m_fRunSpeed			= rkLoader.LoadFloat_e( "run_speed", 0.0f );
	
	//
	int iBuffCnt = rkLoader.LoadInt_e( "blow_cancel_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "blow_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}
}

void ioGrapplingAttackSkill::LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader )
{
	rkForceInfoList.clear();

	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_force_cnt", szForce );
	int iForceCnt = rkLoader.LoadInt( szKey, 0 );
	if( iForceCnt > 0 )
		rkForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "%s_force_move%d", szForce, i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_force_friction%d", szForce, i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "%s_force_angle%d", szForce, i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		rkForceInfoList.push_back( kInfo );
	}
}

ioSkill* ioGrapplingAttackSkill::Clone()
{
	return new ioGrapplingAttackSkill( *this );
}

bool ioGrapplingAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioGrapplingAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				SetSkillRunState( pChar );
			}
		}
		break;
	case SS_RUN:
		if( m_dwActionMotionEndtime <= FRAMEGETTIME() )
		{
			SetSkillCancelState( pChar, false );
		}
		else
		{
			if( m_dwRunLoopEndTime < dwCurTime )
				SetSkillRunEndState( pChar );
			else
				pChar->SetCurMoveSpeed( m_fRunSpeed );
		}
		return;
		break;
	case SS_GRAPPLING:
		{
			if( m_dwGrapplingOrigStartTime + m_dwEnableGrapplingCancelTime < dwCurTime )
				ProcessWoundGrapplingState( pChar, pStage );

			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{

				if( m_dwGrapplingStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillCancelState( pChar, true );
					return;
				}
			}
			else
			{
				SetSkillCancelState( pChar, true );
				return;
			}

			if( pChar->IsNeedProcess() )
			{
				if( m_iCurCombo < m_iMaxCombo )
				{
					if( pChar->IsAttackKey() && m_dwCheckKeyTime+m_dwEnableKeyTime >= dwCurTime )
					{
						m_dwCheckKeyTime = dwCurTime;
						m_bRepeatInput = true;
					}
					else
					{
						m_bRepeatInput = false;
					}

					if( m_bRepeatInput )
					{
						if( m_dwMotionEndTime < FRAMEGETTIME() )
							GrapplingAttackFire( pChar );
					}
					else if( m_dwCheckKeyTime+m_dwEnableKeyTime < dwCurTime )
					{
						m_bCancelGrappling = true;
						SetSkillCancelState( pChar, true );
					}
				}
				else
				{
					SetSkillCancelState( pChar, true );
				}
			}
		}
		break;
	case SS_CANCEL:
		{
			// GrapplingAttackFire()로 인한 타격동작과 곂쳐지지 않도록 약간의 딜레이를 둔다.
			if( m_dwMotionEndTime + g_App.GetGrapplingCheckDelay() < FRAMEGETTIME() )
			{
				CheckSkillGrapplingCancel( pChar );
				SetSkillEndState();
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	pChar->SetCurMoveSpeed( 0 );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_GrapplingTargetName.Clear();

	ClearData();
}

void ioGrapplingAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtCharRot;
	rkPacket >> qtCharRot;

	pChar->SetTargetRotToRotate( qtCharRot, true, false );

	switch( iState )
	{
	case SSC_GRAPPLING:
		{
			if( m_GrapplingTargetName.IsEmpty() )
			{
				m_dwGrapplingOrigStartTime = FRAMEGETTIME();
				SetSkillRunEndState( pChar );
			}

			m_dwGrapplingStartTime	= FRAMEGETTIME();
			m_SkillState = SS_GRAPPLING;
			
			int iGrapplingTargetState;
			ioHashString szTargetName;

			rkPacket >> szTargetName;
			rkPacket >> iGrapplingTargetState;
			rkPacket >> m_SkillGrapplingInfo.m_GrapplingLoopMotion;
			rkPacket >> m_SkillGrapplingInfo.m_bReverseTargetRot;

			m_GrapplingTargetName = szTargetName;
			m_GrapplingTargetState = (GrapplingTargetState)iGrapplingTargetState;

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_ATTACK:
		{
			GrapplingAttackFire( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_CANCEL:
		rkPacket >> m_bCancelGrappling;
		SetSkillCancelState( pChar, false );
		break;
	}
}

void ioGrapplingAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
	pWeapon->SetLiveTime( m_dwRunLoopDuration );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_ATTACK && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
}

bool ioGrapplingAttackSkill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar )	return false;
	if( m_SkillState == SS_END || m_SkillState == SS_CANCEL )
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	if( m_GrapplingTargetName.IsEmpty() )
	{
		m_GrapplingTargetName = szTargetName;
		m_dwGrapplingOrigStartTime	= FRAMEGETTIME();
		m_dwCheckKeyTime			= FRAMEGETTIME();

		SetSkillRunEndState( pChar );
	}
	else if( m_GrapplingTargetName != szTargetName )
		return false;

	m_dwGrapplingStartTime	= FRAMEGETTIME();

	m_SkillState = SS_GRAPPLING;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	if( pTarget->GetState() == CS_BLOW_WOUNDED &&
	    ( pTarget->GetPreState() == CS_BLOW_WOUNDED || pTarget->GetPreState() == CS_BOUND_BLOW_WOUNDED ) &&
		m_GrapplingTargetState == GTS_NONE )
	{
		m_SkillGrapplingInfo.m_GrapplingLoopMotion = m_szGrapplingLoopDownMotion;
		m_GrapplingTargetState = GTS_DOWN;
	}
	else if( m_GrapplingTargetState == GTS_NONE )
	{
		m_SkillGrapplingInfo.m_GrapplingLoopMotion = m_szGrapplingLoopStandMotion;

		D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		float fDotValue = D3DXVec3Dot( &vTargetDir, &vDir );
		float fAngle = RADtoDEG(acosf(fDotValue));

		if( fAngle >=FLOAT90)
		{
			m_GrapplingTargetState = GTS_STAND_FRONT;
			m_SkillGrapplingInfo.m_bReverseTargetRot = true;
		}
		else
		{
			m_GrapplingTargetState = GTS_STAND_BACK;
			m_SkillGrapplingInfo.m_bReverseTargetRot = false;
		}
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		kPacket << (int)m_GrapplingTargetState;
		kPacket << m_SkillGrapplingInfo.m_GrapplingLoopMotion;
		kPacket << m_SkillGrapplingInfo.m_bReverseTargetRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingAttackSkill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_GrapplingTargetState == GTS_STAND_FRONT || m_GrapplingTargetState == GTS_STAND_BACK )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fTargetGapRange > 0.0f )
		{
			vPos += m_fTargetGapRange * vDir;
			return vPos;
		}
		else
			return ioMath::VEC3_ZERO;
	}
	else
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetMoveDir();

		if( m_fDownTargetGapRange > 0.0f )
		{
			vPos += m_fDownTargetGapRange * vDir;
			return vPos;
		}
		else
			return ioMath::VEC3_ZERO;
	}
	return ioMath::VEC3_ZERO;
}

bool ioGrapplingAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_GRAPPLING:
	case SS_RUN:
	case SS_CANCEL:
		return false;
	case SS_END:
		if( m_dwMotionEndTime < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}



bool ioGrapplingAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( m_SkillState == SS_RUN )
	{
		return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_dwGrapplingOrigStartTime>0 && m_dwGrapplingOrigStartTime+m_dwSkillProtectTime > dwCurTime )
	{
		return true;
	}

	return false;
}

bool ioGrapplingAttackSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_END:
		if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
			return false;

		if( m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;
		break;
	}

	return false;
}

bool ioGrapplingAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

int ioGrapplingAttackSkill::GetSkillState()
{
	return (int)m_SkillState;
}

void ioGrapplingAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pChar->AttachEffect( m_SkillEffectName );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwActionMotionEndtime = FRAMEGETTIME();
	m_dwActionMotionEndtime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwActionMotionEndtime += dwPreDelay + GetAttribute().m_dwEndDelay;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}	

	m_GrapplingTargetName.Clear();
}

void ioGrapplingAttackSkill::SetSkillRunState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_NONE )	return;

	m_SkillState = SS_RUN;

	m_dwRunLoopStartTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = m_dwRunLoopStartTime;
	m_dwRunLoopEndTime += m_dwRunLoopDuration;

	AttackSkillFire( pChar );
}

void ioGrapplingAttackSkill::SetSkillRunEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	pChar->SetCurMoveSpeed( 0 );
	m_dwPreDelayStartTime = 0;

	// 이동중 타겟이 잡혔을 경우 Weapon의 LiveTime을 0으로 셋팅.
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex);
	if( pWeapon )
		pWeapon->SetLiveTime( 0 );
}

void ioGrapplingAttackSkill::SetSkillCancelState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	m_SkillState = SS_CANCEL;

	if( pChar->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_CANCEL;
		kPacket << pChar->GetTargetRot();
		kPacket << m_bCancelGrappling;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingAttackSkill::SetSkillEndState()
{
	m_SkillState = SS_END;
}

void ioGrapplingAttackSkill::GrapplingAttackFire( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END || m_SkillState == SS_CANCEL )
		return;

	AttackAttribute vAttackAttribute;
	vAttackAttribute.Init();

	float fTimeRate  = FLOAT1;
	DWORD dwPreDelay = 0;

	bool bFinish = false;

	if( !COMPARE( m_iCurCombo, 0, m_iMaxCombo ) )
		return;

	if( m_iCurCombo >= m_iMaxCombo - 1 )
		bFinish = true;

	switch( m_GrapplingTargetState )
	{
	case GTS_STAND_FRONT:
	case GTS_STAND_BACK:
		{
			if(!bFinish)
			{
				if( m_iCurCombo%2 == 0 )
					m_vStandAttack.m_AttackAnimation = m_szStandAttack_lt;
				else
					m_vStandAttack.m_AttackAnimation = m_szStandAttack_rt;

				vAttackAttribute = m_vStandAttack;
			}
			else
				vAttackAttribute = m_vStandFinishAttack;
		}
		break;
	case GTS_DOWN:
		{
			if(!bFinish)
			{
				if( m_iCurCombo%2 == 0 )
					m_vDownAttack.m_AttackAnimation = m_szDownAttack_lt;
				else
					m_vDownAttack.m_AttackAnimation = m_szDownAttack_rt;

				vAttackAttribute = m_vDownAttack;
			}
			else
				vAttackAttribute = m_vDownFinishAttack;
		}
		break;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( vAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	fTimeRate  = vAttackAttribute.m_fAttackAniRate;
	dwPreDelay = vAttackAttribute.m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex + m_iCurCombo,
											iAniID,
											vAttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + vAttackAttribute.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_iCurCombo++;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ATTACK;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingAttackSkill::CheckSkillGrapplingCancel( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	if( m_GrapplingTargetName.IsEmpty()	)
		return;
	if( !m_bCancelGrappling )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	ioEntityGroup *pTargetGrp = NULL;
	if( pTarget )
	{
		pTargetGrp = pTarget->GetGroup();

		/*pTargetGrp->ClearAllActionAni( FLOAT100, true );
		pTargetGrp->ClearAllLoopAni( FLOAT100, true );*/

		int iAniID = pTargetGrp->GetAnimationIdx( m_szTargetStandCancelMotion );
		if( iAniID )
		{
			//pTargetGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 );

			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioMath::CalcDirectionQuaternion( qtRot, vDir );
			pTarget->SetTargetRotToRotate( qtRot, true, false );

			pTarget->SetWoundedState( m_szTargetStandCancelMotion, 0, FLOAT1, 0, false );
			pTarget->SetReservedSliding( m_vTargetCancelForce, iAniID, FLOAT1, 0 );
		}
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_szStandCancelMotion );
		if( iAniID )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 );

			m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID );

			pChar->SetReservedSliding( m_vCancelForce, iAniID, FLOAT1, 0 );
		}
	}
}

void ioGrapplingAttackSkill::ProcessWoundGrapplingState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage	)	return;
	if( m_GrapplingTargetName.IsEmpty() )
		return;

	if( m_bCancelGrappling )	return;

	if( m_GrapplingTargetState == GTS_STAND_BACK )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

	if( pTarget && pTarget->IsNeedProcess() )
	{
		if( pTarget->IsPrisonerMode() )
		{
			m_bCancelGrappling = false;
			return;
		}

		if(	( !m_bPreLeft && pTarget->CheckCurDirKey() == ioUserKeyInput::DKI_LEFT ) ||
			( m_bPreLeft && pTarget->CheckCurDirKey() == ioUserKeyInput::DKI_RIGHT ) )			
		{
			IORandom eRandom;
			eRandom.Randomize();

			DWORD dwCurFastValue = eRandom.Random( m_dwFastValue );
			dwCurFastValue++;

			m_dwCurGrapplingValue += dwCurFastValue;

			m_bPreLeft ^= true;
		}

		if( m_dwCurGrapplingValue >= m_dwGrapplingMaxValue )
		{
			// cancel
			m_bCancelGrappling = true;

			SetSkillCancelState( pChar, false );
			if( pTarget->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SSC_CANCEL;
				kPacket << pChar->GetTargetRot();
				kPacket << m_bCancelGrappling;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
}

bool ioGrapplingAttackSkill::isEnableCancel()
{
	if( m_GrapplingTargetState == GTS_STAND_BACK )
		return false;

	return true;
}