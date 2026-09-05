

#include "stdafx.h"

#include "ioGrowthUpInfo.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioSpinChargingSkill.h"

#include "WeaponDefine.h"

ioSpinChargingSkill::ioSpinChargingSkill()
{
	m_bSetHandMesh = false;

	m_fCurChargeRate = 0.0f;
	m_dwSpinStartTime = 0;
	m_dwCurSpinSpeed = 0;
	m_SkillChargingState = SCS_NONE;
}

ioSpinChargingSkill::ioSpinChargingSkill( const ioSpinChargingSkill &rhs )
: ioAttackSkill( rhs ),
 m_szSpinLoopAni( rhs.m_szSpinLoopAni ),
 m_fSpinLoopAniRate( rhs.m_fSpinLoopAniRate ),
 m_dwSpinDuration( rhs.m_dwSpinDuration ),
 m_dwDefaultSpinSpeed( rhs.m_dwDefaultSpinSpeed ),
 m_dwAccelSpeed( rhs.m_dwAccelSpeed ),
 m_szSpinFailAni( rhs.m_szSpinFailAni ),
 m_fSpinFailAniRate( rhs.m_fSpinFailAniRate ),
 m_bUseWeaponEffectChange( rhs.m_bUseWeaponEffectChange ),
 m_vChargeWeaponInfo( rhs.m_vChargeWeaponInfo )
{
	m_bSetHandMesh = false;

	m_fCurChargeRate = 0.0f;
	m_dwSpinStartTime = 0;
	m_dwCurSpinSpeed = 0;
	m_SkillChargingState = SCS_NONE;
}

ioSpinChargingSkill::~ioSpinChargingSkill()
{
}

void ioSpinChargingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//
	rkLoader.LoadString_e( "spin_loop_ani", "", szBuf, MAX_PATH );
	m_szSpinLoopAni = szBuf;
	m_fSpinLoopAniRate = rkLoader.LoadFloat_e( "spin_loop_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "spin_fail_ani", "", szBuf, MAX_PATH );
	m_szSpinFailAni = szBuf;
	m_fSpinFailAniRate = rkLoader.LoadFloat_e( "spin_fail_ani_rate", FLOAT1 );

	m_dwDefaultSpinSpeed = rkLoader.LoadInt_e( "spin_start_speed", 0 );
	m_dwAccelSpeed = rkLoader.LoadInt_e( "spin_accel_speed", 0 );

	m_dwSpinDuration = rkLoader.LoadInt_e( "spin_duration", 0 );

	// change weapon effect
	m_bUseWeaponEffectChange = rkLoader.LoadBool_e( "use_change_weapon_effect", false );
	m_vChargeWeaponInfo.clear();

	int iMaxCnt = rkLoader.LoadInt_e( "change_weapon_cnt", 0 );
	if( iMaxCnt > 0 )
	{
		m_vChargeWeaponInfo.reserve( iMaxCnt );

		for( int i=0; i < iMaxCnt; ++i )
		{
			ChargeWeaponInfo kWeaponInfo;

			wsprintf_e( szKey, "change_weapon%d_rate", i+1 );
			kWeaponInfo.m_fChargeRate = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf_e( szKey, "change_weapon%d_effect", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kWeaponInfo.m_WeaponEffect = szBuf;

			m_vChargeWeaponInfo.push_back( kWeaponInfo );
		}
	}
}

ioSkill* ioSpinChargingSkill::Clone()
{
	return new ioSpinChargingSkill( *this );
}

bool ioSpinChargingSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = dwCurTime + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = dwCurTime + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// ExtraAttackForce
	ioGrowthAttackForceUpInfo *pForceInfo = NULL;
	pForceInfo = ToAttackForceUpInfo(GetGrowthUpInfoByType(GT_SKILL_ATTACK_FORCE, pChar->GetCharName()));
	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	if( pForceInfo )
	{
		m_fExtraAttackMoveForce = pForceInfo->GetForceValue(pStage);
		m_fExtraAttackMoveFriction = pForceInfo->GetFrictionValue(pStage);
	}

	SetExtraAttackForce();

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_fCurChargeRate = 0.0f;
	m_SkillChargingState = SCS_NONE;

	m_dwSpinStartTime = 0;
	m_dwCurSpinSpeed = m_dwDefaultSpinSpeed;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetSkillActionState( pChar );
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
			dwTrackingTime += dwCurTime;
		else
			dwTrackingTime = dwCurTime + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioSpinChargingSkill::SetSkillActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillChargingState = SCS_FIRE;

	// Charging Rate
	DWORD dwGapTime = dwCurTime - m_dwSpinStartTime;
	if( m_dwSpinStartTime > 0 && m_dwSpinDuration > 0 )
	{
		if( m_dwSpinDuration > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwSpinDuration;
		else
			m_fCurChargeRate = FLOAT1;
	}

	//
	pChar->AttachEffect( m_SkillEffectName );

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

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
	else
		m_dwAttackEndTime = 0;

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

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	if( pChar && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSpinChargingSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;
	//

	m_SkillChargingState = SCS_NONE;
	m_fCurChargeRate = 0.0f;

	m_dwSpinStartTime = 0;
	m_dwCurSpinSpeed = m_dwDefaultSpinSpeed;
}

bool ioSpinChargingSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioSpinChargingSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	if( pMissile )
	{
		if( m_bUseWeaponEffectChange && m_fCurChargeRate > 0.0f )
		{
			pMissile->ChangeWeaponEffect( CheckChangeWeaponEffect() );
		}
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

void ioSpinChargingSkill::CheckCurSpin( ioBaseChar *pChar )
{
	if( m_dwCurSpinSpeed <= 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	// 회전속도 계산
	if( m_dwAccelSpeed > 0 )
	{
		DWORD dwGapTime = dwCurTime - m_dwSpinStartTime;
		float fDTime = (float)dwGapTime / FLOAT1000;
		float fCurAccel = (float)m_dwAccelSpeed * fDTime * fDTime;

		float fCurSpeed = (float)m_dwCurSpinSpeed - fCurAccel;
		if( fCurSpeed > FLOAT100 )
			m_dwCurSpinSpeed = (DWORD)fCurSpeed;
		else
			m_dwCurSpinSpeed = 100;
	}

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwCurSpinSpeed;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, false );
}

void ioSpinChargingSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_SkillChargingState )
	{
	case SCS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				SetSpinLoopState( pChar );
			}
		}
		break;
	case SCS_SPIN:
		{
			if( m_dwSpinStartTime+m_dwSpinDuration < dwCurTime )
			{
				SetSpinFailState( pChar );
				return;
			}

			CheckCurSpin( pChar );

			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() ||
					pChar->IsDefenseKey() ||
					pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					SetSkillActionState( pChar );
					return;
				}
			}
		}
		break;
	case SCS_FIRE:
		{
		}
		break;
	case SCS_FAIL:
		{
		}
		break;
	}
}

void ioSpinChargingSkill::SetSpinLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_SkillChargingState = SCS_SPIN;
	m_dwSpinStartTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100 );
	pGrp->ClearAllActionAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_szSpinLoopAni );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSpinLoopAniRate );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioSpinChargingSkill::SetSpinFailState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szSpinFailAni );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillChargingState = SCS_FAIL;
	m_dwAttackEndTime = 0;

	float fTimeRate  = m_fSpinFailAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

bool ioSpinChargingSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_SkillChargingState == SCS_FIRE )
	{
		if( m_dwFireMotionEndTime > 0 && m_dwFireMotionEndTime > FRAMEGETTIME() )
		{
			return true;
		}
	}

	return false;
}

void ioSpinChargingSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	if( pChar )
	{
		pChar->SetTargetRotAndMoveDirChange( qtRot );
	}

	if( m_SkillChargingState == SCS_SPIN )
		SetSkillActionState( pChar );
}

bool ioSpinChargingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_SkillChargingState == SCS_SPIN )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

ioHashString ioSpinChargingSkill::CheckChangeWeaponEffect()
{
	ioHashString szCurEffect;

	if( m_vChargeWeaponInfo.empty() )
		return szCurEffect;

	float fCheckRate = 0.0f;
	int iSize = m_vChargeWeaponInfo.size();
	
	for( int i=0; i < iSize; ++i )
	{
		float fWeaponRate = m_vChargeWeaponInfo[i].m_fChargeRate;

		if( COMPARE( m_fCurChargeRate, fCheckRate, fWeaponRate ) )
		{
			szCurEffect = m_vChargeWeaponInfo[i].m_WeaponEffect;
		}

		fCheckRate = fWeaponRate;
	}

	return szCurEffect;
}




