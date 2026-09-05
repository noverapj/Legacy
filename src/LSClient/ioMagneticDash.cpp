#include "stdafx.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioMagneticDash.h"
#include "ItemDefine.h"

ioMagneticDash::ioMagneticDash()
{
	m_bSetCombo					= false;
	m_bNormalAttacked			= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_dwActionEndTime			= 0;	
	m_DashState					= CDS_NONE;
	m_dwCancelCheckTime			= 0;

	m_szInputEventAnimation.Clear();
	m_fInputEventAnimationRate	= FLOAT1;
}

ioMagneticDash::ioMagneticDash( const ioMagneticDash &rhs )
: ioExtendDash( rhs ),
m_ChargingRedNAniList( rhs.m_ChargingRedNAniList ),			// N±Ø(»¡°­)
m_DashRedNAttack( rhs.m_DashRedNAttack ),
m_ExtraDashRedNAttackList( rhs.m_ExtraDashRedNAttackList),
m_ExtendChargeRedNAttack( rhs.m_ExtendChargeRedNAttack ),

m_ChargingBlueSAniList( rhs.m_ChargingBlueSAniList ),		// S±Ø(ÆÄ¶û)
m_DashBlueSAttack( rhs.m_DashBlueSAttack ),
m_ExtraDashBlueSAttackList( rhs.m_ExtraDashBlueSAttackList ),
m_ExtendChargeBlueSAttack( rhs.m_ExtendChargeBlueSAttack ),

m_dwChargingTime( rhs.m_dwChargingTime )
{
	m_bSetCombo					= false;
	m_bNormalAttacked			= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_dwActionEndTime			= 0;	
	m_DashState					= CDS_NONE;
	m_dwCancelCheckTime			= 0;

	m_szInputEventAnimation.Clear();
	m_fInputEventAnimationRate	= FLOAT1;
}

ioExtendDash* ioMagneticDash::Clone()
{
	return new ioMagneticDash( *this );
}

ioMagneticDash::~ioMagneticDash()
{
}

DashType ioMagneticDash::GetType() const
{
	return DT_MAGNETIC;
}

void ioMagneticDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];

	// Â÷Áö µ¿ÀÛ
	m_dwChargingTime	= rkLoader.LoadInt_e( "dash_charge_time", 0 );
	int iMaxChargeAni	= rkLoader.LoadInt_e( "charging_dash_ani_cnt", 0 );

	//=====================================================================================
	// N±Ø(»¡°­) 
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szBuf, "charging_red_n_dash_ani%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_ChargingRedNAniList.push_back( szBuf );
	}

	// Dash Attack
	m_DashRedNAttack.Init();
	LoadAttackAttribute_e( "dash_red_n_attack", m_DashRedNAttack, rkLoader );

	// extradash attack
	int iMaxAttackCnt = rkLoader.LoadInt_e( "extra_dash_max_cnt", 1 );

	m_ExtraDashRedNAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_dash_red_n_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraDashRedNAttackList.push_back( kAttribute );
	}

	m_ExtendChargeRedNAttack.Init();
	LoadAttackAttribute( "extend_charge_red_n_attack", m_ExtendChargeRedNAttack, rkLoader );

	//=====================================================================================
	// S±Ø(ÆÄ¶û)
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szBuf, "charging_blue_s_dash_ani%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_ChargingBlueSAniList.push_back( szBuf );
	}

	// Dash Attack
	m_DashBlueSAttack.Init();
	LoadAttackAttribute_e( "dash_blue_s_attack", m_DashBlueSAttack, rkLoader );

	// extradash attack
	m_ExtraDashBlueSAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_dash_blue_s_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraDashBlueSAttackList.push_back( kAttribute );
	}

	m_ExtendChargeBlueSAttack.Init();
	LoadAttackAttribute( "extend_charge_blue_s_attack", m_ExtendChargeBlueSAttack, rkLoader );
}

bool ioMagneticDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return false;	}

	ioExtendDash::StartDash( pOwner );

	m_bSetCombo					= false;
	m_bNormalAttacked			= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;
	m_dwActionEndTime			= 0;
	m_DashState					= CDS_NONE;
	m_dwCancelCheckTime			= 0;
	pOwner->NormalAttackOver( false );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	ChangeToCharging( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioMagneticDash::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	// N±Ø(»¡°­), S±Ø(ÆÄ¶û) 
	if( m_ChargingRedNAniList.empty() || m_ChargingBlueSAniList.empty() )
	{
		return;
	}

	ioHashString szCurChargingAni;
	ioHashStringVec chargingAniList;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			chargingAniList = m_ChargingRedNAniList;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			chargingAniList = m_ChargingBlueSAniList;
		}
	}


	int iChargingCnt = chargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( m_bSetCombo )
		iCurIndex = m_iCurComboCnt+1;

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
		szCurChargingAni = chargingAniList[iCurIndex];
	else
		szCurChargingAni = chargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	m_dwActionEndTime		= 0;
	m_dwReserveEnableTime	= 0;	

	m_iSkillInput			= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState		= false;	
	m_eSpecialDashCurrDirkey= ioUserKeyInput::DKI_NONE;

	m_DashState				= CDS_CHARGE;
}



void ioMagneticDash::ProcessDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_DashState )
	{
		case CDS_CHARGE:
		{
			ProcessCharging( pOwner );
			break;
		}
		case CDS_NORMAL_ATTACK:
		case CDS_COMBO_ATTACK:
		{
			ProcessNormalComboDash( pOwner );
			break;
		}
		case CDS_CHARGING_ATTACK_FIRE:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

			CheckCancelReserveByConditionalSpecialDash( pOwner );
			if( CheckAniDash_Input_Speical_Dash( pOwner ) )
				return;

			if( dwEndTime > dwCurTime )
			{
				pOwner->CheckExtraAniJump(); 

				if( pOwner->IsNeedProcess() )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}

				CheckExtraReserve( pOwner );
				if( CheckAniJump_Input_Jump_S( pOwner ) )
					return;
			}
			else
			{
				ProcessKeyReserve( pOwner );
			}
			break;
		}
	}
}

bool ioMagneticDash::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fInputEventAnimationRate;

		dwTime += m_dwActionStartTime;
		
		//´Ù¸¥ ·ÎÁ÷¿¡¼­ input_jump_sÀÇ ±â´ÉÀÌ key_reserve¸¦ Ã¼Å©ÇÏÁö ¾Ê´Âµ¥ ÇØ´ç·ÎÁ÷¿¡¼­¸¸ Ã¼Å©ÇÏ°í ÀÖ¾î ¹ö±×·Î ÆÇ´ÜÇÏ¿© ¼öÁ¤
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioMagneticDash::CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_szInputEventAnimation.IsEmpty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szInputEventAnimation );		
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fInputEventAnimationRate;
		dwInputSpecialDashTime += m_dwActionStartTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		ioAttackableItem *pAttack = pOwner->GetPriorityItem( SPT_ATTACK );
		if( pAttack && pAttack->IsConditionalSpecialDashReserve( pOwner ) && m_eSpecialDashCurrDirkey != ioUserKeyInput::DKI_NONE )
		{	
			pAttack->SetChangeConditionalSpecialDash( pOwner, m_eSpecialDashCurrDirkey, ioAttackableItem::CCSD_DASH_ATTACK );
			m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
			return true;
		}
		return false;
	}

	return false;
}

void ioMagneticDash::CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsDirKeyDoubleClick() && m_eSpecialDashCurrDirkey == ioUserKeyInput::DKI_NONE )
	{
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = pOwner->CheckCurDirKey();
		m_iSkillInput = -1;
		pOwner->ClearDirDoubleClick();
	}
}

void ioMagneticDash::ProcessNormalComboDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	// ´ë½Ã 1Å¸¿¡¼­ D~ ½Ã Äµ½½ D~ °ø°ÝÀÌ ¹ß»ý µÇ°Ô ÇÑ´Ù.
	CheckCancelReserveByConditionalSpecialDash( pOwner );
	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
	{
		return;
	}

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump(); 

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// ¿¹¾àÃ¼Å©
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}

			CheckAniJump_Input_Jump_S( pOwner );

			return;
		}
		else if( m_dwReserveEnableEndTime > 0 && m_dwReserveEnableTime > 0 && 
			m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime &&
			m_dwReserveEnableEndTime + pOwner->GetActionStopTotalDelay() >= dwCurTime )
		{
			// ¿¹¾àÃ¼Å©
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt >= iMaxCnt )
			{
				CheckExtraReserve( pOwner );
			}
			else
			{
				CheckKeyReserve( pOwner );
			}			
			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void ioMagneticDash::ProcessKeyReserve( ioBaseChar *pOwner )
{
	// °ø°ÝÅ°°¡ ´­·¯Á® ÀÖÀ¸¸é °ø°Ý »óÅÂ Ã³¸®
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) &&
		!pOwner->IsFloating() )
	{	
		ioObjectItem *pItem = pOwner->GetObject();
		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// ÇÊ»ì±â ¿¹¾àµÇ¾úÀ¸¸é ÇÊ»ì±â ¹ßµ¿
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// ¹æ¾î
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// Á¡ÇÁ
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}
	// °ø°Ý
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// ´ë±â
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetState( CS_DELAY );
}

void ioMagneticDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		ioUserKeyInput::DirKeyInput eCurKey = pOwner->CheckCurDirKey();

		if( pOwner->IsSettedDirection() )
		{
			float fYawD = 180.0f + 45.0f * ( eCurKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}

		ChangeToCharging( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << CDS_CHARGE;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

void ioMagneticDash::CheckExtraReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;		
		m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
}

void ioMagneticDash::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			ChangeToAttackFire( pOwner );
			return;
		}
	}
	else
	{
		if( m_bSetCombo )
		{
			SetComboDash( pOwner );
		}
		else 
		{
			ChangeToNormalAttack( pOwner );
		}
		return;
	}
}

void ioMagneticDash::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;		}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	AttackAttribute attackattribute;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			attackattribute = m_ExtendChargeRedNAttack;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			attackattribute = m_ExtendChargeBlueSAttack;
		}
	}

	int iAniID = pGrp->GetAnimationIdx( attackattribute.m_AttackAnimation );
	m_szInputEventAnimation = attackattribute.m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMagneticDash::ChangeToAttackFire - %s Animation is Not Exist",
								attackattribute.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = attackattribute.m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;	

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, attackattribute.m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, attackattribute.m_dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)attackattribute.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = attackattribute.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + attackattribute.m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime= dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( attackattribute.m_vForceInfoList, iAniID, fTimeRate, attackattribute.m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, attackattribute.m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + attackattribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	m_bNormalAttacked = true;
	m_bSetCombo = true;
	m_DashState = CDS_CHARGING_ATTACK_FIRE;

	CheckKeyReserveTime( pOwner, iAniID, attackattribute.m_fAttackAniRate );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, attackattribute.m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, attackattribute.m_dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_CHARGING_ATTACK_FIRE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_iCurComboCnt++;
}

void ioMagneticDash::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	const AttackAttribute *pAttr = NULL;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			pAttr = &m_DashRedNAttack;
			if( !pAttr )
				return;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			pAttr = &m_DashBlueSAttack;
			if( !pAttr )
				return;
		}
	}

	//const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_DASH_ATTACK );
	//if( !pAttr )
	//	return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMagneticDash::ChangeToNormalAttack - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	m_fInputEventAnimationRate = fTimeRate;

	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;	

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, pAttr->m_dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)pAttr->m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = pAttr->m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime= dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + pAttr->m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	m_bNormalAttacked = true;
	m_bSetCombo = true;
	m_DashState = CDS_NORMAL_ATTACK;

	CheckKeyReserveTime( pOwner, iAniID, pAttr->m_fAttackAniRate );
	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, pAttr->m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_NORMAL_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagneticDash::SetComboDash( ioBaseChar *pOwner, bool bCharge )
{
	if( !pOwner ) return;

	int iMaxCnt = pOwner->GetExtraDashAttackMax();

	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	const AttackAttribute *pAttr = NULL;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			pAttr = &m_ExtraDashRedNAttackList[m_iCurComboCnt];
			if( !pAttr )
				return;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			pAttr = &m_ExtraDashBlueSAttackList[m_iCurComboCnt];
			if( !pAttr )
				return;
		}
	}

	//const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTRA_DASH_ATTACK, m_iCurComboCnt );
	//if( !pAttr )
	//	return;

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	m_szInputEventAnimation = pAttr->m_AttackAnimation;
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMagneticDash::SetComboDash - %s Animation is Not Exist",
								pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	m_dwActionEndTime = 0;	
	m_dwReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_EXTEND_DASH_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime = dwCurTime;
	m_dwActionEndTime = dwCurTime + dwDuration;
	
	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_bSetCombo = true;
	m_DashState = CDS_COMBO_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_COMBO_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << bCharge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
	// send ÀÌÈÄ
	m_iCurComboCnt++;	
}

void ioMagneticDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) {	return;	}

	//ioEntityGroup *pGrp = pOwner->GetGroup();
	//if( !pGrp ) {	return;	}

	//DWORD dwDelayTime	= 0;
	//DWORD dwCurTime		= FRAMEGETTIME();

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurComboCnt;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	bool bCharge;

	switch( iState )
	{	
		case CDS_NONE:
		{
			StartDash( pOwner );
			break;
		}
		case CDS_CHARGE:
		{
			ChangeToCharging( pOwner );
			break;
		}
		case CDS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
		}
		break;

		case CDS_COMBO_ATTACK:
		{
			rkPacket >> bCharge;
			SetComboDash( pOwner, bCharge );
			break;
		}

		case CDS_CHARGING_ATTACK_FIRE:
		{
			ChangeToAttackFire( pOwner );
			break;
		}
	}
}

void ioMagneticDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	m_DashState					= CDS_NONE;
	m_bSetCombo					= false;
	m_iCurComboCnt				= 0;
	m_dwChargingStartTime		= 0;

	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;

	m_iSkillInput				= -1;
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;	
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;

	if( pOwner )
	{
		pOwner->InitExtendAttackTagInfo();
	}
}