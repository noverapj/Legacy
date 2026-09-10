#include "StdAfx.h"
#include "ioBlackSpearItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioDrinkItem.h"

ioBlackSpearItem::ioBlackSpearItem(void)
{
	m_fCurCoolTime = 0.0f;
	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;
	m_dwCheckBuffStartTime = 0;
}

ioBlackSpearItem::ioBlackSpearItem( const ioBlackSpearItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_GatherAnimation( rhs.m_GatherAnimation ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_GatherMove( rhs.m_GatherMove ),
m_GatherAttackList( rhs.m_GatherAttackList ),
m_fGatherRunSpeed( rhs.m_fGatherRunSpeed ),
m_dwGatherRotateTime( rhs.m_dwGatherRotateTime ),
m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
m_fRecoveryTime( rhs.m_fRecoveryTime ),
m_GatheringEffect( rhs.m_GatheringEffect ),
m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
m_dwMaxEffectGatherTime( rhs.m_dwMaxEffectGatherTime ),
m_vCheckBuffList( rhs.m_vCheckBuffList ),
m_vCheckAddBuffList( rhs.m_vCheckAddBuffList ),
m_dwCheckBuffGapTime( rhs.m_dwCheckBuffGapTime ),
m_GatherMoveEndAni( rhs.m_GatherMoveEndAni ),
m_fGatherMoveEndAniRate( rhs.m_fGatherMoveEndAniRate ),
m_fReduceTime( rhs.m_fReduceTime )
{
	m_fCurCoolTime = 0.0f;
	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;
	m_dwCheckBuffStartTime = 0;
}

ioBlackSpearItem::~ioBlackSpearItem(void)
{
}

void ioBlackSpearItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;

	m_dwAttackStartTime = 0;
	m_dwGatheringStartTime = 0;
	m_iGatherAttackIndex = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bTargetRot = false;
	m_bLeftRot = false;
	m_bSetGatheringMoveAni = false;
}

ioItem* ioBlackSpearItem::Clone()
{
	return new ioBlackSpearItem( *this );
}

ioWeaponItem::WeaponSubType ioBlackSpearItem::GetSubType() const
{
	return WST_BLACK_SPEAR_ITEM;
}

void ioBlackSpearItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gathering_move", "", szBuf, MAX_PATH );
	m_GatherMove = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	m_fGatherRunSpeed = rkLoader.LoadFloat_e( "gather_run_speed", 0.0f );
	m_dwGatherRotateTime = (DWORD)rkLoader.LoadInt_e( "gather_rotate_speed", 0 );

	int iMaxGatherAttack = rkLoader.LoadInt_e( "max_gather_attack_cnt", 0 );
	for( int i=0; i < iMaxGatherAttack; ++i )
	{
		GatherAttack rkInfo;
		wsprintf_e( szBuf, "gather_attack%d_min_time", i+1 );
		rkInfo.dwMinTime = rkLoader.LoadInt( szBuf, 0 );
		wsprintf_e( szBuf, "gather_attack%d_max_time", i+1 );
		rkInfo.dwMaxTime = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "gather_attack%d_max_combo", i+1 );
		int iMaxGatherCombo = rkLoader.LoadInt( szBuf, 0 );
		for( int j=0; j < iMaxGatherCombo; ++j )
		{
			AttackAttribute rkAttack;
			wsprintf_e( szBuf, "gather_attack%d_combo%d", i+1, j+1 );
			LoadAttackAttribute( szBuf, rkAttack, rkLoader );
			rkInfo.GatherLinearCombo.push_back( rkAttack );
		}
		m_GatherAttackList.push_back( rkInfo );
	}

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );
	m_fReduceTime = rkLoader.LoadFloat_e( "reduce_cool_time", 0.0f );
	

	//Effect
	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	m_dwMaxEffectGatherTime = (DWORD)rkLoader.LoadInt_e( "gathering_max_effect_time", 0 );

	int iMaxCheckBuffCnt = rkLoader.LoadInt_e( "max_check_buff_count", 0 );
	if ( iMaxCheckBuffCnt > 0 )
	{
		m_vCheckBuffList.reserve( iMaxCheckBuffCnt );
		m_vCheckAddBuffList.reserve( iMaxCheckBuffCnt );

		for ( int i=0 ; i<iMaxCheckBuffCnt ; ++i )
		{
			ioHashString szName;
			wsprintf_e( szBuf, "check_buff%d", i+1 );
			rkLoader.LoadString(szBuf, "", szKey, MAX_PATH );
			szName = szKey;
			m_vCheckBuffList.push_back( szName );
			szName.Clear();

			wsprintf_e( szBuf, "check_buff%d_add_buff", i+1 );
			rkLoader.LoadString(szBuf, "", szKey, MAX_PATH );
			szName = szKey;
			m_vCheckAddBuffList.push_back( szName );
		}
	}

	m_dwCheckBuffGapTime = (DWORD)rkLoader.LoadInt_e( "check_buff_gap_time", 0 );

	rkLoader.LoadString_e( "charge_run_end_ani", "", szBuf, MAX_PATH );
	m_GatherMoveEndAni = szBuf;
	m_fGatherMoveEndAniRate = rkLoader.LoadFloat_e( "charge_run_end_ani_rate", FLOAT1 );
}
void ioBlackSpearItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	ClearData();
	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioBlackSpearItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_GATHERING:
		ProcessGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_GATHER_ATTACK:
		ProcessGatherAttack( pOwner );
		break;
	case CS_GATHER_ATTACK_END:
		ProcessGatherAttackEnd( pOwner );
		break;
	}

	CheckGatherMoveState( pOwner );
}

void ioBlackSpearItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToGathering( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioBlackSpearItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetGatheringMoveAni = false;
	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBlackSpearItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{	
		if ( pOwner->IsDefenseKey() )
			CheckBrandBuff( pOwner );
		

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioBlackSpearItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;
	if ( !pOwner->IsSettedDirection() )
		pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	else
	{
		m_bSetGatheringMoveAni = true;
		pGrp->SetLoopAni( m_GatherMove, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( true );
	
	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
		m_GatheringEffectID = pEffect->GetUniqueID();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GATHERING;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBlackSpearItem::ProcessGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		ClearData();
		DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
		DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );
		pOwner->SetJumpState();
		return;
	}
	else if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
	{
		ClearData();
		DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
		DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );
		pOwner->ChangeDirectionByInputDir( false );
		pOwner->SetDashState( false );
		pOwner->SendDashState( false );
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( pOwner->IsDefenseKey() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				ClearData();
				DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
				DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );

				m_ChargeState = CS_NONE;
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << ST_NONE;
					kPacket << m_iCurCombo;
					kPacket << false;		// delaystate
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}


		if( ( m_dwGatheringStartTime + m_dwMaxEffectGatherTime < dwCurTime ) &&
			m_GatheringMaxEffectID == -1 )
		{
			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
			ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
			if( pEffect )
			{
				m_GatheringMaxEffectID = pEffect->GetUniqueID();
			}

			if( !m_GatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_GATHERING_MAX;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		ChangeToGatherAttackState( pOwner );
		return;
	}
}

void ioBlackSpearItem::ChangeToGatherAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if ( m_GatherAttackList.empty() )
		return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwGatheringStartTime;

	if ( dwGapTime >= m_dwMaxChargeTime )
	{
		CallGatherAttack( pOwner, m_GatherAttackList.size()-1 );
		return;
	}
	else
	{
		int iSize = (int)m_GatherAttackList.size();
		for ( int i=0; i<iSize ; ++i )
		{
			const GatherAttack& rkInfo = m_GatherAttackList[i];
			if ( COMPARE( dwGapTime, rkInfo.dwMinTime, rkInfo.dwMaxTime ) )
			{
				CallGatherAttack( pOwner, i );
				return;
			}
		}
	}
	
}

void ioBlackSpearItem::CallGatherAttack( ioBaseChar *pOwner, int iIndex )
{
	if( !pOwner )
		return;

	if ( m_GatherAttackList.empty() )
		return;

	if( (int)m_GatherAttackList.size() <= iIndex )
		return;

	pOwner->SetAttackMoveEnable( false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	m_iGatherAttackIndex = 0;
	m_CurGatherAttackList.clear();
	m_CurGatherAttackList = m_GatherAttackList[iIndex].GatherLinearCombo;
	CallGatherAttackCombo( pOwner, false );
	if ( pOwner->IsNeedProcess() )
		pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GATHERING_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << iIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioBlackSpearItem::CallGatherAttackCombo( ioBaseChar *pOwner, bool bSend )
{
	if ( !pOwner )
		return;

	if ( m_CurGatherAttackList.empty() )
		return;

	if ( (int)m_CurGatherAttackList.size() <= m_iGatherAttackIndex )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute& rkAttack = m_CurGatherAttackList[m_iGatherAttackIndex];

	ClearCancelInfo();
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_ChargeState = CS_GATHER_ATTACK;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( rkAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
	float fTimeRate = rkAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = rkAttack.m_dwPreDelay;

	pOwner->RefreshFireTimeList( iAniID, rkAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	pOwner->SetReservedSliding( rkAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	m_dwMotionEndTime = dwCurTime + dwPreDelay; 
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwMotionEndTime += rkAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwPreDelay + dwCurTime;

	SetCurAttackAniForInputJump( rkAttack.m_AttackAnimation, fTimeRate );
	
	if ( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GATHERING_ATTACK_COMBO;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iGatherAttackIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iGatherAttackIndex++;
}

void ioBlackSpearItem::ProcessGatherAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	if( pOwner->IsNeedProcess() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if ( m_dwMotionEndTime < dwCurTime )
		{
			if ( m_iGatherAttackIndex < (int)m_CurGatherAttackList.size() )
			{
				int iPreGatherAttackIndex = m_iGatherAttackIndex;
				CallGatherAttackCombo( pOwner, true );
				
				return;
			}
			else
			{
				ChangeToGatherAttackEnd( pOwner );
				return;
			}
		}
		else
		{
			if( pOwner->IsJumpKey() )
			{
				pOwner->SetJumpState();
				return;
			}

			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				ClearData();
				DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
				DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
			CheckKeyInput( pOwner );
			if ( pOwner->IsDefenseKey() )
				CheckBrandBuff( pOwner );
		}
	}

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );
	pOwner->SetCurMoveSpeed( m_fGatherRunSpeed );
	ProcessRotate( pOwner );
}

void ioBlackSpearItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioBlackSpearItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwGatherRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwGatherRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioBlackSpearItem::CheckGatherMoveState( ioBaseChar *pOwner )
{
	if ( m_ChargeState != CS_GATHERING )
		return;
	if( !pOwner || !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;
	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bSetGatheringMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_GatherMove, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bSetGatheringMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetGatheringMoveAni = false;
		}
	}
}

void ioBlackSpearItem::ChangeToGatherAttackEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pOwner->ClearAttackFireTimeAndSkill();

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fGatherMoveEndAniRate > 0.0f )
			fCurRate = m_fGatherMoveEndAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_GatherMoveEndAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetCharCollisionSkipTime( 0, 0 );

	m_ChargeState = CS_GATHER_ATTACK_END;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GATHERING_ATTACK_END;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBlackSpearItem::ProcessGatherAttackEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
	{
		m_dwMotionEndTime = 0;

		// 예약처리
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearData();

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearData();
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearData();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearData();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearData();
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		ClearData();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

int ioBlackSpearItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

int ioBlackSpearItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

void ioBlackSpearItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioBlackSpearItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_dwMaxCoolTime <= m_fCurCoolTime )
		return;

	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fRecoveryTime * fTimePerSec;

	m_fCurCoolTime += fGauge;
	m_fCurCoolTime = (float)min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
}

void ioBlackSpearItem::DecreaseCoolTime( int iNeedBullet )
{
	if( m_fCurCoolTime < (float)iNeedBullet )
	{
		m_fCurCoolTime = 0.0f;
	}
	else
	{
		m_fCurCoolTime -= (float)iNeedBullet;
	}
}

void ioBlackSpearItem::DeleteOwnerAttachEffect( ioBaseChar *pOwner, UniqueObjID& EffectID, bool bGrace /*= true */ )
{
	if ( !pOwner )
		return;
	if( EffectID != -1 )
	{
		pOwner->EndEffect( EffectID, bGrace );
		EffectID = -1;
	}
}

void ioBlackSpearItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem/*=false */ )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
	DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );

	ClearData();
}

void ioBlackSpearItem::CheckBrandBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	if( m_fCurCoolTime < m_fReduceTime )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCheckBuffStartTime + m_dwCheckBuffGapTime > dwCurTime )
		return;

	m_dwCheckBuffStartTime = dwCurTime;

	bool bAddBuff = AddNewBrandAttackBuff( pOwner );

	if ( pOwner->IsNeedSendNetwork() && bAddBuff )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHECK_BRAND_BUFF;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if ( bAddBuff )
		DecreaseCoolTime( m_fReduceTime );
}

bool ioBlackSpearItem::AddNewBrandAttackBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	ioHashString szOwnerName = pOwner->GetCharName();
	bool bCheck = false;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) 
			continue;
		if( pTarget->GetCharName() == GetOwnerName() ) 
			continue;
		if( pTarget->IsSystemState() )
			continue;
		if( !pTarget->IsEnableTarget() )
			continue;

		for ( int i=0; i<(int)m_vCheckBuffList.size() ; ++i )
		{
			if ( pTarget->HasBuff( szOwnerName, m_vCheckBuffList[i] ) )
			{
				pTarget->RemoveBuff( m_vCheckBuffList[i] );
				pTarget->AddNewBuff( m_vCheckAddBuffList[i], pOwner->GetCharName(), GetName(), NULL );
				bCheck = true;
			}
		}
	}

	return bCheck;
}

void ioBlackSpearItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case ST_NONE:
		{
			rkPacket >> m_iCurCombo;
			pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			ClearData();
			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );
			DeleteOwnerAttachEffect( pOwner, m_GatheringMaxEffectID, false );
			m_ChargeState = CS_NONE;
			bool bDelayState;
			rkPacket >> bDelayState;
		}
		break;
	case ST_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case ST_GATHERING:
		{
			rkPacket >> m_iCurCombo;
			ChangeToGathering( pOwner );
		}
		break;
	case ST_GATHERING_MAX:
		{
			rkPacket >> m_iCurCombo;
			DeleteOwnerAttachEffect( pOwner, m_GatheringEffectID, false );

			if( m_GatheringMaxEffectID == -1 )
			{
				ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
				if( pEffect )
				{
					m_GatheringMaxEffectID = pEffect->GetUniqueID();
				}
			}

			if( !m_GatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}
		}
		break;
	case ST_GATHERING_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetWorldPosition( vPos );
			int iIndex;
			rkPacket >> iIndex;
			CallGatherAttack( pOwner, iIndex );
		}
		break;
	case ST_GATHERING_ATTACK_COMBO:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetWorldPosition( vPos );
			rkPacket >> m_iGatherAttackIndex;
			CallGatherAttackCombo( pOwner, false );
		}
		break;
	case ST_GATHERING_ATTACK_END:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetWorldPosition( vPos );
			ChangeToGatherAttackEnd( pOwner );
		}
		break;
	case ST_ROTATE:
		{
			int iDirKey;
			rkPacket >> iDirKey;
			rkPacket >> m_bLeftRot;
			
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			if( iDirKey == ioUserKeyInput::DKI_NONE )
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			else
			{
				m_bTargetRot = false;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
			}
		}
		break;
	case ST_CHECK_BRAND_BUFF:
		AddNewBrandAttackBuff( pOwner );
		break;
	}
}