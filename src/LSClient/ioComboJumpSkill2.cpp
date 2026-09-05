

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioComboJumpSkill2.h"

#include "WeaponDefine.h"

ioComboJumpSkill2::ioComboJumpSkill2()
{
	ClearData();
}

ioComboJumpSkill2::ioComboJumpSkill2( const ioComboJumpSkill2 &rhs )
	: ioNormalSkill( rhs ),
	m_iMaxCombo( rhs.m_iMaxCombo ),
	m_ComboList( rhs.m_ComboList ),
	m_szEndAnimation( rhs.m_szEndAnimation ),
	m_fEndAniRate( rhs.m_fEndAniRate ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_SkillMapEffect( rhs.m_SkillMapEffect )
{
	ClearData();
}

ioComboJumpSkill2::~ioComboJumpSkill2()
{
}

void ioComboJumpSkill2::ClearData()
{
	m_SkillState = SS_NONE;
	m_iCurCombo = 0;

	m_bJump = false;
	m_bReserveJump = false;

	m_vPrePosition = ioMath::VEC3_ZERO;
	m_vMoveDirection = ioMath::VEC3_ZERO;

	m_dwAttackEndTime = 0;

	m_fLastHeight = 0.0f;
}

void ioComboJumpSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_iMaxCombo = rkLoader.LoadInt_e( "combo_count", 0 );
	m_ComboList.clear();
	m_ComboList.reserve( m_iMaxCombo );
	for( int i=0; i<m_iMaxCombo; ++i )
	{
		ComboInfo kInfo;
		wsprintf_e( szBuf, "combo%d_attack", i+1 );
		LoadAttackAttribute( szBuf, kInfo.m_AttackAttribute, rkLoader );

		wsprintf_e( szBuf, "combo%d_enable_min_height", i+1 );
		kInfo.m_fEnableMinHeight = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "combo%d_enable_max_height", i+1 );
		kInfo.m_fEnableMaxHeight = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "combo%d_enable_reserve_height", i+1 );
		kInfo.m_fEnableReserveHeight = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "combo%d_jump_power", i+1 );
		kInfo.m_fJumpPower = rkLoader.LoadFloat( szBuf, 0.0f );

		m_ComboList.push_back( kInfo );
	}

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_szEndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	LoadBuffList( rkLoader );
}


void ioComboJumpSkill2::LoadBuffList( ioINILoader &rkLoader )
{
	ioBuffInfo tBuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_OwnerBuffList.clear();

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
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


ioSkill* ioComboJumpSkill2::Clone()
{
	return new ioComboJumpSkill2( *this );
}

bool ioComboJumpSkill2::OnSkillStart( ioBaseChar *pChar )
{
	ioSkill::OnSkillStart(pChar);

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
	m_iCurActiveCount = m_iMaxActiveCount;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	SetCameraBuff( pChar );

	CheckChangeDirRot( pChar );

	AddOwnerBuff( pChar );


	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtCharRot;
	ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	ioPlayStage *pStage = pChar->GetCreator();

	D3DXVECTOR3 vScale = pChar->GetWorldScale();

	m_dwMapUniqId = 0;

	if( !m_SkillMapEffect.IsEmpty() && pStage )
	{
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_SkillMapEffect, vPos, vScale );

		if( pMapEffect )
		{	
			pMapEffect->SetWorldOrientation( qtCharRot );
			m_dwMapUniqId = pMapEffect->GetUniqueID();
		}
	}
	//////////////////////////////////////////////////////////////////////////

	return true;
}

void ioComboJumpSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage || !pChar )
		return;

	if( m_dwMapUniqId )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );

		D3DXVECTOR3 vPos = pChar->GetWorldPosition();

		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwMapUniqId );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( vPos );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
		else
			m_dwMapUniqId = 0;
	}

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
	case SS_ACTION:
		ProcessActionState( pChar, pStage );
		CheckExtraAniJump( pChar );
		break;
	case SS_END:
		break;
	}
}

void ioComboJumpSkill2::OnSkillEnd( ioBaseChar *pChar )
{
	//자신에게 생성하는 버프를 삭제
	for (int i=0; i < (int)m_OwnerBuffList.size(); ++i)
	{
		ioBuffInfo pOwnerBuff = m_OwnerBuffList[i];
		ioBuff *pBuff = pChar->GetBuff( pOwnerBuff.m_BuffName );
		//pChar->RemoveBuff( pOwnerBuff.m_BuffName, false);			
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}

	ioNormalSkill::OnSkillEnd( pChar );

	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	EndCameraBuff( pChar );
}

void ioComboJumpSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioNormalSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	//ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	/*if( pZoneEffect )
	{
		pZoneEffect->SetCollisionDir( m_vMoveDirection );
		pZoneEffect->SetZoneValue( false, false );
	}*/

	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{				
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}		
	}
}


void ioComboJumpSkill2::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( !m_OwnerBuffList[i].m_BuffName.IsEmpty() )
		{
			pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, "", "", this );
		}
	}
}

void ioComboJumpSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar )
		return;
	if( !pStage )
		return;

	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case SSC_ACTION:
		rkPacket >> m_vMoveDirection;
		CheckRotateByKeyInput( pChar );
		SetAction( pChar, false );
		break;
	case SSC_END:
		bool bEndMotion;
		rkPacket >> bEndMotion;
		SetEndState( pChar, bEndMotion );
		break;
	}
}

bool ioComboJumpSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_ACTION:
		return false;

	case SS_END:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}

	return true;
}

void ioComboJumpSkill2::SetNormalSkill( ioBaseChar *pChar )
{
	SetAction( pChar, false );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	m_SkillState = SS_ACTION;
}

bool ioComboJumpSkill2::IsAttackEndState() const
{
	if( m_iCurCombo >= m_iMaxCombo && m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

void ioComboJumpSkill2::ProcessActionState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )	return;
	if( !pStage )	return;

	if( m_bJump && pChar->GetWorldPosition().y < m_vPrePosition.y )
	{
		m_bJump = false;
	}
	m_vPrePosition = pChar->GetWorldPosition();

	if( !pChar->IsNeedProcess() )
		return;

	if( CheckEnableKeyInput( pChar, pStage ) )
	{
		if( m_bReserveJump )
		{
			SetAction( pChar, true );
		}
		else if( pChar->IsAttackKey() )
		{
			CheckRotateByKeyInput( pChar );
			SetAction( pChar, true );
		}
	}
	else if( CheckReserveKeyInput( pChar, pStage ) )
	{
		if( pChar->IsAttackKey() )
		{
			CheckRotateByKeyInput( pChar );
			m_bReserveJump = true;
		}
	}
	else if( m_iCurCombo >= m_iMaxCombo )
	{
		SetEndState( pChar, false );
	}

	if( CheckLanding( pChar ) )
	{
		SetEndState( pChar, true );
		return;
	}
}

void ioComboJumpSkill2::SetAction( ioBaseChar *pChar, bool bSend )
{
	if( !pChar )
		return;

	if( m_iCurCombo >= m_iMaxCombo )
		return;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDirection );
	pChar->SetTargetRotAndMoveDirChange( qtRot );

	ComboInfo kInfo = m_ComboList[m_iCurCombo];
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = kInfo.m_AttackAttribute.m_AttackAnimation;

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = kInfo.m_AttackAttribute.m_fAttackAniRate;
	float fTimeRate = fAniRate * m_fExtraAniRate;
	DWORD dwPreDelay = kInfo.m_AttackAttribute.m_dwPreDelay;

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

	DWORD dwWeaponBaseIndex = m_dwWeaponBaseIndex + m_iCurCombo;
	if( m_iCurCombo == 0 )
	{
		pChar->RefreshFireTimeListWithBase( dwWeaponBaseIndex,
			iCurSkillAniID,
			kInfo.m_AttackAttribute.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}
	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );
	pChar->SetReservedSliding( kInfo.m_AttackAttribute.m_vForceInfoList, iCurSkillAniID, fTimeRate, dwPreDelay );
	pChar->SetJumpPower( kInfo.m_fJumpPower );

	m_iCurCombo++;
	m_iCurCombo = min( m_iCurCombo, m_iMaxCombo );

	m_bJump = true;
	m_bReserveJump = false;

	m_iCurActiveCount--;
	m_iCurActiveCount = max( 0, m_iCurActiveCount );

	float fHeight = pChar->GetBottomHeight();
	if( fHeight > 0.0f )
		m_fLastHeight = fHeight;

	if( pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ACTION;
		kPacket << m_vMoveDirection;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioComboJumpSkill2::SetEndState( ioBaseChar *pChar, bool bSetEndMotion )
{
	if( m_SkillState == SS_END )
		return;

	m_SkillState = SS_END;

	if( bSetEndMotion )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();

		int iAniID = pGrp->GetAnimationIdx( m_szEndAnimation );
		if( iAniID == -1 )	return;

		pChar->AttachEffect( m_SkillEffectName );

		float fAniRate = m_fEndAniRate;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;
	}

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

bool ioComboJumpSkill2::CheckLanding( ioBaseChar *pChar )
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

bool ioComboJumpSkill2::CheckEnableKeyInput( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_iCurCombo >= m_iMaxCombo )
		return false;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( m_fLastHeight > 0.0f && fMapHeight <= 0.0f )
		fMapHeight = m_fLastHeight;

	float fHeight = vPos.y - fMapHeight;

	float fMinHeight = m_ComboList[m_iCurCombo].m_fEnableMinHeight;
	float fMaxHeight = m_ComboList[m_iCurCombo].m_fEnableMaxHeight;
	if( COMPARE( fHeight, fMinHeight, fMaxHeight ) && !m_bJump )
	{
		return true;
	}

	return false;
}

bool ioComboJumpSkill2::CheckReserveKeyInput( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_iCurCombo >= m_iMaxCombo )
		return false;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	float fHeight = vPos.y - fMapHeight;

	float fMinHeight = m_ComboList[m_iCurCombo].m_fEnableMinHeight;
	float fMaxHeight = m_ComboList[m_iCurCombo].m_fEnableReserveHeight;
	if( COMPARE( fHeight, fMinHeight, fMaxHeight ) && !m_bJump )
	{
		return true;
	}

	return false;
}

void ioComboJumpSkill2::CheckRotateByKeyInput( ioBaseChar *pChar )
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