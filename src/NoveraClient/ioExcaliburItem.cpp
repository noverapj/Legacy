

#include "stdafx.h"

#include "ioExcaliburItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioExcaliburItem::ioExcaliburItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iCurSpecialAttCnt = 0;
	m_fCurExtraGauge = 0.0f;

	m_iSpecialAttEffectIndex = 0;
	m_dwCurSpecialAttEffect = 0;

	m_bDefenseDash = false;
	m_dwDefenseDashEndTime = 0;
}

ioExcaliburItem::ioExcaliburItem( const ioExcaliburItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_iMaxSpecialAttCnt( rhs.m_iMaxSpecialAttCnt ),
m_iNeedSpecialAttCnt( rhs.m_iNeedSpecialAttCnt ),
m_SpecialAttEffectList( rhs.m_SpecialAttEffectList ),
m_SpecialAttEffectExList( rhs.m_SpecialAttEffectExList ),
m_SpecialAttBuffMap( rhs.m_SpecialAttBuffMap ),
m_bDefenseDash( rhs.m_bDefenseDash ),
m_DefenseDashAttribute( rhs.m_DefenseDashAttribute )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	
	m_iCurSpecialAttCnt = 0;
	m_fCurExtraGauge = 0.0f;

	m_iSpecialAttEffectIndex = 0;
	m_dwCurSpecialAttEffect = 0;
}

ioExcaliburItem::~ioExcaliburItem()
{
}

void ioExcaliburItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_iMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_iNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );

	m_SpecialAttEffectList.clear();
	m_SpecialAttEffectExList.clear();
	m_vExtendAttributeList.clear();

	if( m_iMaxSpecialAttCnt > 0 )
	{
		m_SpecialAttEffectList.reserve( m_iMaxSpecialAttCnt );
		m_SpecialAttEffectExList.reserve( m_iMaxSpecialAttCnt );
		m_vExtendAttributeList.reserve( m_iMaxSpecialAttCnt );

		for( int i=0; i < m_iMaxSpecialAttCnt; ++i )
		{
			wsprintf_e( szKey, "special_att_effect%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_SpecialAttEffectList.push_back( szBuf );

			wsprintf_e( szKey, "special_att_effect_ex%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_SpecialAttEffectExList.push_back( szBuf );

			wsprintf_e( szKey, "special_att_buff%d_cnt", i+1 );
			int iBuffCnt = rkLoader.LoadInt( szKey, 0 );
			ioHashStringVec vBuffList;
			for( int j=0; j < iBuffCnt; ++j )
			{
				wsprintf_e( szKey, "special_att_buff%d_name%d", i+1, j+1 );
				rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

				vBuffList.push_back( szBuf );
			}
			m_SpecialAttBuffMap.insert( SpecialAttBuffMap::value_type( i+1, vBuffList ) );

			AttackAttribute kAttribute;
			wsprintf_e( szKey, "extend_attack%d", i+1 );
			LoadAttackAttribute( szKey, kAttribute, rkLoader );
			m_vExtendAttributeList.push_back( kAttribute );
		}
	}
	m_bDefenseDash = rkLoader.LoadBool_e( "enable_defense_dash", false );

	wsprintf_e( szBuf, "defense_dash" );
	LoadAttackAttribute( szBuf, m_DefenseDashAttribute, rkLoader );
}

ioItem* ioExcaliburItem::Clone()
{
	return new ioExcaliburItem( *this );
}

ioWeaponItem::WeaponSubType ioExcaliburItem::GetSubType() const
{
	return WST_EXCALIBUR;
}

void ioExcaliburItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
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
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioExcaliburItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
	case CS_ATTACK_FIRE:
	case CS_DEFENSE_DASH:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioExcaliburItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( (m_iCurSpecialAttCnt >= m_iNeedSpecialAttCnt) && pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAttackFire( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioExcaliburItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioExcaliburItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	m_szCurAni.Clear();
	m_fCurAniRate = FLOAT1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iIndex = m_iCurSpecialAttCnt-1;
	if( !COMPARE( iIndex, 0, m_iMaxSpecialAttCnt ) )
		return;

	pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[iIndex], true );

	m_szCurAni = m_vExtendAttributeList[iIndex].m_AttackAnimation;
	m_fCurAniRate = m_vExtendAttributeList[iIndex].m_fAttackAniRate;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	m_dwKeyReserveTime = dwCurTime;
	m_dwKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fCurAniRate;

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurSpecialAttCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send ÀÌÈÄ
	DecreaseExtraNeedGauge( pOwner );
}

void ioExcaliburItem::DecreaseExtraNeedGauge( ioBaseChar *pOwner )
{
	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		m_fCurExtraGauge = 0.0f;

	ReleaseSpecialAttBuff( pOwner );

	m_iCurSpecialAttCnt -= m_iNeedSpecialAttCnt;
	m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

	SetSpecialAttBuff( pOwner );
}

void ioExcaliburItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			rkPacket >> m_iCurSpecialAttCnt;

			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_DEFENSE_DASH:
		ChangeToDefenseDash( pOwner );
		break;
	}
}

void ioExcaliburItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	rkPacket >> m_fCurExtraGauge;

	int iIndex;
	rkPacket >> iIndex;

	ReleaseSpecialAttBuff( pOwner );

	m_iCurSpecialAttCnt = iIndex;
	SetSpecialAttEffect( pOwner, iIndex );
	SetSpecialAttBuff( pOwner );
}

void ioExcaliburItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioExcaliburItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioExcaliburItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioExcaliburItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioExcaliburItem::GetAutoTargetValue( ioBaseChar *pOwner,
										  float &fRange, float &fMinAngle, float &fMaxAngle,
										  AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
	{
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
			return;
		case ATT_DASH:
			if( m_fDashAutoTargetRange == 0.0f )
				break;

			fRange = m_fDashAutoTargetRange;
			fMinAngle = m_fDashAutoTargetMinAngle;
			fMaxAngle = m_fDashAutoTargetMaxAngle;
			return;
		case ATT_JUMP:
			if( m_fJumpAutoTargetRange == 0.0f )
				break;

			fRange = m_fJumpAutoTargetRange;
			fMinAngle = m_fJumpAutoTargetMinAngle;
			fMaxAngle = m_fJumpAutoTargetMaxAngle;
			return;
		case ATT_COUNTER:
			if( m_fCountAutoTargetRange == 0.0f )
				break;

			fRange = m_fCountAutoTargetRange;
			fMinAngle = m_fCountAutoTargetMinAngle;
			fMaxAngle = m_fCountAutoTargetMaxAngle;
			return;
		}


		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;

		return;
	}
}

int ioExcaliburItem::GetMaxBullet()
{
	return m_iMaxSpecialAttCnt;
}

int ioExcaliburItem::GetCurBullet()
{
	return m_iCurSpecialAttCnt;
}

int ioExcaliburItem::GetNeedBullet()
{
	return m_iNeedSpecialAttCnt;
}

void ioExcaliburItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	iCurBullet = max( 0, min( iCurBullet, m_iMaxSpecialAttCnt ) );

	m_iCurSpecialAttCnt = iCurBullet;
}

void ioExcaliburItem::SetSpecialAttEffect( ioBaseChar *pChar, int iIndex )
{
	if( !pChar )
		return;

	int iMaxSize = m_SpecialAttEffectList.size();
	int iNewIndex = iIndex - 1;

	if( !COMPARE( iNewIndex, 0, iMaxSize ) )
	{
		if( m_dwCurSpecialAttEffect != -1 )
			pChar->EndEffect( m_dwCurSpecialAttEffect, false );

		m_iSpecialAttEffectIndex = 0;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << m_fCurExtraGauge;
			kPacket << iIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	ioHashString szEffect;
	m_iSpecialAttEffectIndex = iIndex;
	szEffect = m_SpecialAttEffectList[iNewIndex];

	if( m_dwCurSpecialAttEffect != -1 )
	{
		pChar->EndEffect( m_dwCurSpecialAttEffect, false );
	}

	if( !szEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( szEffect );
		if( pEffect )
		{
			m_dwCurSpecialAttEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << m_fCurExtraGauge;
			kPacket << iIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioExcaliburItem::ReleaseSpecialAttEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwCurSpecialAttEffect != -1 )
		pChar->EndEffect( m_dwCurSpecialAttEffect, false );

	m_dwCurSpecialAttEffect = 0;
	m_iSpecialAttEffectIndex = 0;
}

void ioExcaliburItem::CheckSpecialAttEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_iCurSpecialAttCnt != m_iSpecialAttEffectIndex )
	{
		SetSpecialAttEffect( pChar, m_iCurSpecialAttCnt );
	}
}

void ioExcaliburItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseSpecialAttBuff( pOwner );
	ReleaseSpecialAttEffect( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void ioExcaliburItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_EXCALIBUR )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_EXCALIBUR )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	m_fCurExtraGauge += fCurGauge;
	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		ReleaseSpecialAttBuff( pOwner );

		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

		SetSpecialAttBuff( pOwner );

		if( m_iCurSpecialAttCnt < m_iMaxSpecialAttCnt )
		{
			m_fCurExtraGauge -= m_fMaxExtraGauge;
			m_fCurExtraGauge = max( 0.0f, min(m_fCurExtraGauge, m_fMaxExtraGauge) );
		}
		else
			m_fCurExtraGauge = m_fMaxExtraGauge;
	}
}

void ioExcaliburItem::SetSpecialAttBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_iCurSpecialAttCnt <= 0 )
		return;

	SpecialAttBuffMap::iterator iter = m_SpecialAttBuffMap.find( m_iCurSpecialAttCnt );
	if( iter != m_SpecialAttBuffMap.end() )
	{
		const ioHashStringVec szBuffList = iter->second;

		int iBuffCnt = szBuffList.size();
		for( int i=0; i < iBuffCnt; ++i )
		{
			pChar->AddNewBuffWithAnimateTime( szBuffList[i], pChar->GetCharName(), m_Name, NULL, 0 );
		}
	}
}

void ioExcaliburItem::ReleaseSpecialAttBuff( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_iCurSpecialAttCnt <= 0 )
		return;

	SpecialAttBuffMap::iterator iter = m_SpecialAttBuffMap.find( m_iCurSpecialAttCnt );
	if( iter != m_SpecialAttBuffMap.end() )
	{
		const ioHashStringVec szBuffList = iter->second;

		int iBuffCnt = szBuffList.size();
		for( int i=0; i < iBuffCnt; ++i )
		{
			pChar->RemoveBuff( szBuffList[i] );
		}
	}
}

bool ioExcaliburItem::IsEnableSpcialAttack()
{
	if( m_iCurSpecialAttCnt >= m_iNeedSpecialAttCnt )
		return true;

	return false;
}

bool ioExcaliburItem::CheckDefenseSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !m_bDefenseDash )
		return false;

	ChangeToDefenseDash( pOwner );
	return true;
}

void ioExcaliburItem::ChangeToDefenseDash( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearDirDoubleClick();
	pOwner->SetState( CS_ATTACK );
	pOwner->SetNormalAttackByAttribute( m_DefenseDashAttribute, true );

	m_szCurAni = m_DefenseDashAttribute.m_AttackAnimation;
	m_fCurAniRate = m_DefenseDashAttribute.m_fAttackAniRate;

	m_ChargeState = CS_DEFENSE_DASH;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}