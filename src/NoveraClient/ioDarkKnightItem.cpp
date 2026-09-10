#include "stdafx.h"

#include "ioDarkKnightItem.h"
#include "ioDarkKnightObjectItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioStatAttribute.h"
#include "WeaponDefine.h"
#include "ioChargeComboDash.h"

ioDarkKnightItem::ioDarkKnightItem()
{
	Init();
}

ioDarkKnightItem::ioDarkKnightItem( const ioDarkKnightItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fObjectGaugeAttack( rhs.m_fObjectGaugeAttack ),
	m_fObjectGaugeAttackDef( rhs.m_fObjectGaugeAttackDef ),
	m_fObjectGaugeWound( rhs.m_fObjectGaugeWound ),
	m_fObjectGaugeWoundDef( rhs.m_fObjectGaugeWoundDef ),
	m_fObjectZeroHPGaugeAttack( rhs.m_fObjectZeroHPGaugeAttack ),
	m_fObjectZeroHPGaugeAttackDef( rhs.m_fObjectZeroHPGaugeAttackDef ),
	m_fObjectZeroHPGaugeWound( rhs.m_fObjectZeroHPGaugeWound ),
	m_fObjectZeroHPGaugeWoundDef( rhs.m_fObjectZeroHPGaugeWoundDef ),
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
	m_dwObjectItem( rhs.m_dwObjectItem ),
	m_dwObjectEquipDealyTime( rhs.m_dwObjectEquipDealyTime ),
	m_ObjectEquipAttribute( rhs.m_ObjectEquipAttribute ),
	m_vDisableChangeObjectBuff( rhs.m_vDisableChangeObjectBuff )
{
	Init();	
}

ioDarkKnightItem::~ioDarkKnightItem()
{
}

void ioDarkKnightItem::Init()
{
	m_ChargeState						= CS_NONE;
	m_eFinalShotCallerType				= CCSD_NONE;
	
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
		
	m_dwObjectEquipDealyStartTime		= 0;

	m_vForceDir							= ioMath::VEC3_ZERO;

	m_eCurreCallerType					= CCSD_NONE;
	m_CurrSpecialDashEffectID			= -1;

	m_bReserveExtendDash				= false;

}

void ioDarkKnightItem::ClearSpecial( ioBaseChar* pOwner )
{
	m_ChargeState = CS_NONE;
}

void ioDarkKnightItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];

	// extra gauge
	m_fNeedGauge				= rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );
	
	// object equip gauge
	m_fObjectGaugeAttack		= rkLoader.LoadFloat_e( "object_gauge_att_rate", 0.0f );
	m_fObjectGaugeAttackDef		= rkLoader.LoadFloat_e( "object_gauge_att_def_rate", 0.0f );
	m_fObjectGaugeWound			= rkLoader.LoadFloat_e( "object_gauge_wound_rate", 0.0f );
	m_fObjectGaugeWoundDef		= rkLoader.LoadFloat_e( "object_gauge_wound_def_rate", 0.0f );

	//object equip, hp-zero gauge
	m_fObjectZeroHPGaugeAttack		= rkLoader.LoadFloat_e( "object_zero_gauge_att_rate", 0.0f );
	m_fObjectZeroHPGaugeAttackDef	= rkLoader.LoadFloat_e( "object_zero_gauge_att_def_rate", 0.0f );
	m_fObjectZeroHPGaugeWound		= rkLoader.LoadFloat_e( "object_zero_gauge_wound_rate", 0.0f );
	m_fObjectZeroHPGaugeWoundDef	= rkLoader.LoadFloat_e( "object_zero_gauge_wound_def_rate", 0.0f );

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

	//object
	m_dwObjectItem				= rkLoader.LoadInt_e( "object_item", 0 );
	m_dwObjectEquipDealyTime	= rkLoader.LoadInt_e( "object_equip_delay_time", 200 );

	wsprintf_e( szBuf, "object_equip" );
	LoadAttackAttribute( szBuf, m_ObjectEquipAttribute, rkLoader );

	int iCount = rkLoader.LoadInt_e( "object_equip_disable_buff_max_count", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		wsprintf_e( szBuf, "object_equip_disable_buff%d", i+1 );		
		m_vDisableChangeObjectBuff.push_back( rkLoader.LoadInt( szBuf, 0 ) );
	}
}

void ioDarkKnightItem::LoadAttribtue( IN ioINILoader &rkLoader, IN const char* szTtitle, OUT AttributeList& rkAttribute )
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

void ioDarkKnightItem::LoadConditionSpecialDash( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "condition_special_dash_animation_front", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashFront		= szBuf;
	m_fConditionSpecialDashFrontRate	= rkLoader.LoadFloat_e( "condition_special_dash_animation_front_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_back", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashBack		= szBuf;
	m_fConditionSpecialDashBackRate		= rkLoader.LoadFloat_e( "condition_special_dash_animation_back_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_right", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashRt			= szBuf;
	m_fConditionSpecialDashRtRate		= rkLoader.LoadFloat_e( "condition_special_dash_animation_right_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_left", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashLt			= szBuf;
	m_fConditionSpecialDashLtRate		= rkLoader.LoadFloat_e( "condition_special_dash_animation_left_rate", FLOAT1 );
	m_fConditionSpecialDashForceAmt		= rkLoader.LoadFloat_e( "condition_special_dash_force_amt", 0.0f );
	m_fConditionSpecialDashForceFric	= rkLoader.LoadFloat_e( "condition_special_dash_force_friction", FLOAT1 );
}

ioItem* ioDarkKnightItem::Clone()
{
	return new ioDarkKnightItem( *this );
}

ioWeaponItem::WeaponSubType ioDarkKnightItem::GetSubType() const
{
	return WST_DARKKNIGHT_ITEM;
}

void ioDarkKnightItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
	
	Init();

	//별도로 초기화
	m_fCurExtraGauge = 0.0f;
}

void ioDarkKnightItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	pOwner->EndEffect( m_CurrSpecialDashEffectID );
	Init();

	//별도로 초기화
	m_fCurExtraGauge = 0.0f;
}

// 일반 공격 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioDarkKnightItem::ChangeToCharge( ioBaseChar *pOwner, int iCurCombo )
{
	int iPreGatherCnt = m_PreAnimationList.size();

	m_eCurreCallerType	= CCSD_NONE;
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetSkillNextJumpAmt( 0 );
	pOwner->SetState( CS_ATTACK );
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

void ioDarkKnightItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioDarkKnightItem::OnAttackFire( ioBaseChar *pOwner )
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

	ioWeaponItem::CheckNormalAttack( pOwner );
}

bool ioDarkKnightItem::SetExtendDashState( ioBaseChar *pOwner )
{
	if( m_iCurrConditionSpecialDashCount >= m_iMaxConditionSpecialDashCount )
	{
		pOwner->SetNormalAttack( 0 );
		return false;
	}

	ClearKeyReserve();
	return ioAttackableItem::SetExtendDashState( pOwner );
}

void ioDarkKnightItem::SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
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

void ioDarkKnightItem::ChangeToAttackFire( ioBaseChar *pOwner )
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
void ioDarkKnightItem::ChangeToExtendAttackFire( ioBaseChar *pOwner )
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

void ioDarkKnightItem::OnCharging( ioBaseChar *pOwner )
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

void ioDarkKnightItem::CheckExtendDashState( ioBaseChar *pOwner )
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

void ioDarkKnightItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
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

void ioDarkKnightItem::ChangeConditionSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
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

void ioDarkKnightItem::ChangeConditionSpecialDashByNormal( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir, int eCallerType )
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

void ioDarkKnightItem::OnConditionSpecialDash( ioBaseChar *pOwner, ioPlayStage* pStage )
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

void ioDarkKnightItem::ChangeConditionSpecialDashByJump( ioBaseChar *pOwner )
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

bool ioDarkKnightItem::CheckLanding( ioBaseChar *pChar )
{
	ioAttackableItem::CheckLanding( pChar );

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

void ioDarkKnightItem::ChangeConditionFinalShotByNormal( ioBaseChar *pOwner )
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

void ioDarkKnightItem::OnConditionSpecialFinalByNormal( ioBaseChar *pOwner, ioPlayStage* pStage )
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

void ioDarkKnightItem::ChangeConditionFinalShotByJump( ioBaseChar *pOwner )
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

void ioDarkKnightItem::OnConditionSpecialFinalByJump( ioBaseChar *pOwner, ioPlayStage* pStage )
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

void ioDarkKnightItem::ChangeConditionFinalShotByDash( ioBaseChar *pOwner )
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

void ioDarkKnightItem::OnConditionSpecialFinalByDash( ioBaseChar *pOwner, ioPlayStage* pStage )
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

void ioDarkKnightItem::ChangeExtendDashStateByonditionSpecialDash( ioBaseChar *pOwner )
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

void ioDarkKnightItem::ChangeEffect( ioBaseChar *pOwner )
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

void ioDarkKnightItem::GetConditionSpecialValue( OUT int& eCurrType, OUT int& iCurrConditionSpecialDashCount )
{
	eCurrType						= m_eFinalShotCallerType;
	iCurrConditionSpecialDashCount	= m_iCurrConditionSpecialDashCount;
}

void ioDarkKnightItem::SetConditionSpecialValue( int eCurrType, int iCurrConditionSpecialDashCount )
{
	m_eFinalShotCallerType				= eCurrType;
	m_iCurrConditionSpecialDashCount	= iCurrConditionSpecialDashCount;
}


// 오브젝트 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightItem::ChangeEquipObjectState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	pOwner->SetObjectEquipState( &m_ObjectEquipAttribute );
	pOwner->EndEffect( m_CurrSpecialDashEffectID );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwObjectItem > 0 )
	{
		m_ChargeState = CS_EQUIP_OBJECT;
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kUdpPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kUdpPacket );			
			kUdpPacket << static_cast<byte>( SS_EQUIP_OBJECT );
			kUdpPacket << pOwner->GetTargetRot();
			kUdpPacket << m_fCurExtraGauge;
			P2PNetwork::SendToAllPlayingUser( kUdpPacket );

			//아래 패킷으로 ioAttackableItem::EquipObjectItemByAttakable()가 호출됨
			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_dwObjectItem;
			kPacket << GetName();
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
			ioObjectItem *pObject = ToObjectItem( pItem );
			if( pObject )
			{
				pObject->ReLoadProperty();
				EquipObjectItemByAttakable( pOwner, pObject );
								
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pOwner->EquipItem( pObject );
			}
		}
	}
}

bool ioDarkKnightItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
			return true;
	}

	return false;
}

void ioDarkKnightItem::ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem )
{
	ioDarkKnightObjectItem* pDarkKnight = ToDarkKnightObjectItem( pObjectItem );
	if( pDarkKnight )
	{
		ReleaseObject( pOwner );
	}
}

void ioDarkKnightItem::ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess /* = true */ )
{
	if( !pOwner )
		return;

	ioObjectItem *pObjectItem = pOwner->GetObject();
	if( pObjectItem && pObjectItem->GetItemCode() != m_dwObjectItem )
		return;

	if( bNeedProcess && !pOwner->IsNeedProcess() )
		return;
		
	if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
	{
		//애니메이션 초기화
		if( pOwner->GetState() == CS_DELAY )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
		}
		else if( pOwner->GetState() == CS_RUN )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__, CS_RUN );
		}
		else if( pOwner->GetState() == CS_DASH )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__, CS_DASH );
		}
		else
		{
			pOwner->ReleaseObjectItem( __FUNCTION__ );
		}
	}
}

void ioDarkKnightItem::ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject )
{	
	m_ChargeState = CS_NONE;

	if( pReleasedObject && pReleasedObject->GetItemCode() == m_dwObjectItem )
	{
		ioDarkKnightObjectItem* pDarkKinht = ToDarkKnightObjectItem( pReleasedObject );
		if( pDarkKinht )
			pDarkKinht->GetConditionSpecialValue( m_eFinalShotCallerType, m_iCurrConditionSpecialDashCount );

		ChangeEffect( pOwner );
	}

	if( m_bReserveExtendDash )
	{
		m_bReserveExtendDash = false;
		SetExtendDashState( pOwner );
	}
}

void ioDarkKnightItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )
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

	CheckObject( pOwner, FRAMEGETTIME() );
}

void ioDarkKnightItem::CheckObject( ioBaseChar *pOwner, DWORD dwCurrTime )
{
	if( !pOwner->IsNeedProcess() )
		return;
		
	if( pOwner->GetState() != CS_DELAY )
	{
		m_dwObjectEquipDealyStartTime = 0;
	}
	else
	{
		if( m_ChargeState != CS_EQUIP_OBJECT )
		{			
			if( m_dwObjectEquipDealyStartTime == 0 )
			{
				m_dwObjectEquipDealyStartTime = dwCurrTime;
			}
			else if( m_dwObjectEquipDealyStartTime + m_dwObjectEquipDealyTime < dwCurrTime )
			{				
				CheckObjectEquip( pOwner );
			}
		}
	}
}

void ioDarkKnightItem::CheckObjectEquip( ioBaseChar *pOwner )
{
	m_dwObjectEquipDealyStartTime = 0;

	for( IntVec::const_iterator iter = m_vDisableChangeObjectBuff.begin(); iter != m_vDisableChangeObjectBuff.end(); ++iter )
	{
		int iBuffType = *iter;
		if( pOwner && pOwner->HasBuff( iBuffType ) )
			return;
	}

	if( pOwner->IsDisableEquipObject() )
		return;

	if( GetCurBullet() < GetNeedBullet() )
		return;

	if( !pOwner->IsEquipEnableObject() )
		return;

	if( pOwner->HasBuff( BT_TARGET_EQUIP_OBJECTITEM ) || pOwner->HasBuff( BT_CREATE_OBJECT_ITEM ) )
		return;
	
	ChangeEquipObjectState( pOwner );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDarkKnightItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case SS_EQUIP_OBJECT:
		{
			D3DXQUATERNION qtRot;			
			rkPacket >> qtRot;
			rkPacket >> m_fCurExtraGauge;

			pOwner->SetTargetRot( qtRot );

			ChangeEquipObjectState( pOwner );
		}
		break;
	}
}

bool ioDarkKnightItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDarkKnightItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioDarkKnightItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CONDITION_SPECIAL_DASH_NORMAL:
	case CS_CONDITION_SPECIAL_DASH_JUMP:
		return false;
	}

	return true;
}

void ioDarkKnightItem::FillInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	ioDarkKnightObjectItem* pDarkKnight = ToDarkKnightObjectItem( pOwner->GetObject() );
	if( pDarkKnight )
	{
		rkPacket << true;
	}
	else
	{
		rkPacket << false;
	}

	rkPacket <<	m_eFinalShotCallerType;
	rkPacket <<	m_iCurrConditionSpecialDashCount;
}

void ioDarkKnightItem::SetInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bEquipObejct;
	rkPacket >> bEquipObejct;
	rkPacket >> m_eFinalShotCallerType;
	rkPacket >> m_iCurrConditionSpecialDashCount;

	if( !bEquipObejct )
	{
		ChangeEffect( pOwner );
	}
}

int ioDarkKnightItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioDarkKnightItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioDarkKnightItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioDarkKnightItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioDarkKnightItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioDarkKnightItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	float	fGauge	= 0.0f;
	int		iHP		= (int)pOwner->GetHP().m_fCurValue;

	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_DARKKNIGHT )
			if( m_ChargeState == CS_EQUIP_OBJECT )
			{
				if( iHP <= 0 && pOwner->IsZeroHPByMode() )
				{
					fGauge = m_fObjectZeroHPGaugeAttack;
				}
				else
				{
					fGauge = m_fObjectGaugeAttack;
				}
			}
			else
			{
				fGauge = m_fExtraGaugeAttack;
			}
		else
		{
			fGauge = 0.0f;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_DARKKNIGHT )
			if( m_ChargeState == CS_EQUIP_OBJECT )
			{
				if( iHP <= 0 && pOwner->IsZeroHPByMode() )
				{
					fGauge = m_fObjectZeroHPGaugeAttackDef;
				}
				else
				{
					fGauge = m_fObjectGaugeAttackDef;
				}
			}
			else
			{
				fGauge = m_fExtraGaugeAttackDef;
			}
		else
		{
			fGauge = 0.0f;
		}
		break;
	case EGT_WOUNDED:
		if( m_ChargeState == CS_EQUIP_OBJECT )
		{
			if( iHP <= 0 && pOwner->IsZeroHPByMode() )
			{
				fGauge = m_fObjectZeroHPGaugeWound;
			}
			else
			{
				fGauge = m_fObjectGaugeWound;
			}
		}
		else
		{
			fGauge = m_fExtraGaugeWound;
		}
		break;
	case EGT_WOUNDED_DEF:
		if( m_ChargeState == CS_EQUIP_OBJECT )
		{
			if( iHP <= 0 && pOwner->IsZeroHPByMode() )
			{
				fGauge = m_fObjectZeroHPGaugeWoundDef;
			}
			else
			{
				fGauge = m_fObjectGaugeWoundDef;
			}
		}
		else
		{
			fGauge = m_fExtraGaugeWoundDef;
		}
		break;
	}
	
	if( fGauge <= 0.0f )
		return;

	if( m_ChargeState == CS_EQUIP_OBJECT )
	{
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( m_fCurExtraGauge, 0.0f );
	}
	else
	{
		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

void ioDarkKnightItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();	
	float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

void ioDarkKnightItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDarkKnightSpecialState::ioDarkKnightSpecialState()
{
}

ioDarkKnightSpecialState::~ioDarkKnightSpecialState()
{
}

void ioDarkKnightSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDarkKnightSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioAttackableItem* pAttackable = pOwner->GetPriorityItem( SPT_ATTACK );
	if( pAttackable )
	{
		switch( pAttackable->GetType() )
		{
		case ioWeaponItem::IT_WEAPON:
			{
				ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
				if( pDarkKnight && eNewState != CS_ATTACK )
					pDarkKnight->ClearSpecial( pOwner );

			}
			break;
		case ioWeaponItem::IT_OBJECT:
			{
				ioDarkKnightObjectItem* pDarkKnight = ToDarkKnightObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
				if( pDarkKnight && eNewState != CS_ATTACK )
					pDarkKnight->ClearSpecial( pOwner );
			}
			break;
		}
	}
}

bool ioDarkKnightSpecialState::IsIgnoreAttackOverDelay()
{
	return true;
}

void ioDarkKnightSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioAttackableItem* pAttackable = pOwner->GetPriorityItem( SPT_ATTACK );
	if( pAttackable )
	{
		switch( pAttackable->GetType() )
		{
		case ioWeaponItem::IT_WEAPON:
			{
				ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
				if( pDarkKnight && pDarkKnight->GetState() == ioDarkKnightItem::CS_CONDITION_SPECIAL_DASH_NORMAL )
				{
					pDarkKnight->OnConditionSpecialDash( pOwner, pOwner->GetCreator() );
					return;
				}
			}
			break;
		case ioWeaponItem::IT_OBJECT:
			{
				ioDarkKnightObjectItem* pDarkKnight = ToDarkKnightObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
				if( pDarkKnight && pDarkKnight->GetState() == ioDarkKnightObjectItem::CS_CONDITION_SPECIAL_DASH_NORMAL )
				{
					pDarkKnight->OnConditionSpecialDash( pOwner, pOwner->GetCreator() );
					return;
				}
			}
			break;
		}
	}

	pOwner->SetState( CS_DELAY );
	pOwner->SetFallState( true );
}

void ioDarkKnightSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	ioAttackableItem* pAttackable = pOwner->GetPriorityItem( SPT_ATTACK );
	if( pAttackable )
	{
		switch( pAttackable->GetType() )
		{
		case ioWeaponItem::IT_WEAPON:
			{
				ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
				if( pDarkKnight && pDarkKnight->GetState() != ioDarkKnightItem::CS_CONDITION_SPECIAL_DASH_JUMP )
				{
					ioSpecialStateBase::CheckFallStateChange( pOwner, fMinFallHeightRate, fBottomHeight, bBottomMaxCheck, fMaxBottomHeight );
				}
			}
			break;
		case ioWeaponItem::IT_OBJECT:
			ioDarkKnightObjectItem* pDarkKnight = ToDarkKnightObjectItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pDarkKnight && pDarkKnight->GetState() != ioDarkKnightItem::CS_CONDITION_SPECIAL_DASH_JUMP )
			{
				ioSpecialStateBase::CheckFallStateChange( pOwner, fMinFallHeightRate, fBottomHeight, bBottomMaxCheck, fMaxBottomHeight );
			}
			break;
		}
	}
}