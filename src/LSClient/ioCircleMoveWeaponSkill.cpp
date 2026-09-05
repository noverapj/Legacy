

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioCircleMoveWeaponSkill.h"

#include "WeaponDefine.h"
#include "BuffDefine.h"

ioCircleMoveWeaponSkill::ioCircleMoveWeaponSkill()
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fMoveRange = 0;

	m_dwCurSkillProtectedTime = 0;
}

ioCircleMoveWeaponSkill::ioCircleMoveWeaponSkill( const ioCircleMoveWeaponSkill &rhs )
: ioNormalSkill( rhs ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
 m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fRandomRange( rhs.m_fRandomRange ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime )
{
	m_State = SS_NONE;

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	m_bSetHandMesh = false;
	m_dwFireStartTime = 0;

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fMoveRange = 0;

	m_dwCurSkillProtectedTime = 0;
}

ioCircleMoveWeaponSkill::~ioCircleMoveWeaponSkill()
{
}

void ioCircleMoveWeaponSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	LoadOwnerBuffList( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_circle_effect", "", szBuf, MAX_PATH );
	m_TargetCircleEffect = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );

	m_fRandomRange = rkLoader.LoadFloat_e( "weapon_random_range", 0.0f );
	m_dwSkillProtectedTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

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
}


void ioCircleMoveWeaponSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_OwnerBuffList.reserve( iCnt );
	
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

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

ioSkill* ioCircleMoveWeaponSkill::Clone()
{
	return new ioCircleMoveWeaponSkill( *this );
}

bool ioCircleMoveWeaponSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fMoveRange = 0;

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();
	m_dwCurSkillProtectedTime = m_dwSkillStartTime;

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	SetWaitState( pChar );

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioCircleMoveWeaponSkill::OnSkillEnd( ioBaseChar *pChar )
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

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fMoveRange = 0;

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_dwCurSkillProtectedTime = 0;

	m_State = SS_NONE;
}

void ioCircleMoveWeaponSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
	{
		fValue = pUpInfo->GetValue(pStage);
	}

	float fCurRange = m_fCircleRange + fValue;
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_WAIT:
		if( pChar )
		{
			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetActionState( pChar, pStage );
				}
			}
		}
		break;
	case SS_ACTION:
		{
			ProcessAction( pChar );
		}
		break;
	}
}

bool ioCircleMoveWeaponSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMoveWeaponSkill::SetNormalSkill( ioBaseChar *pChar )
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

void ioCircleMoveWeaponSkill::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	if( !m_TargetCircleEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_TargetCircleEffect, vPos );

		if( pMapEffect )
		{
			if( m_dwTargetCircleEffect != -1 )
				pChar->EndMapEffect( m_dwTargetCircleEffect, false );

			m_dwTargetCircleEffect = pMapEffect->GetUniqueID();
		}
	}
}

void ioCircleMoveWeaponSkill::SetActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_ACTION;
	AddOwnerBuff( pChar );

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
		fValue = pUpInfo->GetValue(pStage);

	float fCurRange = m_fCircleRange + fValue;


	m_dwPreDelayStartTime = 0;
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, fCurRange, m_fCircleHeightGap, true, true );
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

		if( m_dwTargetCircleEffect != -1 )
		{
			pChar->EndMapEffect( m_dwTargetCircleEffect, false );
			m_dwTargetCircleEffect = -1;
		}
	}

	// CharDir
	m_vAttackDir = m_vCirclePos - pChar->GetWorldPosition();
	m_fMoveRange = D3DXVec3Length( &m_vAttackDir );
	D3DXVECTOR3 vCharDir = m_vAttackDir;
	vCharDir.y = 0.0f;
	D3DXVec3Normalize( &vCharDir, &vCharDir );
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vCharDir );
	pChar->SetTargetRotAndMoveDirChange( qtRot, false );
	pChar->SetWorldOrientation( qtRot );

	// WeaponMove
	DWORD dwSeed = pChar->GetRandomSeed();
	IORandom random;
	random.SetRandomSeed( dwSeed );

	m_iCurWeaponCnt = 0;
	m_vWeaponMoveInfoList.clear();

	int iAttackCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i < iAttackCnt; ++i )
	{
		D3DXVECTOR3 vPos = m_vCirclePos;
		if( m_fRandomRange > 0.0f )
		{
			int iAngle = random.Random( 360 );
			int iRange = (int)(m_fRandomRange);
			iRange = random.Random(iRange);

			int iXPos = cos((double) iAngle ) * iRange;
			int iZPos = sin((double) iAngle ) * iRange;
			vPos.x += iXPos;
			vPos.z += iZPos;
		}

		D3DXVECTOR3 vDir = vPos - pChar->GetWorldPosition();
		vDir.y = 0.0f;

		float fRate = FLOAT1;
		float fRange = D3DXVec3Length( &vDir );
		D3DXVec3Normalize( &vDir, &vDir );

		float fCurWeaponFloatPower = 0.0f;
		float fCurWeaponMoveSpeed = 0.0f;
		if( fRange > 0.0f )
		{
			fRate = fRange / (m_fCircleRange+m_fRandomRange);

			// CurFloat
			fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
			fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

			float fCurGravity = 0.0f;
			float fWeaponMoveSpeed = 0.0f;
			ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
			if( pStage )
				fCurGravity = pStage->GetRoomGravity();

			float fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower );
			if( fFlightDuration > 0.0f )
			{
				fWeaponMoveSpeed = (m_fCircleRange+m_fRandomRange) / (fFlightDuration / FLOAT1000);
			}

			fCurWeaponMoveSpeed = fWeaponMoveSpeed * fRate;
		}

		WeaponMoveInfo kInfo;
		kInfo.m_vMoveDir = vDir;
		kInfo.m_fMoveSpeed = fCurWeaponMoveSpeed;
		kInfo.m_fFloatPower = fCurWeaponFloatPower;

		m_vWeaponMoveInfoList.push_back( kInfo );
	}

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

void ioCircleMoveWeaponSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	SetActionState( pChar, pStage );
}

bool ioCircleMoveWeaponSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( m_dwCurSkillProtectedTime < dwCurTime && dwCurTime < m_dwCurSkillProtectedTime + m_dwSkillProtectedTime )
	{
		return true;
	}
	else
	{
		if( IsPreDelayProtect() )
		{
			if( m_dwPreDelayStartTime != 0 )
				return true;
		}

		if( m_dwPreDelayStartTime == 0 )
		{
			if( m_dwFireMotionEndTime > dwCurTime )
				return true;
		}
	}

	return false;
}

bool ioCircleMoveWeaponSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State != SS_ACTION )
		return false;

	if( m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
	{
		return true;
	}
	
	return false;
}

bool ioCircleMoveWeaponSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCircleMoveWeaponSkill::ProcessAction( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != SS_ACTION ) return;
}

void ioCircleMoveWeaponSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	switch( pWeapon->GetType() )
	{
	case ioWeapon::WT_PRISON_WEAPON:
		{
			ioPrisonWeapon* pPrison = ToPrisonWeapon( pWeapon );
			if( pPrison )						
			{
				pPrison->SetMoveDir( m_vAttackDir );
				pPrison->SetMoveRange( m_fMoveRange );
				pPrison->SetPrisionPos( m_vCirclePos );
			}
		}
		break;
	default:
		{
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

			m_iCurWeaponCnt++;
		}		
	}
}

float ioCircleMoveWeaponSkill::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "CirCleMoveWeapon - [ExpectionValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}

void ioCircleMoveWeaponSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		ioDummyBuff* pBuff = ToDummyBuff( pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), GetOwnerItem()->GetName(), this ) );
		if( pBuff )	
			pBuff->SetWeaponDeadIndex( m_dwWeaponBaseIndex );
	}
}

bool ioCircleMoveWeaponSkill::CheckUseBuff( ioBaseChar *pChar )
{
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( pChar->HasBuff( m_OwnerBuffList[i].m_BuffName ) )
			return true;
	}

	return false;
}