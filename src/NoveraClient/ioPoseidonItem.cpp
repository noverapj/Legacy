#include "StdAfx.h"
#include "ioPoseidonItem.h"
#include "WeaponDefine.h"
#include "ioOwnerTraceWoundAniDummy.h"

ioPoseidonItem::ioPoseidonItem(void)
{
	Init();
}

ioPoseidonItem::ioPoseidonItem( const ioPoseidonItem& rhs ) : ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_ChargingEndAttack( rhs.m_ChargingEndAttack ),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fNeedBullet( rhs.m_fNeedBullet ),
	m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
	m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
	m_fRecoverGaugeDash( rhs.m_fRecoverGaugeDash ),
	m_fRecoverGaugeWound( rhs.m_fRecoverGaugeWound ),
	m_fRecoverGaugeRateWhileSummon( rhs.m_fRecoverGaugeRateWhileSummon ),
	m_AttachWeapon( rhs.m_AttachWeapon ),
	m_szChargingEffect( rhs.m_szChargingEffect ),
	m_vSummonReadyEffectList( rhs.m_vSummonReadyEffectList ),
	m_vSummonUpEffectList( rhs.m_vSummonUpEffectList ),
	m_vSummonLeftEffectList( rhs.m_vSummonLeftEffectList ),
	m_vSummonRightEffectList( rhs.m_vSummonRightEffectList ),
	m_szChargingStartAnimation( rhs.m_szChargingStartAnimation ),
	m_fChargingStartAniRate( rhs.m_fChargingStartAniRate ),
	m_szChargingAnimation( rhs.m_szChargingAnimation ),
	m_fChargingAniRate( rhs.m_fChargingAniRate ),
	m_dwChargingAttackDuration( rhs.m_dwChargingAttackDuration ),
	m_fChargingAttackSpeed( rhs.m_fChargingAttackSpeed ),
	m_fChargingAttackRotateSpeed( rhs.m_fChargingAttackRotateSpeed ),
	m_fChargingAttackEndJumpPower( rhs.m_fChargingAttackEndJumpPower ),
	//m_SummonAttack( rhs.m_SummonAttack ),
	m_DummyCharList( rhs.m_DummyCharList ),
	m_szSummonChargeAniInfo( rhs.m_szSummonChargeAniInfo ),
	m_szSummonAniInfo( rhs.m_szSummonAniInfo ),
	m_vMaxSummonDummyByType( rhs.m_vMaxSummonDummyByType ),
	m_vSummonNeedBullet( rhs.m_vSummonNeedBullet ),
	//m_szSummonEffectBuff( rhs.m_szSummonEffectBuff ),
	m_iChargeNeedBullet( rhs.m_iChargeNeedBullet ),
	m_vMinionOffsetInfoList( rhs.m_vMinionOffsetInfoList ),
	m_iMaxSummonType( rhs.m_iMaxSummonType ),
	m_iMaxSummonDummyCnt( rhs.m_iMaxSummonDummyCnt )

{
	Init();
}

ioPoseidonItem::~ioPoseidonItem(void)
{
}

void	ioPoseidonItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	//Attack Ready
	int	iMaxCombo	= GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i + 1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(  szBuf) );
		}
	}
	LoadAttackAttribute( "charging_end_attack", m_ChargingEndAttack, rkLoader );
	rkLoader.LoadString_e( "charging_animation", "", szBuf, MAX_PATH );
	m_szChargingAnimation	= szBuf;
	m_fChargingAniRate	= rkLoader.LoadFloat_e( "charging_ani_rate", 0.0f );
	rkLoader.LoadString_e( "charging_start_animation", "", szBuf, MAX_PATH );
	m_szChargingStartAnimation	= szBuf;
	m_fChargingStartAniRate	= rkLoader.LoadFloat_e( "charging_start_ani_rate", 0.0f );
	rkLoader.LoadString_e( "charging_effect", "", szBuf, MAX_PATH );
	m_szChargingEffect	= szBuf;

	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedBullet	= rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun	= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_fRecoverGaugeDash	= rkLoader.LoadFloat_e( "recover_gauge_dash", 0.0f );
	m_fRecoverGaugeWound	= rkLoader.LoadFloat_e( "recover_gauge_wound", 0.0f );
	m_fRecoverGaugeRateWhileSummon	= rkLoader.LoadFloat_e( "recover_gauge_rate_whilie_summon", 0.0f );

	m_AttachWeapon.m_iWeaponIdx	= rkLoader.LoadInt( "attach_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_AttachWeapon.m_iResistanceIdx	= rkLoader.LoadInt( "attach_weapon_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString( "attach_weapon_type_wounded", "", szBuf, MAX_PATH );
	m_AttachWeapon.m_WoundedAnimation	= szBuf;
	m_AttachWeapon.m_dwWoundedDuration	= rkLoader.LoadInt( "attach_weapon_type_wounded_time", 0 );
	m_AttachWeapon.m_bWoundedLoopAni	= rkLoader.LoadBool( "attach_weapon_type_wounded_loop_ani", false );

	m_vSummonReadyEffectList.clear();
	int	iCnt	= rkLoader.LoadInt_e( "max_summon_ready_effect", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		wsprintf_e( szBuf, "summon_ready_effect_%d", i + 1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_vSummonReadyEffectList.push_back( szBuf );
	}

	m_vSummonUpEffectList.clear();
	iCnt	= rkLoader.LoadInt_e( "max_summon_up_effect", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		wsprintf_e( szBuf, "summon_effect_up_%d", i + 1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_vSummonUpEffectList.push_back( szBuf );
	}

	m_vSummonLeftEffectList.clear();
	iCnt	= rkLoader.LoadInt_e( "max_summon_left_effect", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		wsprintf_e( szBuf, "summon_effect_left_%d", i + 1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_vSummonLeftEffectList.push_back( szBuf );
	}

	m_vSummonRightEffectList.clear();
	iCnt	= rkLoader.LoadInt_e( "max_summon_right_effect", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		wsprintf_e( szBuf, "summon_effect_right_%d", i + 1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_vSummonRightEffectList.push_back( szBuf );
	}

	m_dwChargingAttackDuration	= rkLoader.LoadInt_e( "charging_duration", 0 );
	m_fChargingAttackSpeed	= rkLoader.LoadFloat_e( "charging_speed", 0.0f );
	m_fChargingAttackRotateSpeed	= (DWORD)rkLoader.LoadInt_e( "charging_rotate_speed", 0 );
	m_fChargingAttackEndJumpPower	= rkLoader.LoadFloat_e( "charging_end_jump_power", 0.0f );

	//rkLoader.LoadString_e( "summon_effect_buff", "", szBuf, MAX_PATH );
	//m_szSummonEffectBuff	= szBuf;

	m_iChargeNeedBullet	= rkLoader.LoadInt_e( "charge_need_gauge", 0 );

	rkLoader.LoadString_e( "summon_charge_ani", "", szBuf, MAX_PATH );
	m_szSummonChargeAniInfo.m_AniName	= szBuf;
	m_szSummonChargeAniInfo.m_fAniRate	= rkLoader.LoadFloat_e( "summon_charge_ani_rate", 0.0f );
	
	rkLoader.LoadString_e( "summon_animation", "", szBuf, MAX_PATH );
	m_szSummonAniInfo.m_AniName	= szBuf;
	m_szSummonAniInfo.m_fAniRate	= rkLoader.LoadFloat_e( "summon_animation_left_rate", 0.0f );

	m_iMaxSummonDummyCnt	= 0;
	m_iMaxSummonType	= rkLoader.LoadInt_e( "max_summon_type", 0 );
	for( int i = 0; i < m_iMaxSummonType; i++ )
	{
		int	iMaxCnt	= 0;
		wsprintf_e( szBuf, "summon_minion_type%d", i + 1 );
		iMaxCnt	= rkLoader.LoadInt( szBuf, 0 );
		m_vMaxSummonDummyByType.push_back( iMaxCnt );
		m_iMaxSummonDummyCnt	+= iMaxCnt;

		wsprintf_e( szBuf, "summon_need_gauge%d", i + 1 );
		iMaxCnt	= rkLoader.LoadInt( szBuf, 0 );
		m_vSummonNeedBullet.push_back( iMaxCnt );
	}

	for( int i = 0; i < m_iMaxSummonType; i++ )
	{
		std::vector< D3DXVECTOR3 > offsetList;
		offsetList.clear();

		D3DXVECTOR3	vOffset;
		wsprintf_e( szBuf, "summon_offset_type%d_cnt", i + 1 );
		int	iCnt	= rkLoader.LoadInt( szBuf, 0 );
		for( int j = 0; j < iCnt; j++ )
		{
			wsprintf_e( szBuf, "summon_offset%d_x%d", i + 1, j + 1 );
			vOffset.x	= rkLoader.LoadFloat( szBuf, 0.0f );
			wsprintf_e( szBuf, "summon_offset%d_y%d", i + 1, j + 1 );
			vOffset.y	= rkLoader.LoadFloat( szBuf, 0.0f );
			wsprintf_e( szBuf, "summon_offset%d_z%d", i + 1, j + 1 );
			vOffset.z	= rkLoader.LoadFloat( szBuf, 0.0f );
			offsetList.push_back( vOffset );
		}

		m_vMinionOffsetInfoList.push_back( offsetList );
	}

	m_DummyCharList.clear();
	ioHashStringVec szList;
	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, szList );
}

ioItem*	ioPoseidonItem::Clone()
{
	return	new	ioPoseidonItem( *this );
}

void	ioPoseidonItem::OnEquiped( ioBaseChar *pOwner )
{
	m_bDummyDestroyOnReleased	= false;

	for( int i = 0; i < m_iMaxSummonType; i++ )
	{
		m_vCurrentSummonIdx[i]	= 0;
	}

	int	iCnt	= m_vSummonedMinionInfoList.size();
	for( int i = 0; i < iCnt; i++ )
	{
		m_vSummonedMinionInfoList[i].clear();
	}

	ioAttackableItem::OnEquiped( pOwner );
}

void	ioPoseidonItem::OnReleased( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ErrorReport::SetPosition( 1600, 100 );

	m_bDummyDestroyOnReleased	= true;

	if( pOwner->IsNeedProcess() )
	{
		int	iCnt	= m_vSummonedMinionInfoList.size();
		for( int i = 0; i < iCnt; i++ )
		{
			std::list< int >::iterator	iter	= m_vSummonedMinionInfoList[i].begin();
			for( ; iter != m_vSummonedMinionInfoList[i].end(); iter++ )
			{
				ErrorReport::SetPosition( 1600, 101 );
				ioDummyChar*	pDummy	= g_DummyCharMgr.FindDummyChar( pOwner, *iter );
				if ( pDummy )
					pDummy->SendDieState( false );
			}
		}
	}

	//m_vSummonedMinionInfoList.clear();
	//m_vCurrentSummonIdx.clear();

	ErrorReport::SetPosition( 1600, 102 );

	ioWeaponItem::OnReleased( pOwner );
}

ioWeaponItem::WeaponSubType	ioPoseidonItem::GetSubType() const
{
	return	WST_POSEIDON_ITEM;
}

void	ioPoseidonItem::Init()
{
	m_SpecialState	= PSS_NONE;

	m_dwChargeAttackWeaponIdx	= 0;
	m_dwAttackStartTime	= 0;
	m_iCurCombo	= 0;
	m_fCurBullet	= 0.0f;

	m_dwChargeStartTime	= 0;

	m_vWeaponMoveDir	= ioMath::VEC3_ZERO;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;

	m_dwSummonStartTime	= 0;
	m_dwSummonEndTime	= 0;
	m_bDummyDestroyOnReleased	= false;

	m_vSummonedMinionInfoList.reserve( SS_LEVEL_MAX + 1 );
	for( int i = 0; i <= SS_LEVEL_MAX; i++ )
	{
		std::list< int > summonList;
		summonList.clear();
		m_vSummonedMinionInfoList.push_back( summonList );
	}

	m_PreSummonState	= SS_LEVEL_1;

	m_vCurrentSummonIdx.reserve( SS_LEVEL_MAX + 1 );
}

void	ioPoseidonItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case	CS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	CS_NORMAL_ATTACK:
		{
			ProcessNormalAttack( pOwner );
		}
		break;

	case	CS_CHARGE_ATTACK:
		{
			ProcessChargeAttack( pOwner );
		}
		break;

	case	CS_CHARGE_ATTACK_MOVE:
		{
			ProcessChargeAttackMove( pOwner );
		}
		break;

	case	CS_CHARGE_END_ATTACK:
		{
			ProcessChargeEndAttack( pOwner );
		}
		break;
	}
}

void	ioPoseidonItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	int	iComboCnt	= GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState	= CS_NONE;
		m_iCurCombo	= iCurCombo;

		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState	= CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo	= iCurCombo;
		m_CurDirKey	= ioUserKeyInput::DKI_NONE;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		DWORD	dwTrackingTime	= FRAMEGETTIME();
		dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void	ioPoseidonItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon )
		return;

	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	switch( pWeapon->GetType() )
	{
	case ioWeapon::WT_ZONE_EFFECT:
		{
			ioZoneEffectWeapon*	pZone	= ToZoneEffectWeapon( pWeapon );
			if( pZone )
			{
				ioBaseChar*	pChar	= pZone->GetOwner();
				if( pChar )
				{
					D3DXVECTOR3	vDir	= pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					pZone->SetCollisionDir( vDir );
					pZone->SetZoneValue( false, false );
					pZone->SetLiveTime( m_dwChargingAttackDuration );
					pZone->SetOrientation( pChar->GetTargetRot() );
					m_dwChargeAttackWeaponIdx	= pZone->GetWeaponIndex();
					m_vWeaponMoveDir	= pZone->GetMoveDir();
					m_vWeaponPos	= pZone->GetPosition();
				}
			}
		}
		break;
	case ioWeapon::WT_BOMB_MISSILE3:
		{
			ioBombMissileWeapon3*	pBombMissileWeapon3	= ToBombMissileWeapon3( pWeapon );
			if( pBombMissileWeapon3 )
			{
				ioBaseChar*	pChar	= pBombMissileWeapon3->GetOwner();
				if( pChar )
				{
					pBombMissileWeapon3->SetOrientation( pChar->GetTargetRot() );
				}
				pBombMissileWeapon3->SetDsiableChangeDir( false );
			}
		}
		break;
	case ioWeapon::WT_ZONE_MOVE_WEAPON:
		{
			ioZoneMoveWeapon*	pZoneMoveWeapon	= ToZoneMoveWeapon( pWeapon );
			if( pZoneMoveWeapon )
			{
				ioBaseChar*	pChar	= pZoneMoveWeapon->GetOwner();
				if( pChar )
				{
					pZoneMoveWeapon->SetMoveDir( m_ChargeAttackDir );
				}
			}
		}
		break;
	}
}

void	ioPoseidonItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	float	fIncreaseGauge	= 0.0f;

	switch( pOwner->GetState() )
	{
	case	CS_DELAY:
		{
			fIncreaseGauge	= m_fRecoverGaugeDelay;
		}
		break;

	case	CS_RUN:
		{
			fIncreaseGauge	= m_fRecoverGaugeRun;
		}
		break;

	case	CS_DASH:
		{
			fIncreaseGauge	= m_fRecoverGaugeDash;
		}
		break;

	case	CS_WOUNDED:
		{
			fIncreaseGauge	= m_fRecoverGaugeWound;
		}
		break;

	default:
		{
			fIncreaseGauge	= 0.0f;
		}
		break;
	}

	int	iCnt	= 0;
	for( int i = 0; i < (int)m_vSummonedMinionInfoList.size(); i++ )
	{
		iCnt	+= (int)m_vSummonedMinionInfoList[i].size();
	}

	if( iCnt >= m_iMaxSummonDummyCnt )
		fIncreaseGauge *= m_fRecoverGaugeRateWhileSummon;

	if( m_fCurBullet < m_fMaxBullet )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurBullet	+= fGauge;
		m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
	}
}

void	ioPoseidonItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	bState;
	rkPacket >> bState;

	switch( bState )
	{
	case	CS_NORMAL_ATTACK:
		{
			D3DXVECTOR3	vPos;
			rkPacket >> vPos;
			rkPacket >> m_iCurCombo;

			pOwner->SetWorldPosition( vPos );
			ChangeToNormalAttack( pOwner );
		}
		break;

	case	CS_CHARGE_ATTACK:
		{
			D3DXVECTOR3	vPos;
			rkPacket >> vPos;
			rkPacket >> m_iCurCombo;

			pOwner->SetWorldPosition( vPos );
			ChangeToChargeAttack( pOwner );
		}
		break;

	case	CS_CHARGE_ATTACK_MOVE:
		{
			int	iCurKey;
			rkPacket >> iCurKey;
			rkPacket >> m_bTargetRot;

			m_CurDirKey	= (ioUserKeyInput::DirKeyInput)iCurKey;
		}
		break;

	case	CS_CHARGE_END_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			rkPacket >> m_ChargeAttackPos;
			rkPacket >> m_ChargeAttackDir;

			ChangeToChargeEndAttack( pOwner );
		}
		break;
	}
}

void	ioPoseidonItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int	iState;
	rkPacket >> iState;

	switch( iState )
	{
	case	PSS_SUMMON_SELECT:
		{
			SetSummonState( pOwner );
		}
		break;

	case	PSS_SUMMON:
		{
			int	iSummonType, iCurIdx;
			rkPacket >> iSummonType;
			rkPacket >> iCurIdx;

			SummonMinion( pOwner, iSummonType );
		}
		break;
	}
}

void	ioPoseidonItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( bFullTime )
	{
		SetSummonReadyState( pOwner );
		return;
	}

	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void	ioPoseidonItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	ioWeaponItem::SetNoneState( pOwner, eNewState );

	if( !m_szChargingEffect.IsEmpty() )
		pOwner->EndEffect( m_szChargingEffect, false );

	ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_dwChargeAttackWeaponIdx );
	ioZoneEffectWeapon*	pChargeAttackWeapon	= ToZoneEffectWeapon( pWeapon );
	if( pChargeAttackWeapon )
		pChargeAttackWeapon->SetWeaponDead();
}

void	ioPoseidonItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_fCurBullet >= m_iChargeNeedBullet )
				ChangeToChargeAttack( pOwner );
			else
				ChangeToNormalAttack( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void	ioPoseidonItem::ChangeToNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetChargingState( false );

	m_fCurChargeRate	= 0.0f;
	m_fCurChargeRateForSkill	= 0.0f;

	int	iAniID	= -1;
	const	AttackAttribute*	pAttribute	= m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		iAniID	= pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	}

	if ( iAniID == -1 )
		return;

	m_szCurAni = pAttribute->m_AttackAnimation;

	m_ChargeState	= CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonItem::ChangeToChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	float	fCurRate	= FLOAT1;
	if( m_fChargingStartAniRate > 0.0f )
		fCurRate = m_fChargingStartAniRate;

	m_fCurBullet	-= m_iChargeNeedBullet;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int	iAniID	= pGrp->GetAnimationIdx( m_szChargingStartAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, fCurRate );

	m_ChargeState	= CS_CHARGE_ATTACK;
	m_dwChargeStartTime	= FRAMEGETTIME();
	m_dwMotionEndTime	= FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID );

	FireTime	kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();
	kFireTime.iAttributeIdx	= m_AttachWeapon.m_iWeaponIdx;
	kFireTime.iResistanceIdx	= m_AttachWeapon.m_iResistanceIdx;
	kFireTime.szWoundedAni	= m_AttachWeapon.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_AttachWeapon.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni	= m_AttachWeapon.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx	= pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType	= FTT_NORMAL_ATTACK;
	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition();
	D3DXVECTOR3	vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVec3Normalize( &vDir, &vDir );

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if( !m_szChargingEffect.IsEmpty() )
		pOwner->AttachEffect( m_szChargingEffect );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonItem::ChangeToChargeAttackMove( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState	= CS_CHARGE_ATTACK_MOVE;

	float	fCurRate	= FLOAT1;
	if( m_fChargingAniRate > 0.0f )
		fCurRate = m_fChargingAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_szChargingAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
}

void	ioPoseidonItem::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void	ioPoseidonItem::ChangeToChargeEndAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !m_szChargingEffect.IsEmpty() )
		pOwner->EndEffect( m_szChargingEffect, false );

	m_ChargeState = CS_CHARGE_END_ATTACK;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_dwChargeAttackWeaponIdx );
	ioZoneEffectWeapon*	pChargeAttackWeapon	= ToZoneEffectWeapon( pWeapon );
	if( pChargeAttackWeapon )
		pChargeAttackWeapon->SetWeaponDead();

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_ChargingEndAttack.m_AttackAnimation );
	float fTimeRate  = m_ChargingEndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargingEndAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_ChargingEndAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	D3DXVECTOR3	vPos	= m_ChargeAttackPos;
	D3DXVECTOR3	vDir	= m_ChargeAttackDir * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVec3Normalize( &vDir, &vDir );
	FireTime	kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();
	kFireTime.iAttributeIdx	= m_ChargingEndAttack.m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx	= m_ChargingEndAttack.m_vWeaponInfoList[0].m_iResistanceIdx;
	kFireTime.szWoundedAni	= m_ChargingEndAttack.m_vWeaponInfoList[0].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_ChargingEndAttack.m_vWeaponInfoList[0].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni	= m_ChargingEndAttack.m_vWeaponInfoList[0].m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx	= pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType	= FTT_NORMAL_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_ChargeAttackPos;
		kPacket << m_ChargeAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonItem::ProcessChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
		ChangeToChargeAttackMove( pOwner );
}

void	ioPoseidonItem::ProcessChargeAttackMove( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_dwChargeAttackWeaponIdx );
	ioZoneEffectWeapon*	pChargeAttackWeapon	= ToZoneEffectWeapon( pWeapon );
	if( !pChargeAttackWeapon )
	{
		m_ChargeAttackPos	= m_vWeaponPos;
		m_ChargeAttackDir	= m_vWeaponMoveDir;
		ChangeToChargeEndAttack( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwChargeStartTime + m_dwChargingAttackDuration < FRAMEGETTIME() )
		{
			m_ChargeAttackPos	= pChargeAttackWeapon->GetPosition();
			m_ChargeAttackDir	= pChargeAttackWeapon->GetMoveDir();
			ChangeToChargeEndAttack( pOwner );
			return;
		}
		else if( !pOwner->IsAttackKeyDown() )
		{
			m_ChargeAttackPos	= pChargeAttackWeapon->GetPosition();
			m_ChargeAttackDir	= pChargeAttackWeapon->GetMoveDir();
			ChangeToChargeEndAttack( pOwner );
			return;
		}
	}

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	float	fDistance	= m_fChargingAttackSpeed * fTimePerSec;
	D3DXVECTOR3	vMoveAmt	= pChargeAttackWeapon->GetPosition() + ( m_vWeaponMoveDir * fDistance );
	pChargeAttackWeapon->SetPosition( vMoveAmt );
	D3DXQUATERNION	qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vWeaponMoveDir );
	pChargeAttackWeapon->SetOrientation( qtTargetRot );
	pChargeAttackWeapon->SetAttackDir( m_vWeaponMoveDir );
	pChargeAttackWeapon->SetCollisionDir( m_vWeaponMoveDir );
	m_vWeaponPos	= vMoveAmt;

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void	ioPoseidonItem::ProcessChargeEndAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_ChargeState	= CS_NONE;
		pOwner->SetState( CS_DELAY );
		//pOwner->SetSKillEndJumpState( m_fChargingAttackEndJumpPower, false, false, true, true );
		m_dwChargeAttackWeaponIdx	= 0;
	}
}

void	ioPoseidonItem::SetSummonReadyState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_vSummonNeedBullet[0] > m_fCurBullet )
		return;

	pOwner->SetState( CS_POSEIDON_SPECIAL );
	m_SpecialState	= PSS_READY;

	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)PSS_SUMMON_SELECT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonItem::SetSummonState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_POSEIDON_SPECIAL )
		pOwner->SetState( CS_POSEIDON_SPECIAL );
	m_SpecialState	= PSS_SUMMON_SELECT;
}

void	ioPoseidonItem::ProcessSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	switch( m_SpecialState )
	{
	case	PSS_READY:
		{
			SetSummonState( pOwner );
		}
		break;

	case	PSS_SUMMON_SELECT:
		{
			SetSelectMInionState( pOwner );
		}
		break;

	case	PSS_SUMMON_SELECT_WAIT:
		{
			ProcessSelectMinionWaitState( pOwner );
		}
		break;

	case	PSS_SUMMON:
		{
			if( m_dwSummonEndTime > 0 && m_dwSummonEndTime < dwCurTime )
			{
				m_dwSummonEndTime	= 0;
				pOwner->SetState( CS_DELAY );
				RemoveAllSummonEffect( pOwner );
			}
		}
		break;
	}
}

void	ioPoseidonItem::SetSelectMInionState( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;


	m_SpecialState	= PSS_SUMMON_SELECT_WAIT;

	ioPoseidonItem::SummonState	summonState	= CheckSummonState();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float	fTimeRate	= m_szSummonChargeAniInfo.m_fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate	= FLOAT1;

	int	iAniID	= pGrp->GetAnimationIdx( m_szSummonChargeAniInfo.m_AniName );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT100, FLOAT1 / fTimeRate, 0.0f );

	m_PreSummonState	= summonState;

	if( pOwner->IsNeedProcess() && COMPARE( m_PreSummonState, 0, (int)m_vSummonReadyEffectList.size() ) )
		pOwner->AttachEffect( m_vSummonReadyEffectList[m_PreSummonState] );
}

void	ioPoseidonItem::ProcessSelectMinionWaitState( ioBaseChar* pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ioPoseidonItem::SummonState	summonState	= CheckSummonState();
	if( m_PreSummonState != summonState )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		float	fTimeRate	= m_szSummonChargeAniInfo.m_fAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate	= FLOAT1;

		int	iAniID	= pGrp->GetAnimationIdx( m_szSummonChargeAniInfo.m_AniName );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT100, FLOAT1 / fTimeRate, 0.0f );

		m_PreSummonState	= summonState;
	}

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput	eDir	= pOwner->CheckCurDirKey();
			switch( eDir )
			{
			case	ioUserKeyInput::DKI_LEFT:
				{
					if( summonState >= SS_LEVEL_2 )
						SummonMinion( pOwner, (int)SS_LEVEL_2 );
					else
					{
						pOwner->SetState( CS_DELAY );
						//pOwner->RemoveBuff( m_szSummonEffectBuff );
						RemoveAllSummonEffect( pOwner );
						m_SpecialState	= PSS_NONE;
					}
				}
				break;

			case	ioUserKeyInput::DKI_UP:
				{
					if( summonState >= SS_LEVEL_1 )
						SummonMinion( pOwner, (int)SS_LEVEL_1 );
					else
					{
						pOwner->SetState( CS_DELAY );
						//pOwner->RemoveBuff( m_szSummonEffectBuff );
						RemoveAllSummonEffect( pOwner );
						m_SpecialState	= PSS_NONE;
					}
				}
				break;

			case	ioUserKeyInput::DKI_RIGHT:
				{
					if( summonState >= SS_LEVEL_MAX )
						SummonMinion( pOwner, (int)SS_LEVEL_MAX );
					else
					{
						pOwner->SetState( CS_DELAY );
						//pOwner->RemoveBuff( m_szSummonEffectBuff );
						RemoveAllSummonEffect( pOwner );
						m_SpecialState	= PSS_NONE;
					}
				}
				break;
			}
		}
		else if( pOwner->IsJumpKeyRelease() )
		{
			SummonMinion( pOwner, (int)SS_LEVEL_1 );
		}
	}
}

void	ioPoseidonItem::SummonMinion( ioBaseChar* pOwner, int iSummonType )
{
	if ( m_DummyCharList.empty() )
		return;

	if ( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	int	iDummySize	= m_DummyCharList.size();
	if( iDummySize <= iSummonType )
		return;

	if( pOwner->IsNeedProcess() && m_fCurBullet < m_vSummonNeedBullet[iSummonType] )
		return;

	pOwner->StopAutoTargetTracking();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int	iAniID = -1;
	if( !m_szSummonAniInfo.m_AniName.IsEmpty() )
		iAniID	= pGrp->GetAnimationIdx( m_szSummonAniInfo.m_AniName );

	if( iAniID != -1)
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_szSummonAniInfo.m_fAniRate, 0.0f, 0.0f, true );

		m_dwSummonEndTime	= pGrp->GetAnimationFullTime( iAniID ) * m_szSummonAniInfo.m_fAniRate + FRAMEGETTIME();
	}

	m_SpecialState	= PSS_SUMMON;

	ioDummyChar*	pDummy	= NULL;
	if( (int)m_vSummonedMinionInfoList[iSummonType].size() >= m_vMaxSummonDummyByType[iSummonType] )
	{
		std::list< int >::iterator	iter	= m_vSummonedMinionInfoList[iSummonType].begin();
		if( iter != m_vSummonedMinionInfoList[iSummonType].end() )
		{
			pDummy	= g_DummyCharMgr.FindDummyChar( pOwner, *iter );
			if ( pDummy )
			{
				pDummy->SendDieState( false );
				//m_vSummonedMinionInfoList[iSummonType].pop_front();
			}
		}
	}

	DummyCharLoadInfo	Info = m_DummyCharList[iSummonType];
	//D3DXVECTOR3	vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXQUATERNION	qtRot	= pOwner->GetWorldOrientation();
	D3DXVECTOR3	vOffset	= GetMinionOffset( iSummonType, m_vCurrentSummonIdx[iSummonType] );
	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition() + ( qtRot * vOffset );//(Info.m_fDummyCharOffset * vDir);
	
	//우선 생성 가능한 지역 체크
	float	fHeight	= pStage->GetMapHeight( vPos.x, vPos.z );
	if ( fHeight > 0 )
		vPos.y	= fHeight;

	//더미 생성!
	int	iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar*	pNewDummy = CreateDummyChar( pOwner, pStage, Info.m_DummyCharName, iCurIndex, vPos, Info.m_fDummyCharStartAngle );

	m_vCurrentSummonIdx[iSummonType]++;
	if( m_vCurrentSummonIdx[iSummonType] >= m_vMaxSummonDummyByType[iSummonType] )
		m_vCurrentSummonIdx[iSummonType]	= 0;

	m_fCurBullet	-= m_vSummonNeedBullet[iSummonType];

	//pOwner->RemoveBuff( m_szSummonEffectBuff );
	RemoveAllSummonEffect( pOwner );
	pOwner->AttachEffect( GetSummonEffectInfo( iSummonType ) );

	if ( pNewDummy && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_SpecialState;
		kPacket << iSummonType;
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

ioDummyChar* ioPoseidonItem::CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szDummyCharName, int iDummyIndex, D3DXVECTOR3 vPos, float fStartAngle/* = 0 */ )
{
	if ( m_DummyCharList.empty() )
		return NULL;

	if ( !pOwner || !pStage )
		return NULL;

	ioDummyChar*	pNewDummy	= pStage->CreateDummyChar( szDummyCharName, iDummyIndex, pOwner->GetCharName(), vPos, fStartAngle, 0 );

	return	pNewDummy;
}

void	ioPoseidonItem::FinalizeSpecialState( ioBaseChar *pOwner )
{
	RemoveAllSummonEffect( pOwner );
}

int	ioPoseidonItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int	ioPoseidonItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int	ioPoseidonItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void	ioPoseidonItem::SetCurBullet( int iCurBullet )
{
	m_fCurBullet	= (float)iCurBullet;
}

void ioPoseidonItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	if( !pOwner->IsNeedProcess() ) 
		return;
	
	ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_dwChargeAttackWeaponIdx );
	ioZoneEffectWeapon*	pChargeAttackWeapon	= ToZoneEffectWeapon( pWeapon );
	if( !pChargeAttackWeapon )
		return;

	bool	bChange	= false;
	bool	bAttackKey	= false;

	ioUserKeyInput::DirKeyInput	eNewDirKey	= pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChange	= true;
		m_bTargetRot	= false;
		m_CurDirKey	= eNewDirKey;

		if( bChange && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (BYTE)CS_CHARGE_ATTACK_MOVE;
			kPacket << (int)m_CurDirKey;
			kPacket << m_bTargetRot;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPoseidonItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_dwChargeAttackWeaponIdx );
	ioZoneEffectWeapon*	pChargeAttackWeapon	= ToZoneEffectWeapon( pWeapon );
	if( !pChargeAttackWeapon )
		return;

	if( m_fChargingAttackRotateSpeed == 0 )
		return;

	if( m_bTargetRot )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float	fFrameGap	= g_FrameTimer.GetCurFrameGap();
	float	fRate	= fFrameGap / m_fChargingAttackRotateSpeed;
	float	fRotateAngle	= 360.0f * fRate;

	D3DXVECTOR3	vCurMoveDir	= m_vWeaponMoveDir;
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	D3DXVECTOR3	vTargetDir	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float	fYawD	= 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD	= ioMath::ArrangeHead( fYawD );

		float	fYawR	= DEGtoRAD( fYawD );
		D3DXQUATERNION	qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		vTargetDir	= qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float	fDotValue	= (float)D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float	fGapAngle	= RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		m_bTargetRot	= true;

		D3DXQUATERNION	qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );

		m_vWeaponMoveDir	= qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &m_vWeaponMoveDir, &m_vWeaponMoveDir );

		return;
	}

	D3DXVECTOR3	vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		D3DXVECTOR3	vTmpAxis;

		if( vCurMoveDir != ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_Y, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}
		else
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_X, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}

		D3DXVec3Cross( &vAxis, &vTmpAxis, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vCurMoveDir, &vTargetDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}

	float	fCurRotateAngle	= fRotateAngle;
	if( fGapAngle < fRotateAngle )
		fCurRotateAngle	= fGapAngle;

	D3DXQUATERNION	qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurRotateAngle) );

	D3DXVECTOR3	vNewDir	= qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	m_vWeaponMoveDir	= vNewDir;
}

void	ioPoseidonItem::RemoveAllSummonEffect( ioBaseChar *pOwner )
{
	for( int i = 0; i < (int)m_vSummonReadyEffectList.size(); i++ )
	{
		pOwner->EndEffect( m_vSummonReadyEffectList[i] );
	}

	for( int i = 0; i < (int)m_vSummonUpEffectList.size(); i++ )
	{
		pOwner->EndEffect( m_vSummonUpEffectList[i] );
	}

	for( int i = 0; i < (int)m_vSummonLeftEffectList.size(); i++ )
	{
		pOwner->EndEffect( m_vSummonLeftEffectList[i] );
	}

	for( int i = 0; i < (int)m_vSummonRightEffectList.size(); i++ )
	{
		pOwner->EndEffect( m_vSummonRightEffectList[i] );
	}
}

ioPoseidonItem::SummonState	ioPoseidonItem::CheckSummonState()
{
	if( m_vSummonedMinionInfoList[SS_LEVEL_1].size() == m_vMaxSummonDummyByType[SS_LEVEL_1] && m_vSummonedMinionInfoList[SS_LEVEL_2].size() == m_vMaxSummonDummyByType[SS_LEVEL_2] )
		return	SS_LEVEL_MAX;
	else if( m_vSummonedMinionInfoList[SS_LEVEL_1].size() == m_vMaxSummonDummyByType[SS_LEVEL_1] )
		return	SS_LEVEL_2;

	return	SS_LEVEL_1;
}

void	ioPoseidonItem::CheckDummyCharCreate( ioBaseChar* pOwner, int iIndex )
{
	if( !pOwner )
		return;

	ioDummyChar*	pDummy	= g_DummyCharMgr.FindDummyChar( pOwner, iIndex );
	if( !pDummy )
		return;

	ioOwnerTraceWoundAniDummy*	pOwnerTraceDummy	= ToOwnerTraceWoundAniDummy( pDummy );
	if( !pOwnerTraceDummy )
		return;


	for( int i = 0; i < (int)m_vSummonedMinionInfoList.size(); i++ )
	{
		std::list< int >::iterator	iter	= m_vSummonedMinionInfoList[i].begin();
		for( ; iter != m_vSummonedMinionInfoList[i].end(); iter++ )
		{
			if( iIndex == *iter )
			{
				return;
			}
		}
	}

	for( int i = 0; i < (int)m_DummyCharList.size(); i++ )
	{
		if( pOwnerTraceDummy->GetDummyCharName() == m_DummyCharList[i].m_DummyCharName )
		{
			m_vSummonedMinionInfoList[i].push_back( iIndex );
		
			break;
		}
	}
}

void	ioPoseidonItem::CheckDummyCharDestroy( ioBaseChar* pOwner, int iIndex )
{
	if( !pOwner )
		return;

	if( m_bDummyDestroyOnReleased )
		return;

	for( int i = 0; i < (int)m_vSummonedMinionInfoList.size(); i++ )
	{
		std::list< int >::iterator	iter	= m_vSummonedMinionInfoList[i].begin();
		for( ; iter != m_vSummonedMinionInfoList[i].end(); iter++ )
		{
			if( iIndex == *iter )
			{
				m_vSummonedMinionInfoList[i].erase( iter );

				return;
			}
		}
	}
}

D3DXVECTOR3	ioPoseidonItem::GetMinionOffset( int iType, int iIdx )
{
	D3DXVECTOR3	vRtnVector;
	if( iType >= (int)m_vMinionOffsetInfoList.size() || iIdx >= (int)m_vMinionOffsetInfoList[iType].size() )
		vRtnVector	= ioMath::VEC3_ZERO;
	else
		vRtnVector	= m_vMinionOffsetInfoList[iType][iIdx];

	return	vRtnVector;
}

ioHashString	ioPoseidonItem::GetSummonEffectInfo( int iSummonType )
{
	ioHashString	effectInfo;
	SummonState	curSummonState	= CheckSummonState();

	switch( iSummonType )
	{
	case	SS_LEVEL_1:
		{
			if( COMPARE( curSummonState, 0, (int)m_vSummonUpEffectList.size() ) )
				effectInfo	= m_vSummonUpEffectList[curSummonState];			
		}
		break;

	case	SS_LEVEL_2:
		{
			if( COMPARE( curSummonState - 1, 0, (int)m_vSummonLeftEffectList.size() ) )
				effectInfo	= m_vSummonLeftEffectList[curSummonState - 1];			
		}
		break;

	case	SS_LEVEL_MAX:
		{
			if( COMPARE( curSummonState - 2, 0, (int)m_vSummonRightEffectList.size() ) )
				effectInfo	= m_vSummonRightEffectList[curSummonState - 2];			
		}
		break;
	}

	return	effectInfo;
}

//////////////////////////////////////////////////////////////////////////////////
ioPoseidonSpecialState::ioPoseidonSpecialState()
{
}

ioPoseidonSpecialState::~ioPoseidonSpecialState()
{
}

void ioPoseidonSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioPoseidonSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioPoseidonItem *pPoseidonItem = ToPoseidonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPoseidonItem )
		pPoseidonItem->FinalizeSpecialState( pOwner );
}

void ioPoseidonSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPoseidonItem *pPoseidonItem = ToPoseidonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPoseidonItem )
		pPoseidonItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioPoseidonSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}