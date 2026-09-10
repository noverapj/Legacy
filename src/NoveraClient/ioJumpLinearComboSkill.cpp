

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioJumpLinearComboSkill.h"

#include "WeaponDefine.h"

ioJumpLinearComboSkill::ioJumpLinearComboSkill()
{
	ClearData();
}

ioJumpLinearComboSkill::ioJumpLinearComboSkill( const ioJumpLinearComboSkill &rhs )
	: ioNormalSkill( rhs ),
	m_JumpMotionAttribute( rhs.m_JumpMotionAttribute ),
	m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
	m_vLinearAttackAttribute( rhs.m_vLinearAttackAttribute ),
	m_fJumpPower( rhs.m_fJumpPower ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioJumpLinearComboSkill::~ioJumpLinearComboSkill()
{
}

void ioJumpLinearComboSkill::ClearData()
{
	m_SkillState = SS_NONE;

	m_bReserveLinearCombo = false;

	m_vPrePosition = ioMath::VEC3_ZERO;
	m_vMoveDirection = ioMath::VEC3_ZERO;

	m_dwAttackEndTime = 0;

	m_fLastHeight = 0.0f;

	m_nCurLinearComboIndex = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioJumpLinearComboSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH];

	LoadAttackAttribute( "jump_motion", m_JumpMotionAttribute, rkLoader );
	m_fJumpPower = rkLoader.LoadFloat( "jump_power", 0.0f );

	LoadAttackAttribute( "jump_land_attack", m_LandAttackAttribute, rkLoader );

	int nMaxLinear = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	for( int i = 0; i < nMaxLinear; i++ )
	{
		AttackAttribute kComboAttack;
		wsprintf_e( szKey, "linear_combo%d", i+1 );
		LoadAttackAttribute( szKey, kComboAttack, rkLoader );

		m_vLinearAttackAttribute.push_back( kComboAttack );
	}

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioJumpLinearComboSkill::Clone()
{
	return new ioJumpLinearComboSkill( *this );
}

bool ioJumpLinearComboSkill::OnSkillStart( ioBaseChar *pChar )
{
	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_SkillSoundID = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetNormalSkill( pChar );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	ClearData();
	m_vMoveDirection = pChar->GetMoveDir();
	//m_iCurActiveCount = m_iMaxActiveCount;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	SetCameraBuff( pChar );

	CheckChangeDirRot( pChar );

	return true;
}

void ioJumpLinearComboSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();
	switch( m_SkillState )
	{
	case SS_NONE:
		if( m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetNormalSkill( pChar );
		}
		break;
	case SS_JUMP:
		ProcessJumpState( pChar, pStage );
		CheckExtraAniJump( pChar );
		break;
	case SS_LANDING:
		ProcessLandingState( pChar, pStage );
		break;
	case SS_LINECOMBO:
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
		ProcessLinearAttackState( pChar, pStage );
		break;
	case SS_END:
		break;
	}
}

void ioJumpLinearComboSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	EndCameraBuff( pChar );
}

void ioJumpLinearComboSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioNormalSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		pZoneEffect->SetCollisionDir( m_vMoveDirection );
		pZoneEffect->SetZoneValue( true, true );
	}
}

void ioJumpLinearComboSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;
	if( !pStage )
		return;

	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case SSC_JUMP:
		rkPacket >> m_vMoveDirection;
		CheckRotateByKeyInput( pChar );
		SetJumpAction( pChar, false );
		break;
	case SSC_LANDING:
		SetLanding( pChar, false );
		break;
	case SSC_LINECOMBO:
		{
			int nComboIndex;
			rkPacket >> nComboIndex;

			int iDirKey;
			rkPacket >> iDirKey;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;	
			pChar->SetTargetRotToDirKey( m_CurDirKey );
			SetLinearComboState( pChar, nComboIndex );

			if( IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	case SSC_END:
		bool bEndMotion;
		rkPacket >> bEndMotion;
		SetEndState( pChar, bEndMotion );
		break;
	}
}

bool ioJumpLinearComboSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_JUMP:
	case SS_LANDING:
	case SS_LINECOMBO:
		return false;

	case SS_END:
		return true;
	}

	return true;
}

void ioJumpLinearComboSkill::SetNormalSkill( ioBaseChar *pChar )
{
	SetJumpAction( pChar, false );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	m_SkillState = SS_JUMP;
}

bool ioJumpLinearComboSkill::IsAttackEndState() const
{
	/*if( m_iCurCombo >= m_iMaxCombo && m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
		return true;*/

	return false;
}

void ioJumpLinearComboSkill::ProcessJumpState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	m_vPrePosition = pChar->GetWorldPosition();

	if( !pChar->IsNeedProcess() )
		return;

	if( CheckLanding( pChar ) )
	{
		SetLanding( pChar, true );
	}
}


void ioJumpLinearComboSkill::ProcessLandingState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar->IsNeedProcess() )
		return;

	if( pChar->IsAttackKey() )
		m_bReserveLinearCombo = true;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( m_bReserveLinearCombo )
			SetLinearComboState( pChar, 0 );
		else
			SetEndState( pChar, false );
	}
}


void ioJumpLinearComboSkill::ProcessLinearAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	/*if( dwCurTime > m_dwEnableReserveTime && m_bReserveLinearCombo )
	{
		SetLinearComboState( pChar, m_nCurLinearComboIndex + 1 );
		return;
	}*/

	if( dwCurTime > m_dwMotionEndTime )
	{
		if( !m_bReserveLinearCombo )
			SetEndState( pChar, false );
		else
			SetLinearComboState( pChar, m_nCurLinearComboIndex + 1 );

		return;
	}

	if( !m_bReserveLinearCombo && dwCurTime > m_dwEnableReserveTime && pChar->IsAttackKey() )
		m_bReserveLinearCombo = true;
		
}


void ioJumpLinearComboSkill::SetJumpAction( ioBaseChar *pChar, bool bSend )
{
	if( !pChar )
		return;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDirection );
	pChar->SetTargetRotAndMoveDirChange( qtRot );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = m_JumpMotionAttribute.m_AttackAnimation;

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = m_JumpMotionAttribute.m_fAttackAniRate;
	float fTimeRate = fAniRate * m_fExtraAniRate;
	DWORD dwPreDelay = m_JumpMotionAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = FRAMEGETTIME() + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	/*DWORD dwWeaponBaseIndex = m_dwWeaponBaseIndex;
	pChar->RefreshFireTimeListWithBase( dwWeaponBaseIndex,
	iCurSkillAniID,
	m_JumpMotionAttribute.m_vWeaponInfoList,
	FTT_SKILL_ATTACK,
	fTimeRate,
	dwPreDelay );*/
	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );
	pChar->SetReservedSliding( m_JumpMotionAttribute.m_vForceInfoList, iCurSkillAniID, fTimeRate, dwPreDelay );
	pChar->SetJumpPower( m_fJumpPower );

	//m_iCurCombo++;
	//m_iCurCombo = min( m_iCurCombo, m_iMaxCombo );

	//m_bJump = true;

	//m_iCurActiveCount--;
	//m_iCurActiveCount = max( 0, m_iCurActiveCount );

	float fHeight = pChar->GetBottomHeight();
	if( fHeight > 0.0f )
		m_fLastHeight = fHeight;

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_JUMP;
		kPacket << m_vMoveDirection;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJumpLinearComboSkill::SetLinearComboState( ioBaseChar *pChar, int nComboIndex )
{
	if( !pChar || (int)m_vLinearAttackAttribute.size() <= nComboIndex )
	{
		SetEndState( pChar, false );
		return;
	}

	m_SkillState = SS_LINECOMBO;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
	{
		SetEndState( pChar, false );
		return;
	}

	m_bReserveLinearCombo = false;

	m_nCurLinearComboIndex = nComboIndex;

	pChar->ClearSkillReserveKey();
	const AttackAttribute &kAttack = m_vLinearAttackAttribute[nComboIndex];

	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );
	}

	CheckRotateByKeyInput( pChar );
	
	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	pChar->InitActionStopDelay( true );
	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pChar->SetAutoTarget( ATT_SKILL );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex + nComboIndex + 1,
		iAniID,
		kAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	//if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LINECOMBO;
		kPacket << m_nCurLinearComboIndex;
		kPacket << (int)m_CurDirKey;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJumpLinearComboSkill::SetLanding( ioBaseChar *pChar, bool bSend )
{
	m_SkillState = SS_LANDING;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = m_LandAttackAttribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	DWORD dwPreDelay = m_LandAttackAttribute.m_dwPreDelay;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	DWORD dwWeaponBaseIndex = m_dwWeaponBaseIndex;
	pChar->RefreshFireTimeListWithBase( dwWeaponBaseIndex,
	iAniID,
	m_LandAttackAttribute.m_vWeaponInfoList,
	FTT_SKILL_ATTACK,
	fAniRate,
	dwPreDelay );

	if( bSend && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LANDING;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioJumpLinearComboSkill::SetEndState( ioBaseChar *pChar, bool bSetEndMotion )
{
	if( m_SkillState == SS_END )
		return;

	m_SkillState = SS_END;


	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << bSetEndMotion;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioJumpLinearComboSkill::CheckLanding( ioBaseChar *pChar )
{
	float fHeightGap = 0.0f;
	float fHeight = pChar->GetWorldPosition().y;
	float fMapHeight = pChar->GetBottomHeight();

	if( pChar->CheckGhostState() )
	{
		fHeightGap = fHeight - pChar->GetPreHeightForGhostState();
	}
	else if( m_fLastHeight > 0.0f && fMapHeight <= 0.0f )
	{
		fHeightGap = fHeight - m_fLastHeight;
	}
	else
	{
		fHeightGap = fHeight - pChar->GetBottomHeight();
	}

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

//bool ioJumpLinearComboSkill::CheckEnableKeyInput( ioBaseChar *pChar, ioPlayStage *pStage )
//{
//	if( m_iCurCombo >= m_iMaxCombo )
//		return false;
//
//	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
//	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
//	if( m_fLastHeight > 0.0f && fMapHeight <= 0.0f )
//		fMapHeight = m_fLastHeight;
//
//	float fHeight = vPos.y - fMapHeight;
//
//	float fMinHeight = m_ComboList[m_iCurCombo].m_fEnableMinHeight;
//	float fMaxHeight = m_ComboList[m_iCurCombo].m_fEnableMaxHeight;
//	if( COMPARE( fHeight, fMinHeight, fMaxHeight ) && !m_bJump )
//	{
//		return true;
//	}
//
//	return false;
//}
//
//bool ioJumpLinearComboSkill::CheckReserveKeyInput( ioBaseChar *pChar, ioPlayStage *pStage )
//{
//	if( m_iCurCombo >= m_iMaxCombo )
//		return false;
//
//	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
//	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
//	float fHeight = vPos.y - fMapHeight;
//
//	float fMinHeight = m_ComboList[m_iCurCombo].m_fEnableMinHeight;
//	float fMaxHeight = m_ComboList[m_iCurCombo].m_fEnableReserveHeight;
//	if( COMPARE( fHeight, fMinHeight, fMaxHeight ) && !m_bJump )
//	{
//		return true;
//	}
//
//	return false;
//}

void ioJumpLinearComboSkill::CheckRotateByKeyInput( ioBaseChar *pChar )
{
	ioUserKeyInput::DirKeyInput eDirKey = pChar->CheckCurDirKey();

	if( eDirKey == ioUserKeyInput::DKI_NONE )
	{
		m_vMoveDirection = pChar->GetMoveDir();
		return;
	}
	else
	{
		D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		float fYawD = 180.0f + 45.0f * ( eDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
		m_vMoveDirection = qtRotate * vDir;
	}
}


bool ioJumpLinearComboSkill::IsProtected( int iDefenseBreakType ) const
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
	if( 0 < m_dwSkillProtectTime && dwCurTime < (m_dwSkillStartTime+m_dwSkillProtectTime) )
		return true;

	return false;
}