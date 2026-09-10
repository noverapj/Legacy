

#include "stdafx.h"

#include "ioTeleportSkill.h"
#include "ioNpcChar.h"

ioTeleportSkill::ioTeleportSkill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwCurEnemyMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_fExtraEndAniRate = FLOAT1;

	m_dwWaitStartTime = 0;
}

ioTeleportSkill::ioTeleportSkill( const ioTeleportSkill &rhs )
: ioNormalSkill( rhs ),
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
 m_dwPreDelayProtectTime( rhs.m_dwPreDelayProtectTime )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwCurEnemyMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_fExtraEndAniRate = FLOAT1;

	m_dwWaitStartTime = 0;
}

ioTeleportSkill::~ioTeleportSkill()
{
}

void ioTeleportSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

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
	m_dwPreDelayProtectTime = rkLoader.LoadInt_e( "pre_delay_protect_time", 0 );
}

ioSkill* ioTeleportSkill::Clone()
{
	return new ioTeleportSkill( *this );
}

bool ioTeleportSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
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
	case TT_NOTMOVE:
	case TT_AUTOTARGET:
	case TT_MASS_TELEPORT:
		SetWaitState( pChar );
		break;
	case TT_IMMEDIATELY:
		SetTeleportState( pChar, NULL );
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

void ioTeleportSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

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

void ioTeleportSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
		if( m_TeleportType == TT_AUTOTARGET )
		{
			InfinityRangeTeleportTarget( pChar, pStage );
		}
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

		if( pChar->IsNeedProcess() )
		{
			if( m_TeleportType == TT_NORMAL && m_SkillUseType == SUT_JUMP )
			{
				if( m_dwWaitStartTime > 0 && m_dwWaitStartTime+m_dwJumpTeleportDuration < dwCurTime )
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
				case TT_NOTMOVE:
					SetEndState( pChar );
					break;
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
		break;
	case SS_CANCEL:
		break;
	}
}

bool ioTeleportSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioTeleportSkill::SetNormalSkill( ioBaseChar *pChar )
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
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioTeleportSkill::SetWaitState( ioBaseChar *pChar )
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

void ioTeleportSkill::SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_TELEPORT;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
	}

	m_vCirclePos = GetMagicCirclePos( pChar, pStage );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}

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
	SetNormalSkill( pChar );

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

void ioTeleportSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	rkPacket >> m_State;

	switch( m_State )
	{
	case SS_TELEPORT:
		SetTeleportState( pChar, pStage );
		break;
	case SS_ACTION:
		SetActionState( pChar, pStage );
		break;
	case SS_MASS_TELEPORT:
		SetMassTeleportState( pChar, pStage );
		break;
	}
}

bool ioTeleportSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;
	return false;
}

void ioTeleportSkill::SetEndState( ioBaseChar *pChar )
{
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

	pChar->DestroyMagicCircle();

	if( m_TeleportType != TT_NOTMOVE )
	{
		if( m_TeleportType == TT_AUTOTARGET )
		{
			D3DXVECTOR3 vOffSet( 0.0f, m_fHeightRange, 0.0f );
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCircleTargetPos() + vOffSet;
			D3DXQUATERNION qtTargetOrientation = pChar->GetMagicCircleTargetRot();
			D3DXVECTOR3 vNewPos = vTargetPos - ( ( qtTargetOrientation * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) ) * m_fBackRange );
			pChar->SetWorldPosition( vNewPos );
			pChar->SetTargetRotAndMoveDirChange( qtTargetOrientation );
		}
		else if( m_TeleportType == TT_MASS_TELEPORT )
		{
			D3DXVECTOR3 vEndPos = pChar->GetEnableTeleportPosition( m_vCirclePos, pChar, m_iEnableAngleCnt, m_fEnableRange, m_fEnableGapHeight );
			pChar->SetWorldPosition( vEndPos );
		}
		else
		{
			pChar->SetWorldPosition( m_vCirclePos );
		}
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

bool ioTeleportSkill::IsProtected( int iDefenseBreakType ) const
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
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if( m_dwPreDelayProtectTime == 0 || m_dwPreDelayProtectTime + m_dwPreDelayStartTime > dwCurTime )
				return true;
		}
	}

	if( m_dwPreDelayStartTime == 0 )
		return true;

	return false;
}

bool ioTeleportSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

void ioTeleportSkill::CheckTeleportTarget( ioBaseChar *pOwnerChar, ioPlayStage *pStage )
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

void ioTeleportSkill::InfinityRangeTeleportTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioTeleportSkill::InfinityRangeTeleportTarget() - Owner is NULL" );
		return;
	}

	if( !pChar->IsNeedProcess() )
		return;

	float fPrevDistSq = m_fCircleRange;
	ioBaseChar *pTargetChar = pChar;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarChar = (*iter);

		if( !pTarChar )
			continue;

		if( pTarChar->IsSystemState() )
			continue;
			
		if( !pTarChar->IsEnableTarget() )
			continue;

		D3DXVECTOR3 vDiff = pTarChar->GetWorldPosition() - pChar->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )	// Enemy
		{
			pTargetChar = pTarChar;
			fPrevDistSq = fDistSq;
		}
	}

	if( pTargetChar )
	{
		if( pChar->GetMagicCircleTarget() != pTargetChar->GetCharName() )
			pStage->SetCameraToOwnerExplicit( pChar );
		pChar->SetMagicCircleTarget( pTargetChar->GetCharName() );
	}
}

void ioTeleportSkill::SetExtraEndAniRate( float fRate )
{
	if( fRate > 0.0f )
		m_fExtraEndAniRate = FLOAT1 - fRate/FLOAT100;
	else
		m_fExtraEndAniRate = FLOAT1;

	if( m_fExtraEndAniRate <= 0.0f )
		m_fExtraEndAniRate = FLOAT1;
}

D3DXVECTOR3 ioTeleportSkill::GetMagicCirclePos( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioTeleportSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
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
		if( pChar->GetTeam() == TEAM_RED )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
		else if( pChar->GetTeam() == TEAM_BLUE )
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
				pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
			else
				pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}

		if( pMapEffect )
		{
			if( m_dwCurMapEffect != -1 )
				pChar->EndMapEffect( m_dwCurMapEffect );

			m_dwCurMapEffect = pMapEffect->GetUniqueID();
		}
	}

	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << m_State;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTeleportSkill::SetMassTeleportState( ioBaseChar *pChar, ioPlayStage *pStage )
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