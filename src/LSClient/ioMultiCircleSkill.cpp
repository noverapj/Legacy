

#include "stdafx.h"

#include "ioMultiCircleSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioMagicCircle.h"

ioMultiCircleSkill::ioMultiCircleSkill()
{
	m_State = SS_NONE;
	m_CircleState = CIS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAttackCnt = 0;
	m_dwCircleChangeTime = 0;
	m_dwTargetingEffectID = -1;
	m_bTargetingMarkUp = false;
	m_fTargetingMarkHeight = 0.0f;
}

ioMultiCircleSkill::ioMultiCircleSkill( const ioMultiCircleSkill &rhs )
: ioNormalSkill( rhs ),
 m_NormalCircle( rhs.m_NormalCircle ),
 m_NormalCircleFix( rhs.m_NormalCircleFix ),
 m_SpecialCircle( rhs.m_SpecialCircle ),
 m_SpecialCircleFix( rhs.m_SpecialCircleFix ),
 m_RedGuidCircle( rhs.m_RedGuidCircle ),
 m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_SkillEndMotion( rhs.m_SkillEndMotion ),
 m_fEndMotionRate( rhs.m_fEndMotionRate ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
 m_iMaxAttackCnt( rhs.m_iMaxAttackCnt ),
 m_bPreventRecoveryGauge( rhs.m_bPreventRecoveryGauge ),
 m_dwNormalDuration( rhs.m_dwNormalDuration ),
 m_dwSpecialDuration( rhs.m_dwSpecialDuration ),
 m_NormalAttackAttribute( rhs.m_NormalAttackAttribute ),
 m_SpecialAttackAttribute( rhs.m_SpecialAttackAttribute ),
 m_AttackType( rhs.m_AttackType ),
 m_szTargetingEffect( rhs.m_szTargetingEffect ),
 m_fTargetingMaxHeightOffset( rhs.m_fTargetingMaxHeightOffset ),
 m_fTargetingMinHeightOffset( rhs.m_fTargetingMinHeightOffset ),
 m_fTargetingMarkUpDownSpeed( rhs.m_fTargetingMarkUpDownSpeed ),
 m_vTargetingMarkAttackOffset( rhs.m_vTargetingMarkAttackOffset ),
 m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	m_State = SS_NONE;
	m_CircleState = CIS_NONE;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAttackCnt = 0;
	m_dwCircleChangeTime = 0;
	m_dwTargetingEffectID = -1;
	m_bTargetingMarkUp = false;
	m_fTargetingMarkHeight = 0.0f;
}

ioMultiCircleSkill::~ioMultiCircleSkill()
{
}

void ioMultiCircleSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "normal_circle", "", szBuf, MAX_PATH );
	m_NormalCircle = szBuf;
	rkLoader.LoadString_e( "normal_fix_circle", "", szBuf, MAX_PATH );
	m_NormalCircleFix = szBuf;

	rkLoader.LoadString_e( "special_circle", "", szBuf, MAX_PATH );
	m_SpecialCircle = szBuf;
	rkLoader.LoadString_e( "special_fix_circle", "", szBuf, MAX_PATH );
	m_SpecialCircleFix = szBuf;

	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	//
	m_dwNormalDuration = (DWORD)rkLoader.LoadInt_e( "normal_circle_duration", 0 );
	m_dwSpecialDuration = (DWORD)rkLoader.LoadInt_e( "special_circle_duration", 0 );

	// normal
	wsprintf_e( szKey, "normal_circle_attack_type" );
	m_NormalAttackAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "normal_circle_attack_resist" );
	m_NormalAttackAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "normal_circle_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_NormalAttackAttribute.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "normal_circle_attack_type_wounded_time" );
	m_NormalAttackAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "normal_circle_attack_type_wounded_loop_ani" );
	m_NormalAttackAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	// special
	wsprintf_e( szKey, "special_circle_attack_type" );
	m_SpecialAttackAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "special_circle_attack_resist" );
	m_SpecialAttackAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "special_circle_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_SpecialAttackAttribute.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "special_circle_attack_type_wounded_time" );
	m_SpecialAttackAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "special_circle_attack_type_wounded_loop_ani" );
	m_SpecialAttackAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	//
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "loop_protect_time", 0 );

	//
	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_iMaxAttackCnt = rkLoader.LoadInt_e( "max_attack_cnt", 0 );

	m_bPreventRecoveryGauge = rkLoader.LoadBool_e( "prevent_recovery_gauge_on_skill", false );

	m_AttackType = (AttackType)rkLoader.LoadInt_e( "attack_type", AT_CIRCLE );
	rkLoader.LoadString_e( "targeting_effect_name", "", szBuf, MAX_PATH );
	m_szTargetingEffect = szBuf;

	m_fTargetingMaxHeightOffset = rkLoader.LoadFloat_e( "targeting_max_height_offset", 0.0f );
	m_fTargetingMinHeightOffset = rkLoader.LoadFloat_e( "targeting_min_height_offset", 0.0f );
	m_fTargetingMarkUpDownSpeed = rkLoader.LoadFloat_e( "targeting_up_down_speed", 0.0f );

	m_vTargetingMarkAttackOffset.x = rkLoader.LoadFloat_e( "targeting_attack_offset_x", 0.0f );
	m_vTargetingMarkAttackOffset.y = rkLoader.LoadFloat_e( "targeting_attack_offset_y", 0.0f );
	m_vTargetingMarkAttackOffset.z = rkLoader.LoadFloat_e( "targeting_attack_offset_z", 0.0f );

	LoadBuffList( rkLoader );
}

void ioMultiCircleSkill::LoadBuffList( ioINILoader &rkLoader )
{
	int i, iCount;
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();
	iCount = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioMultiCircleSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.empty() )	
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<nCnt; ++i )
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
}

void ioMultiCircleSkill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar || m_OwnerBuffList.empty() )	
		return;

	int nCnt = m_OwnerBuffList.size();
	for( int i=0; i<nCnt; ++i )
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName, true );
}

ioSkill* ioMultiCircleSkill::Clone()
{
	return new ioMultiCircleSkill( *this );
}

bool ioMultiCircleSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_bReduceGauge = false;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwActionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAttackCnt = 0;
	m_dwCircleChangeTime = 0;

	m_State = SS_NONE;
	m_CircleState = CIS_NONE;

	pChar->SetSkillProtection();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetMagicCircle( pChar );
		SetTargetingMark( pChar );
		SetLoopState( pChar );
	}

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	if ( m_iMaxActiveCount > 0 )
		m_iCurActiveCount = m_iMaxActiveCount;

	return true;
}

void ioMultiCircleSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );
	RemoveOwnerBuff( pChar );

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_dwFirstLoopStartTime = 0;

	m_iCurAttackCnt = 0;
	m_dwCircleChangeTime = 0;

	m_State = SS_NONE;
	m_CircleState = CIS_NONE;

	if ( m_dwTargetingEffectID != -1 )
		pChar->EndMapEffect( m_dwTargetingEffectID );

	m_iCurActiveCount = 0;
}

void ioMultiCircleSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurRange = m_fCircleRange;

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetMagicCircle( pChar );
				SetTargetingMark( pChar );
				SetLoopState( pChar );
				AddOwnerBuff( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			//
			UpdateMagicCircle( pChar, pStage );
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			UpdateTargeingMark( pChar, pStage );
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetActionState( pChar, pStage );
				return;
			}
		}
		break;
	case SS_ACTION:
		if( m_dwMotionEndTime < dwCurTime )
		{
			if( m_iCurAttackCnt < m_iMaxAttackCnt )
				SetLoopState( pChar );
			else
				SetEndState( pChar );
		}
		else
		{
			if( m_dwFireStartTime < dwCurTime )
			{
				CheckCreateAttack( pChar, pStage );
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioMultiCircleSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioMultiCircleSkill::SetMagicCircle( ioBaseChar *pChar )
{
	if( pChar )
	{
		pChar->CreateMagicCircle( m_NormalCircle, m_NormalCircle, m_NormalCircle, m_fCircleOffSet );

		ioMagicCircle *pCircle = pChar->GetMagicCircle();
		if( pCircle )
		{
			pCircle->SetGalaxyCircle( m_NormalCircle, m_NormalCircleFix, m_SpecialCircle, m_SpecialCircleFix, pChar->GetCreator() );
			
			m_CircleState = CIS_NORMAL;
			m_dwCircleChangeTime = FRAMEGETTIME();
		}

		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
	}
}

void ioMultiCircleSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = m_dwActionStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->AttachEffect( m_StartEffect );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioMultiCircleSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;

	float fCurRange = m_fCircleRange;

	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false, true );
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	// team circle
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedGuidCircle, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueGuidCircle, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueGuidCircle, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedGuidCircle, m_vCirclePos );
	}

	if ( pChar->IsNeedProcess() && m_dwTargetingEffectID != -1 )
	{
		ioMapEffect* pTargetingMark = pStage->FindMapEffect( m_dwTargetingEffectID );
		m_fTargetingMarkHeight = pTargetingMark->GetWorldPosition().y;
	}

	SetNormalSkill( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();

		kPacket << SSS_ACTION;
		kPacket << m_fTargetingMarkHeight;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMultiCircleSkill::SetLoopState( ioBaseChar *pChar )
{
	if( m_LoopAnimation.IsEmpty() || !pChar )
	{
		m_dwLoopStartTime = 0;
		m_dwFirstLoopStartTime = 0;
		return;
	}

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_State = SS_LOOP;
	m_dwLoopStartTime = FRAMEGETTIME();
	if( m_dwFirstLoopStartTime == 0 )
		m_dwFirstLoopStartTime = m_dwLoopStartTime;

	EndPreDelayEffect( pChar );
}

void ioMultiCircleSkill::SetEndState( ioBaseChar *pChar )
{
	if( m_State == SS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_State = SS_END;
	pChar->DestroyMagicCircle();

	if( m_SkillEndMotion.IsEmpty() )
	{
		m_dwMotionStartTime = dwCurTime;
		m_dwMotionEndTime = m_dwMotionStartTime;
		return;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	pChar->AttachEffect( m_EndEffect );
}

void ioMultiCircleSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;
	rkPacket >> m_fTargetingMarkHeight;
	if( iSyncState == SSS_ACTION )
	{
		SetActionState( pChar, pStage );
	}
}

void ioMultiCircleSkill::CheckCreateAttack( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_iCurActiveCount--;
		m_iCurAttackCnt++;
		m_dwFireStartTime = 0;

		if( m_CircleState == CIS_NORMAL )
		{
			m_CircleState = CIS_NORMAL_FIX;
			ChangeMagicCircle( pChar, pStage, CIS_NORMAL_FIX );
		}
		else if( m_CircleState == CIS_SPECIAL )
		{
			m_CircleState = CIS_SPECIAL_FIX;
			ChangeMagicCircle( pChar, pStage, CIS_SPECIAL_FIX );
		}

		if ( m_AttackType == AT_CIRCLE )
		{
			pChar->SkillFireExplicit( GetCircleAttribute(),
				GetName(),
				m_dwWeaponBaseIndex,
				m_vCirclePos );
		}
		else if ( m_AttackType == AT_TARGETING )
		{
			D3DXVECTOR3 vAttackPos = pChar->GetWorldPosition();
			vAttackPos.y = m_fTargetingMarkHeight;
			D3DXVECTOR3 vPosOffset = pChar->GetWorldOrientation() * m_vTargetingMarkAttackOffset;
			vAttackPos += vPosOffset;
			pChar->SkillFireExplicit( GetCircleAttribute(),
				GetName(),
				m_dwWeaponBaseIndex,
				vAttackPos );
		}
	}
}

const WeaponInfo& ioMultiCircleSkill::GetCircleAttribute()
{
	switch( m_CircleState )
	{
	case CIS_SPECIAL:
	case CIS_SPECIAL_FIX:
		return m_SpecialAttackAttribute;
	}

	return m_NormalAttackAttribute;
}

bool ioMultiCircleSkill::IsEndState() const
{
	if( m_State == SS_END )
		return true;

	return false;
}

bool ioMultiCircleSkill::IsProtected( int iDefenseBreakType ) const
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

	switch( m_State )
	{
	case SS_NONE:
		return false;
	case SS_LOOP:
	case SS_ACTION:
		{
			/*
			if( m_dwFirstLoopStartTime > 0 &&
				m_dwLoopProtectTime > 0 &&
				m_dwFirstLoopStartTime + m_dwLoopProtectTime > dwCurTime )
				return true;
			*/

			if( m_dwFireMotionEndTime > dwCurTime )
				return true;
		}
		break;
	case SS_END:
		return false;
	}

	return false;
}

bool ioMultiCircleSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioMultiCircleSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioMultiCircleSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioMultiCircleSkill::UpdateMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_CircleState )
	{
	case CIS_NORMAL:
		if( m_dwCircleChangeTime+m_dwNormalDuration < dwCurTime )
		{
			m_CircleState = CIS_SPECIAL;
			m_dwCircleChangeTime = dwCurTime;

			ChangeMagicCircle( pChar, pStage, CIS_SPECIAL );
		}
		break;
	case CIS_SPECIAL:
		if( m_dwCircleChangeTime+m_dwSpecialDuration < dwCurTime )
		{
			m_CircleState = CIS_NORMAL;
			m_dwCircleChangeTime = dwCurTime;

			ChangeMagicCircle( pChar, pStage, CIS_NORMAL );
		}
		break;
	}
}

void ioMultiCircleSkill::ChangeMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage, CircleState eState )
{
	if( !pChar || !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioMagicCircle *pCircle = pChar->GetMagicCircle();
	if( pCircle )
	{
		switch( eState )
		{
		case CIS_NORMAL:
			pCircle->ChangeGalaxyCircle( ioMagicCircle::MCS_NORMAL, pStage );
			break;
		case CIS_NORMAL_FIX:
			pCircle->ChangeGalaxyCircle( ioMagicCircle::MCS_NORMAL_FIX, pStage );
			break;
		case CIS_SPECIAL:
			pCircle->ChangeGalaxyCircle( ioMagicCircle::MCS_SPECIAL, pStage );
			break;
		case CIS_SPECIAL_FIX:
			pCircle->ChangeGalaxyCircle( ioMagicCircle::MCS_SPECIAL_FIX, pStage );
			break;
		}
	}
}

void ioMultiCircleSkill::SetTargetingMark( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() || m_szTargetingEffect.IsEmpty() )
		return;

	D3DXVECTOR3 vPos = m_vCirclePos;
	vPos.y += m_fTargetingMaxHeightOffset;
	
	ioMapEffect* pMapReadyEffect = pChar->CreateMapEffectBySkill( m_szTargetingEffect, vPos );
	if( pMapReadyEffect )
		m_dwTargetingEffectID = pMapReadyEffect->GetUniqueID();
}

void ioMultiCircleSkill::UpdateTargeingMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;

	if ( m_dwTargetingEffectID == -1 )
		return;

	ioMapEffect* pTargetingMark = pStage->FindMapEffect( m_dwTargetingEffectID );
	if( !pTargetingMark )
		return;

	D3DXVECTOR3 vTargetingPos = pTargetingMark->GetWorldPosition();
	if ( pChar->IsSettedDirection() )
	{
		D3DXVECTOR3 vPreCirclePos = m_vCirclePos;
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );
		float fCircleHeightGap = m_vCirclePos.y - vPreCirclePos.y;
		vTargetingPos.x = m_vCirclePos.x;
		vTargetingPos.y += fCircleHeightGap;
		vTargetingPos.z = m_vCirclePos.z;
	}

	if ( m_fTargetingMarkUpDownSpeed > 0.0f )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fMoveDist = m_fTargetingMarkUpDownSpeed * fTimePerSec;
		if ( m_bTargetingMarkUp )
			vTargetingPos.y +=fMoveDist;
		else
			vTargetingPos.y -=fMoveDist;

		if ( m_bTargetingMarkUp && vTargetingPos.y > m_vCirclePos.y + m_fTargetingMaxHeightOffset )
			m_bTargetingMarkUp = false;
		else if ( !m_bTargetingMarkUp && vTargetingPos.y <= m_vCirclePos.y + m_fTargetingMinHeightOffset )
			m_bTargetingMarkUp = true;
	}

	pTargetingMark->SetWorldPosition( vTargetingPos );
}