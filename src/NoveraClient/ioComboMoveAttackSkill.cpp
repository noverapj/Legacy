

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioComboMoveAttackSkill.h"

#include "WeaponDefine.h"

ioComboMoveAttackSkill::ioComboMoveAttackSkill()
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_iCurLinearCombo = 0;
	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_bSetInputAni = false;
	m_dwCurSkillProtectTime = 0;
}

ioComboMoveAttackSkill::ioComboMoveAttackSkill( const ioComboMoveAttackSkill &rhs )
: ioNormalSkill( rhs ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_vComboList( rhs.m_vComboList ),
 m_ComboInputTimeList( rhs.m_ComboInputTimeList ),
 m_ComboInputAniList( rhs.m_ComboInputAniList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_iCurLinearCombo = 0;
	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_bSetInputAni = false;
	m_dwCurSkillProtectTime = 0;
}

ioComboMoveAttackSkill::~ioComboMoveAttackSkill()
{
	m_vComboList.clear();
	m_ComboInputTimeList.clear();
	m_ComboInputAniList.clear();
}

void ioComboMoveAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	// Combo Attack
	int iComboCnt = rkLoader.LoadInt_e( "max_combo_cnt", 0 );
	m_vComboList.clear();
	m_ComboInputTimeList.clear();
	m_ComboInputAniList.clear();

	for( int i=0; i < iComboCnt; ++i )
	{
		// combo attack
		wsprintf_e( szKey, "combo_attack%d", i+1 );

		AttackAttribute kAttackInfo;
		LoadAttackAttribute( szKey, kAttackInfo, rkLoader );

		m_vComboList.push_back( kAttackInfo );

		// combo input time
		wsprintf_e( szKey, "combo_attack%d_next", i+1 );
		DWORD dwNextTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_ComboInputTimeList.push_back( dwNextTime );

		// combo input ani
		wsprintf_e( szKey, "combo_attack%d_next_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ComboInputAniList.push_back( szBuf );
	}
}

ioSkill* ioComboMoveAttackSkill::Clone()
{
	return new ioComboMoveAttackSkill( *this );
}

bool ioComboMoveAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	if( !pChar )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_fCurChargeRate = 0.0f;

	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_iCurLinearCombo = 0;
	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_SkillSoundID = 0;

	m_bSetInputAni = false;

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

	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetLinearComboState( pChar );
	}

	SetSkillNameChatBubble( pChar );

	SetPreActionState( pChar );

	return true;
}

void ioComboMoveAttackSkill::OnSkillEnd( ioBaseChar *pChar )
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

	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_State = SS_NONE;
	m_bSetInputAni = false;
}

void ioComboMoveAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetLinearComboState( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_COMBO:
		if( !m_bSetInputAni && m_dwComboInputEnableTime < dwCurTime )
		{
			m_bSetInputAni = true;

			ioEntityGroup *pGrp = pChar->GetGroup();
			int iAniID = pGrp->GetAnimationIdx( m_CurInputAni );
			if( iAniID != -1 )
			{
				pGrp->ClearAllActionAni( FLOAT10, true );
				pGrp->ClearAllLoopAni( FLOAT10, true );

				float fAniRate = FLOAT1;

				DWORD dwAniTime = pGrp->GetAnimationFullTime( iAniID );
				DWORD dwGapTime = m_dwNextComboTime - m_dwComboInputEnableTime;
				if( dwGapTime > 0 && dwAniTime > 0 )
				{
					fAniRate = (float)dwGapTime / dwAniTime;
				}

				pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate );
			}
		}

		if( pChar->IsNeedProcess() )
		{
			CheckLinearCombo( pChar );
		}
		break;
	}
}

void ioComboMoveAttackSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = 0;

	pChar->AttachEffect( m_StartEffect );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

void ioComboMoveAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_COMBO:
		{
			int iState, iCombo;
			rkPacket >> iState >> iCombo;

			m_State = (SkillState)iState;
			m_iCurLinearCombo = iCombo;

			if( IsUseAutoTargetValue() )
				pChar->ApplyAutoTargetInfo( rkPacket );

			SetNextLinearCombo( pChar, false );
		}
		break;
	}
}

bool ioComboMoveAttackSkill::IsProtected( int iDefenseBreakType ) const
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

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwCurSkillProtectTime > 0 &&
			m_dwCurSkillProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioComboMoveAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_COMBO && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioComboMoveAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioComboMoveAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

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

void ioComboMoveAttackSkill::SetLinearComboState( ioBaseChar *pChar )
{
	m_State = SS_COMBO;
	m_iCurLinearCombo = 0;
	m_bSetInputAni = false;

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = 0;

	pChar->AttachEffect( m_StartEffect );

	//
	if( m_dwSkillMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwSkillMapEffect, false );
		m_dwSkillMapEffect = -1;
	}

	if( !m_SkillSoundName.IsEmpty() && pChar )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		m_SkillSoundID = g_SoundMgr.PlaySound( m_SkillSoundName, pGrp->GetParentSceneNode() );
	}

	D3DXVECTOR3 vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_SkillMapEffectName, vEffectDir );
	if( pMapEffect )
	{
		m_dwSkillMapEffect = pMapEffect->GetUniqueID();
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	SetNextLinearCombo( pChar, false );
	//if( pChar->IsNeedSendNetwork() )
		//SetNextLinearCombo( pChar, true );
}

void ioComboMoveAttackSkill::SetNextLinearCombo( ioBaseChar *pChar, bool bSend )
{
	m_dwEnableReserveTime = 0;
	m_bSetInputAni = false;
	m_CurInputAni.Clear();

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_vComboList[m_iCurLinearCombo].m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = m_vComboList[m_iCurLinearCombo].m_fAttackAniRate;
	DWORD dwPreDelay = m_vComboList[m_iCurLinearCombo].m_dwPreDelay;
	DWORD dwInputTime = 0;

	if( COMPARE( m_iCurLinearCombo, 0, (int)m_ComboInputTimeList.size() ) )
	{
		dwInputTime = m_ComboInputTimeList[m_iCurLinearCombo];
		m_CurInputAni = m_ComboInputAniList[m_iCurLinearCombo];
	}

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
										m_vComboList[m_iCurLinearCombo].m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_vComboList[m_iCurLinearCombo].m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwFireMotionEndTime = dwCurTime + dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveTime;
	}

	if( m_vComboList[m_iCurLinearCombo].m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = m_vComboList[m_iCurLinearCombo].m_fAttackEndAniRate;

		pGrp->SetActionAni( m_vComboList[m_iCurLinearCombo].m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( m_vComboList[m_iCurLinearCombo].m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	m_dwComboInputEnableTime = m_dwMotionEndTime;
	m_dwNextComboTime = m_dwComboInputEnableTime + dwInputTime;
	m_dwMotionEndTime += dwInputTime;

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	//if( IsUseAutoTargetValue() )
	if( IsUseAutoTargetValue() && pChar->IsNeedProcess() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
	//

	pChar->SetReservedSliding( m_vComboList[m_iCurLinearCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_COMBO;
		kPacket << m_State;
		kPacket << m_iCurLinearCombo;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioComboMoveAttackSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( COMPARE( dwCurTime, m_dwComboInputEnableTime, m_dwNextComboTime ) )
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
				SetNextLinearCombo( pChar, true );
				return;
			}
		}
	}
}

void ioComboMoveAttackSkill::SetPreActionState( ioBaseChar *pChar )
{
	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}