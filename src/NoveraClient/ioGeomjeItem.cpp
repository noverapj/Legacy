

#include "stdafx.h"

#include "ioGeomjeItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioPowerChargeDash.h"

#include "WeaponDefine.h"
#include "FindPredicateImpl.h"

ioGeomjeItem::ioGeomjeItem()
{
	Init();
}

ioGeomjeItem::ioGeomjeItem( const ioGeomjeItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_ExtendAttribute( rhs.m_ExtendAttribute ), 
	m_SwordAddChangeAttribute( rhs.m_SwordAddChangeAttribute ),	
	m_NormalAttackInfoList( rhs.m_NormalAttackInfoList ),
	m_DashAttackInfoList( rhs.m_DashAttackInfoList ),
	m_DashExtraAttackInfoList( rhs.m_DashExtraAttackInfoList ),
	m_DashExtendAttackInfoList( rhs.m_DashExtendAttackInfoList ),
	m_JumpAttackInfoList( rhs.m_JumpAttackInfoList ),
	m_JumpExtendAttackInfoList( rhs.m_JumpExtendAttackInfoList ),
	m_JumpDashAttackInfoList( rhs.m_JumpDashAttackInfoList ),
	m_SwordExtendAttribute( rhs.m_SwordExtendAttribute ),
	m_SwordExtendAttributeOffsetList( rhs.m_SwordExtendAttributeOffsetList ),
	m_dwSwordStateChangeTime( rhs.m_dwSwordStateChangeTime ),
	m_SwordDummyBuff( rhs.m_SwordDummyBuff ),
	m_SwordTargetDummy( rhs.m_SwordTargetDummy ),
	m_SwordControlAttributeList( rhs.m_SwordControlAttributeList ),
	m_iSwordDummyDefaultAttackCode( rhs.m_iSwordDummyDefaultAttackCode ),
	m_fSwordAttackNeedGauge( rhs.m_fSwordAttackNeedGauge ),
	m_SwordEffect( rhs.m_SwordEffect ),
	m_SwordEndEffect( rhs.m_SwordEndEffect )
{
	Init();
}

ioGeomjeItem::~ioGeomjeItem()
{
}

void ioGeomjeItem::Init()
{	
	m_AttackState					= AS_NONE;
	m_SwordChangeState				= SCS_NONE;
	m_CurrFireTimeType				= FTT_NORMAL_ATTACK,

	m_iCurCombo						= 0;
	m_iCurSwordControlCombo			= 0;
	m_iCurAirJump					= 0;

	m_dwBaseWeaponIndex				= 0;
	m_dwAttackStartTime				= 0;
	m_dwCurrAttackStartTime			= 0;
	m_dwKeyReserveTime				= 0;
	m_fAirJumpPowerRate				= 0.0f;

	m_dwSwordStateChangeStartTime	= 0;
	m_dwSwordAddChangeEndTime		= 0;
	m_dwDummyAttackEndTime			= 0;
	m_dwClearStartTime				= 0;

	m_SwordEffectID					= -1;
	m_SwordEndEffectID				= -1;

	m_CurrAttackInfoList.clear();
}

void ioGeomjeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );

	LoadAnimation( "pre_animation", rkLoader, m_PreAnimationList );
	LoadAttackAttribute( "extend_attack", m_ExtendAttribute, rkLoader );
	LoadAttackAttribute( "sword_change", m_SwordAddChangeAttribute, rkLoader );

	//이기어검 추가 발사체
	LoadAttrbute( "normal_swrord", m_NormalAttackInfoList, rkLoader );
	LoadAttrbute( "dash_swrord", m_DashAttackInfoList, rkLoader );
	LoadAttrbute( "dash_extra_swrord", m_DashExtraAttackInfoList, rkLoader );
	LoadAttrbute( "dash_extend_swrord", m_DashExtendAttackInfoList, rkLoader );
	LoadAttrbute( "jump_swrord", m_JumpAttackInfoList, rkLoader );
	LoadAttrbute( "jump_extend_swrord", m_JumpExtendAttackInfoList, rkLoader );
	LoadAttrbute( "jump_dash_swrord", m_JumpDashAttackInfoList, rkLoader );

	//이기어검 변신 딜레이
	m_dwSwordStateChangeTime = rkLoader.LoadInt_e( "sword_change_delay_time", 500 );

	//이기어검 후 특별
	LoadAttackAttribute( "sword_extend_attack", m_SwordExtendAttribute, rkLoader );
	int iOffsetMax = rkLoader.LoadInt_e( "sword_extend_attack_type_count", 0 );
	for( int i = 0; i < iOffsetMax; ++i )
	{
		OffsetProperty Offset;
		wsprintf_e( szBuf, "%s%d", "sword_extend_attack_type", i + 1 );
		Offset.m_dwAttributeIndex = rkLoader.LoadInt( szBuf, 500 );
		LoadOffsetProperty( szBuf, Offset, rkLoader );
		m_SwordExtendAttributeOffsetList.push_back( Offset );
	}

	rkLoader.LoadString( "sword_dummy_buff", "", szBuf, MAX_PATH );
	m_SwordDummyBuff = szBuf;
	rkLoader.LoadString( "sword_dummy_name", "", szBuf, MAX_PATH );
	m_SwordTargetDummy = szBuf;
	
	int iMax = rkLoader.LoadInt_e( "sword_control_max", 0 );
	for( int i = 0; i < iMax; ++i )
	{
		AttackAttribute Attack;	
		wsprintf_e( szBuf, "sword_control%d", i + 1 );
		LoadAttackAttribute( szBuf, Attack, rkLoader );
		m_SwordControlAttributeList.push_back( Attack );
	}

	m_iSwordDummyDefaultAttackCode	= rkLoader.LoadInt_e( "sword_dummy_default_attack_code", 500 );
	m_fSwordAttackNeedGauge			= rkLoader.LoadFloat_e( "sword_dummy_attack_need_gauge", 0.0f );

	//이기어검 후 이펙트	
	rkLoader.LoadString( "sword_effect", "", szBuf, MAX_PATH );
	m_SwordEffect = szBuf;

	rkLoader.LoadString( "sword_end_effect", "", szBuf, MAX_PATH );
	m_SwordEndEffect = szBuf;
}

void ioGeomjeItem::LoadAnimation( const char* szTitle, ioINILoader &rkLoader, ioHashStringVec& List )
{
	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_cnt", szTitle );
	int iCount = rkLoader.LoadInt( szKey , 0 );

	for( int i = 0; i < iCount; ++i )
	{
		wsprintf_e( szKey, "%s%d", szTitle, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		List.push_back( ioHashString( szBuf ) );
	}
}

void ioGeomjeItem::LoadAttrbute( const char* szTitle, AttackInfoList& List, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_max_count", szTitle );
	int iMax = rkLoader.LoadInt( szKey, 0 );

	for( int i = 0; i < iMax; ++i )
	{
		AttackInfo Info;
		wsprintf_e( szKey, "%s%d_element", szTitle, i + 1 );		
		LoadAttrbuteProperty( szKey, Info.m_AttackInfoPropertyList, rkLoader );
		List.push_back( Info );
	}
}

void ioGeomjeItem::LoadAttrbuteProperty( const char* szTitle, AttackInfoPropertyList& List, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_max_count", szTitle );
	int iAttributeMax = rkLoader.LoadInt( szKey, 0 );

	for( int i = 0; i < iAttributeMax; ++i )
	{
		AttackInfoProperty Info;
		wsprintf_e( szKey, "%s%d_type", szTitle, i + 1 );
		Info.m_dwAttributeIndex = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s%d_fire_time", szTitle, i + 1 );
		Info.m_dwFireTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s%d", szTitle, i + 1 );
		LoadOffsetProperty( szKey, Info.m_OffsetProperty, rkLoader );

		wsprintf_e( szKey, "%s%d_need_gauge", szTitle, i + 1 );
		Info.m_fNeedGauge = rkLoader.LoadFloat( szKey, 0.0f );

		List.push_back( Info );
	}
}

void ioGeomjeItem::LoadOffsetProperty( const char* szTitle, OffsetProperty& Offset, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_offset_x", szTitle );
	Offset.m_vOffset.x = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_offset_y", szTitle );
	Offset.m_vOffset.y = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_offset_z", szTitle );
	Offset.m_vOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_vert", szTitle );
	Offset.m_fVert = rkLoader.LoadFloat( szKey, 0.0f );

	wsprintf_e( szKey, "%s_horz", szTitle );
	Offset.m_fHorz = rkLoader.LoadFloat( szKey, 0.0f );
}

ioItem* ioGeomjeItem::Clone()
{
	return new ioGeomjeItem( *this );
}

ioWeaponItem::WeaponSubType ioGeomjeItem::GetSubType() const
{
	return WST_GEOMJE_ITEM;
}

void ioGeomjeItem::OnEquiped( ioBaseChar *pOwner )
{
	DestroyEffect( pOwner );
	DestroyEndEffect( pOwner );
	Init();
	ioAttackableItem::OnEquiped( pOwner );
}

void ioGeomjeItem::OnReleased( ioBaseChar *pOwner )
{	
	DestroyEffect( pOwner );
	DestroyEndEffect( pOwner );
	Init();
	ioAttackableItem::OnReleased( pOwner );
}

// 노말 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_CurrAttackInfoList.clear();
	ClearCancelInfo();

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	int iPreGatherCnt = m_PreAnimationList.size();

	m_AttackState		= AS_CHARGING;
	m_iCurCombo			= iCurCombo;
	m_dwAttackStartTime	= FRAMEGETTIME();

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

void ioGeomjeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_AttackState )
	{
	case AS_CHARGING:
		OnCharging( pOwner );
		break;
	case AS_ATTACK_FIRE:
	case AS_EXTEND_ATTACK_FIRE:
		OnNormalAttack( pOwner );
		break;
	case AS_DUMMY_ATTACK:
		OnSwordExtend( pOwner );
		break;
	}
}

void ioGeomjeItem::OnNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );
		if( ProcessCancel(pOwner) )
			return;
	}

	ProcessAttackFireList( pOwner, FTT_NORMAL_ATTACK );
	ioWeaponItem::CheckNormalAttack( pOwner );
}

// 공용 함수 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::SetBaseAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
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

void ioGeomjeItem::SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
{
	if( !pAttribute )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate  = pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute->m_dwPreDelay;
	DWORD dwEndDelay = pAttribute->m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, pAttribute->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	dwAttackEnd = dwCurTime;
	dwAttackEnd += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;			

	dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( dwReserve > 0 )
		dwReserve += dwCurTime;
}

int ioGeomjeItem::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve )
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

ioWeapon* ioGeomjeItem::AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex, FireTimeType eType )
{
	if ( !pOwner )
		return NULL;

	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= dwAttackIndex;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= dwWeaponIndex;
	kFireTime.eFireTimeType			= eType;

	return pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
}

void ioGeomjeItem::ProcessAttackFireList( ioBaseChar *pOwner, FireTimeType eType )
{
	int iWeaponBaseIndex = 0;
	AttackInfoPropertyList::const_iterator iter = m_CurrAttackInfoList.begin();
	while( iter != m_CurrAttackInfoList.end() )
	{
		const AttackInfoProperty& Info = *iter;
		if( m_dwCurrAttackStartTime + Info.m_dwFireTime < FRAMEGETTIME() )
		{
			SetUseExtraGauge( Info.m_fNeedGauge );

			D3DXQUATERNION qtRot	= ioMath::QUAT_IDENTITY;
			D3DXVECTOR3 vPos		= pOwner->GetWorldPosition();
			D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			ioPlayEntityHelp::MoveOffset( Info.m_OffsetProperty.m_vOffset, pOwner->GetTargetRot(), vPos );			
			ioPlayEntityHelp::RotateMoveDir( Info.m_OffsetProperty.m_fHorz, Info.m_OffsetProperty.m_fVert, vDir, qtRot );

			ioWeapon* pFireWeapon = AttackFire( pOwner, Info.m_dwAttributeIndex, vPos, vDir, m_dwBaseWeaponIndex + iWeaponBaseIndex++, eType );
			if( pFireWeapon )
				pFireWeapon->SetOrientation( qtRot );

			iter = m_CurrAttackInfoList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

// 노말 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	DWORD dwCurTime			= FRAMEGETTIME();
	m_dwCurrAttackStartTime	= dwCurTime;

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	ClearSwordControl( pOwner );

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

	if( m_SwordChangeState == SCS_SWORD && COMPARE( m_iCurCombo, 0, (int)m_NormalAttackInfoList.size() ) )
	{		
		m_CurrAttackInfoList	= m_NormalAttackInfoList[m_iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ASA_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		kPacket << m_dwBaseWeaponIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_SwordChangeState == SCS_NONE )
			{
				ChangeToExtendAttackFire( pOwner );
			}
			else
			{
				if( GetSwordAttackNeedBullet() <= GetCurBullet() )
				{
					ChangeToExtendAttackFire( pOwner );
				}
				else
				{
					ChangeToAttackFire( pOwner );
				}
			}
		}
	}
	else
	{
		if( CheckToSwordExtend( pOwner ) )
		{
			ChangeToSwordExtend( pOwner );
		}
		else
		{
			if( GetCurBullet() <= 0 && m_SwordChangeState == SCS_SWORD )
			{
				ChangeToSwordEnd( pOwner );
			}
			ChangeToAttackFire( pOwner );
		}		
	}
}

void ioGeomjeItem::ChangeToExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	ClearKeyReserve();

	DWORD dwEndTime = 0;
	if( m_SwordChangeState == SCS_NONE )
	{
		SetBaseAttack( pOwner, &m_ExtendAttribute, true, true, dwEndTime, m_dwKeyReserveTime );
	}
	else
	{
		SetUseExtraGauge( GetSwordAttackNeedBullet() );
		SetBaseAttack( pOwner, &m_SwordExtendAttribute, true, true, dwEndTime, m_dwKeyReserveTime );
	}
	pOwner->ApplyNormalAttackResult( 0 );
	
	m_AttackState = AS_EXTEND_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ASA_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << (byte)m_SwordChangeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::ChangeToSwordAddChange( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();	
	ClearCancelInfo();
	ClearKeyReserve();
	
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );
	
	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;
	pOwner->SetObjectEquipState( &m_SwordAddChangeAttribute );

	m_AttackState					= AS_SWORD_ADD;
	m_SwordChangeState				= SCS_PRE_SWORD;
	m_dwSwordStateChangeStartTime	= 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ASA_SWORD_ADD );
		kPacket << pOwner->GetTargetRot();
		kPacket << (byte)m_SwordChangeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGeomjeItem::OnSwordAdd( ioBaseChar *pOwner )
{
	if( pOwner->GetState() != CS_OBJECT_EQUIP )
	{
		m_SwordChangeState = SCS_SWORD;
		CreateEffect( pOwner );
	}	
}

void ioGeomjeItem::CallItemProcess( ioBaseChar* pOwner )
{
	switch( m_SwordChangeState )
	{
	case SCS_NONE:
		CheckSwordAddChange( pOwner );
		break;
	case SCS_PRE_SWORD:
		OnSwordAdd( pOwner );
		break;	
	}

	if( pOwner->GetState() != CS_ATTACK && pOwner->GetState() != CS_OBJECT_EQUIP )
	{
		m_AttackState = AS_NONE;

		if( pOwner->IsNeedProcess() )
		{
			ClearSwordControl( pOwner );
		}
	}
}

void ioGeomjeItem::CheckSwordAddChange( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetState() != CS_DELAY )
	{
		m_dwSwordStateChangeStartTime = 0;
	}
	else
	{
		if( m_dwSwordStateChangeStartTime == 0 )
		{
			m_dwSwordStateChangeStartTime = dwCurTime;
		}
		else if( m_dwSwordStateChangeStartTime + m_dwSwordStateChangeTime < dwCurTime )
		{				
			if( IsEnableSwordAdd( pOwner ) )
			{
				ChangeToSwordAddChange( pOwner );
			}
			else
			{
				m_dwSwordStateChangeStartTime = 0;
			}
		}
	}
}

bool ioGeomjeItem::IsEnableSwordAdd( ioBaseChar* pOwner )
{
	if( GetCurBullet() < GetNeedBullet() )
		return false;
		
	if( !pOwner->IsEquipEnableObject() )
		return false;

	if( pOwner->HasBuff( BT_TARGET_EQUIP_OBJECTITEM ) )
		return false;
		
	if( pOwner->HasBuff( BT_CREATE_OBJECT_ITEM ) )
		return false;
	
	if( m_SwordChangeState != SCS_NONE )
		return false;

	if( m_AttackState == AS_SWORD_ADD )
		return false;

	return true;
}

// 대쉬 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::SetDashAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( m_SwordChangeState == SCS_SWORD && COMPARE( iCurCombo, 0, (int)m_DashAttackInfoList.size() ) )
	{
		m_CurrAttackInfoList	= m_DashAttackInfoList[iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
		m_CurrFireTimeType		= FTT_DASH_ATTACK;
		m_dwCurrAttackStartTime	= FRAMEGETTIME();
	}
}

void ioGeomjeItem::SetExtraDashAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( m_SwordChangeState == SCS_SWORD && COMPARE( iCurCombo, 0, (int)m_DashExtraAttackInfoList.size() ) )
	{		
		m_CurrAttackInfoList	= m_DashExtraAttackInfoList[iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
		m_CurrFireTimeType		= FTT_DASH_ATTACK;
		m_dwCurrAttackStartTime	= FRAMEGETTIME();
	}
}

void ioGeomjeItem::SetExtendDashAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( m_SwordChangeState == SCS_SWORD && COMPARE( iCurCombo, 0, (int)m_DashExtendAttackInfoList.size() ) )
	{		
		m_CurrAttackInfoList	= m_DashExtendAttackInfoList[iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
		m_CurrFireTimeType		= FTT_EXTEND_DASH_ATTACK;
		m_dwCurrAttackStartTime	= FRAMEGETTIME();
	}
}

void ioGeomjeItem::CheckPowerDash( ioBaseChar *pOwner )
{
	ProcessAttackFireList( pOwner, m_CurrFireTimeType );
}

// 점프 공격 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::SetJumpAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( m_SwordChangeState == SCS_SWORD && COMPARE( iCurCombo, 0, (int)m_JumpAttackInfoList.size() ) )
	{
		m_CurrAttackInfoList	= m_JumpAttackInfoList[iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
		m_CurrFireTimeType		= FTT_JUMP_ATTACK;
		m_dwCurrAttackStartTime	= FRAMEGETTIME();
	}
}

void ioGeomjeItem::SetJumpExtendAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( m_SwordChangeState == SCS_SWORD && COMPARE( iCurCombo, 0, (int)m_JumpExtendAttackInfoList.size() ) )
	{
		m_CurrAttackInfoList	= m_JumpExtendAttackInfoList[iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
		m_CurrFireTimeType		= FTT_JUMP_ATTACK;
		m_dwCurrAttackStartTime	= FRAMEGETTIME();
	}
}

void ioGeomjeItem::SetJumpDashAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( m_SwordChangeState == SCS_SWORD && COMPARE( iCurCombo, 0, (int)m_JumpDashAttackInfoList.size() ) )
	{
		m_CurrAttackInfoList	= m_JumpDashAttackInfoList[iCurCombo].m_AttackInfoPropertyList;
		m_dwBaseWeaponIndex		= pOwner->IncreaseWeaponIndexBase();
		m_CurrFireTimeType		= FTT_JUMP_ATTACK;
		m_dwCurrAttackStartTime	= FRAMEGETTIME();
	}
}

void ioGeomjeItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	ioAttackableItem::ProcessJumpAttack( pOwner, fHeightGap );
	ProcessAttackFireList( pOwner, m_CurrFireTimeType );
}

void ioGeomjeItem::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap )
{
	ioAttackableItem::ProcessJumpping( pOwner, fHeightGap );
	ProcessAttackFireList( pOwner, m_CurrFireTimeType );
}

void ioGeomjeItem::ProcessJumppingByNoNeedProcess( ioBaseChar *pOwner, float fHeightGap )
{
	ioAttackableItem::ProcessJumppingByNoNeedProcess( pOwner, fHeightGap );
	ProcessAttackFireList( pOwner, m_CurrFireTimeType );
}

void ioGeomjeItem::ProcessLanding( ioBaseChar *pOwner )
{
	ioAttackableItem::ProcessLanding( pOwner );

	m_CurrFireTimeType		= FTT_NORMAL_ATTACK;
	m_dwBaseWeaponIndex		= 0;	
	m_dwCurrAttackStartTime	= 0;

	m_CurrAttackInfoList.clear();
}

// 이기어검 특별 ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ioGeomjeItem::GetDummyBuffUserCount( ioBaseChar *pOwner )
{
	BaseCharList TargetList, NotTargetList;
	FD_RangeMultipleTarget kFindPred( pOwner, INT_MAX, 0, 360.0f, true );
	kFindPred.GetFindCharList( pOwner->GetCreator()->GetBaseCharList(), TargetList, NotTargetList );

	int iCount = 0;
	for( BaseCharList::iterator iter = TargetList.begin(); iter != TargetList.end(); ++iter )
	{
		ioBaseChar* pTarget = *iter;
		if( pTarget )
		{
			ioBuff* pBuff = pTarget->GetBuff( m_SwordDummyBuff );
			if( pBuff && pBuff->GetCreatorName() == pOwner->GetCharName() )
				iCount++;
		}
	}

	return iCount;
}

bool ioGeomjeItem::CheckToSwordExtend( ioBaseChar *pOwner )
{
	int iCount = GetDummyBuffUserCount( pOwner );
	if( iCount == 0 )
		return false;

	if( m_SwordChangeState != SCS_SWORD )
		return false;

	if( !COMPARE( m_iCurSwordControlCombo, 0, (int)m_SwordControlAttributeList.size() ) )
		return false;

	return true;
}

void ioGeomjeItem::ChangeToSwordExtend( ioBaseChar *pOwner )
{	
	m_AttackState		= AS_DUMMY_ATTACK;
	m_dwClearStartTime	= FRAMEGETTIME();

	ClearCancelInfo();
	ClearKeyReserve();

	pOwner->SetState( CS_ATTACK );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	SetAttack( pOwner, &m_SwordControlAttributeList[m_iCurSwordControlCombo], m_dwDummyAttackEndTime, m_dwKeyReserveTime );

	DummyCharList vList;
	g_DummyCharMgr.FindAllDummyCharToName( pOwner, m_SwordTargetDummy, vList );

	for( DummyCharList::iterator iter = vList.begin(); iter != vList.end();  ++iter )
	{
		ioDummyChar* pDummy = ToDummyChar( *iter );
		if( pDummy )
		{
			pDummy->SetReserveAttackCode( m_iSwordDummyDefaultAttackCode + m_iCurSwordControlCombo );
			pDummy->SetAttackState();
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ASA_DUMMY_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurSwordControlCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurSwordControlCombo++;
}

void ioGeomjeItem::OnSwordExtend( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDummyAttackEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
	}
	else
	{
		if( 0 < m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
		{
			DummyCharList vList;		
			g_DummyCharMgr.FindAllDummyCharToName( pOwner, m_SwordTargetDummy, vList );

			if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && !vList.empty() && CheckToSwordExtend( pOwner ) )
			{
				ChangeToSwordExtend( pOwner );
			}
			else
			{
				CheckReserve( pOwner );
			}
		}
	}
}

void ioGeomjeItem::ClearSwordControl( ioBaseChar* pOwner )
{
	m_iCurSwordControlCombo = 0;

	BaseCharList TargetList, NotTargetList;
	FD_RangeMultipleTarget kFindPred( pOwner, INT_MAX, 0, 360.0f, true );
	kFindPred.GetFindCharList( pOwner->GetCreator()->GetBaseCharList(), TargetList, NotTargetList );

	if( TargetList.empty() )
		return;

	if( 0 < m_dwClearStartTime )
	{
		for( BaseCharList::iterator iter = TargetList.begin(); iter != TargetList.end(); ++iter )
		{
			ioBaseChar* pTarget = *iter;
			if( pTarget )
			{
				ioBuff* pBuff = pTarget->GetBuff( m_SwordDummyBuff );
				if( pBuff && pBuff->GetCreatorName() == pOwner->GetCharName() )
					pBuff->SetReserveEndBuff();
			}
		}

		m_dwClearStartTime = 0;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ASA_CLEAR_BUFF );
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::ChangeToSwordEnd( ioBaseChar *pOwner )
{
	m_SwordChangeState = SCS_NONE;
	DestroyEffect( pOwner );
	CreateEndEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ASA_END_SWORD );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::DestroyEffect( ioBaseChar *pOwner )
{
	if( m_SwordEffectID != -1 )
	{
		pOwner->EndEffect( m_SwordEffectID, false );
		m_SwordEffectID = -1;
	}
}

void ioGeomjeItem::DestroyEndEffect( ioBaseChar *pOwner )
{
	if( m_SwordEndEffectID != -1 )
	{
		pOwner->EndEffect( m_SwordEndEffectID, false );
		m_SwordEndEffectID = -1;
	}
}

void ioGeomjeItem::CreateEffect( ioBaseChar *pOwner )
{
	DestroyEffect( pOwner );

	ioEffect* pEffect = pOwner->AttachEffect( m_SwordEffect );
	if( pEffect )
	{
		m_SwordEffectID = pEffect->GetUniqueID();
	}
}

void ioGeomjeItem::CreateEndEffect( ioBaseChar *pOwner )
{
	DestroyEndEffect( pOwner );

	ioEffect* pEffect = pOwner->AttachEffect( m_SwordEndEffect );
	if( pEffect )
	{
		m_SwordEndEffectID = pEffect->GetUniqueID();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioGeomjeItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		return true;
	}

	return false;
}

bool ioGeomjeItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_AttackState )
		{
		case AS_ATTACK_FIRE:
		case AS_EXTEND_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioGeomjeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case ASA_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			rkPacket >> m_dwBaseWeaponIndex;
			ChangeToAttackFire( pOwner );
		}
		break;
	case ASA_EXTEND_ATTACK:
		{
			byte eSwordState = 0;
			rkPacket >> eSwordState;
			m_SwordChangeState = (SwordChangeState)eSwordState;
			ChangeToExtendAttackFire( pOwner );

			if( m_SwordChangeState == SCS_SWORD )
				SetUseExtraGauge( GetSwordAttackNeedBullet() );
		}
		break;
	case ASA_SWORD_ADD:
		{
			byte eSwordState = 0;
			rkPacket >> eSwordState;
			m_SwordChangeState = (SwordChangeState)eSwordState;
			ChangeToSwordAddChange( pOwner );
		}
		break;
	case ASA_DUMMY_ATTACK:
		{
			rkPacket >> m_iCurSwordControlCombo;
			ChangeToSwordExtend( pOwner );
		}
		break;
	case ASA_CLEAR_BUFF:
		{
			ClearSwordControl( pOwner );
		}
		break;
	case ASA_END_SWORD:
		{
			ChangeToSwordEnd( pOwner );
		}
		break;
	}	
}


void ioGeomjeItem::FillInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << (BYTE)m_AttackState;
	rkPacket << (BYTE)m_SwordChangeState;
	rkPacket << m_fCurExtraGauge;
}

void ioGeomjeItem::SetInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE eAttackState;
	BYTE eSwordState;
	rkPacket >> eAttackState;
	rkPacket >> eSwordState;
	rkPacket >> m_fCurExtraGauge;
	m_AttackState		= (AttackState)eAttackState;
	m_SwordChangeState	= (SwordChangeState)eSwordState;

	if( m_SwordChangeState == SCS_SWORD )
		CreateEffect( pOwner );
}

void ioGeomjeItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_SwordChangeState != SCS_NONE )
	{
		if( m_SwordChangeState == SCS_SWORD && m_AttackState == AS_EXTEND_ATTACK_FIRE )
			return;

		if( m_SwordChangeState == SCS_SWORD && m_AttackState == AS_CHARGING )
			return;

		if(  m_AttackState == AS_DUMMY_ATTACK )
			return;
		
		if( GetCurBullet() <= 0 )
		{
			ChangeToSwordEnd( pOwner );
		}
	}
	else
	{
		switch( pOwner->GetState() )
		{
		case CS_RUN:
		case CS_DASH:
		case CS_DELAY:
			{
				float fTimePerSec = g_FrameTimer.GetSecPerFrame();
				float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
			break;
		}
	}
}

int ioGeomjeItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioGeomjeItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}


int ioGeomjeItem::GetSwordAttackNeedBullet()
{
	return (int)m_fSwordAttackNeedGauge;
}

int ioGeomjeItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioGeomjeItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioGeomjeItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioGeomjeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pOwner = pZone->GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}

	ioRotateReturnWeapon* pRotate = ToRotateReturnWeapon( pWeapon );
	if( pRotate )
	{
		ioBaseChar *pOwner = pRotate->GetOwner();
		if( pOwner )
		{
			for( OffsetPropertyList::iterator iter = m_SwordExtendAttributeOffsetList.begin(); iter != m_SwordExtendAttributeOffsetList.end(); ++iter )
			{
				OffsetProperty& Offset = *iter;
				if( Offset.m_dwAttributeIndex == pWeapon->GetAttributeIndex() )
				{
					D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
					D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					D3DXVec3Normalize( &vDir, &vDir );

					D3DXQUATERNION qtRot;
					ioPlayEntityHelp::MoveOffset( Offset.m_vOffset, pOwner->GetTargetRot(), vPos );	// Offset 계산
					ioPlayEntityHelp::RotateMoveDir( Offset.m_fHorz, Offset.m_fVert, vDir, qtRot );	// Rotate 종/횡 계산

					pRotate->SetPosition( vPos );
					pRotate->SetMoveDir( vDir );
				}
			}
		}
	}
}