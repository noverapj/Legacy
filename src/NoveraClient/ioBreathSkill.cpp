

#include "stdafx.h"

#include "ioBreathSkill.h"
#include "ioBaseChar.h"

ioBreathSkill::ioBreathSkill()
{	
	m_bTargetRot = false;
	m_bSetMoveAni = false;
	m_bSetHandMesh = false;
	m_bReserveAttackKey = false;

	m_iWeaponFireCnt = 0;

	m_dwCheckStartTime = 0;
	m_dwBreathStartTime = 0;
	m_dwCheckWeaponTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_dwNormalStateEffect = -1;
	m_dwFlameStateEffect = -1;
	m_dwBreathStateEffect = -1;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioBreathSkill::ioBreathSkill( const ioBreathSkill &rhs )
: ioAttackSkill( rhs ),
  m_bDirection( rhs.m_bDirection ),
  m_NormalMotion( rhs.m_NormalMotion ),
  m_NormalMoveMotion( rhs.m_NormalMoveMotion ),
  m_BreathMotion( rhs.m_BreathMotion ),
  m_BreathMoveMotion( rhs.m_BreathMoveMotion ),
  m_dwAddDuration( rhs.m_dwAddDuration ),
  m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
  m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
  m_WeaponInfo( rhs.m_WeaponInfo ),
  m_dwSkillFullTime( rhs.m_dwSkillFullTime ),
  m_dwWeaponCreateTime( rhs.m_dwWeaponCreateTime ),
  m_NormalStateEffect( rhs.m_NormalStateEffect ),
  m_FlameStateEffect( rhs.m_FlameStateEffect ),
  m_BreathStateEffect( rhs.m_BreathStateEffect ),
  m_fEndMotionRate( rhs.m_fEndMotionRate ),
  m_SkillEndMotion( rhs.m_SkillEndMotion ),
  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
  m_fBreathMoveRate( rhs.m_fBreathMoveRate ),
  m_bCheckSkillGauge( rhs.m_bCheckSkillGauge )
{
	m_bTargetRot = false;
	m_bSetMoveAni = false;
	m_bSetHandMesh = false;
	m_bReserveAttackKey = false;

	m_iWeaponFireCnt = 0;

	m_dwCheckStartTime = 0;
	m_dwBreathStartTime = 0;
	m_dwCheckWeaponTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_dwNormalStateEffect = -1;
	m_dwFlameStateEffect = -1;
	m_dwBreathStateEffect = -1;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioBreathSkill::~ioBreathSkill()
{
}

void ioBreathSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_bDirection = rkLoader.LoadBool_e( "direction", false );
	m_bCheckSkillGauge = rkLoader.LoadBool_e( "check_skill_gauge", false );

	rkLoader.LoadString_e( "normal_motion", "", szBuf, MAX_PATH );
	m_NormalMotion = szBuf;
	rkLoader.LoadString_e( "normal_move_motion", "", szBuf, MAX_PATH );
	m_NormalMoveMotion = szBuf;
	rkLoader.LoadString_e( "breath_motion", "", szBuf, MAX_PATH );
	m_BreathMotion = szBuf;
	rkLoader.LoadString_e( "breath_move_motion", "", szBuf, MAX_PATH );
	m_BreathMoveMotion = szBuf;

	m_dwSkillFullTime = (DWORD)rkLoader.LoadInt_e( "skill_full_time", 0 );
	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_dwAddDuration = (DWORD)rkLoader.LoadInt_e( "breath_add_time", 0 );
	m_dwEnableKeyTime = (DWORD)rkLoader.LoadInt_e( "breath_enable_key_time", 0 );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "breath_rotate_speed", 0 );
	
	m_dwWeaponCreateTime = (DWORD)rkLoader.LoadInt_e( "create_weapon_time", 0 );

	m_fBreathMoveRate = rkLoader.LoadFloat_e( "breath_move_rate", FLOAT1 );

	rkLoader.LoadString_e( "normal_state_effect", "", szBuf, MAX_PATH );
	m_NormalStateEffect = szBuf;
	rkLoader.LoadString_e( "flame_state_effect", "", szBuf, MAX_PATH );
	m_FlameStateEffect = szBuf;
	rkLoader.LoadString_e( "breath_state_effect", "", szBuf, MAX_PATH );
	m_BreathStateEffect = szBuf;

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;

	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	//
	wsprintf_e( szKey, "weapon_attribute_index" );
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "weapon_attribute_resist_index" );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "weapon_wounded_animation" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "weapon_wounded_duration" );
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "weapon_wounded_loop_ani" );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );
}

ioSkill* ioBreathSkill::Clone()
{
	return new ioBreathSkill( *this );
}

bool ioBreathSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_bReduceGauge = false;

	m_bTargetRot = false;
	m_bSetMoveAni = false;
	m_bSetHandMesh = false;
	m_bReserveAttackKey = false;

	m_iWeaponFireCnt = 0;

	m_dwCheckStartTime = 0;
	m_dwBreathStartTime = 0;
	m_dwCheckWeaponTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_ActionState = AS_READY;
	pChar->SetSkillMoveEnable( false, false );

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	//
	if( m_dwNormalStateEffect != -1 )
	{
		pChar->EndEffect( m_dwNormalStateEffect, false );
		m_dwNormalStateEffect = -1;
	}

	if( m_dwFlameStateEffect != -1 )
	{
		pChar->EndEffect( m_dwFlameStateEffect, false );
		m_dwFlameStateEffect = -1;
	}

	if( m_dwBreathStateEffect != -1 )
	{
		pChar->EndEffect( m_dwBreathStateEffect, false );
		m_dwBreathStateEffect = -1;
	}

	return true;
}

void ioBreathSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bCheckSkillGauge && m_ActionState != AS_END &&
		!CheckSkillGauge( pChar ) )
	{
		SetEndState( pChar );
		return;
	}

	if( m_ActionState != AS_END &&
		m_dwSkillStartTime + m_dwSkillFullTime < dwCurTime )
	{
		SetEndState( pChar );
		return;
	}

	switch( m_ActionState )
	{
	case AS_READY:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				SetNormalState( pChar );
				return;
			}
		}
		break;
	case AS_NORMAL:
		{
			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetFlameState( pChar );

				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SST_CHANGE_STATE;
					kPacket << (int)m_ActionState;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
			else
			{
				CheckNormalMoveAni( pChar );
			}
		}
		break;
	case AS_FLAME:
		ProcessFlameState( pChar );
		break;
	case AS_BREATH:
		ProcessBreathState( pChar );
		break;
	}
}

void ioBreathSkill::OnSkillEnd( ioBaseChar *pChar )
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

	m_bReduceGauge = false;
	m_bSetHandMesh = false;

	m_iWeaponFireCnt = 0;
	m_bSetMoveAni = false;
	m_bReserveAttackKey = false;
	
	m_dwCheckStartTime = 0;
	m_dwCheckWeaponTime = 0;

	pChar->SetSkillMoveEnable( false, false );

	//
	if( m_dwNormalStateEffect != -1 )
	{
		pChar->EndEffect( m_dwNormalStateEffect, false );
		m_dwNormalStateEffect = -1;
	}

	if( m_dwFlameStateEffect != -1 )
	{
		pChar->EndEffect( m_dwFlameStateEffect, false );
		m_dwFlameStateEffect = -1;
	}

	if( m_dwBreathStateEffect != -1 )
	{
		pChar->EndEffect( m_dwBreathStateEffect, false );
		m_dwBreathStateEffect = -1;
	}
}

bool ioBreathSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioBreathSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();

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
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

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

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioBreathSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_CHANGE_STATE:
		{
			int iState;
			rkPacket >> iState;

			switch( iState )
			{
			case AS_NORMAL:
				SetNormalState( pChar );
				break;
			case AS_FLAME:
				SetFlameState( pChar );
				break;
			case AS_BREATH:
				SetBreathState( pChar );
				break;
			}
		}
		break;
	case SST_CHANGE_DIR:
		{
			int iDir;
			rkPacket >> iDir;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			pChar->SetNetworkKeyInput( m_CurDirKey );
		}
		break;
	}
}

void ioBreathSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioBreathSkill::SetNormalState( ioBaseChar *pChar )
{
	if( m_ActionState == AS_END )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();

	m_ActionState = AS_NORMAL;

	m_dwCheckWeaponTime = 0;
	m_iWeaponFireCnt = 0;
	m_bSetMoveAni = false;
	m_bReserveAttackKey = false;
	pChar->SetSkillMoveEnable( true, m_bDirection );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_NormalMotion, FLOAT100 );

	EndPreDelayEffect( pChar );

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

	//
	ioEffect *pEffect = NULL;
	if( m_dwNormalStateEffect == -1 && !m_NormalStateEffect.IsEmpty() )
	{
		pEffect = pChar->AttachEffect( m_NormalStateEffect );
		if( pEffect )
			m_dwNormalStateEffect = pEffect->GetUniqueID();
	}
	
	if( m_dwFlameStateEffect != -1 )
	{
		pChar->EndEffect( m_dwFlameStateEffect, false );
		m_dwFlameStateEffect = -1;
	}

	if( m_dwBreathStateEffect != -1 )
	{
		pChar->EndEffect( m_dwBreathStateEffect, false );
		m_dwBreathStateEffect = -1;
	}
}

void ioBreathSkill::SetFlameState( ioBaseChar *pChar )
{
	if( m_ActionState == AS_END )
		return;

	m_ActionState = AS_FLAME;

	m_bReserveAttackKey = false;
	m_dwEnableReserveTime = 0;
	pChar->SetSkillMoveEnable( false, false );

	AttackSkillFire( pChar );

	//
	ioEffect *pEffect = NULL;
	if( m_dwFlameStateEffect == -1 && !m_FlameStateEffect.IsEmpty() )
	{
		pEffect = pChar->AttachEffect( m_FlameStateEffect );
		if( pEffect )
			m_dwFlameStateEffect = pEffect->GetUniqueID();
	}

	if( m_dwNormalStateEffect != -1 )
	{
		pChar->EndEffect( m_dwNormalStateEffect, false );
		m_dwNormalStateEffect = -1;
	}

	if( m_dwBreathStateEffect != -1 )
	{
		pChar->EndEffect( m_dwBreathStateEffect, false );
		m_dwBreathStateEffect = -1;
	}
}

void ioBreathSkill::SetBreathState( ioBaseChar *pChar )
{
	if( m_ActionState == AS_END )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	m_ActionState = AS_BREATH;
	m_dwBreathStartTime = dwCurTime;
	m_dwCheckWeaponTime = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_BreathMotion );
	if( iAniID == -1 )	return;

	m_iWeaponFireCnt = 0;
	m_bSetMoveAni = false;
	m_bReserveAttackKey = false;
	m_dwCheckStartTime = dwCurTime;
	pChar->SetSkillMoveEnable( false, false );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	//
	ioEffect *pEffect = NULL;
	if( m_dwBreathStateEffect == -1 && !m_BreathStateEffect.IsEmpty() )
	{
		pEffect = pChar->AttachEffect( m_BreathStateEffect );
		if( pEffect )
			m_dwBreathStateEffect = pEffect->GetUniqueID();
	}

	if( m_dwNormalStateEffect != -1 )
	{
		pChar->EndEffect( m_dwNormalStateEffect, false );
		m_dwNormalStateEffect = -1;
	}

	if( m_dwFlameStateEffect != -1 )
	{
		pChar->EndEffect( m_dwFlameStateEffect, false );
		m_dwFlameStateEffect = -1;
	}
}

void ioBreathSkill::SetEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_ActionState = AS_END;
	m_dwPreDelayStartTime = 0;
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;

	if( !pChar  ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iAniID == -1 )	return;

	float fTimeRate = m_fEndMotionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwEnableReserveTime = 0;
	float fReserveValue = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveValue > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveValue;
	}
}

void ioBreathSkill::ProcessFlameState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		if( m_bReserveAttackKey )
		{
			SetBreathState( pChar );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SST_CHANGE_STATE;
				kPacket << (int)m_ActionState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		else
		{
			SetNormalState( pChar );

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pChar->GetCharName();
				kPacket << GetName();
				kPacket << SST_CHANGE_STATE;
				kPacket << (int)m_ActionState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		return;
	}

	if( m_dwEnableReserveTime < dwCurTime )
	{
		if( pChar->IsNeedProcess() )
		{
			if( !m_bReserveAttackKey && pChar->IsAttackKey() )
			{
				m_bReserveAttackKey = true;
			}
		}
	}
}

void ioBreathSkill::ProcessBreathState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	// 키 체크
	if( pChar->IsNeedProcess() )
	{
		DWORD dwCheck1 = m_dwCheckStartTime + m_dwEnableKeyTime;
		DWORD dwCheck2 = m_dwCheckStartTime + m_dwAddDuration;

		// 키연타 체크
		if( !m_bReserveAttackKey && dwCheck1 < dwCheck2 )
		{
			if( m_dwCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
			{
				if( pChar->IsAttackKey() )
				{
					m_bReserveAttackKey = true;
				}
			}
		}

		if( dwCheck2 < dwCurTime )
		{
			if( m_bReserveAttackKey )
			{
				m_bReserveAttackKey = false;
				m_dwCheckStartTime = dwCurTime;
			}
			else
			{
				// 키연타 실패상황
				SetNormalState( pChar );
				return;
			}
		}
	}

	// move
	CheckKeyInput( pChar );
	ProcessRotate( pChar );
	CheckBreathMoveAni( pChar );

	// 충돌체 처리
	if( m_dwWeaponCreateTime == 0 )
		return;

	if( m_dwCheckWeaponTime == 0 )
	{
		CreateWeapon( pChar );
	}
	else if( m_dwCheckWeaponTime+m_dwWeaponCreateTime < dwCurTime )
	{
		CreateWeapon( pChar );
	}
}

void ioBreathSkill::CheckKeyInput( ioBaseChar* pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SST_CHANGE_DIR;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioBreathSkill::ProcessRotate( ioBaseChar* pChar )
{
	if( m_dwRotateSpeed == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		pChar->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.99f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

bool ioBreathSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_ActionState )
	{
	case AS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;
		break;
	}

	return false;
}

void ioBreathSkill::CreateWeapon( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iWeaponFireCnt;

	pOwner->SkillFireExplicit( m_WeaponInfo,
							   GetName(),
							   dwWeaponIndex,
							   vCurPos );

	m_iWeaponFireCnt++;

	m_dwCheckWeaponTime = FRAMEGETTIME();
}

void ioBreathSkill::CheckNormalMoveAni( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{
		if( !m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_NormalMoveMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_NormalMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

void ioBreathSkill::CheckBreathMoveAni( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		if( !m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_BreathMoveMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_BreathMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

bool ioBreathSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioBreathSkill::IsEnableMoveState()
{
	switch( m_ActionState )
	{
	case AS_BREATH:
		return true;
	}

	return false;
}

float ioBreathSkill::GetSkillMoveRate() const
{
	switch( m_ActionState )
	{
	case AS_NORMAL:
		return m_fSkillMoveRate;
	case AS_BREATH:
		return m_fBreathMoveRate;
	}

	return 0.0f;
}

bool ioBreathSkill::CheckSkillGauge( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioItem *pOwnerItem = GetOwnerItem();
	if( !pOwnerItem )
		return true;

	float fGauge = pChar->GetCurItemSkillGauge( pOwnerItem->GetName() );
	if( fGauge > 0.0f )
		return true;

	return false;
}

bool ioBreathSkill::IsEnableReserve() const
{
	if( m_dwPreDelayStartTime != 0 )
		return false;

	if( m_ActionState != AS_END )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}




