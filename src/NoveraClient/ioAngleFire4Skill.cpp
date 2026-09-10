

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioAngleFire4Skill.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioAngleFire4Skill::ioAngleFire4Skill()
{
	ClearData();
}

ioAngleFire4Skill::ioAngleFire4Skill( const ioAngleFire4Skill &rhs )
	: ioAttackSkill( rhs ),
	m_SkillUpMotion( rhs.m_SkillUpMotion ),
	m_SkillDownMotion( rhs.m_SkillDownMotion ),
	m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
	m_SkillUpLoopMotion( rhs.m_SkillUpLoopMotion ),
	m_SkillDownLoopMotion( rhs.m_SkillDownLoopMotion ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_dwFireGuidDuration( rhs.m_dwFireGuidDuration ),
	m_ExtraGuidEffect( rhs.m_ExtraGuidEffect ),
	m_vAimInfoList( rhs.m_vAimInfoList ),
	m_szAimFireEffect( rhs.m_szAimFireEffect ),
//	m_szZeroAimFireEffect( rhs.m_szZeroAimFireEffect ),
	m_bVisibleAimMarkOnlyOwner( rhs.m_bVisibleAimMarkOnlyOwner ),
	m_fReduceTicGauge( rhs.m_fReduceTicGauge ),
	m_dwReduceGaugeTicTime( rhs.m_dwReduceGaugeTicTime ),
	m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fTargetAngle( rhs.m_fTargetAngle ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
 m_fTargetDownHeight( rhs.m_fTargetDownHeight ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_bUseWaitTime( rhs.m_bUseWaitTime ),
 m_dwWaitTime( rhs.m_dwWaitTime )
{
	ClearData();
}

ioAngleFire4Skill::~ioAngleFire4Skill()
{
	ClearData();
}

void ioAngleFire4Skill::ClearData()
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwLoopStartTime	= 0;
	m_dwAimCheckTime	= 0;
	//m_dwZeroAimCheckTime= 0;
	m_fCurMotionRate	= 0.0f;

	m_dwFireStartTime = 0;
	m_dwExtraGuidEffect = -1;

	m_szTargetName.Clear();

	//m_bZeroAimFire = false;

	m_dwCurTic = 0;
	m_fCoolTimeValue = 1.0;

	m_dwNextFireTime = 0;
	m_bReservedFireFromWaitTime = false;
}

void ioAngleFire4Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	rkLoader.LoadString_e( "skill_up_motion", "", szBuf, MAX_PATH );
	m_SkillUpMotion = szBuf;

	rkLoader.LoadString_e( "skill_down_motion", "", szBuf, MAX_PATH );
	m_SkillDownMotion = szBuf;


	rkLoader.LoadString_e( "skill_up_loop_motion", "", szBuf, MAX_PATH );
	m_SkillUpLoopMotion = szBuf;

	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	rkLoader.LoadString_e( "skill_down_loop_motion", "", szBuf, MAX_PATH );
	m_SkillDownLoopMotion = szBuf;

	m_dwLoopDuration = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//
	rkLoader.LoadString_e( "extra_guid_effect", "", szBuf, MAX_PATH );
	m_ExtraGuidEffect = szBuf;

	m_dwFireGuidDuration = rkLoader.LoadInt_e( "fire_guid_effect_duration", 0 );

	int iCnt = rkLoader.LoadInt_e( "aim_info_count", 0 );
	m_vAimInfoList.clear();
	m_vAimInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		AimInfo2 kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "aim%d_mark_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szAimMark = szBuf;
		wsprintf_e( szKey, "Zero_aim%d_mark_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szZeroAimMark = szBuf;
		wsprintf_e( szKey, "aim%d_time", i+1 );
		kInfo.m_dwAimTime = (DWORD)rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "zero_aim%d_time", i+1 );
		kInfo.m_dwZeroAimTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "aim%d_attack", i+1 );
		LoadWeaponInfo( szKey, kInfo.m_AimWeaponInfoList, rkLoader );

		wsprintf_e( szKey, "zero_aim%d_attack", i+1 );
		LoadWeaponInfo( szKey, kInfo.m_ZeroAimWeaponInfoList, rkLoader );

		m_vAimInfoList.push_back(kInfo);
	}

	rkLoader.LoadString_e( "aim_fire_effect", "", szBuf, MAX_PATH );
	m_szAimFireEffect = szBuf;

	/*rkLoader.LoadString_e( "zero_aim_fire_effect", "", szBuf, MAX_PATH );
	m_szZeroAimFireEffect = szBuf;*/

	m_bVisibleAimMarkOnlyOwner = rkLoader.LoadBool_e( "visible_aim_mark_only_owner", false );
	//////////////////////////////////////////////////////////////////////////
	m_fReduceTicGauge = rkLoader.LoadFloat_e( "reduce_gauge", FLOAT1 );
	m_dwReduceGaugeTicTime = rkLoader.LoadInt_e( "reduce_gauge_tic_time", 100 );

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	//
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	//////////////////////////////////////////////////////////////////////////
	m_bUseWaitTime = rkLoader.LoadBool_e( "use_wait_time", false );
	m_dwWaitTime = rkLoader.LoadInt_e( "fire_and_wait_time",  0 );
}

void ioAngleFire4Skill::LoadWeaponInfo( const char *szWeaponInfo, WeaponInfoList &rkWeaponInfoList, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_count", szWeaponInfo );
	int iAttackCount = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<iAttackCount; ++i )
	{
		WeaponInfo kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "%s_type%d", szWeaponInfo, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szWeaponInfo, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szWeaponInfo, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_wounded_time", szWeaponInfo, i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szWeaponInfo, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		rkWeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioAngleFire4Skill::Clone()
{
	return new ioAngleFire4Skill( *this );
}


void ioAngleFire4Skill::CalcCoolTimeValue( ioBaseChar *pChar )
{
	if( pChar == NULL )
	{
		m_fCoolTimeValue = FLOAT1;
		return;
	}

	// 쿨타임 성장요소 관련
	ioGrowthNormalUpInfo *pUpInfo = NULL;
	pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
	if( pUpInfo )
		m_fCoolTimeValue = pUpInfo->GetValue( pChar->GetCreator() );

	m_fCoolTimeValue = min( m_fCoolTimeValue, FLOAT100 );
	m_fCoolTimeValue = FLOAT1 + (m_fCoolTimeValue/FLOAT100);
}


bool ioAngleFire4Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	ClearData();

	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLoopState( pChar, g_SkillMaker.GetPlayStage() );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	pChar->AttachEffect( m_SkillEffectName );

	CheckSkillDesc( pChar );

	m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed );

	int iValue = m_vAimInfoList.size();
	m_iAimType = random.Random( iValue );

	SetSkillNameChatBubble( pChar );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_dwOldTime = FRAMEGETTIME();

	CalcCoolTimeValue( pChar );

	return true;
}

void ioAngleFire4Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	UpdateReduceGauge( pChar );

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime != 0 && m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			CheckAimTime();
			CheckKeyInput( pChar );
			CheckTarget( pChar, pStage );
			CheckVDir( pChar );
			UpdateAimMark( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				float fGauge = pChar->GetCurItemSkillGauge( GetOwnerItem() );
				if( fGauge <= 0 )
				{
					m_SkillState = SS_WILL_DIE;
					return;
				}
				/*if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopDuration < dwCurTime )
				{
					SetFireState( pChar );
					return;
				}
				else */
				if( pChar->IsAttackKey() || m_bReservedFireFromWaitTime )
				{
					SetFireState( pChar );
					return;
				}
			}
		}
		break;
	case SS_WAIT:
		{
			CheckAimTime();
			CheckKeyInput( pChar );
			CheckTarget( pChar, pStage );
			CheckVDir( pChar );
			UpdateAimMark( pChar, pStage );

			if( pChar->IsNeedProcess()  )
			{
				float fGauge = pChar->GetCurItemSkillGauge( GetOwnerItem() );
				if( fGauge <= 0 )
				{
					m_SkillState = SS_WILL_DIE;
					return;
				}

				if( pChar->IsAttackKey()  )
					m_bReservedFireFromWaitTime = true;
			}

			if( dwCurTime > m_dwNextFireTime )
				m_SkillState = SS_LOOP;
		}
		break;
	case SS_FIRE:
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
			if( pTarget )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
				if( pMapEffect )
					pMapEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
			}
			else
				pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );

			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				m_SkillState = SS_LOOP;

				ioEntityGroup *pGrp = pChar->GetGroup();
				if( !pGrp ) return;
				
				pGrp->SetLoopAni( m_SkillUpLoopMotion, 0.0f, 0.0f );
				pGrp->SetLoopAni( m_SkillDownLoopMotion, 0.0f, 0.0f );
				pGrp->SetLoopAni( m_SkillLoopMotion, 0.0f, FLOAT1 );

				if( m_bUseWaitTime )
				{
					m_dwNextFireTime = dwCurTime + m_dwWaitTime;
					m_SkillState = SS_WAIT;
					m_bReservedFireFromWaitTime = false;
				}

			}
		}
		break;
	}
}

void ioAngleFire4Skill::CheckVDir( ioBaseChar *pChar )
{
	if( m_szTargetName.IsEmpty() )
		return;

	ioBaseChar *pTarget = pChar->GetBaseChar( m_szTargetName );
	if( !pTarget )	return;

	D3DXVECTOR3 vPos1 = pChar->GetWorldPosition();
	D3DXVECTOR3 vPos2 = pTarget->GetWorldPosition();

	pChar->SetTargetRotToTargetPos( vPos2, true );

	m_vAttackDir = vPos2 - vPos1;
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	D3DXVECTOR3 vSightDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	float fAngle = 0.0f;
	if( m_vAttackDir != vSightDir )
	{
		float fDotValue = D3DXVec3Dot( &m_vAttackDir, &vSightDir );
		fAngle = RADtoDEG( acosf(fDotValue) );
	}

	//------------------------------------------------------------------------
	// 모션 체크 부분.
	// CheckMotion

	m_fCurMotionRate = 0.0f;
	m_fCurMotionRate = fAngle /FLOAT90;

	ioEntityGroup	*pGrp = pChar->GetGroup();
	ioAnimation		*pUp, *pCenter, *pDown;
	pUp = pCenter = pDown = NULL;

	pUp = pGrp->GetLoopInPlay( m_SkillUpLoopMotion );
	pCenter = pGrp->GetLoopInPlay( m_SkillLoopMotion );
	pDown = pGrp->GetLoopInPlay( m_SkillDownLoopMotion );

	if( !pUp || !pCenter || !pDown )
	{
		LOG.PrintTimeAndLog( 0, "ioAngleFire4Skill::CheckVDir - Motion Error" );
		return;
	}

	if( m_vAttackDir.y == 0.0f || m_fCurMotionRate == 0.0f )		// center
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	else if( m_vAttackDir.y > 0.0f )	// center & up
	{
		pUp->ForceWeight( m_fCurMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	else								// center & down
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		pDown->ForceWeight( m_fCurMotionRate );
	}
}

void ioAngleFire4Skill::OnSkillEnd( ioBaseChar *pChar )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( m_bSetHandMesh && !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bSetHandMesh = false;

	EndPreDelayEffect( pChar );
	pChar->EndEffect( m_SkillEffectName, false );

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	EndCameraBuff( pChar );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCurMoveSpeed( 0.0f );

	m_SkillState = SS_NONE;
	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	DestroyAimMark( pChar );
}

bool ioAngleFire4Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_FIRE:
	case SS_WAIT:
		return false;
	/*case SS_FIRE:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;

		return true;*/
	case SS_WILL_DIE:
		return true;
	}

	return true;
}


void ioAngleFire4Skill::UpdateReduceGauge( ioBaseChar *pOwner )
{
	DWORD dwTime = FRAMEGETTIME();
	float NowfGauge = 0;
	DWORD dwSubTime = dwTime - m_dwOldTime;
	if( dwSubTime >= m_dwReduceGaugeTicTime )
	{
		m_dwOldTime = dwTime;
		float fSubGauge = (float)dwSubTime / (float)m_dwReduceGaugeTicTime * m_fReduceTicGauge / m_fCoolTimeValue;
		NowfGauge =	pOwner->ReduceCurSkillGaugeBySkill(GetOwnerItem(), fSubGauge );
	}
	
}


bool ioAngleFire4Skill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
	case SS_WAIT:
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < FRAMEGETTIME() )
			return true;

		break;
	}

	return false;
}

void ioAngleFire4Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SNT_CHANGE:
		{
			m_dwAimCheckTime = FRAMEGETTIME();
		//	m_dwZeroAimCheckTime = 0;
			DestroyAimMark( pChar );

			rkPacket >> m_szTargetName;
		}
		break;
	case SNT_ATTACK:
		{
			rkPacket >> m_vAttackDir;
		//	rkPacket >> m_bZeroAimFire;
			SetFireState( pChar );
		}
		break;
	}
}

void ioAngleFire4Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !pWeapon ) return;

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	pWeapon->SetMoveDir( m_vAttackDir );

	ioTraceWeaponType2	*pTrace2 = ToTraceWeaponType2( pWeapon );
	if( pTrace2 )
		pTrace2->SetTraceTarget( m_szTargetName );

	ioTraceWeapon7 *pTrace7 = ToTraceWeapon7( pWeapon );
	if( pTrace7 )
		pTrace7->SetTraceTarget( m_szTargetName );
}

void ioAngleFire4Skill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	EndPreDelayEffect( pChar );

	m_SkillState = SS_LOOP;
	m_fCurMotionRate = 0.0f;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( m_SkillUpLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillDownLoopMotion, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_SkillLoopMotion, 0.0f, FLOAT1 );

	if( pChar->IsNeedProcess() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_ExtraGuidEffect );
		if( pEffect )
		{
			m_dwExtraGuidEffect = pEffect->GetUniqueID();
		}
	}

	m_dwAimCheckTime = dwCurTime;
}

void ioAngleFire4Skill::SetFireState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	DestroyAimMark( pChar );
	AttackSkillFire( pChar );

	ioPlayStage *pStage = pChar->GetCreator();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( pStage && pTarget )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		/*if( m_bZeroAimFire )
		{
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szZeroAimMark, vPos, vScale );
			if( pMapEffect )
				m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = pMapEffect->GetUniqueID();

			if( !m_szZeroAimFireEffect.IsEmpty() )
			{
				pTarget->AttachEffect( m_szZeroAimFireEffect );
			}
		}
		else*/
		{
			if( !m_szAimFireEffect.IsEmpty() )
			{
				pTarget->AttachEffect( m_szAimFireEffect );
			}
		}
	}

	m_SkillState = SS_FIRE;

	if( m_dwExtraGuidEffect != -1 )
	{
		pChar->EndEffect( m_dwExtraGuidEffect, false );
		m_dwExtraGuidEffect = -1;
	}

	m_bReservedFireFromWaitTime = false;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SNT_ATTACK;
		kPacket << m_vAttackDir;
		//kPacket << m_bZeroAimFire;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAngleFire4Skill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	WeaponInfoList vWeaponInfoList;

	/*if( m_bZeroAimFire )
		vWeaponInfoList = m_vAimInfoList[m_iAimType].m_ZeroAimWeaponInfoList;
	else*/
		vWeaponInfoList = m_vAimInfoList[m_iAimType].m_AimWeaponInfoList;

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwFireStartTime = 0;
	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

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

	m_dwEnableReserveTime = 0;
	DWORD dwReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwReserveTime > 0 )
		m_dwEnableReserveTime = dwCurTime + dwReserveTime;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( m_vAttackDir.y == 0.0f || m_fCurMotionRate == 0.0f )		// center
	{
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else if( m_vAttackDir.y > 0.0f )	// center & up
	{
		pGrp->SetActionAni( m_SkillUpMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	else								// center & down
	{
		pGrp->SetActionAni( m_SkillDownMotion, 0.0f, 0.0f, m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( iAniID, 0.0f, 0.0f, FLOAT1 - m_fCurMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	//float fGauge =	pChar->ReduceCurSkillGaugeBySkill(GetOwnerItem(), GetShotGauge() );	//pChar->ReduceNeedGaugeBySkill( this, GetShotGauge() );//GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

bool ioAngleFire4Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioAngleFire4Skill::CheckAimTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAimCheckTime > 0 &&
		m_dwAimCheckTime + m_vAimInfoList[m_iAimType].m_dwAimTime < dwCurTime )
	{
		m_dwAimCheckTime = 0;
		//m_dwZeroAimCheckTime = dwCurTime;

		/*if( !m_szTargetName.IsEmpty() )
			m_bZeroAimFire = true;
		else
			m_bZeroAimFire = false;*/
	}
	/*else if( m_dwZeroAimCheckTime > 0 &&
		m_dwZeroAimCheckTime + m_vAimInfoList[m_iAimType].m_dwZeroAimTime < dwCurTime )
	{
		m_dwAimCheckTime = dwCurTime;
		m_dwZeroAimCheckTime = 0;
		m_bZeroAimFire = false;
	}*/
}

void ioAngleFire4Skill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsJumpKey() )
	{
		m_bJumpKey = true;
		m_bDefenseKey = false;
	}
	else if( pOwner->IsDefenseKey() )
	{
		m_bJumpKey = false;
		m_bDefenseKey = true;
	}
	else
	{
		m_bJumpKey = false;
		m_bDefenseKey = false;
	}
}

bool ioAngleFire4Skill::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
		
	if( !pTarget->IsEnableTarget() ) 
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
		return false;

	return true;
}

void ioAngleFire4Skill::CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( !pChar )	return;


	if( !pChar->IsNeedProcess() )
		return;

	//
	D3DXVECTOR3 vCurPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	//
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	m_vTargetInfoList.clear();
	int iSize = rkTargetList.size();
	m_vTargetInfoList.reserve(iSize);

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( pChar->GetCharName() == pTarget->GetCharName() )
			continue;
		if( pChar->GetTeam() == pTarget->GetTeam() )
			continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pChar->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			TargetInfo2	kInfo;
			kInfo.m_TargetName = pTarget->GetCharName();
			kInfo.m_fLenth = fDiffRangeSq;
			m_vTargetInfoList.push_back(kInfo);
		}
	}

	if( m_szTargetName.IsEmpty() )
	{
		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			float fDistSq = m_vTargetInfoList[i].m_fLenth;
			if( fDistSq < fTargetRangeSq )	// Enemy
			{
				fTargetRangeSq = fDistSq;
				m_szTargetName = m_vTargetInfoList[i].m_TargetName;
			}
		}

		if( !m_szTargetName.IsEmpty() )
		{
			m_dwAimCheckTime = FRAMEGETTIME();
		//	m_dwZeroAimCheckTime = 0;
			DestroyAimMark( pChar );
		//	m_bZeroAimFire = false;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SNT_CHANGE;
				kPacket << m_szTargetName;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else if( !m_vTargetInfoList.empty() )
	{
		int iCurTarget = -1;

		int iCnt = m_vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			if( m_szTargetName == m_vTargetInfoList[i].m_TargetName )
				iCurTarget = i;
		}

		if( iCurTarget != -1 )
		{
			bool bTargetChange = false;
			if( m_bJumpKey )
			{
				iCurTarget++;
				bTargetChange = true;
				if( iCurTarget >= iCnt )
					iCurTarget = 0;
			}
			else if( m_bDefenseKey )
			{
				bTargetChange = true;
				iCurTarget--;
				if( iCurTarget < 0 )
					iCurTarget = iCnt-1;
			}

			m_szTargetName = m_vTargetInfoList[iCurTarget].m_TargetName;

			if( bTargetChange )
			{
				m_dwAimCheckTime = FRAMEGETTIME();
		//		m_dwZeroAimCheckTime = 0;
		//		m_bZeroAimFire = false;
				DestroyAimMark( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SNT_CHANGE;
					kPacket << m_szTargetName;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		else
			m_szTargetName.Clear();
	}
	else
		m_szTargetName.Clear();
}


void ioAngleFire4Skill::UpdateAimMark( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	if( m_bVisibleAimMarkOnlyOwner && !pChar->IsNeedProcess() )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
	if( !pTarget )
	{
		DestroyAimMark( pChar );
		return;
	}

	if( m_dwAimCheckTime > 0 )
	{
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		if( m_vAimInfoList[m_iAimType].m_dwAimMarkID == -1 )
		{			
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_vAimInfoList[m_iAimType].m_szAimMark, vPos, vScale );
			if( pMapEffect )
				m_vAimInfoList[m_iAimType].m_dwAimMarkID = pMapEffect->GetUniqueID(); 
		}
		else
		if( m_vAimInfoList[m_iAimType].m_dwAimMarkID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
			if( pMapEffect )
				pMapEffect->SetWorldPosition( vPos );
		}

		if( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID != -1 )
		{
			pStage->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
			m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = -1;
		}
	}
}

void ioAngleFire4Skill::DestroyAimMark( ioBaseChar *pChar )
{
	if( !pChar )	return;

	if( m_bVisibleAimMarkOnlyOwner && !pChar->IsNeedProcess() )
		return;

	pChar->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwAimMarkID );
	m_vAimInfoList[m_iAimType].m_dwAimMarkID = -1;

	pChar->DestroyMapEffect( m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID );
	m_vAimInfoList[m_iAimType].m_dwZeroAimMarkID = -1;
}

bool ioAngleFire4Skill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		//if( 0 < m_dwPreDelayTime && dwCurTime > (m_dwSkillStartTime + m_dwPreDelayTime) )
			//return false;
		return true;

	case SS_LOOP:
	case SS_FIRE:
	case SS_WAIT:
		if( 0 < m_dwSkillProtectTime && dwCurTime < (m_dwLoopStartTime+m_dwSkillProtectTime) )
			return true;
		break;
	}

	return false;
}