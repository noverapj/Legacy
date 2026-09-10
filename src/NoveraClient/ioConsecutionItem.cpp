

#include "stdafx.h"

#include "ioConsecutionItem.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioConsecutionItem::ioConsecutionItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_iCurConsecutionCnt = 0;
	m_dwConsecutionCheckTime = 0;

	m_bCharged = false;

	// 2019-08-13
	m_iCurConsecutionComboCount		= 0;
	m_dwConsecutionComboCheckTime	= 0;
	m_bCurConsecutionCombo			= false;
	m_dwKeyReserveTime				= 0;	
	m_bAttackCancel					= false;
	m_bkeyReserve					= false;

	// 2019-08-30
	m_dwAttackChargeStartTime		= 0;
	m_bAttackChargeKeyDown			= false;
}


ioConsecutionItem::ioConsecutionItem( const ioConsecutionItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_ConsecutionAttribute( rhs.m_ConsecutionAttribute ),
  m_iMaxConsecutionCnt( rhs.m_iMaxConsecutionCnt ),
  m_dwConsecutionTime( rhs.m_dwConsecutionTime ),
  m_ConsecutionComboAttribute( rhs.m_ConsecutionComboAttribute),	// 2019-08-13
  m_iMaxConsecutionComboCount( rhs.m_iMaxConsecutionComboCount),	// 2019-08-13
  m_dwConsecutionComboTime(rhs.m_dwConsecutionComboTime),			// 2019-08-13
  m_HalfAttack(rhs.m_HalfAttack),									// 약 D~ 2019-08-13
  m_dwHalfChargeTime(rhs.m_dwHalfChargeTime),						// 약 D~ 2019-08-13
  m_bkeyReserve(rhs.m_bkeyReserve)									// 2019-08-20
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_iCurConsecutionCnt = 0;
	m_dwConsecutionCheckTime = 0;

	m_bCharged = false;

	// 2019-08-13
	m_iCurConsecutionComboCount		= 0;
	m_dwConsecutionComboCheckTime	= 0;
	m_bCurConsecutionCombo			= false;
	m_dwKeyReserveTime				= 0;
	m_bAttackCancel					= false;

	// 2019-08-30	
	m_dwAttackChargeStartTime		= 0;
	m_bAttackChargeKeyDown			= false;
}

ioConsecutionItem::~ioConsecutionItem()
{
}

void ioConsecutionItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//
	m_iMaxConsecutionCnt = rkLoader.LoadInt_e( "max_consecution_cnt", 0 );
	m_dwConsecutionTime = (DWORD)rkLoader.LoadInt_e( "consecution_limit_time", 0 );
	LoadAttackAttribute_e( "consecution_attack", m_ConsecutionAttribute, rkLoader );

	// 2019-08-13
	LoadAttackAttribute_e( "consecution_combo_attack", m_ConsecutionComboAttribute, rkLoader );	 
	LoadAttackAttribute( "half_attack", m_HalfAttack, rkLoader );
	m_iMaxConsecutionComboCount	= rkLoader.LoadInt_e( "max_consecution_combo_attack_count", 0 );	
	m_dwConsecutionComboTime	= (DWORD)rkLoader.LoadInt_e( "consecution_combo_attack_limit_time", 0 );	
	m_dwHalfChargeTime			= rkLoader.LoadInt_e( "half_charging_time", 0 );		// 약 D~
	m_bkeyReserve				= (bool)rkLoader.LoadBool_e( "bkeyReserve", false );	// Key_reserve true : 발동 2019-08-13
}

ioItem* ioConsecutionItem::Clone()
{
	return new ioConsecutionItem( *this );
}

ioWeaponItem::WeaponSubType ioConsecutionItem::GetSubType() const
{
	return WST_CONSECUTION;
}

void ioConsecutionItem::OnReleased( ioBaseChar *pOwner )
{
	m_iCurConsecutionCnt = 0;
	m_dwConsecutionCheckTime = 0;

	// 2019-08-13
	m_iCurConsecutionComboCount		= 0;
	m_dwConsecutionComboCheckTime	= 0;
	m_bCurConsecutionCombo			= false;

	ioAttackableItem::OnReleased( pOwner );
}

void ioConsecutionItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	// 2019-08-13
	m_dwKeyReserveTime	= 0;
	m_bAttackCancel		= false;

	// 2019-08-30
	m_dwAttackChargeStartTime = 0;	

	m_bCharged			= false;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo; 
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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioConsecutionItem::CheckNormalAttack( ioBaseChar *pOwner )
{	
	switch( m_ChargeState )
	{
		case CS_CHARGING:
		{				
			OnCharging( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
		if( m_bCharged )
			pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( m_bCharged )
			{
				if( pOwner->CheckExtendAttackEndJump() )
					return;
			}

			// 2019-08-30
			if( pOwner->IsAttackKeyRelease() )
			{
				m_dwAttackChargeStartTime = 0;
				m_bAttackChargeKeyDown	= false;
			}
			       
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;

		case CS_NORMAL_ATTACK:
		{	
			// 2019-08-20
			if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < FRAMEGETTIME() )
			{		
				CheckReserve( pOwner );		
			}

			// 2019-08-30
			if( pOwner->IsAttackKeyDown() )
			{
				if( m_dwAttackChargeStartTime <= 0 )
				{
					m_dwAttackChargeStartTime = FRAMEGETTIME() + m_dwChargeTime;					
				}
			}
			else if( pOwner->IsAttackKeyRelease() )
			{
				m_dwAttackChargeStartTime = 0;
				m_bAttackChargeKeyDown	= false;
			}

			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return; 

				// 2019-09-02
				if( true == m_bAttackCancel )
				{
					m_iCurCombo = pOwner->GetCurAttackComboNum();
					if( (m_dwAttackChargeStartTime > 0 ) && (m_dwAttackChargeStartTime < FRAMEGETTIME()) )
					{						
						m_bAttackChargeKeyDown	= true; // 사용안함
						ChangeToAttackFire( pOwner );
						g_TutorialMgr.ActionChargeAttack();						
						pOwner->InitAttackReserve();						
						return;
					}
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;

		case CS_HALF_ATTACK_FIRE: // 2019-08-13 
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

void ioConsecutionItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	// 일단 주석처리 2019-09-02
	//if( true == m_bAttackChargeKeyDown ) 
	//{
	//	pOwner->SetState(CS_DELAY);
	//	m_bAttackChargeKeyDown = false;		
	//	return;
	//}
	
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )		
		{
			ChangeToAttackFire( pOwner );
			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		// 약 D~ 2019-08-13
		if( m_dwHalfChargeTime > 0 )
		{
			if( m_dwAttackStartTime + m_dwHalfChargeTime <= FRAMEGETTIME() )
			{				
				ChangeToHalfAttackFire( pOwner );
				return;
			}
		}

		// 일반공격
		ChangeToNormalAttack( pOwner );
	}
}

void ioConsecutionItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = false;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	m_iCurConsecutionCnt = 0;


	// 2019-08-13
	m_dwKeyReserveTime	= 0;
	m_bAttackCancel		= false;

	// 2019-08-30
	m_dwAttackChargeStartTime = 0;
	m_bAttackChargeKeyDown	= false;

	// 2019-08-13
	if( pOwner->IsNeedProcess() )
	{
		if( m_dwConsecutionComboCheckTime == 0 ||
			m_dwConsecutionComboCheckTime+m_dwConsecutionComboTime < dwCurTime )
		{
			m_iCurConsecutionComboCount = 0;
			m_bCurConsecutionCombo = false;			
		}
	}

	// 2019-08-13
	if( ((0 == m_iCurCombo) && (0 == m_iCurConsecutionComboCount))	||
		((2 == m_iCurCombo) && (2 == m_iCurConsecutionComboCount))	||
		((3 == m_iCurCombo) && (4 == m_iCurConsecutionComboCount))	||
		((2 == m_iCurCombo) && (6 == m_iCurConsecutionComboCount))
	  )
	{	
		if( (2 == m_iCurCombo) && (2 == m_iCurConsecutionComboCount) )
		{
			// 2타 
			m_iCurCombo = 1;
		}
		else if( (3 == m_iCurCombo) && (4 == m_iCurConsecutionComboCount) )
		{
			// 3타 
			m_iCurCombo = 2;
		}

		++m_iCurConsecutionComboCount;
		m_dwConsecutionComboCheckTime = dwCurTime;
		m_bCurConsecutionCombo = true;
	}

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		// 2019-08-13
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( !pGrp )
			return;

		// 2019-08-13
		int iAniID = pGrp->GetAnimationIdx( m_szCurAni );
		if ( iAniID == - 1)
			return;

		// 2019-08-13
		if( m_bkeyReserve )
		{
			DWORD dwPreDelay = pAttribute->m_dwPreDelay;
			pOwner->CheckKeyReserveTime( iAniID, m_fCurAniRate, dwPreDelay );
			if( pOwner->GetCharKeyReserveEnableTime() > 0 )
			{
				m_dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();				
			}
		}
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioConsecutionItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = true;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	// 2019-08-13
	m_dwKeyReserveTime	= 0;
	m_bAttackCancel		= false;	

	// 2019-08-30
	m_dwAttackChargeStartTime = 0;

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwConsecutionCheckTime == 0 ||
			m_dwConsecutionCheckTime+m_dwConsecutionTime < dwCurTime )
		{
			m_iCurConsecutionCnt = 0;
		}

		// 2019-08-13
		if( m_dwConsecutionComboCheckTime == 0 ||
			m_dwConsecutionComboCheckTime+m_dwConsecutionComboTime < dwCurTime )
		{
			m_iCurConsecutionComboCount = 0;
			m_bCurConsecutionCombo = false;
		}
	}

	int iCurConsecutionCnt = m_iCurConsecutionCnt;
	m_dwConsecutionCheckTime = dwCurTime;

	m_iCurConsecutionCnt++;
	bool bConsecutionMax = false;
	int iExtendCnt = m_vExtendAttributeList.size();

	if( m_iMaxConsecutionCnt > 0 && m_iCurConsecutionCnt >= m_iMaxConsecutionCnt )
	{
		bConsecutionMax = true;
		m_iCurConsecutionCnt = 0;

		// 2019-08-13
		m_iCurConsecutionComboCount	= 0;
		m_bCurConsecutionCombo		= false;
	}

	// 2019-08-13
	int iCurConsecutionComboCnt = m_iCurConsecutionComboCount;
	if( true == m_bCurConsecutionCombo )
	{
		m_dwConsecutionComboCheckTime = dwCurTime;
		++m_iCurConsecutionComboCount;
	}

	// 2019-08-13
	bool bConsecutionComboMax = false;
	if( m_iMaxConsecutionComboCount > 0 && m_iCurConsecutionComboCount >= m_iMaxConsecutionComboCount )
	{
		bConsecutionComboMax		= true;
		m_iCurConsecutionCnt		= 0;
		m_iCurConsecutionComboCount	= 0;
		m_bCurConsecutionCombo		= false;
	}
	
	// 2019-08-13
	if( true == bConsecutionComboMax )
	{
		m_szCurAni = m_ConsecutionComboAttribute.m_AttackAnimation;
		m_fCurAniRate = m_ConsecutionComboAttribute.m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_ConsecutionComboAttribute );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_ConsecutionComboAttribute.m_AttackAnimation );
		float fTimeRate  = m_ConsecutionComboAttribute.m_fAttackAniRate;
		DWORD dwPreDelay = m_ConsecutionComboAttribute.m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_iCurCombo			= 2;
		iCurConsecutionCnt	= 2;

		if( m_bkeyReserve )
		{
			pOwner->CheckKeyReserveTime( iAniID, fTimeRate, dwPreDelay );
			if( pOwner->GetCharKeyReserveEnableTime() > 0 )
			{
				m_dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();
			}
		}

	}
	else if( bConsecutionMax )
	{
		m_szCurAni = m_ConsecutionAttribute.m_AttackAnimation;
		m_fCurAniRate = m_ConsecutionAttribute.m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_ConsecutionAttribute );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_ConsecutionAttribute.m_AttackAnimation );
		float fTimeRate  = m_ConsecutionAttribute.m_fAttackAniRate;
		DWORD dwPreDelay = m_ConsecutionAttribute.m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );


		// 2019-08-13
		if( m_bkeyReserve )
		{
			pOwner->CheckKeyReserveTime( iAniID, fTimeRate, dwPreDelay );
			if( pOwner->GetCharKeyReserveEnableTime() > 0 )
			{
				m_dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();
			}
		}
	}
	else if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
		m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
		float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		// 2019-08-13
		if( m_bkeyReserve )
		{
			pOwner->CheckKeyReserveTime( iAniID, fTimeRate, dwPreDelay );
			if( pOwner->GetCharKeyReserveEnableTime() > 0 )
			{
				m_dwKeyReserveTime = pOwner->GetCharKeyReserveEnableTime();
			}
		}
	}

	if( iExtendCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << iCurConsecutionCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioConsecutionItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( iState )
	{
		case CS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
			break;
		}
		case CS_ATTACK_FIRE:
		{
			rkPacket >> m_iCurConsecutionCnt;
			ChangeToAttackFire( pOwner );
			break;
		}
		case CS_HALF_ATTACK_FIRE:	// 약 D~ 2019-08-13
		{
			ChangeToHalfAttackFire( pOwner );
			break;
		}
	}
}

void ioConsecutionItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBaseChar *pOwner = pWeapon->GetOwner();

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}

	bool bInputLiveTime = false;
	DWORD dwLiveTime = 0;

	ioChargeComboJump2 *pCharComboJump2 = ToChargeComboJump2( GetCurExtendJump() );
	if( pOwner && pOwner->GetState() == CS_JUMP )
	{
		if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
		{
			if( pCharComboJump2 &&
				pCharComboJump2->GetCurChargeComboState() == ioChargeComboJump2::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboJump2->GetFlightTime() * 10;
			}
		}
	}

	ioChargeComboDefenseJump *pCharComboDefense = ToChargeComboDefenseJump( GetCurExtendJump() );
	if( pOwner && pOwner->GetState() == CS_JUMP )
	{
		if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
		{
			if( pCharComboDefense &&
				pCharComboDefense->GetCurChargeComboState() == ioChargeComboDefenseJump::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboDefense->GetFlightTime() * 10;
			}
		}
	}

	if( bInputLiveTime )
	{
		pWeapon->SetLiveTime( dwLiveTime );
	}
}

bool ioConsecutionItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioConsecutionItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !m_bCharged ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

bool ioConsecutionItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;
			
			if( m_bCharged && pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );
				
				return true;
			}
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioConsecutionItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioConsecutionItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioConsecutionItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioConsecutionItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

// 2019-08-13
void ioConsecutionItem::ChangeToHalfAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	m_dwKeyReserveTime	= 0;
	m_bAttackCancel		= false;

	// 2019-08-30
	m_dwAttackChargeStartTime = 0;
	m_bAttackChargeKeyDown	= false;

	pOwner->SetNormalAttackByAttribute( m_HalfAttack );
	m_szCurAni					= m_HalfAttack.m_AttackAnimation;
	m_fCurAniRate				= m_HalfAttack.m_fAttackAniRate;
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState				= CS_HALF_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioConsecutionItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		// 2019-08-13
		m_bAttackCancel = false;

		// 2019-08-30
		m_dwAttackChargeStartTime = 0;
		m_bAttackChargeKeyDown	= false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		// 2019-08-30
		m_bAttackChargeKeyDown	= false;

		// 2019-08-13
		if( m_bkeyReserve )
		{
			if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
			{
				pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
			}

			m_bAttackCancel = true;
		}

		m_iSkillInput = -1;
		bool bAttackKeyDown = pOwner->IsAttackKeyDown();		
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;

		// 2019-08-13
		m_bAttackCancel = false;

		// 2019-08-30
		m_dwAttackChargeStartTime = 0;
		m_bAttackChargeKeyDown	= false;

		m_iSkillInput = -1;
	}
}
