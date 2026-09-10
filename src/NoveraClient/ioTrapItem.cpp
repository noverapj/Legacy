

#include "stdafx.h"

#include "ioTrapItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDummyCharManager.h"
#include "ioDummyGradeTurret.h"

ioTrapItem::ioTrapItem()
{
	Init();
}

ioTrapItem::ioTrapItem( const ioTrapItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fTurretSetupOffset( rhs.m_fTurretSetupOffset ),
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_TrapMakeAnimation( rhs.m_TrapMakeAnimation ),
	m_fTurretDummyMakeAnimationRate( rhs.m_fTurretDummyMakeAnimationRate ),
	m_fTurretUpgradeRange( rhs.m_fTurretUpgradeRange ),
	m_nTrapWeaponType( rhs.m_nTrapWeaponType ),
	m_nStep1TrapIndex( rhs.m_nStep1TrapIndex ),
	m_nStep2TrapIndex( rhs.m_nStep2TrapIndex ),
	m_nStep3TrapIndex( rhs.m_nStep3TrapIndex ),
	m_fTrapMakeOffset( rhs.m_fTrapMakeOffset )
{
	Init();
}

ioTrapItem::~ioTrapItem()
{
}

void ioTrapItem::Init()
{	
	m_AttackState						= AS_NONE;

	m_iCurCombo							= 0;	
	m_iCurAirJump						= 0;

	m_dwAttackStartTime					= 0;
	m_dwKeyReserveTime					= 0;
	m_dwTurretDummyMakeEndTime			= 0;
	m_dwTrapMakeEndTime					= 0;

	m_fAirJumpPowerRate					= 0.0f;

}

void ioTrapItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_fNeedGauge			= rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );
	m_fTurretSetupOffset	= rkLoader.LoadFloat_e( "turret_setup_offset", 0.0f );

	int iPreCount = rkLoader.LoadInt_e( "pre_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szKey, "pre_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_PreAnimationList.push_back( ioHashString( szBuf ) );
	}

	
	//Make
	rkLoader.LoadString_e( "turret_dummy_make_animation", "", szBuf, MAX_PATH );
	m_TrapMakeAnimation = szBuf;
	m_fTurretDummyMakeAnimationRate = rkLoader.LoadFloat_e( "turret_dummy_make_animation_rate", FLOAT1 );
	m_fTurretUpgradeRange			= rkLoader.LoadFloat_e( "turret_dummy_upgrade_range", FLOAT100 );

	//////////////////////////////////////////////////////////////////////////
	m_nTrapWeaponType = rkLoader.LoadInt( "trap_weapon_type", -1);

	m_nStep1TrapIndex = rkLoader.LoadInt( "step1_trap", 0 );
	m_nStep2TrapIndex = rkLoader.LoadInt( "step2_trap", 0 );
	m_nStep3TrapIndex = rkLoader.LoadInt( "step3_trap", 0 );

	m_fTrapMakeOffset = rkLoader.LoadFloat( "trap_offset", 0 );
}

ioItem* ioTrapItem::Clone()
{
	return new ioTrapItem( *this );
}

ioWeaponItem::WeaponSubType ioTrapItem::GetSubType() const
{
	return WST_TRAP_ITEM;
}

void ioTrapItem::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );	
}

void ioTrapItem::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );
}

// 노말 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioTrapItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	int iPreGatherCnt = m_PreAnimationList.size();

	m_AttackState		= AS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int iAniID = pGrp->GetAnimationIdx( m_PreAnimationList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioTrapItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_AttackState )
	{
	case AS_CHARGING:
		OnCharging( pOwner );
		break;
	case AS_ATTACK_FIRE:
		OnNormalAttack( pOwner );
		break;
	case AS_MAKE_TRAP:
		OnTrapMake( pOwner );
		break;
	}
}

void ioTrapItem::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

// 공용 함수 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ioTrapItem::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, dwPreDelay, true );
		dwEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
		DWORD dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( 0 < dwReserve )
			dwKeyReserve = FRAMEGETTIME() + dwReserve;

		return iAniID;
	}

	return -1;
}

// 노말 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioTrapItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_AttackState = AS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( AS_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioTrapItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			//ChangeToDummyMake( pOwner->GetCreator(), pOwner, iCurIndex );
			//int iCurIndex = g_WeaponMgr.GetCurLocalIndex();
			ChangeToTrapMake( pOwner->GetCreator(), pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

// 더미/생성 파괴 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



float ioTrapItem::GetTotalTrapGauge( ioBaseChar* pOwner )
{
	DWORDVec vList;
	g_WeaponMgr.FindWeaponByAttributeType( pOwner, m_nTrapWeaponType, vList );

	float fTotalGauge = 0.0f;

	std::vector<ioWeapon*> vWeaponList;

	ioMineWeapon6* pFindWeapon = NULL;
	for( int i = 0; i < (int)vList.size(); i++ )
	{
		DWORD dwWeaponIndex = vList[i];

		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, dwWeaponIndex );

		ioMineWeapon6* pMineWeapon6 = ToMineWeapon6( pWeapon );
		if( pMineWeapon6 )
		{
			if( pMineWeapon6->GetCreateItem() != m_Name || pMineWeapon6->GetOwnerName() != GetOwnerName() )
				continue;

			if( pMineWeapon6->GetMineState() != ioMineWeapon6::MS_SILENCE_DEAD )
				fTotalGauge += pMineWeapon6->GetNeedGauge();	
		}
	}

	return fTotalGauge;
}


// 특별 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioTrapItem::ChangeToTrapMake( ioPlayStage* pStage, ioBaseChar *pOwner )
{
	DWORDVec vList;
	g_WeaponMgr.FindWeaponByAttributeType( pOwner, m_nTrapWeaponType, vList );
	
	std::vector<ioWeapon*> vWeaponList;

	ioMineWeapon6* pFindWeapon = NULL;
	D3DXVECTOR3 vSearchPos = pOwner->GetWorldPosition();
	vSearchPos += (pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 ) * m_fTrapMakeOffset);
	for( int i = 0; i < (int)vList.size(); i++ )
	{
		DWORD dwWeaponIndex = vList[i];

		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, dwWeaponIndex );

		ioMineWeapon6* pMineWeapon6 = ToMineWeapon6( pWeapon );
		if( pMineWeapon6 )
		{
			if( pMineWeapon6->GetCreateItem() != m_Name || pMineWeapon6->GetOwnerName() != GetOwnerName() )
				continue;

			D3DXVECTOR3 vDiff = pMineWeapon6->GetPosition() - vSearchPos;
			float fCurDistSq = D3DXVec3Length( &vDiff );
			if( fCurDistSq < m_fTurretUpgradeRange )
			{
				pFindWeapon = pMineWeapon6;
				break;
			}
		}
	}

	//찾았는지와 못찾았는지로 구분(찾았다면 다음 단계의 Weapon으로 대체한다 )
	if( pFindWeapon && pFindWeapon->CheckEnableUpgradeState())
	{
		//단계를 나누기
		if( pFindWeapon->GetNeedGauge() == 3 )
		{
			//kill
			//pFindWeapon->SetSilenceDeadMine( pStage, false );
			OldTrapKill( pOwner, pStage, pFindWeapon->GetUniqID() );

			//create
			CreateTrapWeapon( pOwner, m_nStep1TrapIndex, pFindWeapon->GetPosition() );

		}
		else if( pFindWeapon->GetNeedGauge() == 2 )
		{
			ioMineWeapon6* pKillWepaon = NULL;
			if( GetTotalTrapGauge( pOwner) >= m_fMaxExtraGauge )
				pKillWepaon = OldTrapKill( pOwner, pStage );

			if( pFindWeapon == pKillWepaon )
			{
				float fGauge = max( 0.0f, m_fMaxExtraGauge - GetTotalTrapGauge( pOwner ) );
				if( fGauge <= 2.0f )
					CreateTrapWeapon( pOwner, m_nStep1TrapIndex, pFindWeapon->GetPosition() );
				else
					CreateTrapWeapon( pOwner, m_nStep3TrapIndex, pFindWeapon->GetPosition() );
			}
			else
			{
				OldTrapKill( pOwner, pStage, pFindWeapon->GetUniqID() );
				CreateTrapWeapon( pOwner, m_nStep3TrapIndex, pFindWeapon->GetPosition() );
			}
			//kill
			//OldTrapKill( pOwner, pStage, pFindWeapon->GetUniqID() );

			//float fGauge = max( 0.0f, m_fMaxExtraGauge - GetTotalTrapGauge( pOwner ) );

			//if( fGauge >= 3.0f )
			////create
			//	CreateTrapWeapon( pOwner, m_nStep3TrapIndex, pFindWeapon->GetPosition() );
			//else
			//	CreateTrapWeapon( pOwner, m_nStep1TrapIndex, pFindWeapon->GetPosition() );
		}
		else
		{
			//kill
			ioMineWeapon6* pKillWepaon = NULL;
			if( GetTotalTrapGauge( pOwner) >= m_fMaxExtraGauge )
				pKillWepaon = OldTrapKill( pOwner, pStage );

			if( pFindWeapon == pKillWepaon )
			{
				float fGauge = max( 0.0f, m_fMaxExtraGauge - GetTotalTrapGauge( pOwner ) );
				if( fGauge <= FLOAT1 )
					CreateTrapWeapon( pOwner, m_nStep1TrapIndex, pFindWeapon->GetPosition() );
				else
					CreateTrapWeapon( pOwner, m_nStep2TrapIndex, pFindWeapon->GetPosition() );
			}
			else
			{
				OldTrapKill( pOwner, pStage, pFindWeapon->GetUniqID() );
				CreateTrapWeapon( pOwner, m_nStep2TrapIndex, pFindWeapon->GetPosition() );
			}
		}
	}
	else
	{
		float fHeight = pStage->GetMapHeight( vSearchPos.x, vSearchPos.z );
		if( FLOAT10 < fHeight )
		{
			vSearchPos.y = fHeight;
			CreateTrapWeapon( pOwner, m_nStep1TrapIndex, vSearchPos );
		}
	}

	if( GetTotalTrapGauge( pOwner) > m_fMaxExtraGauge )
	{
		OldTrapKill( pOwner, pStage );
	}

	m_AttackState = AS_MAKE_TRAP;

	DWORD dwKeyReserve = 0;
	SetActionAni( pOwner, m_TrapMakeAnimation, m_fTurretDummyMakeAnimationRate, 0, m_dwTrapMakeEndTime, dwKeyReserve );
}


ioMineWeapon6* ioTrapItem::OldTrapKill( ioBaseChar* pOwner, ioPlayStage* pStage, int nUniqID )
{
	DWORDVec vList;
	g_WeaponMgr.FindWeaponByAttributeType( pOwner, m_nTrapWeaponType, vList );

	std::vector<ioWeapon*> vWeaponList;

	ioMineWeapon6* pFindWeapon = NULL;
	DWORD nMinTime = 0;
	for( int i = 0; i < (int)vList.size(); i++ )
	{
		DWORD dwWeaponIndex = vList[i];

		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, dwWeaponIndex );

		ioMineWeapon6* pMineWeapon6 = ToMineWeapon6( pWeapon );
		if( pMineWeapon6 )
		{
			if( pMineWeapon6->GetCreateItem() != m_Name || pMineWeapon6->GetOwnerName() != GetOwnerName() )
				continue;

			if( nUniqID == -1 )
			{
				if( nMinTime == 0 )
				{
					nMinTime = pMineWeapon6->GetNetSyncTime();
					pFindWeapon = pMineWeapon6;
				}
				else if( nMinTime > pMineWeapon6->GetNetSyncTime() )
				{
					nMinTime = pMineWeapon6->GetNetSyncTime();
					pFindWeapon = pMineWeapon6;
				}
			}
			else
			{
				if( pMineWeapon6->GetUniqID() == nUniqID )
				{
					pMineWeapon6->SetSilenceDeadMine(pStage, false);
					pFindWeapon = pMineWeapon6;
					break;
				}
			}
		}
	}

	//SendNet
	if( pFindWeapon && pOwner->IsNeedProcess() )
	{
		pFindWeapon->SetSilenceDeadMine( pStage, false );

		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>(AS_TRAP_KILL);
		kPacket << pOwner->GetTargetRot();
		//kPacket << pOwner->GetWorldPosition();
		kPacket << pFindWeapon->GetUniqID();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	return pFindWeapon;
}


void ioTrapItem::CreateTrapWeapon( ioBaseChar* pOwner, int nWeaonIndex, D3DXVECTOR3 vPos, int nUniqID )
{
	DWORD dwCurTime = FRAMEGETTIME();
	//새로운 웨폰을 생성하자
	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	kFireTime.iAttributeIdx = nWeaonIndex;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	pOwner->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = pOwner->GetWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_EXTEND_ATTACK;

	//D3DXVECTOR3 vPos = vPos;
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	ioWeapon* pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, m_Name );
	ioMineWeapon6* pMine6 = ToMineWeapon6( pWeapon );
	if( pMine6 )
	{
		if( pOwner->IsNeedProcess() )
			pMine6->SetUniqID( g_WeaponMgr.GetCurLocalIndex() );
		else
			pMine6->SetUniqID( nUniqID );
		
		pMine6->SetNetSyncTime( FRAMEGETTIME() );
	}

	if( !pOwner->IsNeedProcess() )
	{
		m_AttackState = AS_MAKE_TRAP;

		DWORD dwKeyReserve = 0;
		SetActionAni( pOwner, m_TrapMakeAnimation, m_fTurretDummyMakeAnimationRate, 0, m_dwTrapMakeEndTime, dwKeyReserve );
	}

	//SendPacket
	if( pOwner->IsNeedProcess() && pMine6 )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>(AS_MAKE_TRAP);
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << nWeaonIndex;
		kPacket << vPos;
		kPacket << pMine6->GetUniqID();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}



void ioTrapItem::OnTrapMake( ioBaseChar *pOwner )
{
	if( m_dwTrapMakeEndTime < FRAMEGETTIME() )
	{
		m_AttackState = AS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioTrapItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		return true; 
	}

	return false;
}

bool ioTrapItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_AttackState )
		{
		case AS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioTrapItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;
	byte eState;
	rkPacket >> eState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( eState )
	{
	case AS_ATTACK_FIRE:
		{	
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case AS_MAKE_TRAP:
		{
			D3DXVECTOR3 vPos;
			int nWeaponIndex = 0;
			D3DXVECTOR3 vWeaponPos;
			int nWeaponUniqID = 0;

			rkPacket >> vPos;
			rkPacket >> nWeaponIndex;
			rkPacket >> vWeaponPos;
			rkPacket >> nWeaponUniqID;

			pOwner->SetWorldPosition( vPos );

			CreateTrapWeapon( pOwner, nWeaponIndex, vWeaponPos, nWeaponUniqID );
		}
		break;
	case AS_TRAP_KILL:
		{
			int nUniqID;
			rkPacket >> nUniqID;

			OldTrapKill( pOwner, pOwner->GetCreator(), nUniqID );
		}
		break;
	}	
}

void ioTrapItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	//m_fCurExtraGauge = max( 0.0f, m_fMaxExtraGauge - GetTotalDummyGauge( pOwner ) );
	m_fCurExtraGauge = max( 0.0f, m_fMaxExtraGauge - GetTotalTrapGauge( pOwner ) );
}

int ioTrapItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioTrapItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioTrapItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioTrapItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioTrapItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioTrapItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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