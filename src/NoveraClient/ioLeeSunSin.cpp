

#include "stdafx.h"

#include "ioLeeSunSin.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioLeeSunSin::ioLeeSunSin()
{

	Init();
}

ioLeeSunSin::ioLeeSunSin( const ioLeeSunSin &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fNeedGaugeByMax( rhs.m_fNeedGaugeByMax ),
	m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_GatherEffect( rhs.m_GatherEffect ),
	m_GatherMaxEffect( rhs.m_GatherMaxEffect ), 
	m_ExtendAttack( rhs.m_ExtendAttack ),
	m_WoundedExtendAttack( rhs.m_WoundedExtendAttack ),
	m_AssultExtendAttack( rhs.m_AssultExtendAttack ),
	m_FullChargeExtendAttack( rhs.m_FullChargeExtendAttack ),
	m_FullChargeWoundedExtendAttack( rhs.m_FullChargeWoundedExtendAttack ),
	m_dwWoundedChargeTime( rhs.m_dwWoundedChargeTime ),
	m_dwAssultDashCount( rhs.m_dwAssultDashCount ),
	m_WoundedInfoList( rhs.m_WoundedInfoList ),
	m_WoundedExtendUseBuffList( rhs.m_WoundedExtendUseBuffList )

{
	Init();
}

ioLeeSunSin::~ioLeeSunSin()
{
}

void ioLeeSunSin::Init()
{	
	m_ChargeState				= CS_NONE;
	m_WoundedState				= WS_DELAY;
	m_GatherState				= GS_NONE;

	m_CurrDirKey				= ioUserKeyInput::DKI_NONE;

	m_iCurCombo					= 0;	
	m_iCurAirJump				= 0;

	m_dwWoundedStartChargeTime	= 0;
	m_dwWoundedPreEndTime		= 0;
	m_dwAttackStartTime			= 0;
	m_dwKeyReserveTime			= 0;
	m_dwExtendAttackEnd			= 0;
	m_dwAssultDashCurrCount		= 0;

	m_bCurrMaxEffect			= false;

	m_fAirJumpPowerRate			= 0.0f;
}

void ioLeeSunSin::EndEffect( ioBaseChar* pOwner )
{
	if( m_GatherEffectID != -1 )
	{
		pOwner->EndEffect( m_GatherEffectID );
		m_GatherEffectID = -1;
	}
}

void ioLeeSunSin::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fNeedGauge		= rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );
	m_fNeedGaugeByMax	= rkLoader.LoadFloat_e( "need_extra_gauge_by_max", 0.0f );

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;

	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;

	rkLoader.LoadString_e( "max_gather_effect", "", szBuf, MAX_PATH );
	m_GatherMaxEffect = szBuf;
	
	LoadAttackAttribute( "assult_extend_attack", m_AssultExtendAttack, rkLoader );
	LoadAttackAttribute( "extend_attack", m_ExtendAttack, rkLoader );
	LoadAttackAttribute( "wounded_extend_attack", m_WoundedExtendAttack, rkLoader );
	LoadAttackAttribute( "full_extend_attack", m_FullChargeExtendAttack, rkLoader );
	LoadAttackAttribute( "full_wounded_extend_attack", m_FullChargeWoundedExtendAttack, rkLoader );

	m_dwAssultDashCount = rkLoader.LoadInt_e( "assult_extend_limit_count", 1 );

	LoadWoundedInfo( rkLoader );
	LoadBuffProperty( rkLoader );
}

void ioLeeSunSin::LoadWoundedInfo( IN ioINILoader &rkLoader )
{
	m_dwWoundedChargeTime = rkLoader.LoadInt_e( "blow_charging_time", 100 );

	char szBuf[MAX_PATH];	
	char szKey[MAX_PATH];
	int iCnt = rkLoader.LoadInt( "wounded_extend_max", 0 );

	for( int i = 0; i < iCnt ; ++i )
	{
		WoundedInfo Info;
		wsprintf_e( szKey, "wounded_extend%d_target_blow_type", i+1 );
		Info.eBlowType	= (BlowTargetType)rkLoader.LoadInt( szKey, BTT_NONE );

		wsprintf_e( szKey, "wounded_extend%d_min_height", i+1 );
		Info.fMaxHeight	= rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "wounded_extend%d_max_height", i+1 );
		Info.fMaxHeight	= rkLoader.LoadFloat( szKey, 5.0f );

		wsprintf_e( szKey, "wounded_extend%d_branch_animation", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.BranchAnimation = szBuf;

		wsprintf_e( szKey, "wounded_extend%d_branch_animation_rate", i+1 );
		Info.fAnimationRate	= rkLoader.LoadFloat( szKey, FLOAT1 );

		m_WoundedInfoList.push_back( Info );
	}
}

void ioLeeSunSin::LoadBuffProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "wounded_extend_use_buff_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "wounded_extend_use_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WoundedExtendUseBuffList.push_back( szBuf );
	}
}

ioItem* ioLeeSunSin::Clone()
{
	return new ioLeeSunSin( *this );
}

ioWeaponItem::WeaponSubType ioLeeSunSin::GetSubType() const
{
	return WST_LEESUNSIN_ITEM;
}

void ioLeeSunSin::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );	
}

void ioLeeSunSin::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );

	EndEffect( pOwner );
}

// 노말 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void ioLeeSunSin::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
}

void ioLeeSunSin::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
	case CS_EXTEND_FIRE:
		OnNormalAttack( pOwner );
		break;
	case CS_ASSULT_FIRE:
		OnAsulltAttack( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	}
}

void ioLeeSunSin::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

// 노말 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
{
	if( !pAttribute )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->SetNormalAttackByAttribute( *pAttribute );

	m_szCurAni					= pAttribute->m_AttackAnimation;
	m_fCurAniRate				= pAttribute->m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;

	ioEntityGroup* pGrp	= pOwner->GetGroup();
	int iAniID			= pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate		= pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay	= pAttribute->m_dwPreDelay;	
	dwAttackEnd			= dwCurTime;
	dwAttackEnd			+= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( dwReserve > 0 )
		dwReserve += dwCurTime;

	if( bAutoTarget )
	{
		DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	if( bExtra )
	{
		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
	}
}

void ioLeeSunSin::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_GatherState	= GS_NONE;
	DWORD dwCurTime	= FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	ClearKeyReserve();

	DWORD dwEndTime		= 0;
	DWORD dwKeyRserve	= 0;
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	SetAttack( pOwner, pAttribute, false, true, dwEndTime, dwKeyRserve );

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{	
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

// 특별 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	EndEffect( pOwner );

	m_bCurrMaxEffect = false;
	 ioEffect* pEffect = pOwner->AttachEffect( m_GatherEffect );
	 if( pEffect )
	{
		m_GatherEffectID = pEffect->GetUniqueID();
	 }

	m_ChargeState	= CS_GATHERING;
	m_GatherState	= GS_GATHER;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GATHERING );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLeeSunSin::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE && m_CurrDirKey != pOwner->CheckCurDirKey() )
	{
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		m_CurrDirKey = pOwner->CheckCurDirKey();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( SS_GATHERING_DIR );
			kPacket << static_cast<byte>( m_CurrDirKey );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( pOwner->IsAttackKeyDown() )
		{
			m_GatherState = GS_GATHER;
		}

		if( !pOwner->IsAttackKeyDown() )
		{
			if( GetNeedBullet() > GetCurBullet() )
			{
				ChangeToAttackFire( pOwner );
			}
			else
			{
				ChangeToExtendAttackFire( pOwner, ET_NORMAL );
			}
		}
		else if( pOwner->IsJumpKeyDown() )
		{
			ChangeToGatherEndByJump( pOwner );
		}
		else if( pOwner->IsDefenseKeyDown() )
		{
			ChangeToGatherEndBySkill( pOwner );
		}
		else if( pOwner->IsDirKeyDoubleClick() )
		{
			if( COMPARE( m_dwAssultDashCurrCount, 0, m_dwAssultDashCount ) )
			{	
				ChangeToAssultAttackFire( pOwner );
			}
		}
	}
}

void ioLeeSunSin::ChangeToGatherEndByJump( ioBaseChar *pOwner )
{
	SetNoneState( pOwner );
	pOwner->SetJumpState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GATHERING_END_BY_JUMP );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLeeSunSin::ChangeToGatherEndBySkill( ioBaseChar *pOwner )
{
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( 0, SUT_NORMAL ) )
	{
		SetNoneState( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GATHERING_END_BY_SKILL );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLeeSunSin::ChangeToGatherEndByRun( ioBaseChar *pOwner )
{
	SetNoneState( pOwner );	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GATHERING_END_BY_RUN );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLeeSunSin::ChangeToAssultAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->SetState( CS_ATTACK );
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	ClearKeyReserve();

	EndEffect( pOwner );
	pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	SetAttack( pOwner, &m_AssultExtendAttack, true, true, m_dwExtendAttackEnd, m_dwKeyReserveTime );
	m_ChargeState = CS_ASSULT_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_ASSULT_ATTACK );		
		kPacket << pOwner->GetTargetRot();
		kPacket << (byte)pOwner->CheckCurDirKey();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLeeSunSin::OnAsulltAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( 0 < m_dwExtendAttackEnd && m_dwExtendAttackEnd < FRAMEGETTIME() )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			m_dwAssultDashCurrCount++;
			ChangeToGathering( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}
	}
}

//특별 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::ChangeToExtendAttackFire( ioBaseChar *pOwner, ExtendType eType )
{
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurExtraGauge;
		kPacket << (byte)eType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( GetCurBullet() >= GetMaxBullet() )
	{		
		switch( eType )
		{
		case ET_NORMAL:
			{
				SetUseExtraGauge( GetNeedGaugeByMax() );
				ChangeToExtendAttackFire( pOwner, &m_FullChargeExtendAttack );
			}
			break;
		case ET_WOUNDED:
			{
				SetUseExtraGauge( GetNeedGaugeByMax() );
				ChangeToExtendAttackFire( pOwner, &m_FullChargeWoundedExtendAttack );
				OnWoundedExtensUseAddBuff( pOwner );
			}
			break;
		}
	}
	else
	{
		switch( eType )
		{
		case ET_NORMAL:
			{
				SetUseExtraGauge( GetNeedGaugeByMax() );
				ChangeToExtendAttackFire( pOwner, &m_ExtendAttack );
			}
			break;
		case ET_WOUNDED:
			{
				SetUseExtraGauge( GetNeedGaugeByMax() );
				ChangeToExtendAttackFire( pOwner, &m_WoundedExtendAttack );
				OnWoundedExtensUseAddBuff( pOwner );
			}
			break;
		}
	}

	EndEffect( pOwner );
}

void ioLeeSunSin::ChangeToExtendAttackFire( ioBaseChar *pOwner, AttackAttribute* pAttribute )
{
	if( !pOwner )
		return;

	if( !pAttribute )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->SetState( CS_ATTACK );
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	ClearKeyReserve();

	SetAttack( pOwner, pAttribute, true, true, m_dwExtendAttackEnd, m_dwKeyReserveTime );
	m_ChargeState = CS_EXTEND_FIRE;
}

//피격 중 특별 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioLeeSunSin::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->IsCatchMode() )
		return false;
	
	//D꾹 중에 피격 상태로 넘어가면 D키를 한번 때고 다시 D를 눌러야 피격 특별이 발동
	if( m_GatherState == GS_GATHER )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			return false;
		}
		else
		{
			m_GatherState = GS_NONE;
		}
	}

	if( pOwner->GetState() != CS_WOUNDED && pOwner->GetState() != CS_BLOW_WOUNDED && pOwner->GetState() != CS_BOUND_BLOW_WOUNDED && pOwner->GetState() != CS_BLOW_EXTEND_WOUND )
		return false;

	if( GetNeedBullet() > GetCurBullet() )
		return false;

	switch( m_WoundedState )
	{
	case WS_DELAY:
		{
			if( pOwner->IsAttackKeyDown() )
			{
				CheckWoundExtendUseDelay( pOwner, eType );
			}
		}
		break;
	case WS_TIME_CONDITION_CHECK:
		{
			if( m_dwWoundedStartChargeTime + m_dwWoundedChargeTime < FRAMEGETTIME() )
			{
				if( !pOwner->IsAttackKeyDown() )
				{
					if( !CheckWoundExtendUsePre( pOwner, eType ) )
					{
						m_dwWoundedStartChargeTime	= 0;
						m_WoundedState				= WS_DELAY;
						return false;
					}
				}				
			}
			else
			{
				if( !pOwner->IsAttackKeyDown() )
				{
					m_dwWoundedStartChargeTime	= 0;
					m_WoundedState				= WS_DELAY;
					return false;
				}
			}
		}
		break;
	case WS_PRE:
		{
			if( m_dwWoundedPreEndTime < FRAMEGETTIME() )
			{
				m_WoundedState = WS_ACTION;
			}
		}
		break;
	case WS_ACTION:
		{
			ChangeToExtendAttackFire( pOwner, ET_WOUNDED );
		}
		break;
	}

	return false;
}

void ioLeeSunSin::CheckWoundExtendUseDelay( ioBaseChar* pOwner, BlowTargetType eType )
{
	m_dwWoundedStartChargeTime	= FRAMEGETTIME();
	m_WoundedState				= WS_TIME_CONDITION_CHECK;
}

void ioLeeSunSin::ChangeToWoundedPreState( ioBaseChar* pOwner, const ioHashString& szAnmationName, float fTimeRate )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	int iAniID = pGrp->GetAnimationIdx( szAnmationName );
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );
	m_dwWoundedPreEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_WOUNDED_EXTEND_PRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << szAnmationName;
		kPacket << fTimeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioLeeSunSin::CheckWoundExtendUsePre( ioBaseChar* pOwner, BlowTargetType eType )
{
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	for( WoundedInfoList::iterator iter = m_WoundedInfoList.begin(); iter != m_WoundedInfoList.end(); ++iter )
	{
		const WoundedInfo& Info = *iter;
		if( Info.eBlowType == eType && COMPARE( fHeightGap, Info.fMinHeight, Info.fMaxHeight ) )
		{
			BlowWoundedState eBlowState = pOwner->GetBlowWoundedState();				
			if( eBlowState == BW_BLOW_ENDED && !Info.BranchAnimation.IsEmpty() )
			{
				m_WoundedState = WS_PRE;
				ChangeToWoundedPreState( pOwner, Info.BranchAnimation, Info.fAnimationRate );
			}			
			else
			{
				m_WoundedState = WS_ACTION;
			}

			return true;
		}
	}

	return false;
}

void ioLeeSunSin::OnWoundedExtensUseAddBuff( ioBaseChar* pOwner )
{
	for( int i = 0; i < (int)m_WoundedExtendUseBuffList.size(); ++i )
		pOwner->AddNewBuff( m_WoundedExtendUseBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

//피격 후 상태 초기화 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetState() != CS_WOUNDED && pOwner->GetState() != CS_BLOW_WOUNDED && pOwner->GetState() != CS_BOUND_BLOW_WOUNDED && pOwner->GetState() != CS_BLOW_EXTEND_WOUND )
	{
		m_WoundedState	= WS_DELAY;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	float fGauge = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_LEESUNSIN )
		{
			fGauge = m_fExtraGaugeAttack;
		}
		else
		{
			fGauge = 0.0f;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_LEESUNSIN )
		{
			fGauge = m_fExtraGaugeAttackDef;
		}
		else
		{
			fGauge = 0.0f;
		}
		break;
	case EGT_WOUNDED:
		{
			fGauge = m_fExtraGaugeWound;
		}
		break;
	case EGT_WOUNDED_DEF:		
		{
			fGauge = m_fExtraGaugeWoundDef;
		}
		break;		
	}

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	EndEffect( pOwner );

	m_dwWoundedStartChargeTime	= 0;
	m_dwExtendAttackEnd			= 0;
	m_dwAssultDashCurrCount		= 0;

	m_ChargeState	= CS_NONE;
	m_WoundedState	= WS_DELAY;

	if( eNewState != CS_WOUNDED && eNewState != CS_BLOW_WOUNDED && eNewState != CS_BOUND_BLOW_WOUNDED && eNewState != CS_BLOW_EXTEND_WOUND )
	{
		m_GatherState = GS_NONE;
	}	
}

bool ioLeeSunSin::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		return true; 
	}

	return false;
}

bool ioLeeSunSin::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:		
		case CS_GATHERING:
			if( !bCheck )
				return false;

			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioLeeSunSin::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( eState )
	{
	case SS_ATTACK_FIRE:
		{	
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case SS_GATHERING:
		{
			ChangeToGathering( pOwner );
		}
		break;
	case SS_GATHERING_DIR:
		{
			byte DirKey;
			rkPacket >>  DirKey;
			m_CurrDirKey = (ioUserKeyInput::DirKeyInput)DirKey;
			pOwner->SetTargetRotToDirKey( m_CurrDirKey );
		}
		break;
	case SS_GATHERING_END_BY_JUMP:
		{
			ChangeToGatherEndByJump( pOwner );
		}
		break;
	case SS_GATHERING_END_BY_SKILL:
		{
			ChangeToGatherEndBySkill( pOwner );
		}
		break;
	case SS_GATHERING_END_BY_RUN:
		{
			ChangeToGatherEndByRun( pOwner );
		}
		break;
	case SS_WOUNDED_EXTEND_PRE:
		{
			ioHashString szAnmationName;
			float fTimeRate;
			rkPacket >> szAnmationName;
			rkPacket >> fTimeRate;

			m_WoundedState = WS_PRE;
			ChangeToWoundedPreState( pOwner, szAnmationName, fTimeRate );
		}
		break;
	case SS_EXTEND_ATTACK:
		{
			rkPacket >> m_fCurExtraGauge;
			byte eType;
			rkPacket >> eType;
			ChangeToExtendAttackFire( pOwner, (ExtendType)eType );
		}
		break;
	case SS_ASSULT_ATTACK:
		{
			byte eDirKey;
			rkPacket >> eDirKey;
			pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)eDirKey );
			ChangeToAssultAttackFire( pOwner );
		}
		break;
	}	
}

void ioLeeSunSin::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();		
	switch( m_ChargeState )
	{
	case CS_GATHERING:
	case CS_ASSULT_FIRE:
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}

	UpdateEffect( pOwner );
}

void ioLeeSunSin::UpdateEffect( ioBaseChar *pOwner )
{
	if( GetCurExtraGauge() == GetMaxBullet() )
	{
		if( !m_bCurrMaxEffect )
		{
			EndEffect( pOwner );
			ioEffect* pEffect = pOwner->AttachEffect( m_GatherMaxEffect );
			if( pEffect )
			{
				m_GatherEffectID = pEffect->GetUniqueID();
				m_bCurrMaxEffect = true;
			}
		}
	}
	else
	{
		if( m_bCurrMaxEffect )
		{
			EndEffect( pOwner );
			m_bCurrMaxEffect = false;
		}
	}
}

int ioLeeSunSin::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioLeeSunSin::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioLeeSunSin::GetNeedGaugeByMax()
{
	return (int)m_fNeedGaugeByMax;
}

int ioLeeSunSin::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioLeeSunSin::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioLeeSunSin::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioLeeSunSin::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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