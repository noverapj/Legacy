

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioComboAttackSkill.h"

#include "WeaponDefine.h"

ioComboAttackSkill::ioComboAttackSkill()
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_dwActionStartTime = 0;

	m_iCurLinearCombo = 0;
	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_bSetInputAni = false;
	m_dwCurSkillProtectTime = 0;

	m_fReduceGauge = 0.0f;
}

ioComboAttackSkill::ioComboAttackSkill( const ioComboAttackSkill &rhs )
: ioNormalSkill( rhs ),
 m_fRangeMaxRadius( rhs.m_fRangeMaxRadius ),
 m_fRangeMinRadius( rhs.m_fRangeMinRadius ),
 m_fRangeSpeed( rhs.m_fRangeSpeed ),
 m_szRangeEffect( rhs.m_szRangeEffect ),
 m_szRangeArcEffect( rhs.m_szRangeArcEffect ),
 m_ArcEffectAngleList( rhs.m_ArcEffectAngleList ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_iComboCount( rhs.m_iComboCount ),
 m_vComboList( rhs.m_vComboList ),
 m_ComboInputTimeList( rhs.m_ComboInputTimeList ),
 m_ComboInputAniList( rhs.m_ComboInputAniList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	m_State = SS_NONE;
	m_bSetHandMesh = false;

	m_dwActionStartTime = 0;

	m_iCurLinearCombo = 0;
	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_bSetInputAni = false;
	m_dwCurSkillProtectTime = 0;

	m_fReduceGauge = 0.0f;
}

ioComboAttackSkill::~ioComboAttackSkill()
{
	m_vComboList.clear();
	m_ComboInputTimeList.clear();
	m_ComboInputAniList.clear();
}

void ioComboAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_fRangeMaxRadius	= rkLoader.LoadFloat_e( "range_max_radius", 0.0f );
	m_fRangeMinRadius	= rkLoader.LoadFloat_e( "range_min_radius", 0.0f );
	m_fRangeSpeed		= rkLoader.LoadFloat_e( "range_speed", 0.0f );
	rkLoader.LoadString_e( "range_effect", "", szBuf, MAX_PATH );		m_szRangeEffect		= szBuf;
	rkLoader.LoadString_e( "range_arc_effect", "", szBuf, MAX_PATH );	m_szRangeArcEffect = szBuf;
	
	m_ArcEffectAngleList.clear();
	int iCnt = rkLoader.LoadInt_e( "range_arc_effect_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "range_arc_effect_angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_ArcEffectAngleList.push_back( fAngle );
	}

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );	m_EndEffect = szBuf;

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	// Combo Attack
	m_iComboCount = rkLoader.LoadInt_e( "max_combo_cnt", 0 );
	m_vComboList.clear();
	m_ComboInputTimeList.clear();
	m_ComboInputAniList.clear();

	for( int i=0; i < m_iComboCount; ++i )
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

ioSkill* ioComboAttackSkill::Clone()
{
	return new ioComboAttackSkill( *this );
}

bool ioComboAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	if( !pChar )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_dwActionStartTime = 0;

	m_fRangeCurRadius = m_fRangeMinRadius;

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
		SetActionState( pChar );
	}

	float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
	m_fReduceGauge = fCurGauge / m_iComboCount;

	SetSkillNameChatBubble( pChar );
	
	return true;
}

void ioComboAttackSkill::OnSkillEnd( ioBaseChar *pChar )
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

	if( m_dwRangeEffect != -1 )
	{
		pChar->EndMapEffect( m_dwRangeEffect, true );
		m_dwSkillMapEffect = -1;
	}

	if( !m_dwRangeArcEffectList.empty() )
	{
		int iCnt = m_dwRangeArcEffectList.size();
		
		ioPlayStage *pStage = pChar->GetCreator();
		if(pStage)
		{
			for( int i=0; i<iCnt; ++i )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwRangeArcEffectList[i] );
				if( pMapEffect )	pMapEffect->EndEffectGrace();
			}
			m_dwRangeArcEffectList.clear();			
		}
	}

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}

	m_dwActionStartTime = 0;

	m_fRangeCurRadius = m_fRangeMinRadius;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_dwComboInputEnableTime = 0;
	m_dwNextComboTime = 0;

	m_State = SS_NONE;
	m_bSetInputAni = false;

	m_fReduceGauge = 0.0f;
}

void ioComboAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetActionState( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fRate = (fTimePerSec*1000) / m_fRangeSpeed;
			float fAddRange = m_fRangeMaxRadius * fRate;
			
			if( COMPARE( m_fRangeCurRadius, m_fRangeMinRadius, m_fRangeMaxRadius ) )
			{
				m_fRangeCurRadius += fAddRange;
			}

			if( m_fRangeCurRadius >= m_fRangeMaxRadius)
				SetEndState();

			if( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetLinearComboState( pChar );
			}

			UpdateRangeEffect( pStage );
		}
		break;
	case SS_LOOP:
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

void ioComboAttackSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = 0;

	pChar->AttachEffect( m_StartEffect );
}

void ioComboAttackSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_LOOP:
		{
			m_State = SS_LOOP;
			int iState, iCombo;
			rkPacket >> iState >> iCombo;

			m_State = (SkillState)iState;
			m_iCurLinearCombo = iCombo;

			SetNextLinearCombo( pChar, false );
		}
		break;
	}
}

bool ioComboAttackSkill::IsProtected( int iDefenseBreakType ) const
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

bool ioComboAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_LOOP && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_State )
	{
	case SS_NONE:
	case SS_LOOP:
	case SS_ACTION:
		return false;	
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		break;
	}

	return false;
}

bool ioComboAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioComboAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			if( pZone->GetCollisionType() == TCT_DONUT )
				pZone->SetZoneValue( false, true );
			else
				pZone->SetZoneValue( m_fRangeCurRadius, false );
		}
	}
}

void ioComboAttackSkill::SetActionState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	
	m_State = SS_ACTION;
	m_dwActionStartTime = FRAMEGETTIME();
	SetNormalSkill( pChar );
	CreateRangeEffect( pChar );
}

void ioComboAttackSkill::SetLinearComboState( ioBaseChar *pChar )
{
	m_State = SS_LOOP;
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

	SetNextLinearCombo( pChar, true );
}

void ioComboAttackSkill::SetEndState()
{
	m_State = SS_END;
	
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		if( m_dwRangeEffect != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwRangeEffect );

			if( pMapEffect )	pMapEffect->EndEffectForce();

			m_dwRangeEffect = -1;
		}

		if( !m_dwRangeArcEffectList.empty() )
		{
			int iCnt = m_dwRangeArcEffectList.size();

			for( int i=0; i<iCnt; ++i )
			{
				ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwRangeArcEffectList[i] );
				if( pMapEffect )	pMapEffect->EndEffectGrace();
			}
			m_dwRangeArcEffectList.clear();			
		}
	}
}

void ioComboAttackSkill::SetNextLinearCombo( ioBaseChar *pChar, bool bSend )
{
	m_dwEnableReserveTime = 0;
	m_bSetInputAni = false;
	m_CurInputAni.Clear();

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
	{
		SetEndState();
		return;
	}

	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), m_fReduceGauge );
	m_bReduceGauge = true;

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

	/*float fReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserveTime > 0.0f )
	{
		m_dwEnableReserveTime = dwCurTime + fReserveTime;
	}*/

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

	if( IsUseAutoTargetValue() )
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
		kPacket << SSC_LOOP;
		kPacket << m_State;
		kPacket << m_iCurLinearCombo;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;

	CreateRangeArcEffect( pChar );
}

void ioComboAttackSkill::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
	{
		SetEndState();
		return;
	}

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
	
	if( dwCurTime > m_dwNextComboTime )
		SetEndState();
}

void ioComboAttackSkill::CreateRangeEffect( ioBaseChar *pChar )
{
	if( !pChar)
		return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		// 이펙트 처리
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition() + D3DXVECTOR3( 0.0f, 0.1f, 0.0f );

		ioMapEffect *pMapEffect = NULL;
		pMapEffect = pStage->CreateMapEffect( m_szRangeEffect, vPos, vScale );
		if( pMapEffect )
		{
			m_dwRangeEffect = pMapEffect->GetUniqueID();

			pMapEffect->SetWorldOrientation( pChar->GetTargetRot() );
		}
	}
}

void ioComboAttackSkill::UpdateRangeEffect( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	float fCurScaleRate = FLOAT1;
	
	float fCurScale = m_fRangeCurRadius / FLOAT100;
	fCurScale += FLOAT1;

	ioMapEffect *pMapEff = pStage->FindMapEffect( m_dwRangeEffect );
	if( pMapEff )
	{
		ioEffect *pEffect = pMapEff->GetEffect();
		if( pEffect )
		{
			pEffect->GetParentNode()->SetScale( fCurScale, fCurScale, fCurScale );
		}
	}
}

void ioComboAttackSkill::CreateRangeArcEffect( ioBaseChar *pChar )
{
	if( !pChar)
		return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		// 이펙트 처리
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		
		int iCnt = m_ArcEffectAngleList.size();
		for(int i=0; i<iCnt; ++i)
		{
			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD( m_ArcEffectAngleList[i] ), 0.0f, 0.0f );

			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vDir = qtAngle * vDir;
			D3DXVECTOR3 vPos = pChar->GetWorldPosition() + ( vDir * m_fRangeCurRadius );		

			ioMapEffect *pMapEffect = NULL;
			pMapEffect = pStage->CreateMapEffect( m_szRangeArcEffect, vPos, vScale );
			if( pMapEffect )
			{
				m_dwRangeArcEffectList.push_back( pMapEffect->GetUniqueID() );

				D3DXQUATERNION	qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -vDir );
				pMapEffect->SetWorldOrientation( qtRot );
			}
		}
	}
}