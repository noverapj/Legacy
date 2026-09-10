
#include "stdafx.h"

#include "ioEarthQuakeItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

#include "ioChargeComboJump2.h"

ioEarthQuakeItem::ioEarthQuakeItem()
{	
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	Init();
}

ioEarthQuakeItem::ioEarthQuakeItem( const ioEarthQuakeItem &rhs )
	: ioWeaponItem( rhs ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_szWaitPreDelayAni( rhs.m_szWaitPreDelayAni ),
	  m_fWaitPreDelayAniRate( rhs.m_fWaitPreDelayAniRate ),
	  m_szWaitLoopAni( rhs.m_szWaitLoopAni ),
	  m_szWaitMoveAni( rhs.m_szWaitMoveAni ),
	  m_fWaitMoveAniRate( rhs.m_fWaitMoveAniRate ),
	  m_DashInfo( rhs.m_DashInfo ),
	  m_nMaxSpecialAttCnt( rhs.m_nMaxSpecialAttCnt ),
	  m_nNeedSpecialAttCnt( rhs.m_nNeedSpecialAttCnt ),
	  m_ExtraGaugeInc( rhs.m_ExtraGaugeInc ),
	  m_szChargeBuff( rhs.m_szChargeBuff ),
	  m_szChargeEffect( rhs.m_szChargeEffect ),
	  m_vChargeEffectOffset( rhs.m_vChargeEffectOffset ),
	  m_szCharEffect( rhs.m_szCharEffect ),
	  m_ReleaseAttackList( rhs.m_ReleaseAttackList ),
	  m_szDummyChar( rhs.m_szDummyChar ),
	  m_JumpExtraInfo( rhs.m_JumpExtraInfo ),
	  m_ChargeAttackInfo( rhs.m_ChargeAttackInfo )
{
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	Init();
}

ioEarthQuakeItem::~ioEarthQuakeItem()
{
}

void ioEarthQuakeItem::LoadProperty( ioINILoader &rkLoader )
{
	//기본 파일 읽기
	ioWeaponItem::LoadProperty( rkLoader );

	m_AttackReadyAniList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	//공격 준비 애니메이션
	int nMaxCombo = GetMaxCombo();
	for( int i=0; i < nMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}
	
	rkLoader.LoadString_e( "wait_pre_delay_ani", "", szBuf, MAX_PATH );
	m_szWaitPreDelayAni = szBuf;
	m_fWaitPreDelayAniRate = rkLoader.LoadFloat_e( "wait_pre_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "wait_loop_ani", "", szBuf, MAX_PATH );
	m_szWaitLoopAni = szBuf;
	rkLoader.LoadString_e( "wait_move_ani", "", szBuf, MAX_PATH );
	m_szWaitMoveAni = szBuf;
	m_fWaitMoveAniRate = rkLoader.LoadFloat_e( "wait_move_ani_rate", FLOAT1 );

	LoadAttackAttribute( "dash_info", m_DashInfo, rkLoader );
	LoadAttackAttribute( "jump_extra_info", m_JumpExtraInfo, rkLoader );
	LoadAttackAttribute( "charge_attack_info", m_ChargeAttackInfo, rkLoader );

	m_nMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_nNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );

	rkLoader.LoadString_e( "charge_buff", "", szBuf, MAX_PATH );
	m_szChargeBuff = szBuf;
	
	m_szChargeEffect.clear();
	for ( int i=0; i<m_nMaxSpecialAttCnt; i++ )
	{
		wsprintf_e( szKey, "charge_effect%d", i+1 ); 
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szChargeEffect.push_back( szBuf );
	}

	m_vChargeEffectOffset.x = rkLoader.LoadFloat_e( "charge_effect_offset_x", 0.0f );
	m_vChargeEffectOffset.y = rkLoader.LoadFloat_e( "charge_effect_offset_y", 0.0f );
	m_vChargeEffectOffset.z = rkLoader.LoadFloat_e( "charge_effect_offset_z", 0.0f );

	m_szCharEffect.clear();
	for ( int i=0; i<m_nMaxSpecialAttCnt; i++ )
	{
		wsprintf_e( szKey, "char_effect%d", i+1 ); 
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szCharEffect.push_back( szBuf );
	}

	m_ReleaseAttackList.clear();
	for ( int i=0; i<m_nMaxSpecialAttCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "release_attack%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_ReleaseAttackList.push_back( sAttackAttribute );
	}

	m_szDummyChar.clear();
	for ( int i=0; i<m_nMaxSpecialAttCnt; i++ )
	{
		wsprintf_e( szKey, "dummy_char%d", i+1 ); 
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szDummyChar.push_back( szBuf );
	}

	m_ExtraGaugeInc.clear();
	for ( int i=0; i<m_nMaxSpecialAttCnt; i++ )
	{
		wsprintf_e( szKey, "extra_gauge%d_inc_rate", i+1 ); 
		float fBaseInc = rkLoader.LoadFloat( szKey, 0.0f );
		m_ExtraGaugeInc.push_back( fBaseInc );
	}
}

ioItem* ioEarthQuakeItem::Clone()
{
	return new ioEarthQuakeItem( *this );
}

ioWeaponItem::WeaponSubType ioEarthQuakeItem::GetSubType() const
{
	return WST_EARTHQUAKE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::SetNormalAttack( ioBaseChar *pOwner, int nCurCombo )
{
	ClearCancelInfo();	
	
	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int nComboCnt = GetMaxCombo();
	if( !COMPARE( nCurCombo, 0, nComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, nCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[nCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = nCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioEarthQuakeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;
	case WS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

bool ioEarthQuakeItem::SetJumpLandingExtraState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	ioChargeComboJump2 *pChargeCombo2 = ToChargeComboJump2( GetCurExtendJump() );
	if ( !pChargeCombo2 || !pChargeCombo2->IsChargeAttacked() )
		return false;

	pOwner->SetState( CS_EARTHQUAKE_SPECIAL );

	SetJumpExtraState( pOwner );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioEarthQuakeItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_WeaponItemState == WS_CHECK_CHARGING )
			return true;
	}

	return false;
}

bool ioEarthQuakeItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_SpecialState )
	{
	case SS_LOOP:
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );	

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_INIT:
		{
			pOwner->SetState( CS_EARTHQUAKE_SPECIAL );

			SetPreDelayState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		RecvSpecialState( pOwner, rkPacket );
		break;
	}
}

void ioEarthQuakeItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case SS_DASH:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			SetDashState( pOwner );
		};
		break;
	case SS_RELEASE_ATTACK:
		{
			int nCurKey;
			rkPacket >> nCurKey;
			rkPacket >> m_nCurSpecialAttCnt;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			SetReleaseAttackState( pOwner );
		}
		break;
	case SS_CREATE_DUMMY:
		{
			int nIndex;
			rkPacket >> nIndex;
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;

			if ( !COMPARE( m_nCurSpecialAttCnt, 1, m_nMaxSpecialAttCnt+1 ) || 
				!COMPARE( (int)m_szDummyChar.size(), 1, m_nMaxSpecialAttCnt+1 ) )
				return;

			ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyChar[m_nCurSpecialAttCnt-1], nIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
		}
		break;
	case SS_JUMP_EXTRA:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->SetState( CS_EARTHQUAKE_SPECIAL );

			SetJumpExtraState( pOwner );
		}
		break;
	case SS_CHARGE_ATTACK:
		{
			SetChargeAttackState( pOwner );
		}
		break;
	case SS_INIT:
		{
			ClearState( pOwner );
		}
		break;
	case SS_ATTACH_EFFECT:
		{
			rkPacket >> m_nCurSpecialAttCnt;

			if ( COMPARE( m_nCurSpecialAttCnt, 1, (int)m_szCharEffect.size()+1 ) )
			{
				EndEffect( pOwner, m_dwCharEffectID );

				m_dwCharEffectID = AttachEffect( pOwner, m_szCharEffect[m_nCurSpecialAttCnt-1] );
			}
		}
		break;
	case SS_ADD_BUFF:
		{
			ioHashString szBuffName;
			rkPacket >> szBuffName;

			SetOwnerAddBuff( pOwner, szBuffName, false );
		}
		break;
	case SS_REMOVE_BUFF:
		{
			ioHashString szBuffName;
			rkPacket >> szBuffName;

			SetOwnerReserveEndBuff( pOwner, szBuffName, false );
		}
		break;
	case SS_END:
		{
			m_SpecialState = SS_END;			
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
		
	if( m_nCurSpecialAttCnt >= m_nMaxSpecialAttCnt )
		return;

	switch( m_SpecialState )
	{
	case SS_LOOP:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = GetBaseInc() * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fMaxExtraGauge );

	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_nCurSpecialAttCnt++;
		m_nCurSpecialAttCnt = max( 0, min(m_nCurSpecialAttCnt, m_nMaxSpecialAttCnt) );

		m_fCurExtraGauge = 0.0f;
		
		UpdateGaugeEffect( pOwner );
		SetOwnerAddBuff( pOwner, m_szChargeBuff, true );
	}
}

int ioEarthQuakeItem::GetMaxBullet()
{
	return m_nMaxSpecialAttCnt;
}

int ioEarthQuakeItem::GetCurBullet()
{
	return m_nCurSpecialAttCnt;
}

void ioEarthQuakeItem::SetCurBullet( int nCurBullet )
{
	if( nCurBullet < 0 )
		return;

	nCurBullet = max( 0, min( nCurBullet, m_nMaxSpecialAttCnt ) );

	m_nCurSpecialAttCnt = nCurBullet;
}

int ioEarthQuakeItem::GetNeedBullet()
{
	return m_nNeedSpecialAttCnt;
}

float ioEarthQuakeItem::GetBaseInc()
{
	if ( COMPARE( m_nCurSpecialAttCnt, 0, (int)m_szChargeEffect.size() ) )
		return m_ExtraGaugeInc[m_nCurSpecialAttCnt];

	return m_fExtraGaugeBaseInc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::Init()
{	
	m_WeaponItemState = WS_NONE;
	m_SpecialState = SS_NONE;

	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_nCurSpecialAttCnt = 0;
	m_fCurExtraGauge = 0.0f;
	m_dwChargeEffectID = -1;
	m_bSetMove = false;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
}

void ioEarthQuakeItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess()  )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetEarthQuakeSpecialState( pOwner );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioEarthQuakeItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponItemState = WS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponItemState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::ClearState( ioBaseChar *pOwner )
{
	SetOwnerReserveEndBuff( pOwner, m_szChargeBuff, false );
	EndEffect( pOwner, m_dwCharEffectID );
	DestroyMapEffect( pOwner, m_dwChargeEffectID );

	Init();
}

void ioEarthQuakeItem::SetEarthQuakeSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	pOwner->SetState( CS_EARTHQUAKE_SPECIAL );
	
	SetPreDelayState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEarthQuakeItem::ProcessEarthQuakeSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialState )
	{
	case SS_PRE_DELAY:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetLoopState( pOwner );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopMoveAni( pOwner );
			CheckInputKey( pOwner );
		}
		break;
	case SS_DASH:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( m_nCurSpecialAttCnt > 0 )
					SetReleaseAttackState( pOwner );
				else
					pOwner->SetState( CS_DELAY );
			}
		}
		break;
	case SS_CHARGE_ATTACK:
	case SS_RELEASE_ATTACK:
	case SS_JUMP_EXTRA:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					if ( m_nCurSpecialAttCnt <= m_nMaxSpecialAttCnt )
						SetOwnerReserveEndBuff( pOwner, m_szChargeBuff, true );

					CheckReserveState( pOwner );
					return;
				}
				else
				{
					if ( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
						CreateDummy( pOwner );

					if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
						ReserveInputKey( pOwner );
				}
			}
		}
		break;
	case SS_END:
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	}

	UpdateMapEffect( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( CheckInputJump( pOwner ) )
		return;

	if ( CheckInputDash( pOwner ) )
		return;
	
	if ( CheckReleaseAttack( pOwner ) )
		return;
}

bool ioEarthQuakeItem::CheckInputDash( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( pOwner->IsDirKeyDoubleClick() )
	{
		pOwner->ClearDirDoubleClick();

		ioUserKeyInput::DirKeyInput CurKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( CurKey );

		SetDashState( pOwner );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_DASH;
			kPacket << (int)CurKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

bool ioEarthQuakeItem::CheckInputJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		ClearState( pOwner );

		pOwner->SetJumpState();

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_INIT;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}
	
	return false;
}

bool ioEarthQuakeItem::CheckReleaseAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( !pOwner->IsAttackKeyDown() )
	{
		if ( m_nCurSpecialAttCnt > 0 )
			SetReleaseAttackState( pOwner );
		else
			SetChargeAttackState( pOwner );
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::SetPreDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szWaitPreDelayAni );
	if( nAniID == -1 )	
		return;

	float fTimeRate = m_fWaitPreDelayAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );

	m_SpecialState = SS_PRE_DELAY;
}

void ioEarthQuakeItem::SetLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsSettedDirection() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );
		m_bSetMove = true;
	}
	else
	{		
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );
		m_bSetMove = false;
	}

	m_SpecialState = SS_LOOP;

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false );
}

void ioEarthQuakeItem::SetDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_DashInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_DashInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipSkillTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DashInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	m_SpecialState = SS_DASH;

	SetOwnerReserveEndBuff( pOwner, m_szChargeBuff, false );
}

void ioEarthQuakeItem::SetReleaseAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( !COMPARE( m_nCurSpecialAttCnt, 1, m_nMaxSpecialAttCnt+1 ) || 
		 !COMPARE( (int)m_ReleaseAttackList.size(), 1, m_nMaxSpecialAttCnt+1 ) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_ReleaseAttackList[m_nCurSpecialAttCnt-1].m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_ReleaseAttackList[m_nCurSpecialAttCnt-1].m_fAttackAniRate;
	DWORD dwPreDelay = m_ReleaseAttackList[m_nCurSpecialAttCnt-1].m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_ReleaseAttackList[m_nCurSpecialAttCnt-1].m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	//pOwner->RefreshFireTimeList( nAniID, m_ReleaseAttackList[m_nCurSpecialAttCnt-1].m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwFireTime = dwCurTime;
	m_dwFireTime += pGrp->GetAnimationEventTime_e( nAniID, "fire_s" ) * fTimeRate;

	m_dwEnableReserveKeyTime = dwCurTime;
	m_dwEnableReserveKeyTime += pGrp->GetAnimationEventTime( nAniID, "key_reserve" ) * fTimeRate;

	if ( m_nCurSpecialAttCnt < m_nMaxSpecialAttCnt )
		SetOwnerReserveEndBuff( pOwner, m_szChargeBuff, false );

	m_SpecialState = SS_RELEASE_ATTACK;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	
	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_RELEASE_ATTACK;
			kPacket << (int)eNewDirKey;
			kPacket << m_nCurSpecialAttCnt;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioEarthQuakeItem::SetJumpExtraState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_JumpExtraInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_JumpExtraInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpExtraInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwEnableReserveKeyTime = dwCurTime;
	m_dwEnableReserveKeyTime += pGrp->GetAnimationEventTime( nAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpExtraInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_JumpExtraInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_SpecialState = SS_JUMP_EXTRA;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_JUMP_EXTRA;
			kPacket << (int)eNewDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioEarthQuakeItem::SetChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_ChargeAttackInfo.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_ChargeAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttackInfo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwEnableReserveKeyTime = dwCurTime;
	m_dwEnableReserveKeyTime += pGrp->GetAnimationEventTime( nAniID, "key_reserve" ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_ChargeAttackInfo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_ChargeAttackInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_SpecialState = SS_CHARGE_ATTACK;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	SetOwnerReserveEndBuff( pOwner, m_szChargeBuff, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_CHARGE_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::SetOwnerAddBuff( ioBaseChar *pOwner, ioHashString szBuffName, bool bSendNetwork )
{
	if ( !pOwner || szBuffName.IsEmpty() )
		return;
	
	pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );

	if ( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_ADD_BUFF;
		kPacket << szBuffName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEarthQuakeItem::SetOwnerReserveEndBuff( ioBaseChar *pOwner, ioHashString szBuffName, bool bSendNetwork )
{
	if ( !pOwner || szBuffName.IsEmpty() )
		return;

	ioBuff *pBuff = pOwner->GetBuff( szBuffName, pOwner->GetCharName() );
	if ( pBuff )
		pBuff->SetReserveEndBuff();

	if ( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_REMOVE_BUFF;
		kPacket << szBuffName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioEarthQuakeItem::AttachEffect( ioBaseChar *pOwner, ioHashString& szEffect )
{
	if ( !pOwner || szEffect.IsEmpty() )
		return -1;

	ioEffect *pEffect = pOwner->AttachEffect( szEffect );
	if( pEffect )
		return pEffect->GetUniqueID();

	return -1;
}

void ioEarthQuakeItem::EndEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )
		return;

	pOwner->EndEffect( dwEffectID, false );

	dwEffectID = -1;
}

DWORD ioEarthQuakeItem::CreateMapEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pOwner || szEffect.IsEmpty() || !pOwner->IsNeedProcess() )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioEarthQuakeItem::DestroyMapEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioEarthQuakeItem::UpdateMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || m_dwChargeEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
		
	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwChargeEffectID );
	if ( pEffect )		
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vChargeEffectOffset;		
		float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
		if ( fMapHeight > 0.0f )
			vPos.y = fMapHeight + m_vChargeEffectOffset.y;

		pEffect->SetWorldPosition( vPos );
		pEffect->SetWorldOrientation( qtRot );
	}
}

void ioEarthQuakeItem::UpdateGaugeEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( COMPARE( m_nCurSpecialAttCnt, 1, (int)m_szChargeEffect.size()+1 ) )
	{
		DestroyMapEffect( pOwner, m_dwChargeEffectID );

		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vChargeEffectOffset;
		float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
		if ( fMapHeight > 0.0f )
			vPos.y = fMapHeight + m_vChargeEffectOffset.y;

		m_dwChargeEffectID = CreateMapEffect( pOwner, m_szChargeEffect[m_nCurSpecialAttCnt-1], vPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
	}

	if ( COMPARE( m_nCurSpecialAttCnt, 1, (int)m_szCharEffect.size()+1 ) )
	{
		EndEffect( pOwner, m_dwCharEffectID );

		m_dwCharEffectID = AttachEffect( pOwner, m_szCharEffect[m_nCurSpecialAttCnt-1] );
		
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_ATTACH_EFFECT;
			kPacket << m_nCurSpecialAttCnt;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || m_dwChargeEffectID == -1 && m_SpecialState != SS_RELEASE_ATTACK )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( !COMPARE( m_nCurSpecialAttCnt, 1, m_nMaxSpecialAttCnt+1 ) || 
		!COMPARE( (int)m_szDummyChar.size(), 1, m_nMaxSpecialAttCnt+1 ) )
		return;

	D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
	ioMapEffect *pEffect = pStage->FindMapEffect( m_dwChargeEffectID );
	if ( pEffect )		
		vPos = pEffect->GetWorldPosition();

	int nCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDummyChar[m_nCurSpecialAttCnt-1], nCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_CREATE_DUMMY;
		kPacket << nCurIndex;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	EndEffect( pOwner, m_dwCharEffectID );
	DestroyMapEffect( pOwner, m_dwChargeEffectID );
	m_dwFireTime = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioEarthQuakeItem::IsEnableMoveState()
{
	switch ( m_SpecialState )
	{
	case SS_LOOP:
		return true;
	}

	return false;
}

void ioEarthQuakeItem::CheckLoopMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );

			m_bSetMove = true;
		}
	}
	else
	{		
		if( m_bSetMove )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );

			m_bSetMove = false;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioEarthQuakeItem::ReserveInputKey(ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioEarthQuakeItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	m_SpecialState = SS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioEarthQuakeItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioEarthQuakeItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioEarthQuakeItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioEarthQuakeSpecialState::ioEarthQuakeSpecialState()
{
}

ioEarthQuakeSpecialState::~ioEarthQuakeSpecialState()
{
}

void ioEarthQuakeSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioEarthQuakeSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioEarthQuakeItem *pEarthQuakeItem = ToEarthQuakeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEarthQuakeItem )
		pEarthQuakeItem->ClearState( pOwner );
}

void ioEarthQuakeSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioEarthQuakeItem *pEarthQuakeItem = ToEarthQuakeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEarthQuakeItem )
		pEarthQuakeItem->ProcessEarthQuakeSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioEarthQuakeSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioEarthQuakeItem *pEarthQuakeItem = ToEarthQuakeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEarthQuakeItem && pEarthQuakeItem->IsEnableMoveState() )
		pOwner->ChangeDirectionByInputDir( false );
}

bool ioEarthQuakeSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioEarthQuakeItem *pEarthQuake = ToEarthQuakeItem( pOwner->GetWeapon() );
	if( pEarthQuake && pEarthQuake->IsEnableAttackDashCheck( pOwner, true ) )
		return true;

	return false;
}