

#include "stdafx.h"

#include "ioCountTeleportAttackSkill2.h"
#include "ioNpcChar.h"
#include "FindPredicateImpl.h"
#include "ioBombMissileWeapon3.h"

ioCountTeleportAttackSkill2::ioCountTeleportAttackSkill2()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwCurEnemyMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_fExtraEndAniRate = FLOAT1;

	m_dwWaitStartTime = 0;
}

ioCountTeleportAttackSkill2::ioCountTeleportAttackSkill2( const ioCountTeleportAttackSkill2 &rhs )
: ioCountSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetRedCircleEffect( rhs.m_TargetRedCircleEffect ),
 m_TargetBlueCircleEffect( rhs.m_TargetBlueCircleEffect ),
 m_SkillStartMotion( rhs.m_SkillStartMotion ),
 m_fStartMotionRate( rhs.m_fStartMotionRate ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_TeleportStartEffect( rhs.m_TeleportStartEffect ),
 m_TeleportEndEffect( rhs.m_TeleportEndEffect ),
 m_TeleportCameraBuff( rhs.m_TeleportCameraBuff ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_EnemyRedEffect( rhs.m_EnemyRedEffect ),
 m_EnemyBlueEffect( rhs.m_EnemyBlueEffect ),
 m_TeleportType( rhs.m_TeleportType ),
 m_bReverseChange( rhs.m_bReverseChange ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fTeleportRange( rhs.m_fTeleportRange ),
 m_fUpHeight( rhs.m_fUpHeight ),
 m_fUnderHeight( rhs.m_fUnderHeight ),
 m_fBackRange( rhs.m_fBackRange ),		
 m_fHeightRange( rhs.m_fHeightRange ),
 m_fEnableGapHeight( rhs.m_fEnableGapHeight ),
 m_fEnableRange( rhs.m_fEnableRange ),
 m_iEnableAngleCnt( rhs.m_iEnableAngleCnt ),
 m_dwJumpTeleportDuration( rhs.m_dwJumpTeleportDuration ),
 m_dwNormalTeleportDuration( rhs.m_dwNormalTeleportDuration ),
 m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
 m_fHeightOffset( rhs.m_fHeightOffset ),
 m_DefaultFireList( rhs.m_DefaultFireList ),
 m_fTeleportOffset( rhs.m_fTeleportOffset ),
 m_fTeleportNoTarget( rhs.m_fTeleportNoTarget ),
 m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
 m_fTeleportJumpPower( rhs.m_fTeleportJumpPower ),
 m_TeleportAutoMove( rhs.m_TeleportAutoMove ),
 m_TeleportAutoJump( rhs.m_TeleportAutoJump ),
 m_NomralAttackByAir( rhs.m_NomralAttackByAir ),
 m_JumpAttackByAir( rhs.m_JumpAttackByAir ),
 m_bTargetPosAttack ( rhs.m_bTargetPosAttack )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwCurEnemyMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_fExtraEndAniRate = FLOAT1;

	m_dwWaitStartTime = 0;
}

ioCountTeleportAttackSkill2::~ioCountTeleportAttackSkill2()
{
}

void ioCountTeleportAttackSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_red_circle_effect", "", szBuf, MAX_PATH );
	m_TargetRedCircleEffect = szBuf;
	rkLoader.LoadString_e( "target_blue_circle_effect", "", szBuf, MAX_PATH );
	m_TargetBlueCircleEffect = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "enemy_red_team_effect", "", szBuf, MAX_PATH );
	m_EnemyRedEffect = szBuf;
	rkLoader.LoadString_e( "enemy_blue_team_effect", "", szBuf, MAX_PATH );
	m_EnemyBlueEffect = szBuf;

	rkLoader.LoadString_e( "skill_start_motion", "", szBuf, MAX_PATH );
	m_SkillStartMotion = szBuf;
	m_fStartMotionRate = rkLoader.LoadFloat_e( "start_motion_rate", FLOAT1 );
	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_TeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_TeleportEndEffect = szBuf;
	rkLoader.LoadString_e( "teleport_camera_buff", "", szBuf, MAX_PATH );
	m_TeleportCameraBuff = szBuf;

	m_TeleportType = (TeleportType)rkLoader.LoadInt_e( "teleport_type", TT_NORMAL );
	m_bReverseChange = rkLoader.LoadBool_e( "teleport_reverse", false );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fTeleportRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fUnderHeight = rkLoader.LoadFloat_e( "target_under_height", 0.0f );

	m_fBackRange = rkLoader.LoadFloat_e( "back_move_range", 0.0f );
	m_fHeightRange = rkLoader.LoadFloat_e( "back_move_height_range", 0.0f );

	m_fEnableGapHeight = rkLoader.LoadFloat_e( "mass_teleport_enable_height", FLOAT100 );
	m_fEnableRange = rkLoader.LoadFloat_e( "mass_teleport_enable_range", FLOAT100 );
	m_iEnableAngleCnt = rkLoader.LoadInt_e( "mass_teleport_enable_angle_cnt", 0 );

	m_dwJumpTeleportDuration = (DWORD)rkLoader.LoadInt_e( "jump_teleport_duration", 1000 );
	m_dwNormalTeleportDuration = (DWORD)rkLoader.LoadInt_e( "normal_teleport_duration", 1000 );
	
	m_fTargetMaxRange	= rkLoader.LoadFloat_e( "max_target_range", FLOAT500 );
	m_fHeightOffset		= rkLoader.LoadFloat_e( "height_offset", FLOAT100 );
	
	int iMax = rkLoader.LoadInt_e( "max_default_info", 0 );
	for( int i = 0; i < iMax; ++i )
	{
		sprintf_s_e( szBuf, "default_range%d", i+1 );
		float fRange = rkLoader.LoadFloat( szBuf, 0.0f );

		sprintf_s_e( szBuf, "default_angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szBuf, 0.0f );
		m_DefaultFireList.push_back( DefulatFire( fRange, fAngle ) );
	}

	LoadWeaponInfo( "attack_air", m_NomralAttackByAir, rkLoader );
	LoadWeaponInfo( "jump_attack_air", m_JumpAttackByAir, rkLoader );

	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_offset", 0.0f );
	m_fTeleportNoTarget = rkLoader.LoadFloat_e( "teleport_no_target", FLOAT500 );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );
	m_fTeleportJumpPower = rkLoader.LoadFloat_e( "teleport_jump_power", 0.0f );

	LoadAttackAttribute( "teleport_auto_move", m_TeleportAutoMove, rkLoader );
	LoadAttackAttribute( "teleport_auto_jump", m_TeleportAutoJump, rkLoader );

	m_bTargetPosAttack = rkLoader.LoadBool_e( "target_pos_attack", false );
}

ioSkill* ioCountTeleportAttackSkill2::Clone()
{
	return new ioCountTeleportAttackSkill2( *this );
}

bool ioCountTeleportAttackSkill2::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;
	if( !ioCountSkill::OnSkillStart(pChar) )
		return false;

	m_dwWaitStartTime = 0;
	pChar->AttachEffect( m_SkillEffectName );

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->SetSkillProtection();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	ioGrowthTeleportUpInfo *pTeleInfo = ToTeleportUpInfo( GetGrowthUpInfoByType(GT_TELEPORT_ANI_RATE, pChar->GetCharName()) );
	if( pTeleInfo )
	{
		SetExtraAniRate( pTeleInfo->GetAniRate(pStage) );
		SetExtraEndAniRate( pTeleInfo->GetEndAniRate(pStage) );
	}

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	switch( m_TeleportType )
	{
	case TT_NORMAL:
	case TT_MASS_TELEPORT:
	case TT_ATTACK:
		SetWaitState( pChar );
		break;
	case TT_IMMEDIATELY:
		SetTeleportState( pChar, NULL );
		break;
	case TT_AUTOTARGET:
		SetTeleportAutoTargetState( pChar, pStage );
		break;
	default:
		SetWaitState( pChar );
		break;
	}

	if( !m_TeleportCameraBuff.IsEmpty() )
		pChar->AddNewBuff( m_TeleportCameraBuff, "", "", this );

	CheckSkillDesc( pChar );

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioCountTeleportAttackSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );

	pChar->RemoveBuff( m_TeleportCameraBuff );
	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}

	if( m_dwCurEnemyMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurEnemyMapEffect );
		m_dwCurEnemyMapEffect = -1;
	}

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioCountTeleportAttackSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_WAIT:
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );
		if( pChar->IsNeedProcess() )
		{
			if( m_TeleportType == TT_NORMAL || m_TeleportType == TT_ATTACK )
			{
				if(( m_SkillUseType == SUT_JUMP && m_dwWaitStartTime > 0 && m_dwWaitStartTime+m_dwJumpTeleportDuration < dwCurTime ) ||
				   ( m_SkillUseType == SUT_NORMAL && m_dwWaitStartTime > 0 && m_dwWaitStartTime+m_dwNormalTeleportDuration < dwCurTime ))
				{
					m_State = SS_CANCEL;
					return;
				}				
			}

			if( pChar->IsAttackKey() ||
				pChar->IsDefenseKey() ||
				pChar->IsJumpKey() ||
				pChar->GetSkillKeyInput() == iSlot )
			{
				switch( m_TeleportType )
				{
				case TT_MASS_TELEPORT:
					{
						D3DXVECTOR3 vPos = GetMagicCirclePos( pChar, pStage );
						float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
						if( fMapHeight > 0.0f )
						{
							SetActionState( pChar, pStage );
						}
					}
					break;
				case TT_ATTACK:
					SetTeleportAttackState( pChar, pStage );
					break;
				default:
					SetTeleportState( pChar, pStage );
					break;
				}
			}
		}
		break;
	case SS_TELEPORT:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pChar );
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetMassTeleportState( pChar, pStage );
		break;
	case SS_MASS_TELEPORT:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pChar );
		break;
	case SS_END:
	case SS_CANCEL:
		break;
	case SS_TELEPORT_ATTACK:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			CheckTeleportAttackState( pChar, pStage );
		break;
	case SS_TELEPORT_ATTACK_FIRE:
		SetEndState( pChar );
		break;
	case SS_TELEPORT_AUTO:
		ProcessTeleportAuto( pChar, pStage );
		break;
	case SS_TELEPORT_AUTO_ATTACK:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pChar );
		break;
	}
}

void ioCountTeleportAttackSkill2::ProcessTeleportAuto( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
		SetTeleportAutoState( pOwner, pStage, true );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

bool ioCountTeleportAttackSkill2::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioCountSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCountTeleportAttackSkill2::SetTeleportAttackSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_TeleportStartEffect );
	
	if( m_State != SS_END )
	{
		if( !m_bReduceOnParent )
		{
			int iOffSet = 1;
			CharState eState = pChar->GetState();
			if( eState == CS_WOUNDED || eState == CS_BLOW_WOUNDED ||
				eState == CS_BOUND_BLOW_WOUNDED || eState == CS_BLOW_EXTEND_WOUND )
				iOffSet = 2;

			ReduceNeedCount( m_iNeedCount * iOffSet );
		}
		m_bReduceGauge = true;
	}
}

void ioCountTeleportAttackSkill2::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;

	m_dwWaitStartTime = FRAMEGETTIME();

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	ioMapEffect *pMapEffect = NULL;
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->GetTeam() == TEAM_RED )
	{
		if( !m_TargetRedCircleEffect.IsEmpty() )
			pMapEffect = pChar->CreateMapEffectBySkill( m_TargetRedCircleEffect, vPos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		if( !m_TargetBlueCircleEffect.IsEmpty() )
			pMapEffect = pChar->CreateMapEffectBySkill( m_TargetBlueCircleEffect, vPos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		{
			if( !m_TargetBlueCircleEffect.IsEmpty() )
				pMapEffect = pChar->CreateMapEffectBySkill( m_TargetBlueCircleEffect, vPos );
		}
		else
		{
			if( !m_TargetRedCircleEffect.IsEmpty() )
				pMapEffect = pChar->CreateMapEffectBySkill( m_TargetRedCircleEffect, vPos );
		}
	}

	if( pMapEffect )
	{
		if( m_dwTargetCircleEffect != -1 )
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );

		m_dwTargetCircleEffect = pMapEffect->GetUniqueID();
	}
}

void ioCountTeleportAttackSkill2::SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_TELEPORT;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
	}

	m_vCirclePos = GetMagicCirclePos( pChar, pStage );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	else
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();

		if( m_dwTargetCircleEffect != -1 )
		{
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );
			m_dwTargetCircleEffect = -1;
		}
	}

	EndPreDelayEffect( pChar );
	SetTeleportAttackSkill( pChar );

	CheckTeleportTarget( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::SetTeleportAutoTargetState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_TELEPORT_AUTO;
	m_dwPreDelayStartTime = 0;

	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}
	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

	EndPreDelayEffect( pChar );
	SetTeleportAttackSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::SetTeleportAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_TELEPORT_ATTACK;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
	}

	m_vCirclePos = GetMagicCirclePos( pChar, pStage );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	else
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();

		if( m_dwTargetCircleEffect != -1 )
		{
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );
			m_dwTargetCircleEffect = -1;
		}
	}

	EndPreDelayEffect( pChar );
	SetTeleportAttackSkill( pChar );
	CheckTeleportTarget( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	rkPacket >> m_State;
	switch( m_State )
	{
	case SS_TELEPORT:
		SetTeleportState( pChar, pStage );
		break;
	case SS_TELEPORT_ATTACK:
		SetTeleportAttackState( pChar, pStage );
		break;
	case SS_ACTION:
		SetActionState( pChar, pStage );
		break;
	case SS_MASS_TELEPORT:
		SetMassTeleportState( pChar, pStage );
		break;
	case SS_TELEPORT_ATTACK_FIRE:
		{
			rkPacket >> m_vCirclePos;

			int iSize = 0;
			rkPacket >> iSize;

			if( m_bTargetPosAttack )
			{
				for( int i = 0; i < iSize; ++i )
				{
					byte eState			= TS_NONE;
					D3DXVECTOR3 vPos	= ioMath::VEC3_ZERO;
					D3DXVECTOR3 vDir	= ioMath::VEC3_ZERO;
					rkPacket >> eState;
					rkPacket >> vPos;
					rkPacket >> vDir;

					AttackFireToPos( pChar, pStage, vPos, vDir, (TargetState)eState );
				}
			}
			else
			{
				for( int i = 0; i < iSize; ++i )
				{
					byte eState			= TS_NONE;
					D3DXVECTOR3 vDir	= ioMath::VEC3_ZERO;
					rkPacket >> eState;
					rkPacket >> vDir;

					AttackFireToDir( pChar, pStage, vDir, (TargetState)eState );
				}
			}
			SetEndState( pChar );
		}
		break;
	case SS_TELEPORT_AUTO:
		SetTeleportAutoTargetState( pChar, pStage );
		break;
	case SS_TELEPORT_AUTO_ATTACK:
		{
			rkPacket >> m_vCirclePos;
			rkPacket >> m_vTargetPos;
			SetTeleportAutoState( pChar, pStage, false );
		}
		break;
	}
}

bool ioCountTeleportAttackSkill2::IsEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

void ioCountTeleportAttackSkill2::SetTeleportAutoState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSendPacket )
{
	m_State = SS_TELEPORT_AUTO_ATTACK;

	FD_RangeTarget kFindPred( pChar, m_fTargetMaxRange, 0, false );
	ioBaseChar *pTarget = pChar->GetCreator()->FindUser( kFindPred );
	if( bSendPacket )
	{
		if( pTarget )
		{
			D3DXVECTOR3 vOffset = pTarget->GetMoveDir() * m_fTeleportOffset;

			m_vTargetPos = pTarget->GetWorldPosition();
			m_vCirclePos = m_vTargetPos - vOffset;
		}
		else
		{
			D3DXVECTOR3 vOffset = pChar->GetMoveDir() * m_fTeleportNoTarget;

			m_vCirclePos = pChar->GetWorldPosition() + vOffset;
			m_vTargetPos = m_vCirclePos + vOffset;
		}
	}
	bool bAfterJump = false;
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vCirclePos.x, m_vCirclePos.z );
	if ( m_vCirclePos.y - fMapHeight >= m_fJumpHeightGap )
		bAfterJump =  true;
	else if( fMapHeight > m_vCirclePos.y )
		m_vCirclePos.y = fMapHeight;

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	else
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );

	if( pMapEffect )
		m_dwCurMapEffect = pMapEffect->GetUniqueID();

	pChar->SetWorldPosition( m_vCirclePos );
	pChar->SetTargetRotToTargetPos( m_vTargetPos, true );
	pChar->ChangeDirectionByInputDir( true );

	AttackAttribute m_Attribute;
	if( bAfterJump )
		m_Attribute = m_TeleportAutoJump;
	else
		m_Attribute = m_TeleportAutoMove;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_Attribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = m_Attribute.m_dwPreDelay;
	float fTimeRate  = m_Attribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pChar->RefreshFireTimeList( iAniID, m_Attribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwAttackEndTime = m_dwMotionEndTime + 100;

	pChar->SetSkillNextJumpAmt( m_fTeleportJumpPower );

	if( bSendPacket && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		kPacket << m_vCirclePos;
		kPacket << m_vTargetPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::SetEndState( ioBaseChar *pChar )
{
	if( m_State == SS_TELEPORT_AUTO || m_State == SS_TELEPORT_AUTO_ATTACK )
		m_State = SS_TELEPORT_AUTO_END;
	else
		m_State = SS_END;

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}
	if( m_dwCurEnemyMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurEnemyMapEffect );
		m_dwCurEnemyMapEffect = -1;
	}
	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}
	if( m_State == SS_TELEPORT_AUTO_END )
		return;

	pChar->DestroyMagicCircle();

	if( m_TeleportType == TT_MASS_TELEPORT )
	{
		D3DXVECTOR3 vEndPos = pChar->GetEnableTeleportPosition( m_vCirclePos, pChar, m_iEnableAngleCnt, m_fEnableRange, m_fEnableGapHeight );
		pChar->SetWorldPosition( vEndPos );
	}
	else
	{
		pChar->SetWorldPosition( m_vCirclePos );
	}

	if( m_bReverseChange )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationYawPitchRoll( &qtRot, DEGtoRAD(180.0f), 0.0f, 0.0f );
		vDir = qtRot * vDir;

		D3DXQUATERNION qtRotate;
		ioMath::CalcDirectionQuaternion( qtRotate, -vDir );

		pChar->SetTargetRotAndMoveDirChange( qtRotate );
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate * m_fExtraEndAniRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_TeleportEndEffect );
}

bool ioCountTeleportAttackSkill2::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

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
	return false;
}

bool ioCountTeleportAttackSkill2::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	CharState eState = pChar->GetState();
	if( eState == CS_WOUNDED || eState == CS_BLOW_WOUNDED ||
		eState == CS_BOUND_BLOW_WOUNDED || eState == CS_BLOW_EXTEND_WOUND )
	{
		if( m_iCurUseCount < m_iNeedCount * 2 )
			return false;
	}
	return true;
}

bool ioCountTeleportAttackSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

void ioCountTeleportAttackSkill2::CheckTeleportTarget( ioBaseChar *pOwnerChar, ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( !pOwnerChar ) return;
	if( m_fTeleportRange <= 0.0f ) return;

	BaseCharList vTargetList;
	pOwnerChar->GetTargetCharListByCylinder( pOwnerChar->GetWorldPosition(),
											 m_fTeleportRange,
											 360.0f,
											 m_fUpHeight,
											 m_fUnderHeight,
											 vTargetList );

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);

		if( !pChar ) continue;
		if( !pChar->IsNeedProcess() ) continue;
		if( pOwnerChar->GetCharName() == pChar->GetCharName() ) continue;			// 스킬 사용자는 처리하지 않음
		if( !pStage->IsCollisionCheckByMode() ) continue;
		if( pOwnerChar->GetTeam() != pChar->GetTeam() ) continue;
		if( pChar->IsProtectState() ) continue;

		ioNpcChar *pNpc = ToNpcChar( pChar );
		if( pNpc && Help::IsMonsterDungeonMode(pStage->GetModeType()) )	
			if( COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )	
				continue;

		if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		// 왕관 쓰고 있으면 제외
		if( pChar->IsHasCrown() )
			continue;

		D3DXVECTOR3 vEndPos, vOwnerPos, vTargetPos;
		
		vOwnerPos = pOwnerChar->GetWorldPosition();
		vTargetPos = pChar->GetWorldPosition();
		vEndPos = vTargetPos - vOwnerPos;
		vEndPos.y = 0.0f;

		vEndPos += m_vCirclePos;

		// Check Drop Pos
		if( pStage )
		{
			vEndPos = pOwnerChar->GetEnableTeleportPosition( m_vCirclePos, pChar, m_iEnableAngleCnt, m_fEnableRange, m_fEnableGapHeight );
		}

		float fTimeRate = FLOAT1;
		ioHashString szSkillMotion;

		switch( m_TeleportType )
		{
		case TT_MASS_TELEPORT:
			{
				fTimeRate = m_fStartMotionRate;
				szSkillMotion = m_SkillStartMotion;
			}
			break;
		default:
			{
				float fAniRate = GetSkillMotionRate();

				fTimeRate = fAniRate * m_fExtraAniRate;

				szSkillMotion = GetSkillMotionName();
			}
			break;
		}

		float fCurEndRate = m_fEndMotionRate * m_fExtraEndAniRate;
		if( fCurEndRate <= 0.0f )
			fCurEndRate = FLOAT1;

		pChar->SetLastAttackerName( pOwnerChar->GetCharName(), GetName() );
		pChar->SetTeleportState( szSkillMotion,
								 fTimeRate,
								 m_SkillEndMotion,
								 fCurEndRate,
								 vEndPos );
	}
}

void ioCountTeleportAttackSkill2::SetExtraEndAniRate( float fRate )
{
	if( fRate > 0.0f )
		m_fExtraEndAniRate = FLOAT1 - fRate/FLOAT100;
	else
		m_fExtraEndAniRate = FLOAT1;

	if( m_fExtraEndAniRate <= 0.0f )
		m_fExtraEndAniRate = FLOAT1;
}

D3DXVECTOR3 ioCountTeleportAttackSkill2::GetMagicCirclePos( ioBaseChar *pChar, ioPlayStage *pStage )
{
	D3DXVECTOR3 vCurPos = ioMath::VEC3_ZERO;

	if( !pChar ) return vCurPos;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;

	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
	}

	vCurPos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
	
	return vCurPos;
}

void ioCountTeleportAttackSkill2::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	m_State = SS_ACTION;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
	}

	m_vCirclePos = GetMagicCirclePos( pChar, pStage );

	ioBaseChar *pClientChar = NULL;
	if( pStage )
	{
		pClientChar = pStage->GetOwnerChar();
	}

	if( pClientChar && pClientChar->GetTeam() == pChar->GetTeam() )
	{
		ioMapEffect *pMapEffect = NULL;
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );

		if( pMapEffect )
		{
			if( m_dwCurMapEffect != -1 )
				pChar->EndMapEffect( m_dwCurMapEffect );

			m_dwCurMapEffect = pMapEffect->GetUniqueID();
		}
	}

	EndPreDelayEffect( pChar );
	SetTeleportAttackSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::SetMassTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_MASS_TELEPORT;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = m_SkillStartMotion;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime;

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID != -1 )
	{
		float fTimeRate = m_fStartMotionRate;
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	}

	if( m_dwTargetCircleEffect != -1 )
	{
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );
		m_dwTargetCircleEffect = -1;
	}

	ioBaseChar *pClientChar = NULL;
	if( pStage )
	{
		pClientChar = pStage->GetOwnerChar();
	}

	if( pClientChar && pClientChar->GetTeam() != pChar->GetTeam() )
	{
		ioMapEffect *pMapEffect = NULL;
		if( pChar->GetTeam() == TEAM_RED )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_EnemyRedEffect, m_vCirclePos );
		}
		else if( pChar->GetTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_EnemyBlueEffect, m_vCirclePos );
		}
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
				pMapEffect = pChar->CreateMapEffectBySkill( m_EnemyBlueEffect, m_vCirclePos );
			else
				pMapEffect = pChar->CreateMapEffectBySkill( m_EnemyRedEffect, m_vCirclePos );
		}

		if( pMapEffect )
		{
			if( m_dwCurEnemyMapEffect != -1 )
				pChar->EndMapEffect( m_dwCurEnemyMapEffect );

			m_dwCurEnemyMapEffect = pMapEffect->GetUniqueID();
		}
	}

	CheckTeleportTarget( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::CheckTeleportAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_TELEPORT_ATTACK_FIRE;

	FindTarget( pChar, pStage );

	for( TargetDirectionList::iterator iter = m_TargetDirectionList.begin(); iter != m_TargetDirectionList.end(); ++iter )
	{
		TargetInfo Info = *iter;

		if( m_bTargetPosAttack )
			AttackFireToPos( pChar, pStage, Info.vPos, Info.vDir, Info.eState );
		else
			AttackFireToDir( pChar, pStage, Info.vDir, Info.eState );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		kPacket << m_vCirclePos;
		int iSize = (int)m_TargetDirectionList.size();
		kPacket << iSize;
		if( m_bTargetPosAttack )
		{
			for( int i = 0; i < iSize; ++i )
			{
				kPacket << (byte)m_TargetDirectionList[i].eState;
				kPacket << m_TargetDirectionList[i].vPos;
				kPacket << m_TargetDirectionList[i].vDir;
			}
		}
		else
		{
			for( int i = 0; i < iSize; ++i )
			{
				kPacket << (byte)m_TargetDirectionList[i].eState;
				kPacket << m_TargetDirectionList[i].vDir;
			}
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCountTeleportAttackSkill2::FindTarget( ioBaseChar* pChar, ioPlayStage* pStage )
{
	m_TargetDirectionList.clear();

	//타겟 리스트 가져오기
	FD_RangeMultipleTarget kFindPred( pChar, m_fTargetMaxRange, 0, 360, true );
	BaseCharList TargetList, NotTargetList;
	kFindPred.GetFindCharList( pStage->GetBaseCharList(), TargetList, NotTargetList, true, m_vCirclePos );

	//발사방향 계산
	D3DXVECTOR3 vPos = m_vCirclePos;
	vPos.y += m_fHeightOffset;
	for( BaseCharList::iterator iter = TargetList.begin(); iter != TargetList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		if( pTarget )
		{
			TargetInfo Info;			
			Info.vPos = pTarget->GetWorldPosition();
			Info.vDir = pTarget->GetMidPositionByRate() - vPos;
			D3DXVec3Normalize( &Info.vDir, &Info.vDir );
			
			//타겟 상태 체크
			float fBottomHeight = pTarget->GetBottomHeight();
			if( 5.0f < pTarget->GetWorldPosition().y - fBottomHeight )
			{
				Info.eState	= TS_AIR;
			}
			else
			{
				Info.eState	= TS_NORMAL;
			}

			m_TargetDirectionList.push_back( Info );
		}
	}

	//타겟이 없으면 지정한 각도 세팅
	if( m_TargetDirectionList.empty() )
	{
		for( int i = 0; i < (int)m_DefaultFireList.size(); ++i )
		{
			float fCurAngle = ioMath::ArrangeHead( m_DefaultFireList[i].dir );

			D3DXVECTOR3	vDir;
			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD( fCurAngle ), 0.0f, 0.0f );

			vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vDir = qtAngle * vDir;
			D3DXVec3Normalize( &vDir, &vDir );

			TargetInfo Info;
			Info.vPos = vPos + ( vDir * m_DefaultFireList[i].range );
			Info.vDir = vDir;
			Info.eState = TS_NORMAL;
			m_TargetDirectionList.push_back( Info );
		}
	}
}

void ioCountTeleportAttackSkill2::AttackFireToDir( ioBaseChar* pChar, ioPlayStage* pStage, D3DXVECTOR3& vDir, TargetState eState )
{
	WeaponInfoList List;
	if( GetSkillUseType() == SUT_NORMAL )
	{		
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = GetAttribute().m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_NomralAttackByAir.m_vWeaponInfoList;
			break;
		}
	}
	else
	{
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = GetAttribute().m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_JumpAttackByAir.m_vWeaponInfoList;
			break;
		}
	}
	
	for( WeaponInfoList::const_iterator iter = List.begin(); iter != List.end(); ++iter )
	{
		D3DXVECTOR3 vPos = m_vCirclePos;
		vPos.y += m_fHeightOffset;

		const WeaponInfo& rkInfo = *iter;
		ioBombMissileWeapon3* pWeapon = ToBombMissileWeapon3( pChar->SkillFireExplicit( rkInfo, GetName(), pChar->IncreaseWeaponIndexBase(), vPos, vDir ) );
		if( pWeapon && eState != TS_NONE )
			pWeapon->SetDsiableChangeDir( true );
	}
}

void ioCountTeleportAttackSkill2::AttackFireToPos( ioBaseChar* pChar, ioPlayStage* pStage, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, TargetState eState )
{
	WeaponInfoList List;
	if( GetSkillUseType() == SUT_NORMAL )
	{		
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = GetAttribute().m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_NomralAttackByAir.m_vWeaponInfoList;
			break;
		}
	}
	else
	{
		switch( eState )
		{
		case TS_NONE:
		case TS_NORMAL:
			List = GetAttribute().m_vWeaponInfoList;
			break;
		case TS_AIR:
			List = m_JumpAttackByAir.m_vWeaponInfoList;
			break;
		}
	}

	for( WeaponInfoList::const_iterator iter = List.begin(); iter != List.end(); ++iter )
	{
		vPos.y += m_fHeightOffset;
		const WeaponInfo& rkInfo = *iter;

		pChar->SkillFireExplicit( rkInfo, GetName(), pChar->IncreaseWeaponIndexBase(), vPos, vDir );
	}
}

int ioCountTeleportAttackSkill2::ReduceNeedCount( int iNeedCount )
{
	if( m_TeleportType == TT_AUTOTARGET && m_iCurUseCount == m_iMaxUseCount )
	{
		m_fCurRecoveryCount = 0.f;
		m_dwTicCheckTime = FRAMEGETTIME();
	}
	m_iCurUseCount -= iNeedCount;
	m_iCurUseCount = max( m_iCurUseCount, 0 );

	return m_iCurUseCount;
}