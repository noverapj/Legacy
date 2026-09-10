

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingSkill2.h"

#include "WeaponDefine.h"

ioGrapplingSkill2::ioGrapplingSkill2()
{
	ClearData();
}

ioGrapplingSkill2::ioGrapplingSkill2( const ioGrapplingSkill2 &rhs )
: ioAttackSkill( rhs ),
 m_bUseTeleport( rhs.m_bUseTeleport ),
 m_fTeleportSkillRange( rhs.m_fTeleportSkillRange ),
 m_fTeleportSkillAngle( rhs.m_fTeleportSkillAngle ),
 m_fTeleportSkillOffSet( rhs.m_fTeleportSkillOffSet ),
 m_fTeleportMoveRange( rhs.m_fTeleportMoveRange ),
 m_fTeleportMoveHeightRange( rhs.m_fTeleportMoveHeightRange ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
 m_dwLoopRotateTime( rhs.m_dwLoopRotateTime ),
 m_fCharHeightRate( rhs.m_fCharHeightRate ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_vComboList( rhs.m_vComboList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_dwEnableComboTime( rhs.m_dwEnableComboTime ),
 m_bUseEndOtherAttack( rhs.m_bUseEndOtherAttack ),
 m_EndOtherAttack( rhs.m_EndOtherAttack ),
 m_bUseCombo2State( rhs.m_bUseCombo2State ),
 m_fCombo2Range( rhs.m_fCombo2Range ),
 m_fCombo2OffSet( rhs.m_fCombo2OffSet ),
 m_bNoGravityCombo( rhs.m_bNoGravityCombo ),
 m_iCombo2PosCnt( rhs.m_iCombo2PosCnt ),
 m_DownGrapplingInfo( rhs.m_DownGrapplingInfo ),
 m_DownGrapplingAnimation( rhs.m_DownGrapplingAnimation ),
 m_fDownGrapplingAniRate( rhs.m_fDownGrapplingAniRate ),
 m_fDownGrapJumpPower( rhs.m_fDownGrapJumpPower ),
 m_LandAttack( rhs.m_LandAttack )
{
	ClearData();
}

ioGrapplingSkill2::~ioGrapplingSkill2()
{
}

void ioGrapplingSkill2::ClearData()
{
	m_SkillState = SS_NONE;

	m_fCurChargeRate = 0.0f;
	
	m_dwSkillLoopStartTime	= 0;
	m_dwSkillLoopEndTime	= 0;

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh	= false;
	m_bUsedEndOtherAttack = false;

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_Combo2PosList.clear();
}

void ioGrapplingSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// teleport
	m_bUseTeleport = rkLoader.LoadBool_e( "use_target_teleport", false );

	m_fTeleportSkillRange = rkLoader.LoadFloat_e( "teleport_range", 0.0f );
	m_fTeleportSkillAngle = rkLoader.LoadFloat_e( "teleport_angle", 0.0f );
	m_fTeleportSkillOffSet = rkLoader.LoadFloat_e( "teleport_position_offset", 0.0f );

	m_fTeleportMoveRange = rkLoader.LoadFloat_e( "teleport_move_range", 0.0f );
	m_fTeleportMoveHeightRange = rkLoader.LoadFloat_e( "teleport_move_height_range", 0.0f );

	// 잡고 있는 동작
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration	= rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwLoopRotateTime		= rkLoader.LoadInt_e( "skill_loop_rotate_speed", 0 );

	m_dwEnableComboTime = rkLoader.LoadInt_e( "skill_combo_enable_time", 0 );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );

	m_fCharHeightRate	= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fCharJumpPower	= rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	// 
	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );
		}
	}

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//
	m_bUseEndOtherAttack = rkLoader.LoadBool_e( "use_end_other_attack", false );

	LoadAttackAttribute_e( "end_other_attack", m_EndOtherAttack, rkLoader );

	// combo2
	m_bUseCombo2State = rkLoader.LoadBool_e( "use_combo2_state", false );
	m_fCombo2Range = rkLoader.LoadFloat_e( "combo2_range", 0.0f );
	m_fCombo2OffSet = rkLoader.LoadFloat_e( "combo2_offset", 0.0f );
	m_iCombo2PosCnt = rkLoader.LoadInt_e( "combo2_pos_cnt", 1 );

	m_bNoGravityCombo = rkLoader.LoadBool_e( "no_gravity_combo", false );

	m_DownGrapplingInfo.Init();
	m_DownGrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "down_grap_target_reverse_rotate", false );
	m_DownGrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "down_grap_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "down_grap_loop_motion", "", szBuf, MAX_PATH );
	m_DownGrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_DownGrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "down_grap_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "down_grappling_motion", "", szBuf, MAX_PATH );	
	m_DownGrapplingAnimation = szBuf;
	m_fDownGrapplingAniRate = rkLoader.LoadFloat_e( "down_grappling_motion_rate", 0.0f );

	m_fDownGrapJumpPower = rkLoader.LoadFloat_e( "down_grap_jump_power", 0.0f );

	LoadAttackAttribute_e( "land_attack", m_LandAttack, rkLoader );
}

ioSkill* ioGrapplingSkill2::Clone()
{
	return new ioGrapplingSkill2( *this );
}

bool ioGrapplingSkill2::OnSkillStart( ioBaseChar *pChar )
{
	ClearData();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	InitExtraAniJump();
	
	return true;
}

void ioGrapplingSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				if( m_bUseTeleport )
					SetTeleportList( pChar, pStage );
				else
					AttackSkillFire( pChar );

				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		CheckExtraAniJump( pChar );
		break;
	case SS_LOOP:
		{
			CheckExtraAniJump( pChar );
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwSkillLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar, true );
				return;
			}

			if( m_dwSkillLoopStartTime + m_dwEnableComboTime < dwCurTime )
			{
				if( m_bUseCombo2State )
				{
					SetNextLinearCombo2( pChar, pStage );
					return;
				}
				else
				{
					if( pChar->IsNeedProcess() )
					{
						if( pChar->IsAttackKey() )
						{
							SetNextLinearCombo( pChar, false );
							return;
						}
						else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
						{				
							m_bUsedEndOtherAttack = true;
							SetNextLinearCombo( pChar, true );
							return;
						}
					}
				}
			}

			if( m_dwSkillLoopEndTime < dwCurTime )
			{
				SetSkillEndState( pChar, false );
				return;
			}

			ProcessLoopDir( pChar );
		}
		break;
	case SS_COMBO:
		{
			CheckExtraAniJump( pChar );
			CheckLinearCombo( pChar );
			CheckAirJump( pChar );
		}
		break;
	case SS_COMBO2:
		if( pChar->IsNeedProcess() && !CheckTargetValidate( pChar, pStage ) )
		{
			SetSkillEndState( pChar, true );
			return;
		}

		CheckExtraAniJump( pChar );
		CheckLinearCombo2( pChar, pStage );
		CheckAirJump( pChar );
		break;
	case SS_DWON_GRAP:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwMotionStartTime + g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, false );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar, false );
				return;
			}

			//motion 시간 체크
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillEndState( pChar, false );
				return;
			}

			
			if ( m_fDownGrapJumpPower )
			{
				if( CheckLanding( pChar ) )
				{
					SetLandAttackState( pChar );
					return;
				}
			}
		}
		break;
	case SS_LAND_ATTACK:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetSkillEndState( pChar, false );
				return;
			}
		}
		break;
	case SS_END:
		break;
	}
}

void ioGrapplingSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
}

void ioGrapplingSkill2::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );
	pChar->CreateMapEffectBySkill( m_SkillMapEffectName, pChar->GetWorldPosition() );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
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
			kInfo.m_dwTime += dwCurTime;
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
	//

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	m_dwAttackEndTime = 0;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_ACTION;
	
	m_GrapplingTargetName.Clear();

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
}

void ioGrapplingSkill2::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_fCharJumpPower > 0.0f )
		pChar->SetJumpPower( m_fCharJumpPower );

	m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
	m_dwSkillLoopEndTime += m_dwSkillLoopDuration;
}

void ioGrapplingSkill2::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime = dwCurTime;
	m_dwAttackEndTime = dwCurTime;
	m_dwMotionEndTime += 100;
	m_dwEnableReserveTime = 0;

	m_SkillState = SS_END;

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
		{
			pTarget->SetGrapplingBlowWound();
		}
	}
}

void ioGrapplingSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

bool ioGrapplingSkill2::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_SkillState != SS_ACTION && m_SkillState != SS_COMBO2 )
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	if( m_SkillState == SS_ACTION )
	{
		SetSkillLoopState( pChar );
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_LOOP;
			kPacket << pChar->GetTargetRot();
			kPacket << szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if ( m_SkillState == SS_COMBO2 )
	{
		SetDownGrapAttackState( pChar );
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_DOWN_GRAP;
			kPacket << pChar->GetTargetRot();
			kPacket << szTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
		bNPC = true;

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || bNPC )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
	}

	return true;
}

D3DXVECTOR3 ioGrapplingSkill2::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fTargetLoopGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fTargetLoopGapRange > 0.0f )
			vPos += m_fTargetLoopGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_ACTION:
	case SS_COMBO:
	case SS_COMBO2:
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioGrapplingSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case SSC_TELEPORT:
		{
			D3DXVECTOR3 vTargetPos;
			rkPacket >> vTargetPos;

			pChar->SetWorldPosition( vTargetPos );

			AttackSkillFire( pChar );
		}
		break;
	case SSC_LOOP:
		{
			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			SetSkillLoopState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
		break;
	case SSC_END:
		{
			if( m_SkillState != SS_END )
			{
				SetSkillEndState( pChar, false );
				ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

				if( pTarget && pTarget->IsNeedProcess() && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
				{
					pTarget->SetGrapplingBlowWound();
				}
			}
		}
		break;
	case SSC_COMBO:
		{
			bool bOther;
			rkPacket >> m_iCurLinearCombo;
			rkPacket >> bOther;

			SetNextLinearCombo( pChar, bOther );

			if( IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	case SSC_DOWN_GRAP:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			SetDownGrapAttackState( pChar );

			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}
		}
	}
}

void ioGrapplingSkill2::ProcessLoopDir( ioBaseChar *pChar )
{
	if( m_dwLoopRotateTime == 0 )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
}

bool ioGrapplingSkill2::IsProtected( int iDefenseBreakType ) const
{
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

	switch( m_SkillState )
	{
	case SS_ACTION:
		return true;
	case SS_LOOP:
	case SS_COMBO:
	case SS_COMBO2:
		if( m_dwCurSkillProtectTime < dwCurTime )
			return false;
		break;
	case SS_END:
		return false;
	}

	return true;
}

bool ioGrapplingSkill2::IsEnableReserve() const
{
	return false;
}

bool ioGrapplingSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioGrapplingSkill2::IsComboState()
{
	if( m_SkillState == SS_COMBO )
		return true;

	return false;
}

void ioGrapplingSkill2::SetNextLinearCombo( ioBaseChar *pChar, bool bOther )
{
	m_dwEnableReserveTime = 0;
	m_SkillState = SS_COMBO;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = m_vComboList[m_iCurLinearCombo];

	if( bOther )
		kAttack = m_EndOtherAttack;

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );
	//

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_bNoGravityCombo )
	{
		pChar->SetGravityAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetCurJumpPowerAmt( 0.0f );
	}

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_bRefreshFireTimeList )
	{
		if( m_iCurLinearCombo == 0 )
			m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
											iAniID,
											kAttack.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	m_dwAttackEndTime = m_dwMotionEndTime;
	m_dwMotionEndTime += 100;

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	// For AirJumpPower
	SetAirJump( pChar, iAniID, fTimeRate );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_COMBO;
		kPacket << pChar->GetTargetRot();
		kPacket << m_iCurLinearCombo;
		kPacket << bOther;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioGrapplingSkill2::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_bUsedEndOtherAttack )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwEnableReserveTime == 0 )
		return;

	if( m_dwEnableReserveTime < dwCurTime )
	{
		if( pChar->IsNeedProcess() )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( pChar->IsAttackKey() && !bReserveSkill )
			{
				SetNextLinearCombo( pChar, false );
				return;
			}
			else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
			{				
				m_bUsedEndOtherAttack = true;
				SetNextLinearCombo( pChar, true );
				return;
			}
		}
	}
}

bool ioGrapplingSkill2::IsJumpState() const
{
	switch( m_SkillState )
	{
	case SS_COMBO:
	case SS_COMBO2:
		return true;
	}

	return false;
}

bool ioGrapplingSkill2::IsNoDropState() const
{
	if( !m_bNoGravityCombo )
		return false;

	switch( m_SkillState )
	{
	case SS_COMBO:
	case SS_COMBO2:
		return true;
	}

	return false;
}

void ioGrapplingSkill2::SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fTeleportSkillRange * m_fTeleportSkillRange;
	float fHalfCosine = cosf( DEGtoRAD( m_fTeleportSkillAngle ) * FLOAT05 );

	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar )
			continue;

		if( pChar == pOwner )  
			continue;

		if( pOwner->GetCharName() == pChar->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pChar->GetTeam() )
			continue;

		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget() ) 
			continue;

		if( pChar->IsPrisonerMode() )
			continue;

		if( pChar->IsApplyDownState(false) )
			continue;

		D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vSightDir, &vSightDir );
		
		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVECTOR3 vTargetDir = vDiff;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		if( D3DXVec3Dot( &vSightDir, &vTargetDir ) < fHalfCosine )
			continue;

		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )	// Enemy
		{
			pTargetChar = pChar;
			fPrevDistSq = fDistSq;
		}
	}

	if( !m_bReduceGauge )
	{
		pOwner->ReduceNeedGaugeBySkill( this, GetNeedGauge(pOwner->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( pOwner && pTargetChar )
	{
		D3DXVECTOR3 vOffSet( 0.0f, m_fTeleportMoveHeightRange, 0.0f );
		D3DXVECTOR3 vTargetPos = pTargetChar->GetWorldPosition() + vOffSet;
		D3DXQUATERNION qtTargetOrientation = pTargetChar->GetTargetRot();
		D3DXVECTOR3 vNewPos = vTargetPos + ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fTeleportMoveRange );
		
		pOwner->SetWorldPosition( vNewPos );
		pOwner->SetTargetRotToTargetPos( vTargetPos, false );
		if( pStage && pOwner->IsOwnerChar() )
			pStage->SetCameraToOwnerExplicit( pOwner );

		AttackSkillFire( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TELEPORT;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioGrapplingSkill2::CheckTargetValidate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return false;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	if( !pStage->IsCollisionCheckByMode() )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
	float fDistSq = D3DXVec3LengthSq( &vDiff );

	float fEnableDistSq = m_fCombo2Range*m_fCombo2Range;
	if( fDistSq > fEnableDistSq )
		return false;

	return true;
}

void ioGrapplingSkill2::CheckLinearCombo2( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		pChar->SetSkillMoveEnable( false, true );
		SetNextLinearCombo2( pChar, pStage );
	}

	return;
}

void ioGrapplingSkill2::SetNextLinearCombo2( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;

	CheckNextLinearCombo2Pos( pChar );

	if( pStage && pChar->IsOwnerChar() )
		pStage->SetCameraToOwnerExplicit( pChar );

	//
	m_dwEnableReserveTime = 0;
	m_SkillState = SS_COMBO2;

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = m_vComboList[m_iCurLinearCombo];

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );

	pChar->InitActionStopDelay( true );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );
	//

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
										iAniID,
										kAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	// For AirJumpPower
	SetAirJump( pChar, iAniID, fTimeRate );

	m_iCurLinearCombo++;
}

void ioGrapplingSkill2::CheckNextLinearCombo2Pos( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;

	if( m_iCombo2PosCnt == 0 )
		return;

	if( m_Combo2PosList.empty() && m_iCombo2PosCnt > 0 )
	{
		float fAngleGap = 360.0f / (float)m_iCombo2PosCnt;

		for( int i=0; i < m_iCombo2PosCnt; ++i )
		{
			float fCurAngle = fAngleGap * i;
			m_Combo2PosList.push_back( fCurAngle );
		}

		if( m_Combo2PosList.size() > 2 )
		{
			std::random_shuffle( m_Combo2PosList.begin(), m_Combo2PosList.end() );
		}
	}

	float fCurAngle = m_Combo2PosList.front();
	m_Combo2PosList.pop_front();

	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vCurPos, vCurDir;
	vCurDir = qtRot * vDir;
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vNewPos = pTarget->GetWorldPosition() + ( m_fCombo2OffSet * vCurDir );

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vNewPos;
	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	D3DXQUATERNION qtRotate;
	ioMath::CalcDirectionQuaternion( qtRotate, -vDiff );

	pChar->SetWorldPosition( vNewPos );
	pChar->SetTargetRotToRotate( qtRotate, true );
}

GrapplingSkillWoundedInfo ioGrapplingSkill2::GetSkillWoundGrapplingInfo()
{
	if ( m_SkillState == SS_COMBO2 )
		return m_DownGrapplingInfo;

	return m_SkillGrapplingInfo;
}

void ioGrapplingSkill2::SetDownGrapAttackState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	m_SkillState = SS_DWON_GRAP;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = m_fDownGrapplingAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( m_DownGrapplingAnimation );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetJumpPower( m_fDownGrapJumpPower );

}

bool ioGrapplingSkill2::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
		return false;

	return true;
}

void ioGrapplingSkill2::SetLandAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_LandAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	m_SkillState = SS_LAND_ATTACK;

	float fTimeRate = m_LandAttack.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_LandAttack.m_dwPreDelay;

	
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_LandAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );
	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_LandAttack.m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime = dwCurTime + dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime = pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	if ( m_dwFireMotionEndTime > 0 )
		m_dwFireMotionEndTime += dwCurTime + dwPreDelay;

	m_dwEnableReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveTime > 0 )
		m_dwEnableReserveTime += dwPreDelay + dwCurTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pChar->SetReservedSliding( m_LandAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}
