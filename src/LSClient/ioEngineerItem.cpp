

#include "stdafx.h"

#include "ioEngineerItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ioDummyCharManager.h"
#include "ioDummyGradeTurret.h"

ioEngineerItem::ioEngineerItem()
{
	Init();
}

ioEngineerItem::ioEngineerItem( const ioEngineerItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fTurretSetupOffset( rhs.m_fTurretSetupOffset ),
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_TurrretDummyInfoList( rhs.m_TurrretDummyInfoList ),
	m_TurretDummyMakeAnimation( rhs.m_TurretDummyMakeAnimation ),
	m_fTurretDummyMakeAnimationRate( rhs.m_fTurretDummyMakeAnimationRate ),
	m_fTurretUpgradeRange( rhs.m_fTurretUpgradeRange )
{
	Init();
}

ioEngineerItem::~ioEngineerItem()
{
}

void ioEngineerItem::Init()
{	
	m_AttackState						= AS_NONE;
	
	m_iCurCombo							= 0;	
	m_iCurAirJump						= 0;
	m_iRidingDummyCharIndex				= 0;

	m_dwAttackStartTime					= 0;
	m_dwKeyReserveTime					= 0;
	m_dwTurretDummyMakeEndTime			= 0;

	m_fAirJumpPowerRate					= 0.0f;
	
}

void ioEngineerItem::LoadProperty( ioINILoader &rkLoader )
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

	//Dummy
	int iDummyCount = rkLoader.LoadInt_e( "turret_dummy_cnt", 0 );
	for( int i = 0; i < iDummyCount; ++i )
	{
		GroupDummyInfo Info;

		wsprintf_e( szKey, "turret_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.szDummyName = szBuf;

		wsprintf_e( szKey, "turret_dummy%d_need_gauge", i+1 );
		Info.fNeedGauge = rkLoader.LoadFloat( szKey, 0.0f );
		m_TurrretDummyInfoList.push_back( Info );
	}

	//Make
	rkLoader.LoadString_e( "turret_dummy_make_animation", "", szBuf, MAX_PATH );
	m_TurretDummyMakeAnimation = szBuf;
	m_fTurretDummyMakeAnimationRate = rkLoader.LoadFloat_e( "turret_dummy_make_animation_rate", FLOAT1 );
	m_fTurretUpgradeRange			= rkLoader.LoadFloat_e( "turret_dummy_upgrade_range", FLOAT100 );
}

ioItem* ioEngineerItem::Clone()
{
	return new ioEngineerItem( *this );
}

ioWeaponItem::WeaponSubType ioEngineerItem::GetSubType() const
{
	return WST_ENGINEER_ITEM;
}

void ioEngineerItem::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );	
}

void ioEngineerItem::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );
}

// 노말 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioEngineerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioEngineerItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	case AS_MAKE_DUMMY:
		OnDummyMake( pOwner );
		break;
	}
}

void ioEngineerItem::OnNormalAttack( ioBaseChar *pOwner )
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

int ioEngineerItem::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve )
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

void ioEngineerItem::ChangeToAttackFire( ioBaseChar *pOwner )
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

void ioEngineerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !m_TurrretDummyInfoList.empty() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
			ChangeToDummyMake( pOwner->GetCreator(), pOwner, iCurIndex );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

// 더미/생성 파괴 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDummyGroup* ioEngineerItem::GetDummyGroup( ioBaseChar* pOwner )
{
	ioDummyGroup* pDummyGroup = g_DummyCharMgr.GetDummyGroup( pOwner->GetCharName(), GetName() );
	if( !pDummyGroup )
		pDummyGroup = g_DummyCharMgr.AddDummyGroup( pOwner->GetCharName(), GetName() );

	return pDummyGroup;
}

ioDummyChar* ioEngineerItem::CreateDummyChar( const ioHashString& szName, ioPlayStage *pStage, ioBaseChar *pOwner, int iCurIndex )
{	
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vDir * m_fTurretSetupOffset;
	float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( 0.0f < fHeight )
		vPos.y = fHeight;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurIndex );
	if( pDummy )
		pDummy->SetDieState( false );

	return pStage->CreateDummyChar( szName, iCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
}

const GroupDummyInfo* const ioEngineerItem::GetGroupDummyInfo( const ioHashString& szDummyName )
{
	for( int i = 0; i < (int)m_TurrretDummyInfoList.size(); ++i )
	{
		if( m_TurrretDummyInfoList[i].szDummyName == szDummyName )
		{	
			return &m_TurrretDummyInfoList[i];
		}
	}

	return NULL;
}

float ioEngineerItem::GetDummyNeedGauge( const ioHashString& szDummyName )
{
	const GroupDummyInfo* const pInfo = GetGroupDummyInfo( szDummyName );
	if( pInfo )
	{
		return pInfo->fNeedGauge;
	}

	return 0.0f;
}

float ioEngineerItem::GetTotalDummyGauge( ioBaseChar* pOwner )
{
	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )		
		return 0.0f;

	DummyCharList List;
	pGroup->GetDummyAll( List );

	float fTotalGauge = 0.0f;
	for( DummyCharList::iterator iter = List.begin(); iter != List.end(); ++iter )
	{
		ioDummyChar* pDummy = *iter;
		if( pDummy )
		{
			fTotalGauge += GetDummyNeedGauge( pDummy->GetDummyCharName() );
		}
	}

	return fTotalGauge;
}

void ioEngineerItem::FirstDummyName( OUT ioHashString& szDummyName )
{
	szDummyName.Clear();
	if( 0 < (int)m_TurrretDummyInfoList.size() )
	{
		szDummyName = m_TurrretDummyInfoList[0].szDummyName;
	}
}

void ioEngineerItem::NextDummyName( IN const ioHashString& szCurrDummyName, OUT ioHashString& szDummyName )
{
	szDummyName.Clear();
	for( int i = 0; i < (int)m_TurrretDummyInfoList.size(); ++i )
	{
		if( m_TurrretDummyInfoList[i].szDummyName == szCurrDummyName )
		{
			if( COMPARE( i + 1, 0, (int)m_TurrretDummyInfoList.size() ) )
			{
				szDummyName = m_TurrretDummyInfoList[ i + 1 ].szDummyName;
			}
			else
			{
				szDummyName = m_TurrretDummyInfoList[0].szDummyName;
			}
		}
	}
}

// 특별 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioEngineerItem::ChangeToDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner, int iCurIndex )
{
	ioHashString	szName;
	ioDummyChar*	pCurDummy = NULL;

	ioDummyGroup* pGroup = GetDummyGroup( pOwner );
	if( !pGroup )
		return;
	
	if( pOwner->IsNeedProcess() )
	{
		//지정된 영역안의 더미 찾기
		DummyCharList List;
		pGroup->GetDummyByArea( m_fTurretUpgradeRange, List );

		//찾은 더미 중에 가장 낮은 게이지 비용을 가진 더미 찾기
		float fMinGauge = m_fMaxExtraGauge;
		for( DummyCharList::iterator iter = List.begin(); iter != List.end(); ++iter )
		{
			ioDummyChar* pDummy = *iter;
			if( pDummy && GetDummyNeedGauge( pDummy->GetDummyCharName() ) < fMinGauge )
			{
				pCurDummy = pDummy;
			}
		}

		//업그레이드 더미 가져오기, 업그레이드 할 더미가 없으면 새로 생성할 더미 이름 가저오기
		if( pCurDummy )
		{
			NextDummyName( pCurDummy->GetDummyCharName(), szName );
		}	
		else
		{
			FirstDummyName( szName );
		}

		if( szName.IsEmpty() )
			return;

		//더미 생성
		ioDummyChar* pNewDummy = CreateDummyChar( szName, pStage, pOwner, iCurIndex );
		if( !pNewDummy )
			return;

		DummyUpgrade( pOwner, pGroup, pCurDummy, pNewDummy );

		//게이지 체크, 맥스값보다 크다면 생성한지 가장 오래된 더미 제거
		ioDummyChar* pDieDummy = NULL;
		if( GetTotalDummyGauge( pOwner ) > 5.0f )
		{
			pDieDummy = pGroup->FirstDummyDie( pNewDummy );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( AS_MAKE_DUMMY );
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();

			if( pNewDummy )
			{
				kPacket << szName;
				kPacket << pNewDummy->GetDummyCharIdx();
			}
			else
			{
				kPacket << 0;
				kPacket << "";
			}

			if( pCurDummy )
			{
				kPacket << pCurDummy->GetDummyCharIdx();
			}
			else
			{
				kPacket << 0;
			}

			if( pDieDummy )
			{
				kPacket << pDieDummy->GetDummyCharIdx();
			}
			else
			{
				kPacket << 0;
			}

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_AttackState = AS_MAKE_DUMMY;

	DWORD dwKeyReserve = 0;
	SetActionAni( pOwner, m_TurretDummyMakeAnimation, m_fTurretDummyMakeAnimationRate, 0, m_dwTurretDummyMakeEndTime, dwKeyReserve );
}

void ioEngineerItem::DummyUpgrade( ioBaseChar* pOwner, ioDummyGroup* pGroup, ioDummyChar* pCurDummy, ioDummyChar* pNewDummy )
{
	//업그레이드 할 더미 생성 후 기존 더미 제거
	if( pCurDummy )
	{
		pCurDummy->SetDieState( false );

		if( pNewDummy )
		{
			pNewDummy->SetWorldPosition( pCurDummy->GetWorldPosition() );
			pNewDummy->SetWorldOrientation( pCurDummy->GetWorldOrientation() );
			pOwner->SetTargetRotToTargetPos( pNewDummy->GetWorldPosition(), true, true );

			ioDummyGradeTurret* pCurTurret = ToDummyGradeTurret( pCurDummy );
			ioDummyGradeTurret* pNewTurret = ToDummyGradeTurret( pNewDummy );
			if( pNewTurret && pCurTurret )
				pNewTurret->SetReserveUpgradeInfo( pCurDummy->GetWorldPosition(), pCurTurret->GetAttackDir(), pCurDummy->GetWorldOrientation() );
		}
	}
	else
	{
		ioDummyGradeTurret* pNewTurret = ToDummyGradeTurret( pNewDummy );
		if( pNewTurret )
		{
			pNewTurret->SetAttackDir( pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
			pOwner->SetTargetRotToTargetPos( pNewDummy->GetWorldPosition(), true, true );
		}
	}

	//새로 생성한 더미를 더미그룹에 추가
	pGroup->AddDummy( pNewDummy->GetDummyCharIdx() );
}

void ioEngineerItem::OnDummyMake( ioBaseChar *pOwner )
{
	if( m_dwTurretDummyMakeEndTime < FRAMEGETTIME() )
	{
		m_AttackState = AS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioEngineerItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		return true; 
	}

	return false;
}

bool ioEngineerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioEngineerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case AS_MAKE_DUMMY:
		{
			D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
			rkPacket >> vPos;

			ioHashString szNewDummy;
			int iNewDummyIndex = 0;
			int iCurDummyIndex = 0;			
			int iDieDummyIndex = 0;

			rkPacket >> szNewDummy;
			rkPacket >> iNewDummyIndex;
			rkPacket >> iCurDummyIndex;
			rkPacket >> iDieDummyIndex;

			ioDummyChar* pNewDummy = CreateDummyChar( szNewDummy, pOwner->GetCreator(), pOwner, iNewDummyIndex );
			if( !pNewDummy )
			{
				LOG.PrintTimeAndLog(0, "%s Create Dummy Failed : %s", __FUNCTION__, szNewDummy.c_str() );
				return;
			}

			ioDummyChar *pCurDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurDummyIndex );
			ioDummyChar *pDieDummy = g_DummyCharMgr.FindDummyChar( pOwner, iDieDummyIndex );
			
			ioDummyGroup* pGroup = GetDummyGroup( pOwner );
			if( !pGroup )
				return;
			
			DummyUpgrade( pOwner, pGroup, pCurDummy, pNewDummy );
			if( pDieDummy )
				pDieDummy->SetDieState( true );

			ChangeToDummyMake( pOwner->GetCreator(), pOwner, 0 );
		}
		break;
	}	
}

void ioEngineerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_fCurExtraGauge = max( 0.0f, m_fMaxExtraGauge - GetTotalDummyGauge( pOwner ) );
}

int ioEngineerItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioEngineerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioEngineerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioEngineerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioEngineerItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioEngineerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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