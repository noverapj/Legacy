
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCrossBowItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioCrossBowItem::ioCrossBowItem()
{
	m_FireState = FS_NONE;

	m_iCurCombo = 0;

	m_dwChargeStartTime = 0;
	m_dwGatheringStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwSearchCheckTime = 0;
	m_dwNextBulletAddTime = 0;
	
	m_dwBaseWeaponIndex = 0;
	m_GatheringEffectID = -1;
	m_GuidEffectID = -1;

	m_bSetGatherMoveAni = false;
	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_CurTargetName.Clear();
	m_TargetBulletInfoList.clear();

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioCrossBowItem::ioCrossBowItem( const ioCrossBowItem &rhs )
: ioWeaponItem( rhs ),
 m_FireAttribute( rhs.m_FireAttribute ),
 m_GatheringInfoList( rhs.m_GatheringInfoList ),
 m_AttackReadyAni( rhs.m_AttackReadyAni ),
 m_dwRotateSpeed( rhs.m_dwRotateSpeed ),
 m_fMarkerRange( rhs.m_fMarkerRange ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_GatherMoveAnimation( rhs.m_GatherMoveAnimation ),
 m_GatherJumpAnimation( rhs.m_GatherJumpAnimation ),
 m_fGatherJumpAniRate( rhs.m_fGatherJumpAniRate ),
 m_fCheckBoxX( rhs.m_fCheckBoxX ),
 m_fCheckBoxY( rhs.m_fCheckBoxY ),
 m_fCheckBoxZ( rhs.m_fCheckBoxZ ),
 m_fTargetRange( rhs.m_fTargetRange ),
 m_fTargetMinRange( rhs.m_fTargetMinRange ),
 m_fTargetAngle( rhs.m_fTargetAngle ),
 m_iMaxBullet( rhs.m_iMaxBullet ),
 m_dwTargetEndDuration( rhs.m_dwTargetEndDuration ),
 m_dwTargetSearchTime( rhs.m_dwTargetSearchTime ),
 m_TargetEffect( rhs.m_TargetEffect ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_GuidEffect( rhs.m_GuidEffect )
{
	m_FireState = FS_NONE;

	m_iCurCombo = 0;

	m_dwChargeStartTime = 0;
	m_dwGatheringStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwSearchCheckTime = 0;
	m_dwNextBulletAddTime = 0;

	m_dwBaseWeaponIndex = 0;
	m_GatheringEffectID = -1;
	m_GuidEffectID = -1;

	m_bSetGatherMoveAni = false;
	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_CurTargetName.Clear();
	m_TargetBulletInfoList.clear();

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioCrossBowItem::~ioCrossBowItem()
{
	ClearTargetBulletInfo();
}

void ioCrossBowItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	// gathering
	rkLoader.LoadString_e( "guid_effect", "", szBuf, MAX_PATH );
	m_GuidEffect = szBuf;

	// attack
	LoadAttackAttribute_e( "target_attack", m_FireAttribute, rkLoader );

	//
	rkLoader.LoadString_e( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	rkLoader.LoadString_e( "gathering_ani", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gathering_move_ani", "", szBuf, MAX_PATH );
	m_GatherMoveAnimation = szBuf;

	rkLoader.LoadString_e( "gathering_jump_ani", "", szBuf, MAX_PATH );
	m_GatherJumpAnimation = szBuf;

	m_fGatherJumpAniRate = rkLoader.LoadFloat_e( "gathering_jump_ani_rate", FLOAT1 );

	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "gathering_rotate_speed", 0 );

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_GatheringInfoList.clear();
	m_GatheringInfoList.reserve( m_iMaxBullet );

	for( int i=0; i < m_iMaxBullet; ++i )
	{
		GatheringInfo kInfo;

		wsprintf_e( szKey, "gathering_effect%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_GatheringEffect = szBuf;

		wsprintf_e( szKey, "gathering_sound%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_GatheringSound = szBuf;

		wsprintf_e( szKey, "bullet_add_time%d", i+1 );
		kInfo.m_dwCreateTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_GatheringInfoList.push_back( kInfo );
	}

	// marker
	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	// target
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_TargetEffect = szBuf;

	m_fCheckBoxX = rkLoader.LoadFloat_e( "check_box_x", 0.0f );
	m_fCheckBoxY = rkLoader.LoadFloat_e( "check_box_y", 0.0f );
	m_fCheckBoxZ = rkLoader.LoadFloat_e( "check_box_z", 0.0f );

	m_dwTargetSearchTime = (DWORD)rkLoader.LoadInt_e( "target_search_time", 0 );
	m_dwTargetEndDuration = (DWORD)rkLoader.LoadInt_e( "target_end_duration", 0 );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_fTargetMinRange = min( m_fTargetMinRange, m_fTargetRange );
}

ioItem* ioCrossBowItem::Clone()
{
	return new ioCrossBowItem( *this );
}

ioWeaponItem::WeaponSubType ioCrossBowItem::GetSubType() const
{
	return WST_CROSS_BOW;
}

void ioCrossBowItem::ClearTargetBulletInfo()
{
	m_CurTargetName.Clear();

	if( m_TargetBulletInfoList.empty() )
		return;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage )
	{
		for( int i=0; i < (int)m_TargetBulletInfoList.size(); ++i )
		{
			ioHashString szTargetName = m_TargetBulletInfoList[i].m_szTargetName;
			if( !szTargetName.IsEmpty() && !m_TargetEffect.IsEmpty() )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
				if( pTarget )
				{
					pTarget->EndEffect( m_TargetEffect, false );
				}
			}
		}
	}

	m_TargetBulletInfoList.clear();
}

void ioCrossBowItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( !pOwner ) return;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwSearchCheckTime = 0;
	m_dwNextBulletAddTime = 0;
	
	m_dwBaseWeaponIndex = pOwner->GetWeaponIndexBase();

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( m_GatheringInfoList.empty() )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_FireState = FS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetChargingState( false );
		m_bSetGatherMoveAni = false;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioCrossBowItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_GATHERING:
		OnGathering( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case FS_FIRE:
		OnFire( pOwner );
		break;
	}
}

bool ioCrossBowItem::SetAimState( ioBaseChar *pChar )
{
	if( pChar->GetState() == CS_JUMP && CheckAimState() )
	{
		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
		pChar->SetState( CS_ATTACK );

		m_FireState = FS_GATHERING;
	}
	else
	{
		ChangeToGathering( pChar );

		if( !pChar->IsJumpState() )
		{
			pChar->SetState( CS_ATTACK );
		}
	}

	pChar->SetChargingState( true );

	return true;
}

bool ioCrossBowItem::CheckAimState(bool bAimMove)
{
	switch( m_FireState )
	{
	case FS_GATHERING:
	case FS_JGATHERING:
		return true;
	}

	return false;
}

void ioCrossBowItem::CheckAimJumpState( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_JGATHERING:
		OnJGathering( pOwner );
		break;
	}
}

void ioCrossBowItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToGathering( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_iCurCombo;
				kPacket << (int)m_FireState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << m_iCurCombo;
			kPacket << (int)m_FireState;
			if( !m_bChargeAutoTarget )
				pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

void ioCrossBowItem::OnGathering( ioBaseChar *pOwner )
{

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		// 공격버튼땜, 발사.
		if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToAttackFire( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_iCurCombo;
				kPacket << (int)m_FireState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}

		// 점프키 누름. 점프상태 전환
		if( pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetCrossBowJumpState();
			return;
		}

		// SD 스킬 입력.
		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				if( m_GuidEffectID != -1 )
				{
					pOwner->EndEffect( m_GuidEffectID, false );
					m_GuidEffectID = -1;
				}

				SetNoneState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << m_iCurCombo;
					kPacket << (int)m_FireState;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
	}

	CheckMoveKey( pOwner );
	CheckMoveDir( pOwner );
	CheckMoveAni( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		ioHashStringVec vReleaseList, vFindList;
		bool bRefresh = CheckTargetState( pOwner, vReleaseList, vFindList );

		ioHashString szAddTarget;
		bool bAdd = CheckNextAddBullet( pOwner, szAddTarget );

		if( pOwner->IsNeedSendNetwork() && (bRefresh || bAdd) )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_CHARGE;
			kPacket << m_iCurCombo;

			kPacket << bRefresh;
			if( bRefresh )
			{
				kPacket << (int)vReleaseList.size();
				for( int i=0; i < (int)vReleaseList.size(); ++i )
					kPacket << vReleaseList[i];

				kPacket << (int)vFindList.size();
				for( int j=0; j < (int)vFindList.size(); ++j )
					kPacket << vFindList[j];
			}

			kPacket << bAdd;
			if( bAdd )
			{
				kPacket << szAddTarget;
			}

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCrossBowItem::OnFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver( false );

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			SetNoneState( pOwner );

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
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() )
		{
			int iCurInput = m_iSkillInput;
			if( pOwner->SetUseSkill( iCurInput, SUT_NORMAL ) )
			{
				SetNoneState( pOwner );
				return;
			}
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			SetNoneState( pOwner );
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			SetNoneState( pOwner );
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			SetNoneState( pOwner );
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

		SetNoneState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}

	if( m_FireAttribute.m_vWeaponInfoList.empty() )
		return;

	if( m_dwFireStartTime == 0 || m_dwFireStartTime >= dwCurTime )
		return;

	int iCurBulletCnt = m_TargetBulletInfoList.size();
	int iCurTargetCnt = GetCurTargetCnt();

	int iMaxFireCnt = iCurTargetCnt;
	if( iMaxFireCnt == 0 && iCurBulletCnt > 0 )
		iMaxFireCnt = 1;

	ioHashStringVec vTargetList;
	GetTargetList( vTargetList );

	const WeaponInfo &rkInfo = m_FireAttribute.m_vWeaponInfoList.front();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	int iCurFireCnt = 0;
	DWORD dwWeaponIndex = m_dwBaseWeaponIndex;

	// target fire
	int iSize = vTargetList.size();
	for( int i=0; i < iSize; ++i )
	{
		m_CurTargetName = vTargetList[i];

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime,
									vPos,
									vDir,
									GetName() );

		iCurFireCnt++;
		dwWeaponIndex++;
	}

	// none target fire
	if( iMaxFireCnt > 0 && iCurFireCnt < iMaxFireCnt )
	{
		m_CurTargetName.Clear();

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime,
									vPos,
									vDir,
									GetName() );
	}

	m_dwFireStartTime = 0;
	ClearTargetBulletInfo();
}

void ioCrossBowItem::OnJGathering( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckMoveKey( pOwner );
	CheckMoveDir( pOwner );

	if( !pOwner->IsNeedProcess() )	return;

	ioHashStringVec vReleaseList, vFindList;
	bool bRefresh = CheckTargetState( pOwner, vReleaseList, vFindList );

	ioHashString szAddTarget;
	bool bAdd = CheckNextAddBullet( pOwner, szAddTarget );
	
	if( pOwner->IsNeedSendNetwork() && (bRefresh || bAdd) )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE;
		kPacket << m_iCurCombo;

		kPacket << bRefresh;
		if( bRefresh )
		{
			kPacket << (int)vReleaseList.size();
			for( int i=0; i < (int)vReleaseList.size(); ++i )
				kPacket << vReleaseList[i];

			kPacket << (int)vFindList.size();
			for( int j=0; j < (int)vFindList.size(); ++j )
				kPacket << vFindList[j];
		}

		kPacket << bAdd;
		if( bAdd )
		{
			kPacket << szAddTarget;
		}

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCrossBowItem::ChangeToGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetTargetMarkerTarget( "", false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwGatheringStartTime = dwCurTime;
	m_FireState = FS_GATHERING;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );

	ClearTargetBulletInfo();
	m_dwNextBulletAddTime = 0;

	// 1발 기본 충전
	m_iCurCombo = 1;
	TargetBulletInfo kInfo;
	m_TargetBulletInfoList.push_back( kInfo );

	// NextTime
	CheckAddBulletTime();

	// Effect
	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	ioHashString szCurEffect = GetCurGatheringEffect();
	if( !szCurEffect.IsEmpty() )
	{
		ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
		if( pEffect )
			m_GatheringEffectID = pEffect->GetUniqueID();
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( !m_GuidEffect.IsEmpty() )
	{
		ioEffect *pEffect = pOwner->AttachEffect( m_GuidEffect );
		if( pEffect )
			m_GuidEffectID = pEffect->GetUniqueID();
	}

	// Sound
	ioHashString szCurSound = GetCurGatheringSound();
	if( !szCurSound.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
		}
	}
}

void ioCrossBowItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	m_fCurChargeRate = 0.0f;
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	
	m_FireState = FS_NORMAL_ATTACK;
}

void ioCrossBowItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetChargingState( false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	int iAniID = pGrp->GetAnimationIdx( m_FireAttribute.m_AttackAnimation );
	float fAniRate = m_FireAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_FireAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	float fValue = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	m_dwFireStartTime = dwCurTime + fValue;

	m_dwFireEndTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fAniRate);
	m_dwFireEndTime += m_FireAttribute.m_dwEndDelay;

	pOwner->SetReservedSliding( m_FireAttribute.m_vForceInfoList, iAniID, fAniRate, dwPreDelay );

	m_dwKeyReserveTime = 0;
	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if( fKeyReserve > 0.0f )
	{
		m_dwKeyReserveTime = dwCurTime + fKeyReserve;
	}

	m_FireState = FS_FIRE;
}

void ioCrossBowItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket >> m_iCurCombo;

	int iFireState;
	rkPacket >> iFireState;

	switch( iFireState )
	{
	case FS_NONE:
		{
			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			if( m_GuidEffectID != -1 )
			{
				pOwner->EndEffect( m_GuidEffectID, false );
				m_GuidEffectID = -1;
			}

			SetNoneState( pOwner );
		}
		break;
	case FS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		{
			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;
	case FS_FIRE:
		{
			ChangeToAttackFire( pOwner );
		}
		break;
	}
}

void ioCrossBowItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iType;
	rkPacket >> iType;

	ioPlayStage *pStage = pOwner->GetCreator();

	switch( iType )
	{
	case ST_CHARGE:
		{
			rkPacket >> m_iCurCombo;

			ioHashString szName;
			bool bRefresh, bAdd;

			rkPacket >> bRefresh;
			if( bRefresh )
			{
				

				int iReleaseCnt;
				rkPacket >> iReleaseCnt;
				for( int i=0; i < iReleaseCnt; ++i )
				{
					rkPacket >> szName;
					ReleaseTarget( szName, pStage );
				}

				int iFindCnt;
				rkPacket >> iFindCnt;
				for( int j=0; j < iFindCnt; ++j )
				{
					rkPacket >> szName;
					AddTarget( szName, pStage );
				}
			}

			rkPacket >> bAdd;
			if( bAdd )
			{
				rkPacket >> szName;
				AddBulletInfo( szName, pStage );

				// Effect
				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				ioHashString szCurEffect = GetCurGatheringEffect();
				if( !szCurEffect.IsEmpty() )
				{
					ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
					if( pEffect )
						m_GatheringEffectID = pEffect->GetUniqueID();
				}

				if( m_GuidEffectID != -1 )
				{
					pOwner->EndEffect( m_GuidEffectID, false );
					m_GuidEffectID = -1;
				}

				if( !m_GuidEffect.IsEmpty() )
				{
					ioEffect *pEffect = pOwner->AttachEffect( m_GuidEffect );
					if( pEffect )
						m_GuidEffectID = pEffect->GetUniqueID();
				}

				// Sound
				ioHashString szCurSound = GetCurGatheringSound();
				if( !szCurSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
					}
				}
			}
		}
		break;
	case ST_DIR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			m_bTargetRot = false;
			pOwner->SetTargetRot( qtRot );

			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			pOwner->SetNetworkKeyInput( m_CurDirKey );
		}
		break;
	}
}

void ioCrossBowItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioTraceWeaponType2 *pTrace = ToTraceWeaponType2( pWeapon );
	if( pTrace && !m_CurTargetName.IsEmpty() )
	{
		pTrace->SetTraceTarget( m_CurTargetName );
	}
}

void ioCrossBowItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_CROSS_BOW,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioCrossBowItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ClearTargetBulletInfo();

	ioWeaponItem::OnReleased( pOwner );
}

bool ioCrossBowItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_CHARGING ||
			m_FireState == FS_GATHERING )
			return true;
	}

	return false;
}

void ioCrossBowItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	ClearTargetBulletInfo();

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_FireState = FS_NONE;

	m_iCurCombo = 0;
	m_bSetGatherMoveAni = false;

	m_dwFireEndTime = 0;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

void ioCrossBowItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( !pOwner ) return;

	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}
}

float ioCrossBowItem::GetAttackSpeedRate() const
{
	return m_fAttackSpeedRate;
}

bool ioCrossBowItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_FireState )
	{
	case FS_GATHERING:
		return true;
	}

	return false;
}

bool ioCrossBowItem::CheckAddBulletTime()
{
	if( m_GatheringInfoList.empty() )
		return false;

	int iIndex = m_iCurCombo-1;
	if( !COMPARE( iIndex, 0, m_iMaxBullet ) )
	{
		return false;
	}

	m_dwNextBulletAddTime = m_dwGatheringStartTime + m_GatheringInfoList[iIndex].m_dwCreateTime;
	return true;
}

ioHashString ioCrossBowItem::GetCurGatheringEffect()
{
	ioHashString szName;

	if( m_GatheringInfoList.empty() )
		return szName;

	int iIndex = m_iCurCombo-1;

	if( COMPARE( iIndex, 0, m_iMaxBullet ) )
		szName = m_GatheringInfoList[iIndex].m_GatheringEffect;

	return szName;
}

ioHashString ioCrossBowItem::GetCurGatheringSound()
{
	ioHashString szName;

	if( m_GatheringInfoList.empty() )
		return szName;

	int iIndex = m_iCurCombo-1;

	if( COMPARE( iIndex, 0, m_iMaxBullet ) )
		szName = m_GatheringInfoList[iIndex].m_GatheringSound;

	return szName;
}

int ioCrossBowItem::GetCurTargetCnt()
{
	ioHashStringVec vList;
	if( GetTargetList( vList ) )
	{
		return (int)vList.size();
	}

	return 0;
}

bool ioCrossBowItem::GetTargetList( ioHashStringVec &vList )
{
	if( m_TargetBulletInfoList.empty() )
		return false;

	bool bExist = false;
	int iSize = m_TargetBulletInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		DWORD dwTargetTime = m_TargetBulletInfoList[i].m_dwTargetTime;
		ioHashString szTargetName = m_TargetBulletInfoList[i].m_szTargetName;

		if( dwTargetTime > 0 && !szTargetName.IsEmpty() )
		{
			bExist = true;
			vList.push_back( szTargetName );
		}
	}

	return bExist;
}

bool ioCrossBowItem::CheckTargetState( ioBaseChar *pOwner, ioHashStringVec &vReleseList, ioHashStringVec &vFindList )
{
	bool bEnd = CheckValidateTarget( pOwner, vReleseList );
	bool bNewTarget = CheckFindTarget( pOwner, vFindList );

	if( bEnd || bNewTarget )
		return true;

	return false;
}

bool ioCrossBowItem::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;
		
	if( pTarget->HasChangeTeamBuff(true) )
		return false;
		
	if( bCheckTargetWound )
	{
		// 쓰러짐 관련 체크
		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			return false;
	}

	return true;
}

bool ioCrossBowItem::CheckValidateTarget( ioBaseChar *pOwner, ioHashStringVec &vReleaseList )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( m_TargetBulletInfoList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	
	bool bEnd = false;
	int iSize = m_TargetBulletInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		DWORD dwTargetTime = m_TargetBulletInfoList[i].m_dwTargetTime;
		ioHashString szTargetName = m_TargetBulletInfoList[i].m_szTargetName;

		if( dwTargetTime == 0 || szTargetName.IsEmpty() )
			continue;

		// target 유효성 체크
		ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
		if( !pTarget || !IsEnableTargetState( pTarget, false ) )
		{
			ReleaseTarget( szTargetName, pStage );

			bEnd = true;
			vReleaseList.push_back( szTargetName );
			continue;
		}

		// 거리체크
		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
		{
			ReleaseTarget( szTargetName, pStage );

			bEnd = true;
			vReleaseList.push_back( szTargetName );
			continue;
		}

		// 시간체크
		DWORD dwEndTime = dwTargetTime + m_dwTargetEndDuration;
		if( dwEndTime < dwCurTime )
		{
			ReleaseTarget( szTargetName, pStage );

			bEnd = true;
			vReleaseList.push_back( szTargetName );
			continue;
		}
	}

	return bEnd;
}

bool ioCrossBowItem::CheckFindTarget( ioBaseChar *pOwner, ioHashStringVec &vFindList )
{
	if( !pOwner ) return false;
	ioPlayStage *pStage = pOwner->GetCreator();

	// max check
	int iCurTargetCnt = GetCurTargetCnt();
	int iMaxTargetCnt = m_TargetBulletInfoList.size();
	if( iMaxTargetCnt <= iCurTargetCnt )
		return false;

	// time check
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSearchCheckTime > 0 && m_dwSearchCheckTime+m_dwTargetSearchTime >= dwCurTime )
		return false;

	bool bAdd = false;
	ioHashString szTarget = FindTarget( pOwner );
	if( !szTarget.IsEmpty() )
	{
		AddTarget( szTarget, pStage );
		vFindList.push_back( szTarget );
		
		m_dwSearchCheckTime = dwCurTime;
		bAdd = true;
	}

	return bAdd;
}

bool ioCrossBowItem::CheckNextAddBullet( ioBaseChar *pOwner, ioHashString &szAddTarget )
{
	if( !pOwner ) return false;
	if( m_dwNextBulletAddTime <= 0 )
	{
		return false;
	}

	if( m_iMaxBullet <= (int)m_TargetBulletInfoList.size() )
	{
		m_dwNextBulletAddTime = 0;
		return false;
	}

	ioPlayStage *pStage = pOwner->GetCreator();

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwNextBulletAddTime < dwCurTime )
	{
		m_iCurCombo++;

		// add targetbullet
		AddBulletInfo( pOwner, pStage );

		if( !CheckAddBulletTime() )
		{
			m_dwNextBulletAddTime = 0;
		}
		else
		{
			// Effect
			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			ioHashString szCurEffect = GetCurGatheringEffect();
			if( !szCurEffect.IsEmpty() )
			{
				ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
				if( pEffect )
					m_GatheringEffectID = pEffect->GetUniqueID();
			}

			// Sound
			ioHashString szCurSound = GetCurGatheringSound();
			if( !szCurSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
				}
			}
		}

		return true;
	}

	return false;
}

ioHashString ioCrossBowItem::AddBulletInfo( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	TargetBulletInfo kInfo;

	ioHashString szTarget = FindTarget( pOwner );
	if( !szTarget.IsEmpty() )
	{
		kInfo.m_dwTargetTime = FRAMEGETTIME();
		kInfo.m_szTargetName = szTarget;

		if( pStage )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
			if( pTarget && !m_TargetEffect.IsEmpty() )
			{
				pTarget->EndEffect( m_TargetEffect, false );
				pTarget->AttachEffect( m_TargetEffect );
			}
		}
	}

	m_TargetBulletInfoList.push_back( kInfo );
	
	return szTarget;
}

void ioCrossBowItem::AddBulletInfo( const ioHashString &szName, ioPlayStage *pStage )
{
	TargetBulletInfo kInfo;

	if( !szName.IsEmpty() )
	{
		kInfo.m_dwTargetTime = FRAMEGETTIME();
		kInfo.m_szTargetName = szName;

		if( pStage )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( szName );
			if( pTarget && !m_TargetEffect.IsEmpty() )
			{
				pTarget->EndEffect( m_TargetEffect, false );
				pTarget->AttachEffect( m_TargetEffect );
			}
		}
	}

	m_TargetBulletInfoList.push_back( kInfo );
}

void ioCrossBowItem::AddTarget( const ioHashString &szName, ioPlayStage *pStage )
{
	int iSize = m_TargetBulletInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szTargetName = m_TargetBulletInfoList[i].m_szTargetName;
		DWORD dwTargetTime = m_TargetBulletInfoList[i].m_dwTargetTime;

		if( dwTargetTime == 0 || szTargetName.IsEmpty() )
		{
			m_TargetBulletInfoList[i].m_dwTargetTime = FRAMEGETTIME();
			m_TargetBulletInfoList[i].m_szTargetName = szName;

			if( pStage )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( szName );
				if( pTarget && !m_TargetEffect.IsEmpty() )
				{
					pTarget->EndEffect( m_TargetEffect, false );
					pTarget->AttachEffect( m_TargetEffect );
				}
			}
			return;
		}
	}
}

void ioCrossBowItem::ReleaseTarget( const ioHashString &szName, ioPlayStage *pStage )
{
	if( szName.IsEmpty() )
		return;

	int iSize = m_TargetBulletInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_TargetBulletInfoList[i].m_szTargetName == szName )
		{
			m_TargetBulletInfoList[i].m_dwTargetTime = 0;
			m_TargetBulletInfoList[i].m_szTargetName.Clear();

			ioBaseChar *pTarget = pStage->GetBaseChar( szName );
			if( pTarget && !m_TargetEffect.IsEmpty() )
			{
				pTarget->EndEffect( m_TargetEffect, false );
			}
			return;
		}
	}
}

ioHashString ioCrossBowItem::FindTarget( ioBaseChar *pOwner )
{
	ioHashString szTarget;

	if( !pOwner )
		return szTarget;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return szTarget;

	// CheckBox
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	ioOrientBox eCheckBox;
	D3DXVECTOR3 vMax, vMin;
	D3DXVECTOR3 vMidPos = pOwner->GetMidPositionByRate();

	vMax = vMin = vMidPos;

	vMax.x += m_fCheckBoxX;
	vMax.y += m_fCheckBoxY;
	vMax.z += m_fCheckBoxZ;

	//z값은 그대로 여야함
	vMin.x -= m_fCheckBoxX;
	vMin.y -= m_fCheckBoxY;

	eCheckBox.SetBoxByMinMax( vMin, vMax );
	eCheckBox.SetCenter( vMidPos );

	// 방향전환
	eCheckBox.Transform( vMidPos, ioMath::UNIT_ALL, qtRot );
	// 회전후 최종위치로 조정
	D3DXVECTOR3 vCencter = vMidPos + ( m_fCheckBoxZ * FLOAT05 * vDir );
	eCheckBox.SetCenter( vCencter );

#ifndef SHIPPING
	eCheckBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	// target list
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	
	ioBaseChar *pTarget = NULL;
	float fGapDistance = std::numeric_limits<float>::infinity();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar == pOwner )
			continue;

		if( IsExistTarget( pCurChar->GetCharName() ) )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		if( !pCurChar->IsWoundedCollision( eCheckBox ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fTargetAngle )
			continue;

		if( fLengthSq < fGapDistance )
		{
			pTarget = pCurChar;
			fGapDistance = fLengthSq;
		}
	}

	if( pTarget )
	{
		szTarget = pTarget->GetCharName();
	}

	return szTarget;
}

bool ioCrossBowItem::IsExistTarget( const ioHashString &szName )
{
	if( szName.IsEmpty() )
		return false;

	if( m_TargetBulletInfoList.empty() )
		return false;

	bool bExist = false;
	int iSize = m_TargetBulletInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szTargetName = m_TargetBulletInfoList[i].m_szTargetName;
		if( szName == szTargetName )
		{
			bExist = true;
			break;
		}
	}

	return bExist;
}

void ioCrossBowItem::CheckMoveKey( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_DIR;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCrossBowItem::CheckMoveAni( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		if( !m_bSetGatherMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_GatherMoveAnimation);
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetGatherMoveAni = true;
		}
	}
	else
	{
		if( m_bSetGatherMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetGatherMoveAni = false;
		}
	}
}

void ioCrossBowItem::CheckMoveDir( ioBaseChar *pOwner )
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

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
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
		pOwner->SetTargetRotToRotate( qtRotate, true );
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
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

bool ioCrossBowItem::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_FireState )
		{
		case FS_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioCrossBowItem::CheckLanding( ioBaseChar *pChar )
{
	if( !CheckAimState() )
	{
		m_bSetGatherMoveAni = false;
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->GetGroup()->ReleaseAllExtraRotate();
		SetNoneState( pChar );
		return true;
	}

	m_bSetGatherMoveAni = false;
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f );
	pGrp->SetLoopAni( m_GatherAnimation, 0.0f );

	pChar->GetGroup()->ReleaseAllExtraRotate();

	return false;
}

int ioCrossBowItem::GetMaxBullet()
{
	return m_iMaxBullet;
}

int ioCrossBowItem::GetCurBullet()
{
	return (int)m_TargetBulletInfoList.size();
}

void ioCrossBowItem::SetAimJumpState( ioBaseChar *pOwner, bool bAimed )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_FireState = FS_JGATHERING;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GatherJumpAnimation );
	float fAniRate = m_fGatherJumpAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );

	if( !bAimed )
	{
		pOwner->SetTargetMarkerTarget( "", false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

		pOwner->SetChargingState( true );

		m_dwGatheringStartTime = dwCurTime;

		ClearTargetBulletInfo();
		m_dwNextBulletAddTime = 0;

		// 1발 기본 충전
		m_iCurCombo = 1;
		TargetBulletInfo kInfo;
		m_TargetBulletInfoList.push_back( kInfo );

		// NextTime
		CheckAddBulletTime();

		// Effect
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		ioHashString szCurEffect = GetCurGatheringEffect();
		if( !szCurEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
			if( pEffect )
				m_GatheringEffectID = pEffect->GetUniqueID();
		}

		if( m_GuidEffectID != -1 )
		{
			pOwner->EndEffect( m_GuidEffectID, false );
			m_GuidEffectID = -1;
		}

		if( !m_GuidEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_GuidEffect );
			if( pEffect )
				m_GuidEffectID = pEffect->GetUniqueID();
		}

		// Sound
		ioHashString szCurSound = GetCurGatheringSound();
		if( !szCurSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
			}
		}
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

		// Effect
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		ioHashString szCurEffect = GetCurGatheringEffect();
		if( !szCurEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
			if( pEffect )
				m_GatheringEffectID = pEffect->GetUniqueID();
		}

		if( m_GuidEffectID != -1 )
		{
			pOwner->EndEffect( m_GuidEffectID, false );
			m_GuidEffectID = -1;
		}

		if( !m_GuidEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_GuidEffect );
			if( pEffect )
				m_GuidEffectID = pEffect->GetUniqueID();
		}

		// Sound
		ioHashString szCurSound = GetCurGatheringSound();
		if( !szCurSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
			}
		}
	}
}

bool ioCrossBowItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	ClearAirJump();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetChargingState( false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_dwBaseWeaponIndex = pOwner->GetWeaponIndexBase();

	AttackType eExtendType = AT_JUMP_ATTACK;
	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioCrossBowItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fAniRate = pAttr->m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	float fValue = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	m_dwFireStartTime = dwCurTime + fValue;

	m_dwFireEndTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fAniRate);
	m_dwFireEndTime += pAttr->m_dwEndDelay;

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fAniRate, dwPreDelay );

	//
	SetAirJump( pOwner, iAniID, fTimeRate );

	m_dwKeyReserveTime = 0;
	m_FireState = FS_JFIRE;

	return true;
}

void ioCrossBowItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	//
	CheckAirJump( pOwner );

	//
	if( m_TargetBulletInfoList.empty() )
		return;

	if( m_dwFireEndTime < dwCurTime )
	{
		ClearTargetBulletInfo();
		return;
	}

	AttackType eExtendType = AT_JUMP_ATTACK;
	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioCrossBowItem::ProcessJumpAttack - AttackAttribute Not Exist" );
		return;
	}

	if( pAttr->m_vWeaponInfoList.empty() )
		return;

	if( m_dwFireStartTime == 0 || m_dwFireStartTime >= dwCurTime )
		return;

	int iCurBulletCnt = m_TargetBulletInfoList.size();
	int iCurTargetCnt = GetCurTargetCnt();

	int iMaxFireCnt = iCurTargetCnt;
	if( iMaxFireCnt == 0 && iCurBulletCnt > 0 )
		iMaxFireCnt = 1;

	ioHashStringVec vTargetList;
	GetTargetList( vTargetList );

	const WeaponInfo &rkInfo = pAttr->m_vWeaponInfoList.front();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	int iCurFireCnt = 0;
	DWORD dwWeaponIndex = m_dwBaseWeaponIndex;

	// target fire
	int iSize = vTargetList.size();
	for( int i=0; i < iSize; ++i )
	{
		m_CurTargetName = vTargetList[i];

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_JUMP_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime,
									vPos,
									vDir,
									GetName() );

		iCurFireCnt++;
		dwWeaponIndex++;
	}

	// none target fire
	if( iMaxFireCnt > 0 && iCurFireCnt < iMaxFireCnt )
	{
		m_CurTargetName.Clear();

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_JUMP_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime,
									vPos,
									vDir,
									GetName() );
	}

	m_dwFireStartTime = 0;
	ClearTargetBulletInfo();
}






