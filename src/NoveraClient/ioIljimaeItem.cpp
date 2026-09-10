

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioIljimaeItem.h"

ioIljimaeItem::ioIljimaeItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwReturnMarkID = -1;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_fCurCoolTime = 0.0f;

	m_vReturnPosition = ioMath::VEC3_ZERO;
	m_dwReturnMarkEndTime = 0;
}

ioIljimaeItem::ioIljimaeItem( const ioIljimaeItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_GatheringEffect( rhs.m_GatheringEffect ),
m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
m_fUseCoolTime( rhs.m_fUseCoolTime ),
m_fRecoveryTime( rhs.m_fRecoveryTime ),
m_fRecoveryExTime( rhs.m_fRecoveryExTime ),
m_SearchMotion( rhs.m_SearchMotion ),
m_fSearchMotionRate( rhs.m_fSearchMotionRate ),
m_TeleportMotion( rhs.m_TeleportMotion ),
m_fTeleportMotionRate( rhs.m_fTeleportMotionRate ),
m_TeleportEndMotion( rhs.m_TeleportEndMotion ),
m_fTeleportEndMotionRate( rhs.m_fTeleportEndMotionRate ),
m_fDisableRange( rhs.m_fDisableRange ),
m_fSearchRange( rhs.m_fSearchRange ),
m_fSearchAngle( rhs.m_fSearchAngle ),
m_iSearchCount( rhs.m_iSearchCount ),
m_dwSearchDuration( rhs.m_dwSearchDuration ),
m_fTeleportRange( rhs.m_fTeleportRange ),
m_fTeleportAngle( rhs.m_fTeleportAngle ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_fMoveRange( rhs.m_fMoveRange ),
m_fMoveHeightRange( rhs.m_fMoveHeightRange ),
m_TeleportMark( rhs.m_TeleportMark ),
m_ReturnMark( rhs.m_ReturnMark ),
m_dwReturnMarkDuration( rhs.m_dwReturnMarkDuration )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_fCurCoolTime = 0.0f;

	m_vReturnPosition = ioMath::VEC3_ZERO;
	m_dwReturnMarkEndTime = 0;
}

ioIljimaeItem::~ioIljimaeItem()
{
}

void ioIljimaeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iCnt = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0 ; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;

	rkLoader.LoadString_e( "search_motion", "", szBuf, MAX_PATH );
	m_SearchMotion = szBuf;
	m_fSearchMotionRate = rkLoader.LoadFloat_e( "search_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "teleport_motion", "", szBuf, MAX_PATH );
	m_TeleportMotion = szBuf;
	m_fTeleportMotionRate = rkLoader.LoadFloat_e( "teleport_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "teleport_end_motion", "", szBuf, MAX_PATH );
	m_TeleportEndMotion = szBuf;
	m_fTeleportEndMotionRate = rkLoader.LoadFloat_e( "teleport_end_motion_rate", FLOAT1 );

	m_fDisableRange = rkLoader.LoadFloat_e( "disable_target_range", FLOAT1000 );

	m_fSearchRange = rkLoader.LoadFloat_e( "search_target_range", 0.0f );
	m_fSearchAngle = rkLoader.LoadFloat_e( "search_target_angle", 0.0f );
	m_iSearchCount = rkLoader.LoadInt_e( "search_target_count", 1 );
	m_dwSearchDuration = (DWORD)rkLoader.LoadInt_e( "search_target_duration", 0 );

	m_fTeleportRange = rkLoader.LoadFloat_e( "teleport_target_range", 0.0f );
	m_fTeleportAngle = rkLoader.LoadFloat_e( "teleport_target_angle", 0.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fMoveRange = rkLoader.LoadFloat_e( "teleport_move_range", 0.0f );
	m_fMoveHeightRange = rkLoader.LoadFloat_e( "teleport_move_height_range", 0.0f );

	rkLoader.LoadString_e( "target_mark_effect", "", szBuf, MAX_PATH );
	m_TeleportMark = szBuf;

	rkLoader.LoadString_e( "return_mark_effect", "", szBuf, MAX_PATH );
	m_ReturnMark = szBuf;
	m_dwReturnMarkDuration = (DWORD)rkLoader.LoadInt_e( "return_mark_duration", 0 );

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_fUseCoolTime = rkLoader.LoadFloat_e( "use_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );
	m_fRecoveryExTime = rkLoader.LoadFloat_e( "recover_cool_time_ex", 0.0f );
}

void ioIljimaeItem::OnReleased( ioBaseChar *pOwner )
{
	ClearAllTarget( pOwner );
	ioAttackableItem::OnReleased( pOwner );
}

ioItem* ioIljimaeItem::Clone()
{
	return new ioIljimaeItem( *this );
}

ioWeaponItem::WeaponSubType ioIljimaeItem::GetSubType() const
{
	return WST_ILJIMAE;
}

void ioIljimaeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioIljimaeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_SEARCHING:
		OnSearching(pOwner);
		break;
	case CS_TELEPORT:
		OnTeleport( pOwner );
		break;
	case CS_TELEPORT_END:
		OnTeleportEnd( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_END:
	case CS_SEARCHING_END:
		break;
	}
}

void ioIljimaeItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioIljimaeItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );

				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				m_ChargeState = CS_NONE;
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
		}

		// MaxCheck
		if( m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime )
		{
			if( m_fCurCoolTime >= m_fUseCoolTime )
			{
				DecreaseCoolTime( m_fUseCoolTime );
				ChangeToTeleport( pOwner );			
				return;
			}
			else
			{
				ChangeToSearching( pOwner );
				return;
			}
		}
	}
	else
	{
		ChangeToSearching( pOwner );
	}
}

void ioIljimaeItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
	{
		m_GatheringEffectID = pEffect->GetUniqueID();
	}

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

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
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		m_ChargeState = CS_NONE;
		break;
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_SEARCHING:
		ChangeToSearching( pOwner );
		break;
	case CS_TELEPORT:
		ChangeToTeleport( pOwner );
		break;
	case CS_TELEPORT_END:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket << qtRot;
			rkPacket << vPos;
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			pOwner->SetWorldPosition( vPos );

			m_ChargeState = CS_TELEPORT_END;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fTeleportEndMotionRate > 0.0f )
				fCurRate = m_fTeleportEndMotionRate;

			int iAniID = pGrp->GetAnimationIdx( m_TeleportEndMotion );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

			m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

			pOwner->SetExtendAttackEndJumpInfo( iAniID, fCurRate, 0 );

			int iTargetCnt;
			rkPacket >> iTargetCnt;
			m_vTargetInfo.clear();
			m_vTargetInfo.reserve(iTargetCnt);
			for( int i=0 ; i<iTargetCnt; ++i )
			{
				TargetInfo kInfo;
				rkPacket >> kInfo.m_TargetName;
				rkPacket >> kInfo.m_dwEndTime;
				rkPacket >> kInfo.m_dwMarkEffectID;
				rkPacket >> kInfo.m_fLengthSq;
				m_vTargetInfo.push_back(kInfo);
			}
		}
		break;
	}
}

void ioIljimaeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
}

bool ioIljimaeItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioIljimaeItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}
}

bool ioIljimaeItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioIljimaeItem::GetAutoTargetValue( ioBaseChar *pOwner,
											float &fRange, float &fMinAngle, float &fMaxAngle,
											AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
	{
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
			return;
		case ATT_DASH:
			if( m_fDashAutoTargetRange == 0.0f )
				break;

			fRange = m_fDashAutoTargetRange;
			fMinAngle = m_fDashAutoTargetMinAngle;
			fMaxAngle = m_fDashAutoTargetMaxAngle;
			return;
		case ATT_JUMP:
			if( m_fJumpAutoTargetRange == 0.0f )
				break;

			fRange = m_fJumpAutoTargetRange;
			fMinAngle = m_fJumpAutoTargetMinAngle;
			fMaxAngle = m_fJumpAutoTargetMaxAngle;
			return;
		case ATT_COUNTER:
			if( m_fCountAutoTargetRange == 0.0f )
				break;

			fRange = m_fCountAutoTargetRange;
			fMinAngle = m_fCountAutoTargetMinAngle;
			fMaxAngle = m_fCountAutoTargetMaxAngle;
			return;
		}

		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;
	}
}

bool ioIljimaeItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioIljimaeItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_dwMaxCoolTime <= m_fCurCoolTime )
		return;

	float fRecoveryTime = 0.0f;
	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		fRecoveryTime = m_fRecoveryTime;
		break;
	default:
		fRecoveryTime = m_fRecoveryExTime;
		break;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = fRecoveryTime * fTimePerSec;

	m_fCurCoolTime += fGauge;
	m_fCurCoolTime = (float)min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
}

int ioIljimaeItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioIljimaeItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioIljimaeItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioIljimaeItem::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioIljimaeItem::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioIljimaeItem::DecreaseCoolTime( int iNeedBullet )
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

void ioIljimaeItem::ChangeToSearching( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_ChargeState = CS_SEARCHING;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fSearchMotionRate > 0.0f )
		fCurRate = m_fSearchMotionRate;
	
	int iAniID = pGrp->GetAnimationIdx( m_SearchMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	CheckTarget( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeItem::OnSearching( ioBaseChar *pOwner )
{
	if( pOwner->CheckDefaultNormalAttack( m_dwMotionStartTime,
										  m_dwMotionEndTime - m_dwMotionStartTime,
										  0 ) )
	{
		m_ChargeState = CS_SEARCHING_END;
	}
}

void ioIljimaeItem::ChangeToTeleport( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_ChargeState = CS_TELEPORT;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fTeleportMotionRate > 0.0f )
		fCurRate = m_fTeleportMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioIljimaeItem::OnTeleport( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	if( !pOwner->IsNeedProcess() )	return;

	if( m_dwMotionEndTime <= FRAMEGETTIME() )
	{
		ioHashString szCurTarget;
		szCurTarget.Clear();

		ioBaseChar *pTarget = pOwner;

		int iCnt = m_vTargetInfo.size();
		float fPreDistSq = m_fTeleportRange * m_fTeleportRange;
		for( int i=0; i<iCnt; ++i )
		{
			if( m_vTargetInfo[i].m_TargetName == m_PreTargetName )
				continue;

			pTarget = pOwner->GetBaseChar( m_vTargetInfo[i].m_TargetName );
			if( !pTarget )
				continue;
			
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
			float fDistSq = D3DXVec3LengthSq( &vDiff );
			if( fPreDistSq > fDistSq )
			{
				D3DXVECTOR3 vDir = vDiff;
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVec3Normalize( &vSearchDir, &vSearchDir );

				float fHalfCosine = cosf( DEGtoRAD( m_fTeleportAngle ) * FLOAT05 );

				if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
				{
					szCurTarget = m_vTargetInfo[i].m_TargetName;
					fPreDistSq = fDistSq;
				}
			}
		}

		m_PreTargetName = szCurTarget;
		pTarget = pOwner->GetBaseChar( szCurTarget );
		if( !pTarget )
		{
			pTarget = pOwner;
			m_PreTargetName.Clear();
		}

		D3DXVECTOR3 vTargetPos;
		if( szCurTarget.IsEmpty() )
		{
			if( m_vReturnPosition == ioMath::VEC3_ZERO )
			{
				vTargetPos = pOwner->GetWorldPosition();
				m_vReturnPosition = vTargetPos;
				m_vReturnPosition.y = pOwner->GetBottomHeight();
			}
			else
			{
				D3DXVECTOR3 vDiff = m_vReturnPosition - pOwner->GetWorldPosition();
				fPreDistSq = m_fTeleportRange * m_fTeleportRange;
				float fDistSq = D3DXVec3LengthSq( &vDiff );
				if( fPreDistSq > fDistSq )
				{
					vTargetPos = m_vReturnPosition;
					m_vReturnPosition = pOwner->GetWorldPosition();
					m_vReturnPosition.y = pOwner->GetBottomHeight();
				}
				else
				{
					vTargetPos = pOwner->GetWorldPosition();
					m_vReturnPosition = vTargetPos;
					m_vReturnPosition.y = pOwner->GetBottomHeight();
				}
			}
		}
		else
		{
			vTargetPos = pTarget->GetWorldPosition();
			m_vReturnPosition = pOwner->GetWorldPosition();
			m_vReturnPosition.y = pOwner->GetBottomHeight();
		}

		pOwner->SetRandomSeed( 0 );
		IORandom eRandom;
		DWORD dwSeed = pOwner->GetRandomSeed();

		D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
		Vector3Vec	vPosList;

		for( int i=0; i<8; ++i )
		{			
			eRandom.SetRandomSeed( dwSeed );
			int iRand = eRandom.Random( 8 );

			float fCurAngle = 45.0f * iRand;
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fCurAngle) );

			D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vTargetDir = qtRot * vTargetDir;
			vNewPos = vTargetPos - ( vTargetDir * m_fMoveRange );
			
			ioPlayStage *pStage = pOwner->GetCreator();
			if( !pStage )
				vNewPos = pOwner->GetWorldPosition();
			else
			{
				float fMapHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+3.0f );
				if( fMapHeight <= 0.0f )
					continue;
			}

			vPosList.push_back( vNewPos );
		}

		int iSize = vPosList.size();
		if( iSize == 0 )
		{
			D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vNewPos = pOwner->GetWorldPosition() - ( vTargetDir * m_fMoveRange );
		}
		else
		{
			m_dwReturnMarkEndTime = FRAMEGETTIME() + m_dwReturnMarkDuration;
			pOwner->SetReturnMark();
		}

		eRandom.SetRandomSeed( dwSeed );

		int iRandom = eRandom.Random( iSize );
		if( COMPARE( iRandom, 0, iSize ) )
		{
			vNewPos = vPosList[iRandom];
		}

		D3DXVECTOR3 vDir = vTargetPos - vNewPos;
		vDir.y = 0.0f;
		vNewPos.y += m_fMoveHeightRange;
		D3DXVec3Normalize( &vDir, &vDir );

		
		D3DXQUATERNION qtLookRot;
		ioMath::CalcDirectionQuaternion( qtLookRot, -vDir );
		pOwner->SetTargetRotAndMoveDirChange( qtLookRot );
		pOwner->SetWorldPosition( vNewPos );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << qtLookRot;
			kPacket << vNewPos;
			int iTargetCnt = m_vTargetInfo.size();
			kPacket << iTargetCnt;
			for( int i=0 ; i<iTargetCnt; ++i )
			{
				kPacket << m_vTargetInfo[i].m_TargetName;
				kPacket << m_vTargetInfo[i].m_dwEndTime;
				kPacket << m_vTargetInfo[i].m_dwMarkEffectID;
				kPacket << m_vTargetInfo[i].m_fLengthSq;
			}
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		// 텔레포트 종료 모션으로 전환
		m_ChargeState = CS_TELEPORT_END;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fCurRate = FLOAT1;
		if( m_fTeleportEndMotionRate > 0.0f )
			fCurRate = m_fTeleportEndMotionRate;

		int iAniID = pGrp->GetAnimationIdx( m_TeleportEndMotion );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

		pOwner->SetExtendAttackEndJumpInfo( iAniID, fCurRate, 0 );
	}
}

void ioIljimaeItem::OnTeleportEnd( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( m_dwMotionEndTime <= FRAMEGETTIME() )
	{
		if( pOwner->CheckExtendAttackEndJump() )
			m_ChargeState = CS_END;
	}
}

void ioIljimaeItem::CheckTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	m_PreTargetName.Clear();
	std::vector<TargetInfo>	vTargetInfoList;
	vTargetInfoList.clear();

	int iTargetCount = 0;

	float fPrevDistSq = m_fSearchRange * m_fSearchRange;
	ioBaseChar *pTargetChar = pOwner;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if( !IsEnableTarget( pTarget ) )	continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )
		{
			D3DXVECTOR3 vDir = vDiff;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSearchDir, &vSearchDir );

			float fHalfCosine = cosf( DEGtoRAD( m_fSearchAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
			{
				TargetInfo kInfo;
				kInfo.Init();
				kInfo.m_TargetName = pTarget->GetCharName();
				kInfo.m_dwEndTime = FRAMEGETTIME() + m_dwSearchDuration;
				kInfo.m_fLengthSq = fDistSq;
				vTargetInfoList.push_back( kInfo );

				iTargetCount++;
			}
		}
	}
	if( !vTargetInfoList.empty() )
		std::sort( vTargetInfoList.begin(), vTargetInfoList.end(), TargetDistSort() );

	int iSize = vTargetInfoList.size();
	if( iSize >= m_iSearchCount )
	{
		ClearAllTarget( pOwner );
		m_vTargetInfo.reserve( m_iSearchCount );
		for( int i=0; i<m_iSearchCount; ++i )
		{
			_AttachTargetEffect( pOwner, vTargetInfoList[i] );
			m_vTargetInfo.push_back( vTargetInfoList[i] );			
		}
	}
	else
	{
		// 1. 중복된 대상은 미리 삭제한다.
		std::vector<TargetInfo>::iterator new_it;
		for( new_it = vTargetInfoList.begin(); new_it != vTargetInfoList.end(); ++new_it )
		{
			std::vector<TargetInfo>::iterator it;
			it = m_vTargetInfo.begin();
			while( it != m_vTargetInfo.end() )
			{
				if( (*new_it).m_TargetName == (*it).m_TargetName )
				{
					_DestroyTargetEffect( pStage, *it );
					it = m_vTargetInfo.erase( it );
				}
				else
				{
					++it;
				}
			}
		}

		// 2. 추가로 들어올 대상을 위한 자리를 확보한다.
		int iOverCount = m_vTargetInfo.size() + vTargetInfoList.size();
		if( iOverCount > m_iSearchCount )
		{
			int iBlank = iOverCount - m_iSearchCount;
			std::vector<TargetInfo>::iterator it;
			it = m_vTargetInfo.begin();
			while( iBlank > 0 )
			{
				_DestroyTargetEffect( pStage, *it );
				it = m_vTargetInfo.erase( it );
				iBlank--;
			}
		}

		// 3. 추가로 들어올 대상을 m_vTargetInfo 리스트에 넣는다.
		int iCnt = vTargetInfoList.size();
		for( int i=0; i<iCnt; ++i )
		{
			_AttachTargetEffect( pOwner, vTargetInfoList[i] );
			m_vTargetInfo.push_back( vTargetInfoList[i] );
		}
	}
}

bool ioIljimaeItem::IsEnableTarget( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	// 쓰러짐 관련 체크
	if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
	{
		if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
	{
		if( pChar->IsApplyDownState(false) )
			return false;
	}
	else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
	{
		if( pChar->IsApplyDownState() ||
			( pChar->GetState() == CS_GRAPPLING_WOUNDED ||
			pChar->GetState() == CS_GRAPPLING_PUSHED ||
			pChar->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
			pChar->GetState() == CS_GRAPPLING_WOUNDED_SWING ) )
		{
			return false;
		}
	}
	else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
	{
		if( !pChar->IsFloating() )
			return false;
	}
	else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
	{
		if( !pChar->IsApplyDownState() || pChar->IsFloating() )
		{			
			return false;
		}
	}

	return true;
}

bool ioIljimaeItem::IsNoFallState()
{
	if( m_ChargeState == CS_TELEPORT_END )
		return true;

	return false;
}

const ioHashString& ioIljimaeItem::GetReturnMarkEffet() const
{
	return m_ReturnMark;
}

DWORD ioIljimaeItem::GetReturnMarkDuration()
{
	return m_dwReturnMarkDuration;
}

bool ioIljimaeItem::IsShowMarkEffect()
{
	if( m_dwReturnMarkEndTime > 0 && m_dwReturnMarkEndTime > FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

void ioIljimaeItem::SetDisableReturnMarkEffect()
{
	m_dwReturnMarkEndTime = 0;
	m_vReturnPosition = ioMath::VEC3_ZERO;
}

void ioIljimaeItem::ClearAllTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	int iCnt = m_vTargetInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vTargetInfo[i].m_dwMarkEffectID != -1 )
		{
			_DestroyTargetEffect( pStage, m_vTargetInfo[i] );
		}
	}
	m_vTargetInfo.clear();
}

void ioIljimaeItem::CheckTargetDuration( ioPlayStage *pStage )
{
	if( !pStage )					return;
	if( m_vTargetInfo.empty() )		return;

	std::vector<TargetInfo>::iterator iter;
	iter = m_vTargetInfo.begin();
	while( iter != m_vTargetInfo.end() )
	{
		if( (*iter).m_dwEndTime > 0 && (*iter).m_dwEndTime <= FRAMEGETTIME() )
		{
			_DestroyTargetEffect( pStage, *iter );
			iter = m_vTargetInfo.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioIljimaeItem::CheckTargetRange( ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( m_vTargetInfo.empty() )	return;
	
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( !pOwner )	return;

	std::vector<TargetInfo>::iterator iter;
	iter = m_vTargetInfo.begin();
	while( iter != m_vTargetInfo.end() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).m_TargetName );
		if( !pTarget )
		{
			_DestroyTargetEffect( pStage, *iter );
			iter = m_vTargetInfo.erase( iter );
			continue;
		}

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDist = D3DXVec3Length( &vDiff );
		if( fDist >= m_fDisableRange )
		{
			_DestroyTargetEffect( pStage, *iter );
			iter = m_vTargetInfo.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioIljimaeItem::_AttachTargetEffect( ioBaseChar *pOwner, TargetInfo& rkTargetInfo )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( rkTargetInfo.m_TargetName );
	if( !pTarget )
		return;

	ioEffect *pEffect = pTarget->AttachEffect( m_TeleportMark );
	if( pEffect )
	{
		rkTargetInfo.m_dwMarkEffectID = pEffect->GetUniqueID();
	}
}

void ioIljimaeItem::_DestroyTargetEffect( ioPlayStage *pStage, TargetInfo& rkTargetInfo )
{
	if( !pStage )	return;
	ioBaseChar *pTarget = pStage->GetBaseChar( rkTargetInfo.m_TargetName );
	if( !pTarget )	return;

	ioEffect *pEffect = pTarget->GetEffectByObjID( rkTargetInfo.m_dwMarkEffectID );
	if( pEffect )
	{
		pEffect->EndEffectForce();
		rkTargetInfo.m_dwMarkEffectID = -1;
	}
}

void ioIljimaeItem::SetReturnMarkEndTime( DWORD dwEndTime )
{
	m_dwReturnMarkEndTime = dwEndTime;
}

void ioIljimaeItem::GetCurTargetList( ioHashStringVec &vTargetNameList )
{
	int iCnt = m_vTargetInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		vTargetNameList.push_back( m_vTargetInfo[i].m_TargetName );
	}
}