

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMagicStaffItem3.h"



#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMagicStaffItem3::ioMagicStaffItem3()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_dwGatherAttEndTime = 0;
	m_GatherOffSetEffID = -1;
	m_GatherAttPos = ioMath::VEC3_ZERO;
	m_dwCurGatherAttCnt = 1;

	m_dwNextGatherTime = 0;
	m_dwMagicElementAttEndTime = 0;
}

ioMagicStaffItem3::ioMagicStaffItem3( const ioMagicStaffItem3 &rhs )
: ioWeaponItem( rhs ),
 m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_GatherEffect( rhs.m_GatherEffect ),
 m_fMoveSpeed( rhs.m_fMoveSpeed ),
 m_fRange( rhs.m_fRange ),
 m_fHeightGap( rhs.m_fHeightGap ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_EnableCircle( rhs.m_EnableCircle ),
 m_fMagicCircleRadius( rhs.m_fMagicCircleRadius ),
 m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_TargetCircle( rhs.m_TargetCircle ),
 m_MagicElementAtt( rhs.m_MagicElementAtt ),
 m_GatherAttAni( rhs.m_GatherAttAni ),
 m_fGatherAttAniRate( rhs.m_fGatherAttAniRate ),
 m_GatherAttOffset( rhs.m_GatherAttOffset ),
 m_dwMaxGatherAttCnt( rhs.m_dwMaxGatherAttCnt ),
 m_GatherEffInfo( rhs.m_GatherEffInfo )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_dwEnableChrageCircleID	= -1;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();

	m_dwGatherAttEndTime = 0;
	m_GatherOffSetEffID = -1;
	m_GatherAttPos = ioMath::VEC3_ZERO;
	m_dwCurGatherAttCnt = 1;

	m_dwNextGatherTime = 0;
	m_dwMagicElementAttEndTime = 0;
}

ioMagicStaffItem3::~ioMagicStaffItem3()
{
	m_vAirJumpTimeList.clear();
}

void ioMagicStaffItem3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;
	rkLoader.LoadString_e( "target_circle", "", szBuf, MAX_PATH );
	m_TargetCircle = szBuf;

	m_fMagicCircleRadius = rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT100 );
	m_fRange = rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );
	
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	// gather attack
	rkLoader.LoadString_e( "gather_att_ani", "", szBuf, MAX_PATH );
	m_GatherAttAni = szBuf;
	m_fGatherAttAniRate = rkLoader.LoadFloat_e( "gather_att_ani_rate", FLOAT1 );

	m_GatherAttOffset.x = rkLoader.LoadFloat_e( "gather_att_offset_x", 0.0f );
	m_GatherAttOffset.y = rkLoader.LoadFloat_e( "gather_att_offset_y", 0.0f );
	m_GatherAttOffset.z = rkLoader.LoadFloat_e( "gather_att_offset_z", 0.0f );

	rkLoader.LoadString_e( "gather_att_area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	m_dwMaxGatherAttCnt = (DWORD)rkLoader.LoadFloat_e( "gather_att_max_cnt", 1 );

	m_GatherEffInfo.clear();
	m_GatherEffInfo.reserve( m_dwMaxGatherAttCnt );

	for( int i=0 ; i < (int)m_dwMaxGatherAttCnt; ++i )
	{
		SummonAttackInfo kInfo;

		wsprintf_e( szKey, "gather_time%d", i+1 );
		kInfo.m_dwCreateTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "gather_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_SummonEffect = szBuf;

		wsprintf_e( szKey, "gather_sound%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_SummonSound = szBuf;

		m_GatherEffInfo.push_back( kInfo );
	}

	// elelment attack
	m_MagicElementAtt.Init();

	LoadAttackAttribute_e( "fire_element_attack", m_MagicElementAtt.m_FireAttack, rkLoader );
	LoadAttackAttribute_e( "ice_element_attack", m_MagicElementAtt.m_IceAttack, rkLoader );
	LoadAttackAttribute_e( "light_element_attack", m_MagicElementAtt.m_LightAttack, rkLoader );
}

ioItem* ioMagicStaffItem3::Clone()
{
	return new ioMagicStaffItem3( *this );
}

void ioMagicStaffItem3::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_FireState = FS_NONE;
	m_dwMagicElementAttEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
	else
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioMagicStaffItem3::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	pOwner->AttachEffect( m_GatherEffect );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	//
	m_FireState = FS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	m_dwCurGatherAttCnt = 1;
	m_dwGatherAttEndTime = 0;
	m_GatherAttPos = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vOffSet = pOwner->GetTargetRot() * m_GatherAttOffset;
	m_GatherAttPos = pOwner->GetWorldPosition() + vOffSet;

	// eff & snd
	CheckNextGatherInfo();

	// Effect
	if( m_GatherOffSetEffID != -1 )
	{
		pOwner->EndMapEffect( m_GatherOffSetEffID, false );
		m_GatherOffSetEffID = -1;
	}

	ioHashString szCurEffect = GetCurGatherEffect();
	if( !szCurEffect.IsEmpty() )
	{
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pStage )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

			ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurEffect, m_GatherAttPos, vScale );
			if( pMapEffect )
			{
				m_GatherOffSetEffID = pMapEffect->GetUniqueID();
				pMapEffect->SetWorldOrientation( pOwner->GetTargetRot() );
			}
		}
	}

	// Sound
	ioHashString szCurSound = GetCurGatherSound();
	if( !szCurSound.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
		}
	}


	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem3::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		// check element attack
		if( pOwner && pOwner->IsNeedProcess() )
		{
			bool bCheckCombo = false;
			if( m_iCurCombo == GetMaxCombo()-1 )
				bCheckCombo = true;

			if( bCheckCombo && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			{
				if( pOwner->IsAttackKey() )
				{
					ChangeToElementAttack( pOwner, MET_FIRE );
					return;
				}
				else if( pOwner->IsDefenseKey() )
				{
					ChangeToElementAttack( pOwner, MET_ICE );
					return;
				}
				else if( pOwner->IsJumpKey() )
				{
					ChangeToElementAttack( pOwner, MET_LIGHT );
					return;
				}
			}
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case FS_GATHERING:
		OnGathering( pOwner );
		break;
	case FS_GATHERING_ATTACK:
		OnGatheringAttack( pOwner );
		break;
	case FS_ELEMENT_ATTACK:
		OnElementAttack( pOwner );
		break;
	}
}

void ioMagicStaffItem3::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioMagicStaffItem3::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )	// 버튼 뗌
	{
		ChangeToGatheringAttack( pOwner );
		return;
	}

	//Gather Gauge
	float fCurRange = m_fRange;

	if( pOwner->CheckMagicCircle() )
	{
		// 뱡향 변화 체크
		pOwner->CheckChangeDirectionForMagicCircle();

		// MagicCircle 위치 갱신
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
			pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, fCurRange, m_fHeightGap, true );
		else
			pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, fCurRange, m_fHeightGap, false );

		// MagicCircle 위치방향으로 캐릭터 회전
		D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage && m_GatherOffSetEffID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_GatherOffSetEffID );
			if( pMapEffect )
				pMapEffect->SetWorldOrientation( pOwner->GetTargetRot() );
		}
	}

	if( CheckNextGather( pOwner ) && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_dwCurGatherAttCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem3::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	//
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		ClearKeyReserve();
		m_dwKeyReserveTime = 0;

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		float fTimeRate = pAttribute->m_fAttackAniRate;
		DWORD dwPreDelay = pAttribute->m_dwPreDelay;

		float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserve > 0.0f )
		{
			m_dwKeyReserveTime = FRAMEGETTIME() + fReserve + dwPreDelay;
		}
	}
	//

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem3::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_FireState = FS_NORMAL_ATTACK;
		break;
	case FS_GATHERING:
		pOwner->EndEffect( m_GatherEffect );
		ChangeToGathering( pOwner );
		break;
	case FS_GATHERING_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_dwCurGatherAttCnt;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_GatherAttPos;
			rkPacket >> qtRot;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( !pGrp ) return;

			m_fCurChargeRate = 0.0f;
			m_fCurChargeRateForSkill = 0.0f;

			pOwner->EndEffect( m_GatherEffect );

			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

			// ani
			int iAniID = pGrp->GetAnimationIdx( m_GatherAttAni );
			float fTimeRate = m_fGatherAttAniRate;
			if( fTimeRate <= 0.0f )
				fTimeRate = FLOAT1;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

			m_dwGatherAttEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			m_FireState = FS_GATHERING_ATTACK;

			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			if( pStage )
			{
				D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		
				if( m_GatherOffSetEffID != -1 )
				{
					pOwner->EndMapEffect( m_GatherOffSetEffID );
					m_GatherOffSetEffID = -1;
				}
			}

			pOwner->DestroyMagicCircle();

			// create areaweapon
			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "",
																	m_AreaWeaponName,
																	m_GatherAttPos,
																	qtRot,
																	ioAreaWeapon::CT_NORMAL );
	
			if( pArea )
			{
				pArea->SetTargetAttInfo( m_vCirclePos, m_dwCurGatherAttCnt );
			}
		}
		break;
	case FS_NONE:
		m_FireState = FS_NONE;

		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );
		break;
	case FS_ELEMENT_ATTACK:
		{
			int iSubType;
			rkPacket >> iSubType;

			int iNewKey;
			rkPacket >> iNewKey;

			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iNewKey );
			ChangeToElementAttack( pOwner, iSubType );
		}
		break;
	}
}

void ioMagicStaffItem3::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iFireState;
	rkPacket >> iFireState;

	switch( iFireState )
	{
	case FS_GATHERING:
		{
			rkPacket >> m_dwCurGatherAttCnt;

			// Effect
			if( m_GatherOffSetEffID != -1 )
			{
				pOwner->EndMapEffect( m_GatherOffSetEffID, false );
				m_GatherOffSetEffID = -1;
			}

			ioHashString szCurEffect = GetCurGatherEffect();
			if( !szCurEffect.IsEmpty() )
			{
				ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
				if( pStage )
				{
					D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

					ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurEffect, m_GatherAttPos, vScale );
					if( pMapEffect )
					{
						m_GatherOffSetEffID = pMapEffect->GetUniqueID();
					}
				}
			}

			// Sound
			ioHashString szCurSound = GetCurGatherSound();
			if( !szCurSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
				}
			}
		}
		break;
	}
}

ioWeaponItem::WeaponSubType ioMagicStaffItem3::GetSubType() const
{
	return WST_MAGIC_STAFF3;
}

void ioMagicStaffItem3::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );

	if( m_GatherOffSetEffID != -1 )
	{
		pOwner->EndMapEffect( m_GatherOffSetEffID );
		m_GatherOffSetEffID = -1;
	}

	if( m_dwEnableChrageCircleID != -1 )
	{
		pOwner->EndMapEffect( m_dwEnableChrageCircleID );
		m_dwEnableChrageCircleID = -1;
	}
}

float ioMagicStaffItem3::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

bool ioMagicStaffItem3::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
			return true;
	}

	return false;
}

void ioMagicStaffItem3::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
		IsCanUpdateGauge(pOwner) )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
			 pOwner->GetState() == CS_VIEW ||
			 pOwner->GetState() == CS_LOADING ||
			 pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	float fNewRecovery = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				m_fCurSkillGauge = min( m_fCurSkillGauge+fNewRecovery, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

//#ifndef SHIPPING 
//	if( Setting::ShowExtraInfo() )
//	{
//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
//
//		if( fMaxGauge < FLOAT100 )
//		{
//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//
//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//		}
//	}
//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioMagicStaffItem3::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

void ioMagicStaffItem3::ChangeToGatheringAttack( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pChar->EndEffect( m_GatherEffect );

	DWORD dwCurTime = FRAMEGETTIME();

	// ani
	int iAniID = pGrp->GetAnimationIdx( m_GatherAttAni );
	float fTimeRate = m_fGatherAttAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_dwGatherAttEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_FireState = FS_GATHERING_ATTACK;

	if( pChar->GetUseEnableSkill( 0, SUT_NORMAL ) )
		pChar->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, true, true );
	else
		pChar->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, false, true );

	m_vCirclePos = pChar->GetMagicCirclePos( m_fMagicCircleOffSet, true );

	// eff
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		
		if( m_GatherOffSetEffID != -1 )
		{
			pChar->EndMapEffect( m_GatherOffSetEffID );
			m_GatherOffSetEffID = -1;
		}
	}

	if( pStage && !m_TargetCircle.IsEmpty() )
	{
		if( m_dwEnableChrageCircleID != -1 )
		{
			pChar->EndMapEffect( m_dwEnableChrageCircleID );
			m_dwEnableChrageCircleID = -1;
		}

		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_TargetCircle, m_vCirclePos, vScale );
		if( pMapEffect )
		{
			m_dwEnableChrageCircleID = pMapEffect->GetUniqueID();
		}
	}

	pChar->DestroyMagicCircle();

	// create areaweapon
	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(), "", "",
															m_AreaWeaponName,
															m_GatherAttPos,
															qtRot,
															ioAreaWeapon::CT_NORMAL );
	
	if( pArea )
	{
		pArea->SetTargetAttInfo( m_vCirclePos, m_dwCurGatherAttCnt );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;

		kPacket << m_dwCurGatherAttCnt;
		kPacket << m_vCirclePos;
		kPacket << m_GatherAttPos;
		kPacket << qtRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem3::ChangeToElementAttack( ioBaseChar *pOwner, int iSubType )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iAniID = -1;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;

	switch( iSubType )
	{
	case MET_FIRE:
		pOwner->SetNormalAttackByAttribute( m_MagicElementAtt.m_FireAttack );
		iAniID = pGrp->GetAnimationIdx( m_MagicElementAtt.m_FireAttack.m_AttackAnimation );
		fTimeRate = m_MagicElementAtt.m_FireAttack.m_fAttackAniRate;
		dwPreDelay = m_MagicElementAtt.m_FireAttack.m_dwPreDelay;
		break;
	case MET_ICE:
		pOwner->SetNormalAttackByAttribute( m_MagicElementAtt.m_IceAttack );
		iAniID = pGrp->GetAnimationIdx( m_MagicElementAtt.m_IceAttack.m_AttackAnimation );
		fTimeRate = m_MagicElementAtt.m_IceAttack.m_fAttackAniRate;
		dwPreDelay = m_MagicElementAtt.m_IceAttack.m_dwPreDelay;
		break;
	case MET_LIGHT:
		pOwner->SetNormalAttackByAttribute( m_MagicElementAtt.m_LightAttack );
		iAniID = pGrp->GetAnimationIdx( m_MagicElementAtt.m_LightAttack.m_AttackAnimation );
		fTimeRate = m_MagicElementAtt.m_LightAttack.m_fAttackAniRate;
		dwPreDelay = m_MagicElementAtt.m_LightAttack.m_dwPreDelay;
		break;
	}

	m_FireState = FS_ELEMENT_ATTACK;
	
	ClearKeyReserve();
	m_dwKeyReserveTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMagicElementAttEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
	{
		m_dwKeyReserveTime = dwCurTime + fReserve + dwPreDelay;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << iSubType;
		kPacket << eNewDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem3::OnGatheringAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwGatherAttEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver( false );
		SetNoneState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioMagicStaffItem3::OnElementAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMagicElementAttEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioMagicStaffItem3::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_FireState = FS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

void ioMagicStaffItem3::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

void ioMagicStaffItem3::SetCancelFire( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	SetNoneState( pOwner );

	pOwner->DestroyMagicCircle();

	pOwner->SetState( CS_DELAY );
}

bool ioMagicStaffItem3::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	AttackType eExtendType = AT_JUMP_ATTACK;

	if( m_pExtendJump )
	{
		m_pExtendJump->SetJumpAttack( pOwner, iAniID, fTimeRate );
	}

	m_vAirJumpTimeList.erase( m_vAirJumpTimeList.begin(), m_vAirJumpTimeList.end() );
	m_iCurAirJump = 0;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	fTimeRate   = pAttr->m_fAttackAniRate;

	SetAirJump( pOwner, iAniID, fTimeRate );
	return false;
}

void ioMagicStaffItem3::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioMagicStaffItem3::CheckNextGatherInfo()
{
	if( m_GatherEffInfo.empty() )
		return false;

	if( m_dwCurGatherAttCnt >= m_dwMaxGatherAttCnt )
		return false;

	int iIndex = m_dwCurGatherAttCnt - 1;
	if( COMPARE( iIndex, 0, (int)m_GatherEffInfo.size() ) )
	{
		m_dwNextGatherTime = m_dwGatheringStartTime + m_GatherEffInfo[iIndex].m_dwCreateTime;
		return true;
	}

	return false;
}

bool ioMagicStaffItem3::CheckNextGather( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_dwNextGatherTime <= 0 ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwNextGatherTime < dwCurTime )
	{
		m_dwCurGatherAttCnt++;

		// Effect
		if( m_GatherOffSetEffID != -1 )
		{
			pOwner->EndMapEffect( m_GatherOffSetEffID, false );
			m_GatherOffSetEffID = -1;
		}

		ioHashString szCurEffect = GetCurGatherEffect();
		if( !szCurEffect.IsEmpty() )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			if( pStage )
			{
				D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

				ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurEffect, m_GatherAttPos, vScale );
				if( pMapEffect )
				{
					m_GatherOffSetEffID = pMapEffect->GetUniqueID();
				}
			}
		}

		// Sound
		ioHashString szCurSound = GetCurGatherSound();
		if( !szCurSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
			}
		}

		if( !CheckNextGatherInfo() )
		{
			m_dwNextGatherTime = 0;
		}

		return true;
	}

	return false;
}

ioHashString ioMagicStaffItem3::GetCurGatherEffect()
{
	ioHashString szName;

	if( m_GatherEffInfo.empty() )
		return szName;

	int iIndex = m_dwCurGatherAttCnt - 1;
	if( COMPARE( iIndex, 0, (int)m_GatherEffInfo.size() ) )
		szName = m_GatherEffInfo[iIndex].m_SummonEffect;

	return szName;
}

ioHashString ioMagicStaffItem3::GetCurGatherSound()
{
	ioHashString szName;

	if( m_GatherEffInfo.empty() )
		return szName;

	int iIndex = m_dwCurGatherAttCnt - 1;
	if( COMPARE( iIndex, 0, (int)m_GatherEffInfo.size() ) )
		szName = m_GatherEffInfo[iIndex].m_SummonSound;

	return szName;
}

