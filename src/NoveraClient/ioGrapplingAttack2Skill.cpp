

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingAttack2Skill.h"

#include "WeaponDefine.h"

ioGrapplingAttack2Skill::ioGrapplingAttack2Skill()
{
	ClearData();
}

ioGrapplingAttack2Skill::ioGrapplingAttack2Skill( const ioGrapplingAttack2Skill &rhs )
: ioAttackSkill( rhs ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
m_fCharHeightRate( rhs.m_fCharHeightRate ),
m_vAttackAttributeList( rhs.m_vAttackAttributeList ),
m_szCancelMotion( rhs.m_szCancelMotion ),
m_szTargetCancelMotion( rhs.m_szTargetCancelMotion ),
m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
m_iMaxCombo( rhs.m_iMaxCombo ),
m_dwRunLoopDuration( rhs.m_dwRunLoopDuration ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_vCancelForce( rhs.m_vCancelForce ),
m_vTargetCancelForce( rhs.m_vTargetCancelForce ),
m_bChangeGrappplingTargetPosList( rhs.m_bChangeGrappplingTargetPosList )
{
	ClearData();
}

ioGrapplingAttack2Skill::~ioGrapplingAttack2Skill()
{
}

void ioGrapplingAttack2Skill::ClearData()
{
	m_SkillState			= SS_NONE;
	m_bCancelGrappling = false;

	m_dwCheckKeyTime		= 0;
	m_dwGrapplingStartTime	= 0;
	m_dwGrapplingOrigStartTime	= 0;
	m_dwMotionStartTime		= 0;
	m_dwActionMotionEndtime	= 0;

	m_iCurCombo = 0;

	m_bSetMotion = false;

	m_bRepeatInput	= false;
	m_vAttackDir = ioMath::VEC3_ZERO;
}

void ioGrapplingAttack2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
	// 타겟 위치 보정
	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );
	m_fCharHeightRate		= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );

	// 콤보
	m_iMaxCombo = rkLoader.LoadInt_e( "max_combo", 1 );

	// 기립잡기 타격정보
	m_vAttackAttributeList.clear();
	m_vAttackAttributeList.reserve( m_iMaxCombo );
	for( int i=0; i<m_iMaxCombo; ++i )
	{
		AttackAttribute kInfo;
		kInfo.Init();
		wsprintf_e( szKey, "grappling_attack%d", i+1 );
		LoadAttackAttribute( szKey, kInfo, rkLoader );	
		m_vAttackAttributeList.push_back( kInfo );
	}

	// 상태별 잡기캔슬 모션
	rkLoader.LoadString_e( "grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_szCancelMotion = szBuf;
	rkLoader.LoadString_e( "target_grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_szTargetCancelMotion = szBuf;

	_ENCSTR("grappling_cancel", szEncGrapplingCancel);
	_ENCSTR("target_grappling_cancel", szEncTargetGrapplingCancel);
	LoadForceInfo( szEncGrapplingCancel, m_vCancelForce, rkLoader);
	LoadForceInfo( szEncTargetGrapplingCancel, m_vTargetCancelForce, rkLoader);

	// 키 연타.
	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "enable_key_time", 0 );

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

	m_bChangeGrappplingTargetPosList.clear();
	m_bChangeGrappplingTargetPosList.reserve( m_iMaxCombo );
	for(int i=0; i<m_iMaxCombo; ++i )
	{
		bool bChangeTargetPos;
		wsprintf_e( szKey, "change_grappling_target_position%d", i+1 );
		bChangeTargetPos = rkLoader.LoadBool( szKey, false );
		m_bChangeGrappplingTargetPosList.push_back( bChangeTargetPos );
	}
}

void ioGrapplingAttack2Skill::LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader )
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

ioSkill* ioGrapplingAttack2Skill::Clone()
{
	return new ioGrapplingAttack2Skill( *this );
}

bool ioGrapplingAttack2Skill::OnSkillStart( ioBaseChar *pChar )
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

void ioGrapplingAttack2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			CheckAniRotate( pChar );

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

void ioGrapplingAttack2Skill::OnSkillEnd( ioBaseChar *pChar )
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

void ioGrapplingAttack2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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
			
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			m_GrapplingTargetName = szTargetName;

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

void ioGrapplingAttack2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		pZoneEffect->SetCollisionDir( m_vAttackDir );
		pZoneEffect->SetZoneValue( false, true );
	}
}

bool ioGrapplingAttack2Skill::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar )	return false;
	if( m_SkillState == SS_END || m_SkillState == SS_CANCEL )
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	if( m_GrapplingTargetName.IsEmpty() )
	{
		m_GrapplingTargetName		= szTargetName;
		m_dwGrapplingOrigStartTime	= FRAMEGETTIME();
		m_dwCheckKeyTime			= FRAMEGETTIME();

		SetSkillRunEndState( pChar );
	}
	else if( m_GrapplingTargetName != szTargetName )
		return false;

	m_dwGrapplingStartTime	= FRAMEGETTIME();

	m_SkillState = SS_GRAPPLING;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget )	return false;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_GRAPPLING;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
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

D3DXVECTOR3 ioGrapplingAttack2Skill::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
	D3DXVECTOR3 vDir = pChar->GetMoveDir();

	if( m_fTargetLoopGapRange > 0.0f )
	{
		vPos += m_fTargetLoopGapRange * vDir;
		return vPos;
	}
	else
		return ioMath::VEC3_ZERO;

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingAttack2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
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



bool ioGrapplingAttack2Skill::IsProtected( int iDefenseBreakType ) const
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

bool ioGrapplingAttack2Skill::IsEnableReserve() const
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

bool ioGrapplingAttack2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

int ioGrapplingAttack2Skill::GetSkillState()
{
	return (int)m_SkillState;
}

void ioGrapplingAttack2Skill::AttackSkillFire( ioBaseChar *pChar )
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

	//pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
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

void ioGrapplingAttack2Skill::SetSkillRunState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_NONE )	return;

	m_SkillState = SS_RUN;

	m_dwRunLoopStartTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = m_dwRunLoopStartTime;
	m_dwRunLoopEndTime += m_dwRunLoopDuration;

	AttackSkillFire( pChar );
}

void ioGrapplingAttack2Skill::SetSkillRunEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	pChar->SetCurMoveSpeed( 0 );
	m_dwPreDelayStartTime = 0;

	// 이동중 타겟이 잡혔을 경우 Weapon의 LiveTime을 0으로 셋팅.
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex);
	if( pWeapon )
		pWeapon->SetLiveTime( 0 );
}

void ioGrapplingAttack2Skill::SetSkillCancelState( ioBaseChar *pChar, bool bSendNet )
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

void ioGrapplingAttack2Skill::SetSkillEndState()
{
	m_SkillState = SS_END;
}

void ioGrapplingAttack2Skill::GrapplingAttackFire( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_SkillState == SS_END || m_SkillState == SS_CANCEL )
		return;
	if( !COMPARE( m_iCurCombo, 0, m_iMaxCombo ) )
		return;

	m_bChangeGrapplingTargetPos = m_bChangeGrappplingTargetPosList[m_iCurCombo];

	float fTimeRate  = FLOAT1;
	DWORD dwPreDelay = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_vAttackAttributeList[m_iCurCombo].m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	fTimeRate  = m_vAttackAttributeList[m_iCurCombo].m_fAttackAniRate;
	dwPreDelay = m_vAttackAttributeList[m_iCurCombo].m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex + m_iCurCombo,
											iAniID,
											m_vAttackAttributeList[m_iCurCombo].m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_vAttackAttributeList[m_iCurCombo].m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	// Ani Rotate
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += FRAMEGETTIME();
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}

	pChar->SetReservedSliding( m_vAttackAttributeList[m_iCurCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_iCurCombo++;
	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

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

void ioGrapplingAttack2Skill::CheckSkillGrapplingCancel( ioBaseChar *pChar )
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

		int iAniID = pTargetGrp->GetAnimationIdx( m_szTargetCancelMotion );
		if( iAniID )
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioMath::CalcDirectionQuaternion( qtRot, vDir );
			pTarget->SetTargetRotToRotate( qtRot, true, false );

			pTarget->SetWoundedState( m_szTargetCancelMotion, 0, FLOAT1, 0, false );
			pTarget->SetReservedSliding( m_vTargetCancelForce, iAniID, FLOAT1, 0 );
		}
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_szCancelMotion );
		if( iAniID )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 );

			m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID );

			pChar->SetReservedSliding( m_vCancelForce, iAniID, FLOAT1, 0 );
		}
	}
}
