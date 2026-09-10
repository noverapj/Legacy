#include "stdafx.h"

#include "ioDarkKnightItem.h"
#include "ioDarkKnightObjectItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioStatAttribute.h"
#include "WeaponDefine.h"
#include "ioChargeComboDash.h"

ioDarkKnightObjectItem::ioDarkKnightObjectItem()
{
	Init();
}

ioDarkKnightObjectItem::ioDarkKnightObjectItem( const ioDarkKnightObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_PreAnimationList( rhs.m_PreAnimationList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_szConditionSpecialDashFront( rhs.m_szConditionSpecialDashFront ),
	m_fConditionSpecialDashFrontRate( rhs.m_fConditionSpecialDashFrontRate ),
	m_szConditionSpecialDashBack( rhs.m_szConditionSpecialDashBack ),
	m_fConditionSpecialDashBackRate( rhs.m_fConditionSpecialDashBackRate ),
	m_szConditionSpecialDashRt( rhs.m_szConditionSpecialDashRt ),
	m_fConditionSpecialDashRtRate( rhs.m_fConditionSpecialDashRtRate ),
	m_szConditionSpecialDashLt( rhs.m_szConditionSpecialDashLt ),
	m_fConditionSpecialDashLtRate( rhs.m_fConditionSpecialDashLtRate ),
	m_iMaxConditionSpecialDashCount( rhs.m_iMaxConditionSpecialDashCount ),
	m_ConditionSpecialDashEffectList( rhs.m_ConditionSpecialDashEffectList ),
	m_fConditionSpecialDashForceAmt( rhs.m_fConditionSpecialDashForceAmt ),
	m_fConditionSpecialDashForceFric( rhs.m_fConditionSpecialDashForceFric ),
	m_vSpecialByNormalAttributeList( rhs.m_vSpecialByNormalAttributeList ),
	m_vSpecialByJumpAttributeList( rhs.m_vSpecialByJumpAttributeList ),
	m_vSpecialByDashAttributeList( rhs.m_vSpecialByDashAttributeList ),
	m_EquipMaleMeshList( rhs.m_EquipMaleMeshList ),
	m_EquipFemaleMeshList( rhs.m_EquipFemaleMeshList )
{
	Init();	
}

ioDarkKnightObjectItem::~ioDarkKnightObjectItem()
{
}

void ioDarkKnightObjectItem::Init()
{
	m_ChargeState						= CS_NONE;
	m_eFinalShotCallerType				= CCSD_NONE;

	m_fCurExtraGauge					= 0.0f;

	m_dwConditionSpecialDashEndTime		= 0;
	m_dwConditionSpecialDashForceTime	= 0;
	m_iCurrConditionSpecialDashCount	= 0;

	m_iCurrSpecialComboByNormal			= 0;
	m_iCurrSpecialComboByJump			= 0;
	m_iCurrSpecialCmoboByDash			= 0;

	m_dwSpecialByNormalEndTime			= 0;
	m_dwSpecialByJumpEndTime			= 0;
	m_dwSpecialByDashEndTime			= 0;

	m_dwSpecialByNormalReseveTime		= 0;
	m_dwSpecialByJumpReseveTime			= 0;
	m_dwSpecialByDashReseveTime			= 0;

	m_vForceDir							= ioMath::VEC3_ZERO;

	m_eCurreCallerType					= CCSD_NONE;
	m_CurrSpecialDashEffectID			= -1;
}

void ioDarkKnightObjectItem::ClearSpecial( ioBaseChar* pOwner )
{
	m_ChargeState = CS_NONE;
}

void ioDarkKnightObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//Pre
	int iPreCount	= rkLoader.LoadInt_e( "pre_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreAnimationList.push_back( ioHashString( szBuf ) );
	}

	//Extend
	LoadAttribtue( rkLoader, "extend", m_vExtendAttributeList );

	//Cancel Dash(Normal, DashAttack)
	LoadConditionSpecialDash( rkLoader );

	//Special Final
	m_iMaxConditionSpecialDashCount = rkLoader.LoadInt_e( "condition_special_dash_max_cnt", FLOAT1 );
	for( int i = 0; i < m_iMaxConditionSpecialDashCount; ++i )
	{
		wsprintf_e( szBuf, "condition_special_dash%d_effect", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_ConditionSpecialDashEffectList.push_back( szBuf );
	}

	LoadAttribtue( rkLoader, "special_final_by_normal", m_vSpecialByNormalAttributeList );
	LoadAttribtue( rkLoader, "special_final_by_jump", m_vSpecialByJumpAttributeList );
	LoadAttribtue( rkLoader, "special_final_by_dash", m_vSpecialByDashAttributeList );

	//mesh
	int iMeshCnt = rkLoader.LoadInt_e( "equip_male_mesh_cnt", 0 );
	for( int i = 0; i<iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "equip_male_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EquipMaleMeshList.push_back( szBuf );
	}

	iMeshCnt = rkLoader.LoadInt_e( "equip_female_mesh_cnt", 0 );
	for( int i = 0; i<iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "equip_female_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EquipFemaleMeshList.push_back( szBuf );
	}
}

void ioDarkKnightObjectItem::LoadAttribtue( IN ioINILoader &rkLoader, IN const char* szTtitle, OUT AttributeList& rkAttribute )
{
	char szBuf[MAX_PATH];
	wsprintf_e( szBuf, "%s_attack_cnt", szTtitle );
	int iCnt = rkLoader.LoadInt( szBuf, 0 );

	rkAttribute.clear();
	rkAttribute.reserve( iCnt );
	for( int i = 0; i < iCnt ; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "%s_attack0%d", szTtitle, i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		rkAttribute.push_back( kAttribute );
	}
}

void ioDarkKnightObjectItem::LoadConditionSpecialDash( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "condition_special_dash_animation_front", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashFront = szBuf;
	m_fConditionSpecialDashFrontRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_front_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_back", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashBack = szBuf;
	m_fConditionSpecialDashBackRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_back_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_right", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashRt = szBuf;
	m_fConditionSpecialDashRtRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_right_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_left", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashLt = szBuf;
	m_fConditionSpecialDashLtRate		= rkLoader.LoadFloat_e( "condition_special_dash_animation_left_rate", FLOAT1 );

	m_fConditionSpecialDashForceAmt		= rkLoader.LoadFloat_e( "condition_special_dash_force_amt", 0.0f );
	m_fConditionSpecialDashForceFric	= rkLoader.LoadFloat_e( "condition_special_dash_force_friction", FLOAT1 );	
}

ioItem* ioDarkKnightObjectItem::Clone()
{
	return new ioDarkKnightObjectItem( *this );
}

ioObjectItem::ObjectSubType ioDarkKnightObjectItem::GetObjectSubType() const
{
	return OST_DARKKNIGHT;
}

void ioDarkKnightObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ChangeMesh( pOwner );

	ioObjectItem::OnEquiped( pOwner );

	if( GetObjectCreateItem().IsEmpty() )
		Init();

	ioDarkKnightItem* pDarkKinht = ToDarkKnightItem( pOwner->GetWeapon() );
	if( pDarkKinht )
	{
		pDarkKinht->GetConditionSpecialValue( m_eFinalShotCallerType, m_iCurrConditionSpecialDashCount );
		ChangeEffect( pOwner );
	}
}

void ioDarkKnightObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseMesh( pOwner );

	ioObjectItem::OnReleased( pOwner );

	pOwner->EndEffect( m_CurrSpecialDashEffectID );
}

bool ioDarkKnightObjectItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioDarkKnightObjectItem::IsEnableDash() const
{
	return true;
}

bool ioDarkKnightObjectItem::IsEnableDefense() const
{
	return true;
}

// 일반 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	//캔슬 대쉬 발동조건이 만족한 경우 마지막 캔슬 대쉬 기준으로 별도의 어택로직 수행
	if( m_iCurrConditionSpecialDashCount >= m_iMaxConditionSpecialDashCount )
	{
		switch( m_eFinalShotCallerType )
		{		
		case ioAttackableItem::CCSD_JUMP:
			ChangeConditionFinalShotByJump( pOwner );
			break;
		case ioAttackableItem::CCSD_DASH_ATTACK:
			ChangeConditionFinalShotByDash( pOwner );
			break;
		default:
			ChangeConditionFinalShotByNormal( pOwner );
			break;
		}
	}
	else
	{
		//대쉬 어택 중 캔슬 대쉬로 상태변환 후 공격키 입력시 대쉬 공격이 다시 나가도록 함
		if( m_eCurreCallerType == CCSD_DASH_ATTACK )
		{
			ChangeExtendDashStateByonditionSpecialDash( pOwner );
		}
		else
		{
			ChangeToCharge( pOwner, iCurCombo );
		}
	}
}

void ioDarkKnightObjectItem::ChangeToCharge( ioBaseChar *pOwner, int iCurCombo )
{
	int iPreGatherCnt = m_PreAnimationList.size();

	m_eCurreCallerType	= CCSD_NONE;
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetSkillNextJumpAmt( 0 );
	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

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

void ioDarkKnightObjectItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pOwner || !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_ATTACK_FIRE:
	case CS_EXTEND_ATTACK_FIRE:
		OnAttackFire( pOwner );
		break;
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_SPECIAL_FINAL_BY_NORMAL:
		OnConditionSpecialFinalByNormal( pOwner, pStage );
		break;
	case CS_SPECIAL_FINAL_BY_JUMP:
		OnConditionSpecialFinalByJump( pOwner, pStage );
		break;
	case CS_SPECIAL_FINAL_BY_DASH:
		OnConditionSpecialFinalByDash( pOwner, pStage );
		break;
	}
}

void ioDarkKnightObjectItem::OnAttackFire( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioObjectItem::CheckNormalAttack( pOwner );
}

bool ioDarkKnightObjectItem::SetExtendDashState( ioBaseChar *pOwner )
{	
	if( IsObjectReleaseTCPSend() )
	{
		ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pOwner->GetWeapon() );
		if( pDarkKnight )
		{
			pDarkKnight->SetReserveExtendDash( true );			
		}
	}
	else
	{
		if( m_iCurrConditionSpecialDashCount >= m_iMaxConditionSpecialDashCount )
		{
			pOwner->SetNormalAttack( 0 );
			return false;
		}

		ClearKeyReserve();
		return ioAttackableItem::SetExtendDashState( pOwner );
	}

	return false;
}

void ioDarkKnightObjectItem::SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
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

void ioDarkKnightObjectItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
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
		kPacket << static_cast<byte>( SS_NORMAL_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// D꾹 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ioDarkKnightObjectItem::ChangeToExtendAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	ClearKeyReserve();

	DWORD dwEndTime		= 0;
	DWORD dwKeyRserve	= 0;
	int iMax = (int)m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iMax ) )
	{
		SetAttack( pOwner, &m_vExtendAttributeList[m_iCurCombo], false, true, dwEndTime, dwKeyRserve );
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}
	else
	{
		SetAttack( pOwner, &m_vExtendAttributeList[iMax-1], false, true, dwEndTime, dwKeyRserve );
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	m_ChargeState = CS_EXTEND_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_EXTEND_ATTACK );		
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDarkKnightObjectItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToExtendAttackFire( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

// 일반 캔슬 대쉬(대쉬,노말) ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
		{
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		}
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioDarkKnightObjectItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	switch( eCallerType )
	{
	case ioAttackableItem::CCSD_JUMP:
		{
			ChangeConditionSpecialDashByJump( pOwner );
		}
		break;
	default:
		{
			ChangeConditionSpecialDash( pOwner, eCurrDir, eCallerType );
		}
		break;
	}
}

void ioDarkKnightObjectItem::ChangeConditionSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	// KeyDir
	if( eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	m_vForceDir = ioMath::VEC3_ZERO;

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	if( m_szConditionSpecialDashFront.IsEmpty() || m_szConditionSpecialDashRt.IsEmpty() || m_szConditionSpecialDashLt.IsEmpty() || m_szConditionSpecialDashBack.IsEmpty() )
		return;

	switch( iCnt )
	{
	case 0:
		if( 40 <= iRest )
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashRt, m_fConditionSpecialDashRtRate, vMoveDir, eCallerType );
		}
		else
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashFront, m_fConditionSpecialDashFrontRate, vMoveDir, eCallerType );
		}
		break;
	case 1:		
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashRt, m_fConditionSpecialDashRtRate, vMoveDir, eCallerType );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashRt, m_fConditionSpecialDashRtRate, vMoveDir, eCallerType );
		}
		else
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashBack, m_fConditionSpecialDashBackRate, vMoveDir, eCallerType );
		}
		break;
	case 3:
	case 4:
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashBack, m_fConditionSpecialDashBackRate, vMoveDir, eCallerType );		
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashLt, m_fConditionSpecialDashLtRate, vMoveDir, eCallerType );
		}
		else
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashBack, m_fConditionSpecialDashBackRate, vMoveDir, eCallerType );
		}
		break;
	case 6:		
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashLt, m_fConditionSpecialDashLtRate, vMoveDir, eCallerType );		
		}
		break;
	case 7:
		if( iRest < 5 )
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashLt, m_fConditionSpecialDashLtRate, vMoveDir, eCallerType );
		}
		else
		{
			ChangeConditionSpecialDashByNormal( pOwner, m_szConditionSpecialDashFront, m_fConditionSpecialDashFrontRate, vMoveDir, eCallerType );
		}
		break;
	default:
		{
			return;
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<byte>( SS_SPECIAL_DASH_BY_NORMAL );
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		kPacket << m_iCurrConditionSpecialDashCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurrConditionSpecialDashCount++;
}

void ioDarkKnightObjectItem::ChangeConditionSpecialDashByNormal( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir, int eCallerType )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();	
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );	
	pOwner->SetState( CS_DARKKNIGHT_SPECIAL );
	
	m_ChargeState			= CS_CONDITION_SPECIAL_DASH_NORMAL;
	m_eCurreCallerType		= eCallerType;
	m_eFinalShotCallerType	= eCallerType;

	m_dwKeyReserveTime					= 0;
	m_dwConditionSpecialDashEndTime		= 0;
	m_dwConditionSpecialDashForceTime	= 0;
		
	ChangeEffect( pOwner );

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwConditionSpecialDashEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fCurRate;
	m_dwConditionSpecialDashForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
}

void ioDarkKnightObjectItem::OnConditionSpecialDash( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwConditionSpecialDashEndTime > 0 && m_dwConditionSpecialDashEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_eCurreCallerType == CCSD_DASH_ATTACK )
			{
				if( pOwner->IsAttackKey() )
				{
					SetExtendDashState( pOwner );
					return;
				}
			}

			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}

		m_eCurreCallerType = CCSD_NONE;
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			{
				CheckReserve( pOwner );
			}
		}

		if( m_dwConditionSpecialDashForceTime > 0 && m_dwConditionSpecialDashForceTime < dwCurTime )
		{
			m_dwConditionSpecialDashForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fConditionSpecialDashForceAmt, m_fConditionSpecialDashForceFric );
		}
	}
}

// 점프 캔슬 대쉬 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeConditionSpecialDashByJump( ioBaseChar *pOwner )
{	
	m_ChargeState			= CS_CONDITION_SPECIAL_DASH_JUMP;
	m_eCurreCallerType		= CCSD_JUMP;
	m_eFinalShotCallerType	= CCSD_JUMP;

	ChangeEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_SPECIAL_DASH_BY_JUMP );
		kPacket << m_iCurrConditionSpecialDashCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurrConditionSpecialDashCount++;
	m_dwConditionSpecialDashForceTime = 0;
}

bool ioDarkKnightObjectItem::CheckLanding( ioBaseChar *pChar )
{
	ioObjectItem::CheckLanding( pChar );

	switch( m_ChargeState )
	{
	case CS_CONDITION_SPECIAL_DASH_JUMP:
		{
			m_ChargeState = CS_NONE;
		}
		break;
	}

	return true;
}

// 캔슬 대쉬 풀차징 발동 공격(노말) ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeConditionFinalShotByNormal( ioBaseChar *pOwner )
{
	ClearCancelInfo();
	ClearKeyReserve();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->SetState( CS_ATTACK );
	pOwner->EndEffect( m_CurrSpecialDashEffectID );

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	m_ChargeState = CS_SPECIAL_FINAL_BY_NORMAL;

	int iMax = (int)m_vSpecialByNormalAttributeList.size();
	if( !m_vSpecialByNormalAttributeList.empty() )
	{
		if( COMPARE( m_iCurrSpecialComboByNormal, 0, iMax ) )
		{
			SetAttack( pOwner, &m_vSpecialByNormalAttributeList[m_iCurrSpecialComboByNormal], true, true, m_dwSpecialByNormalEndTime, m_dwSpecialByNormalReseveTime );
			pOwner->ApplyNormalAttackResult( m_iCurrSpecialComboByNormal );
		}
		else
		{
			SetAttack( pOwner, &m_vSpecialByNormalAttributeList[iMax-1], true, true, m_dwSpecialByNormalEndTime, m_dwSpecialByNormalReseveTime );
			pOwner->ApplyNormalAttackResult( m_iCurrSpecialComboByNormal, true );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<byte>( SS_SPECIAL_FINAL_BY_NORMAL );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurrSpecialComboByNormal;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurrSpecialComboByNormal++;
}

void ioDarkKnightObjectItem::OnConditionSpecialFinalByNormal( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSpecialByNormalEndTime < dwCurTime )
	{
		Init();

		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}
	}
	else
	{
		if( pOwner->IsAttackKey() )
		{
			if( COMPARE( m_iCurrSpecialComboByNormal, 0, (int)m_vSpecialByNormalAttributeList.size() ) )
			{
				if( m_dwSpecialByNormalReseveTime > 0 && m_dwSpecialByNormalReseveTime < dwCurTime )
				{
					ChangeConditionFinalShotByNormal( pOwner );
					return;
				}
			}
		}

		pOwner->CheckExtraAniJump();

		CheckReserve( pOwner );		
		if( ProcessCancel(pOwner) )
		{
			Init();
			return;
		}
	}
}

// 캔슬 대쉬 풀차징 발동 공격(점프) ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeConditionFinalShotByJump( ioBaseChar *pOwner )
{
	ClearCancelInfo();
	ClearKeyReserve();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->SetState( CS_ATTACK );

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	m_ChargeState = CS_SPECIAL_FINAL_BY_JUMP;

	pOwner->EndEffect( m_CurrSpecialDashEffectID );

	int iMax = (int)m_vSpecialByJumpAttributeList.size();
	if( !m_vSpecialByJumpAttributeList.empty() )
	{
		if( COMPARE( m_iCurrSpecialComboByJump, 0, iMax ) )
		{
			SetAttack( pOwner, &m_vSpecialByJumpAttributeList[m_iCurrSpecialComboByJump], true, true, m_dwSpecialByJumpEndTime, m_dwSpecialByJumpReseveTime );
			pOwner->ApplyNormalAttackResult( m_iCurrSpecialComboByJump );
		}
		else
		{
			SetAttack( pOwner, &m_vSpecialByJumpAttributeList[iMax-1], true, true, m_dwSpecialByJumpEndTime, m_dwSpecialByJumpReseveTime );
			pOwner->ApplyNormalAttackResult( m_iCurrSpecialComboByJump, true );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<byte>( SS_SPECIAL_FINAL_BY_JUMP );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurrSpecialComboByJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurrSpecialComboByJump++;
}

void ioDarkKnightObjectItem::OnConditionSpecialFinalByJump( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSpecialByJumpEndTime < dwCurTime )
	{
		Init();

		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}	
	}
	else
	{
		if( pOwner->IsAttackKey() )
		{
			if( COMPARE( m_iCurrSpecialComboByJump, 0, (int)m_vSpecialByJumpAttributeList.size() ) )
			{
				if( m_dwSpecialByJumpReseveTime > 0 && m_dwSpecialByJumpReseveTime < dwCurTime )
				{
					ChangeConditionFinalShotByJump( pOwner );
					return;
				}
			}
		}

		pOwner->CheckExtraAniJump();

		CheckReserve( pOwner );		
		if( ProcessCancel(pOwner) )
		{
			Init();
			return;
		}
	}
}

// 캔슬 대쉬 풀차징 발동 공격(대쉬) ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeConditionFinalShotByDash( ioBaseChar *pOwner )
{
	ClearCancelInfo();
	ClearKeyReserve();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->SetState( CS_ATTACK );

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	m_ChargeState = CS_SPECIAL_FINAL_BY_DASH;	

	pOwner->EndEffect( m_CurrSpecialDashEffectID );

	int iMax = (int)m_vSpecialByDashAttributeList.size();
	if( !m_vSpecialByDashAttributeList.empty() )
	{
		if( COMPARE( m_iCurrSpecialCmoboByDash, 0, iMax ) )
		{
			SetAttack( pOwner, &m_vSpecialByDashAttributeList[m_iCurrSpecialCmoboByDash], true, true, m_dwSpecialByDashEndTime, m_dwSpecialByDashReseveTime );
			pOwner->ApplyNormalAttackResult( m_iCurrSpecialCmoboByDash );
		}
		else
		{
			SetAttack( pOwner, &m_vSpecialByDashAttributeList[iMax-1], true, true, m_dwSpecialByDashEndTime, m_dwSpecialByDashReseveTime );
			pOwner->ApplyNormalAttackResult( m_iCurrSpecialCmoboByDash, true );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<byte>( SS_SPECIAL_FINAL_BY_DASH );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurrSpecialCmoboByDash;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurrSpecialCmoboByDash++;
}

void ioDarkKnightObjectItem::OnConditionSpecialFinalByDash( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSpecialByDashEndTime < dwCurTime )
	{
		Init();

		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}
	}
	else
	{
		if( pOwner->IsAttackKey() )
		{
			if( COMPARE( m_iCurrSpecialCmoboByDash, 0, (int)m_vSpecialByDashAttributeList.size() ) )
			{
				if( m_dwSpecialByDashReseveTime > 0 && m_dwSpecialByDashReseveTime < dwCurTime )
				{
					ChangeConditionFinalShotByDash( pOwner );
					return;
				}
			}
		}

		pOwner->CheckExtraAniJump();

		CheckReserve( pOwner );
		if( ProcessCancel(pOwner) )
		{
			Init();
			return;
		}
	}
}

// 대쉬 공격 -> 캔슬 대쉬 -> D키 입력시 대쉬공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeExtendDashStateByonditionSpecialDash( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	m_eCurreCallerType = CCSD_NONE;

	if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	SetExtendDashState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<byte>( SS_EXTEND_ATTACK_BY_SPECIAL_DASH );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;	
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// 이펙트 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeEffect( ioBaseChar *pOwner )
{
	pOwner->EndEffect( m_CurrSpecialDashEffectID );

	int iCurrIdx = 0;
	if( !COMPARE( m_iCurrConditionSpecialDashCount, 0, (int)m_ConditionSpecialDashEffectList.size() )  )	
	{
		iCurrIdx = (int)m_ConditionSpecialDashEffectList.size() - 1;
	}
	else
	{
		iCurrIdx = m_iCurrConditionSpecialDashCount;
	}

	ioEffect* pEffect = pOwner->AttachEffect( m_ConditionSpecialDashEffectList[iCurrIdx] );		
	if( pEffect )
		m_CurrSpecialDashEffectID = pEffect->GetUniqueID();

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::GetConditionSpecialValue( OUT int& eCurrType, OUT int& iCurrConditionSpecialDashCount )
{
	eCurrType						= m_eFinalShotCallerType;
	iCurrConditionSpecialDashCount	= m_iCurrConditionSpecialDashCount;
}


void ioDarkKnightObjectItem::SetConditionSpecialValue( ioBaseChar* pOwner, int eCurrType, int iCurrConditionSpecialDashCount )
{
	pOwner->EndEffect( m_CurrSpecialDashEffectID );
	Init();

	m_eFinalShotCallerType				= eCurrType;
	m_iCurrConditionSpecialDashCount	= iCurrConditionSpecialDashCount;

	ChangeEffect( pOwner );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->GetState() != CS_DELAY )
		return;

	switch( m_ChargeState )
	{
	case CS_CONDITION_SPECIAL_DASH_JUMP:
		{
			m_ChargeState		= CS_NONE;
			m_eCurreCallerType	= CCSD_NONE;
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::ChangeMesh( ioBaseChar* pOwner )
{
	pOwner->SetInvisibleState( true, true, true );

	if( pOwner->IsMale() )
	{
		for( int i = 0; i < (int)m_EquipMaleMeshList.size(); ++i )
			pOwner->AddEntity( m_EquipMaleMeshList[i] );
	}
	else
	{
		for( int i = 0; i < (int)m_EquipFemaleMeshList.size(); ++i )
			pOwner->AddEntity( m_EquipFemaleMeshList[i] );
	}
}

void ioDarkKnightObjectItem::ReleaseMesh( ioBaseChar* pOwner )
{
	if( pOwner->IsMale() )
	{
		for( int i = 0; i < (int)m_EquipMaleMeshList.size(); ++i )
			pOwner->DestroyEntity( m_EquipMaleMeshList[i] );
	}
	else
	{
		for( int i = 0; i < (int)m_EquipFemaleMeshList.size(); ++i )
			pOwner->DestroyEntity( m_EquipFemaleMeshList[i] );
	}

	pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightObjectItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SS_NORMAL_ATTACK:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case SS_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_iCurCombo;
			ChangeToExtendAttackFire( pOwner );
		}
		break;
	case SS_EXTEND_ATTACK_BY_SPECIAL_DASH:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_iCurCombo;
			ChangeExtendDashStateByonditionSpecialDash( pOwner );
		}
		break;
	case SS_SPECIAL_DASH_BY_JUMP:
		{
			rkPacket >> m_iCurrConditionSpecialDashCount;
			ChangeConditionSpecialDashByJump( pOwner );
		}
		break;
	case SS_SPECIAL_DASH_BY_NORMAL:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			int eCurrDir, eCallerType;
			rkPacket >> eCurrDir;
			rkPacket >> eCallerType;
			rkPacket >> m_iCurrConditionSpecialDashCount;

			ChangeConditionSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
		}
		break;
	case SS_SPECIAL_FINAL_BY_NORMAL:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_iCurrSpecialComboByNormal;
			ChangeConditionFinalShotByNormal( pOwner );
		}
		break;
	case SS_SPECIAL_FINAL_BY_JUMP:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_iCurrSpecialComboByJump;
			ChangeConditionFinalShotByJump( pOwner );
		}
		break;
	case SS_SPECIAL_FINAL_BY_DASH:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_iCurrSpecialCmoboByDash;
			ChangeConditionFinalShotByDash( pOwner );
		}
		break;
	}
}

bool ioDarkKnightObjectItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDarkKnightObjectItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			{
				if( !bCheck )
					return false;

				return true;
			}
		}
	}
	return false;
}

bool ioDarkKnightObjectItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CONDITION_SPECIAL_DASH_NORMAL:
	case CS_CONDITION_SPECIAL_DASH_JUMP:
		return false;
	}

	return true;
}

void ioDarkKnightObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioDarkKnightObjectItem::IsReleaseItem( const ioBaseChar* pOwner ) const
{
	if( pOwner )
	{
		ioWeaponItem *pItem = pOwner->GetWeapon();
		ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pItem );
		if( pDarkKnight && pDarkKnight->GetCurBullet() > 0 )
			return false;
	}

	return true;
}

bool ioDarkKnightObjectItem::CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	bool bRet = ioObjectItem::CheckReleaseProcess( pStage, pOwner );
	if( bRet )
		return true;

	return CheckRelease( pStage, pOwner );
}

bool ioDarkKnightObjectItem::CheckRelease( ioPlayStage *pStage, ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pOwner->GetWeapon() );

	//자신을 생성한 무기가 없는 상태에서는 무조건 릴리즈 되어야 함
	if( !pDarkKnight )
		return true;

	if( IsReleaseItem( pOwner ) )
	{	
		int iSkillNum = pOwner->GetSkillInput();
		if( iSkillNum >= 0 )
			return true;	

		if( pOwner->GetState() == CS_DELAY )
			return true;
	}

	return false;
}