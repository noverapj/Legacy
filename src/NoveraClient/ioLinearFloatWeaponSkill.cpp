

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioLinearFloatWeaponSkill.h"

#include "WeaponDefine.h"

ioLinearFloatWeaponSkill::ioLinearFloatWeaponSkill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();
}

ioLinearFloatWeaponSkill::ioLinearFloatWeaponSkill( const ioLinearFloatWeaponSkill &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
 m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
 m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fWeaponGapDistance( rhs.m_fWeaponGapDistance ),
 m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
 m_fRevisionRate( rhs.m_fRevisionRate ),
 m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
 m_fUpLimitRange( rhs.m_fUpLimitRange )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();
}

ioLinearFloatWeaponSkill::~ioLinearFloatWeaponSkill()
{
}

void ioLinearFloatWeaponSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fWeaponGapDistance = rkLoader.LoadFloat_e( "weapon_gap_distance", 0.0f );
	m_fRevisionRate = rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );

	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 1 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	//
	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
}

ioSkill* ioLinearFloatWeaponSkill::Clone()
{
	return new ioLinearFloatWeaponSkill( *this );
}

bool ioLinearFloatWeaponSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();

	pChar->AttachEffect( m_SkillEffectName );

	m_bSetHandMesh = false;
	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	m_State = SS_NONE;

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );

		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		SetActionState( pChar, pStage );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetCameraBuff( pChar );

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioLinearFloatWeaponSkill::OnSkillEnd( ioBaseChar *pChar )
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

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	m_dwCurMapEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioLinearFloatWeaponSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
	{
		fValue = pUpInfo->GetValue(pStage);
	}

	DWORD dwPreDelayTime = GetPreDelayTime();
	switch( m_State )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar, pStage );
		}
		break;
	case SS_ACTION:
		{
			ProcessAction( pChar );
		}
		break;
	}
}

bool ioLinearFloatWeaponSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioLinearFloatWeaponSkill::SetNormalSkill( ioBaseChar *pChar )
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

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurSkillAniID,
										m_AttackAttribute.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										m_AttackAttribute.m_dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioLinearFloatWeaponSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;
	m_dwPreDelayStartTime = 0;

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );	// 마법진 위치 동기화를 위함.
	}

	m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

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
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}

	CheckWeaponFloatInfo( pChar );

	EndPreDelayEffect( pChar );
	SetNormalSkill( pChar );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLinearFloatWeaponSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	SetActionState( pChar, pStage );
}

bool ioLinearFloatWeaponSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioLinearFloatWeaponSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State != SS_ACTION )
		return false;

	if( m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
	{
		return true;
	}
	
	return false;
}

bool ioLinearFloatWeaponSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioLinearFloatWeaponSkill::ProcessAction( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != SS_ACTION ) return;
}

void ioLinearFloatWeaponSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;
	float fFloat = 0.0f;

	if( COMPARE( m_iCurWeaponCnt, 0, (int)m_vWeaponMoveInfoList.size() ) )
	{
		vDir = m_vWeaponMoveInfoList[m_iCurWeaponCnt].m_vMoveDir;
		fSpeed = m_vWeaponMoveInfoList[m_iCurWeaponCnt].m_fMoveSpeed;
		fFloat = m_vWeaponMoveInfoList[m_iCurWeaponCnt].m_fFloatPower;
	}

	pWeapon->SetMoveDir( vDir );
	pWeapon->SetMoveSpeed( fSpeed );
	pWeapon->SetFloatPower( fFloat );

	ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
	if( pFloatWeapon )
	{
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

	m_iCurWeaponCnt++;
}

float ioLinearFloatWeaponSkill::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioLinearFloatWeaponSkill::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioLinearFloatWeaponSkill::CheckWeaponFloatInfo( ioBaseChar *pChar )
{
	// CharDir
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vCharDir = m_vCirclePos - vCharPos;
	vCharDir.y = 0.0f;

	float fFirstRange = D3DXVec3Length( &vCharDir );
	D3DXVec3Normalize( &vCharDir, &vCharDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vCharDir );
	pChar->SetTargetRotAndMoveDirChange( qtRot, false );
	pChar->SetWorldOrientation( qtRot );

	// WeaponMove
	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();

	int iAttackCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i < iAttackCnt; ++i )
	{
		float fGapDistance = m_fWeaponGapDistance * i;
		D3DXVECTOR3 vPos = m_vCirclePos + (vCharDir * fGapDistance);
		D3DXVECTOR3 vDir = vPos - vCharPos;
		vDir.y = 0.0f;

		float fRate = FLOAT1;
		float fRange = D3DXVec3Length( &vDir );
		D3DXVec3Normalize( &vDir, &vDir );

		float fCurWeaponFloatPower = 0.0f;
		float fCurWeaponMoveSpeed = 0.0f;
		if( fRange > 0.0f )
		{
			fRate = fRange / (m_fCircleRange+fGapDistance);

			// CurFloat
			fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
			fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

			float fCurGravity = 0.0f;
			float fWeaponMoveSpeed = 0.0f;
			ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
			if( pStage )
				fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

			float fFlightDuration = 0.0f;
			float fGapHeight = pChar->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
			float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
			if( fGapHeight2 > 0.0f )
			{
				fGapHeight += fGapHeight2;
				fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

				if( fFlightDuration > 0.0f )
					fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

				fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
			}
			else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
			{
				float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
				fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
				fRange -= 35.0f;

				if( fFlightDuration > 0.0f )
					fCurWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);
			}
			else
			{
				fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

				if( fFlightDuration > 0.0f )
					fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

				fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
			}
		}

		WeaponMoveInfo kInfo;
		kInfo.m_vMoveDir = vDir;
		kInfo.m_fMoveSpeed = fCurWeaponMoveSpeed;
		kInfo.m_fFloatPower = fCurWeaponFloatPower;

		m_vWeaponMoveInfoList.push_back( kInfo );
	}
}