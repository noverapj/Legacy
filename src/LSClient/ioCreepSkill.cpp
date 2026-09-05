

#include "stdafx.h"

#include "ioCreepSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioCreepSkill::ioCreepSkill()
{
	m_State = SS_NONE;
}

ioCreepSkill::ioCreepSkill( const ioCreepSkill &rhs )
: ioNormalSkill( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_dwCreepDuraiton( rhs.m_dwCreepDuraiton ),
 m_CreepEndStandAni( rhs.m_CreepEndStandAni ),
 m_fCreepEndStandAniRate( rhs.m_fCreepEndStandAniRate ),
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
 m_DummyCharList( rhs.m_DummyCharList ),
 m_DummyCharEffect( rhs.m_DummyCharEffect )
{
	m_State = SS_NONE;
}

ioCreepSkill::~ioCreepSkill()
{
	m_vDestroyDummyCharList.clear();
	m_DummyCharList.clear();
}

void ioCreepSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwCreepDuraiton = rkLoader.LoadInt_e( "creep_duration", 0 );

	// Normal
	rkLoader.LoadString_e( "creep_end_stand_ani", "", szBuf, MAX_PATH );
	m_CreepEndStandAni = szBuf;
	m_fCreepEndStandAniRate = rkLoader.LoadFloat_e( "creep_end_stand_ani_rate", FLOAT1 );

	// DirKey End
	rkLoader.LoadString_e( "creep_end_attack_ani", "", szBuf, MAX_PATH );
	m_AttackAttribute.m_AttackAnimation = szBuf;
	m_AttackAttribute.m_fAttackAniRate  = rkLoader.LoadFloat_e( "creep_end_attack_rate", FLOAT1 );

	/*
	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_AttackAttribute.m_AttackEndAnimation = szBuf;
	m_AttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "skill_end_time_rate", FLOAT1 );

	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );
	*/

	char szKey[MAX_PATH];
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
	rkLoader.LoadString_e( "dummy_char_effect", "", szBuf, MAX_PATH );
	m_DummyCharEffect = szBuf;

	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
}

ioSkill* ioCreepSkill::Clone()
{
	return new ioCreepSkill( *this );
}

bool ioCreepSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	m_State = SS_NONE;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetNormalSkill( pChar );

		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		CheckDummyChar( pChar, pStage );
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

void ioCreepSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;
}

void ioCreepSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	int iSlot = GetOwnerItem()->GetType() - 1;

	switch( m_State )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );

			SetNormalSkill( pChar );
			CheckDummyChar( pChar, pStage );
		}
		break;
	case SS_CREEP:
		if( pChar->IsNeedProcess() )
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetEndState( pChar, false );
			}
			else
			{
				bool bSet = false;
				bool bDirKey = false;

				if( pChar->IsAttackKey() || pChar->IsDefenseKey() || pChar->IsJumpKey() ||
					pChar->GetSkillKeyInput() == iSlot )
				{
					bSet = true;
					bDirKey = false;
				}
				else if( pChar->IsSettedDirection() )
				{
					bSet = true;
					bDirKey = true;
				}

				if( bSet )
				{
					SetEndState( pChar, bDirKey );
				}
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioCreepSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCreepSkill::SetNormalSkill( ioBaseChar *pChar )
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
	m_dwMotionEndTime = m_dwMotionStartTime + m_dwCreepDuraiton;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_State = SS_CREEP;

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioCreepSkill::SetEndState( ioBaseChar *pChar, bool bDirKey )
{
	if( !pChar ) return;

	m_State = SS_END;
	bool bUseAttack = false;

	if( bDirKey )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( !pGrp ) return;

		int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
		if( iAniID == -1 )	return;

		pChar->ChangeDirectionByInputDir( false );

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
		}

		float fTimeRate  = m_AttackAttribute.m_fAttackAniRate;
		DWORD dwPreDelay = m_AttackAttribute.m_dwPreDelay;

		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											m_AttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
		m_dwMotionEndTime += dwPreDelay + m_AttackAttribute.m_dwEndDelay;

		DWORD dwFireStartTime;
		dwFireStartTime = m_dwFireMotionEndTime = FRAMEGETTIME();

		dwFireStartTime += dwPreDelay;
		dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

		m_dwFireMotionEndTime += dwPreDelay;
		m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

		m_dwEnableReserveTime = FRAMEGETTIME();
		m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

		if( dwFireStartTime >= m_dwFireMotionEndTime )
		{
			m_dwFireMotionEndTime = m_dwMotionEndTime;
		}

		if( m_AttackAttribute.m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

			float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
			float fEndTimeRate = m_AttackAttribute.m_fAttackEndAniRate;

			pGrp->SetActionAni( m_AttackAttribute.m_AttackEndAnimation, 0.0f, FLOAT100,
								1.0f, FLOAT1/fEndTimeRate, fWaitTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackEndAnimation );
			m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
		}

		DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}

		pChar->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
		bUseAttack = true;
	}
	else
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		int iCurSkillAniID = pGrp->GetAnimationIdx( m_CreepEndStandAni );
		if( iCurSkillAniID == -1 )	return;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1 );

		m_dwMotionStartTime = FRAMEGETTIME();
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fCreepEndStandAniRate;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << bDirKey;
		kPacket << bUseAttack;
		if( bUseAttack )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCreepSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SSC_END:
		{
			bool bDirKey, bUseAttack;
			rkPacket >> bDirKey;
			rkPacket >> bUseAttack;

			SetEndState( pChar, bDirKey );

			if( bUseAttack )
				pChar->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SSC_DUMMY:
		{
			g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iCreateCnt;
			rkPacket >> iCreateCnt;

			for( int i=0; i < iCreateCnt; ++i )
			{
				int iCurIndex;
				ioHashString szDummyCharName;
				float fStartAngle;
				D3DXVECTOR3 vTargetPos;

				rkPacket >> szDummyCharName >> iCurIndex;
				rkPacket >> fStartAngle;
				rkPacket >> vTargetPos;

				ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
															   pChar->GetCharName(),
															   vTargetPos,
															   fStartAngle,
															   0, false );

				if( pDummy )
				{
					pDummy->SetControlSkillName( GetName() );

					if( pDummy->GetType() == DCT_NO_CONTROL )
						pDummy->SetWorldOrientation( pChar->GetTargetRot() );
				}
			}
		}
		break;
	}
}

bool ioCreepSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_State == SS_CREEP && m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioCreepSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_State == SS_END && m_dwMotionEndTime + dwActionStop < FRAMEGETTIME() )
		return true;
	
	return false;
}

bool ioCreepSkill::IsDownState() const
{
	if( m_State == SS_CREEP )
		return true;

	return false;
}

void ioCreepSkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharList.empty() )
		return;

	D3DXVECTOR3 vCreatePos = pChar->GetWorldPosition();

	pChar->CreateMapEffectBySkill( m_DummyCharEffect, vCreatePos );

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	//
	vDummyCharCreateInfoList vCreateList;
	vCreateList.clear();

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = vCreatePos + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
													   pChar->GetCharName(),
													   vTargetPos,
													   fStartAngle,
													   0, true );

		if( pDummy )
		{
			pDummy->SetControlSkillName( GetName() );

			if( pDummy->GetType() == DCT_NO_CONTROL )
				pDummy->SetWorldOrientation( pChar->GetTargetRot() );
		}

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		vCreateList.push_back( kInfo );
	}

	int iCreateCnt = vCreateList.size();
	if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_DUMMY;
		kPacket << pChar->GetTargetRot();

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << vCreateList[i].m_DummyCharName;
			kPacket << vCreateList[i].m_iDummyCharIdx;
			kPacket << vCreateList[i].m_fDummyCharStartAngle;
			kPacket << vCreateList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
