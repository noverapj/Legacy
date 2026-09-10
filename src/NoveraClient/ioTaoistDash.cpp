#include "StdAfx.h"
#include "ioTaoistDash.h"
#include "ioZoneEffectWeapon.h"
#include "ioTaoistItem.h"


ioTaoistDash::ioTaoistDash(void)
{
	Init();
}

ioTaoistDash::ioTaoistDash( const ioTaoistDash &rhs )
: ioExtendDash( rhs ),
m_ChargingAniList( rhs.m_ChargingAniList ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_ChargingAttack( rhs.m_ChargingAttack )
{
	Init();
}

ioTaoistDash::~ioTaoistDash(void)
{
}

ioExtendDash*	ioTaoistDash::Clone()
{
	return new ioTaoistDash( *this );
}

DashType	ioTaoistDash::GetType() const
{
	return DT_TAOIST;
}

void	ioTaoistDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char	szKey[MAX_PATH];
	char	szBuf[MAX_PATH];

	m_dwChargingTime	= rkLoader.LoadInt_e( "dash_charge_time", 0 );
	int	iMaxChargeAni	= rkLoader.LoadInt_e( "charging_dash_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_dash_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ChargingAniList.push_back( szBuf );
	}

	LoadAttackAttribute_e( "dash_charging_attack", m_ChargingAttack, rkLoader );
}

void	ioTaoistDash::Init()
{
	m_TaoistDashState	= TDS_NONE;
	m_iCurComboCnt	= 0;
	m_bSetCombo	= false;
	m_dwChargingStartTime	= 0;
	m_dwActionStartTime	= 0;
	m_dwActionEndTime	= 0;
	m_dwCancelCheckTime	= 0;
	m_szInputEventAnimation.Clear();
}

bool	ioTaoistDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return	false;

	ioExtendDash::StartDash( pOwner );

	m_TaoistDashState	= TDS_NONE;
	m_iCurComboCnt	= 0;
	m_bSetCombo	= false;
	m_dwChargingStartTime	= 0;
	m_dwActionStartTime	= 0;
	m_dwActionEndTime	= 0;
	m_dwCancelCheckTime	= 0;

	pOwner->NormalAttackOver( false );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetNoWoundState( false );
	pOwner->SetState( CS_EXTEND_DASH );

	ChangeToCharging( pOwner );

	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << DST_STATE;
		kPacket << DS_NONE;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/

	return	true;
}

void	ioTaoistDash::ProcessDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_TaoistDashState )
	{
	case	TDS_CHARGE:
		{
			ProcessCharging( pOwner );
		}
		break;

	case	TDS_NORMAL_ATTACK:
		{
			ProcessNormalAttack( pOwner );
		}
		break;

	case	TDS_COMBO_ATTACK:
		{
			ProcessComboAttack( pOwner );
		}
		break;

	case	TDS_CHARGE_ATTACK:
		{
			ProcessChargingAttack( pOwner );
		}
		break;
	}
}

void	ioTaoistDash::ExtendDashEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_TaoistDashState	= TDS_NONE;
	m_iCurComboCnt	= 0;
	m_bSetCombo	= false;
	m_dwChargingStartTime	= 0;
	m_dwActionStartTime	= 0;
	m_dwActionEndTime	= 0;
}

void	ioTaoistDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int	iState;
	rkPacket >> iState;

	switch( iState )
	{
	case	TDS_CHARGE:
		{
			D3DXQUATERNION	qtRot;

			rkPacket >> m_iCurComboCnt;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToCharging( pOwner );
		}
		break;

	case	TDS_NORMAL_ATTACK:
		{
			D3DXVECTOR3	vPos;
			D3DXQUATERNION	qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			ChangeToNormalAttack( pOwner );
		}
		break;

	case	TDS_COMBO_ATTACK:
		{
			D3DXQUATERNION	qtRot;

			rkPacket >> m_iCurComboCnt;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToComboAttack( pOwner );
		}
		break;

	case	TDS_CHARGE_ATTACK:
		{
			D3DXVECTOR3	vPos;
			D3DXQUATERNION	qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			ChangeToChargingAttack( pOwner );
		}
		break;
	}
}

void	ioTaoistDash::CheckKeyReserve( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	bool	bReserveSkill	= false;
	int	iSkillNum	= pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput	= iSkillNum;
		bReserveSkill	= true;
	}

	int iMaxCnt = pOwner->GetExtraDashAttackMax();
	if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill && m_iCurComboCnt < iMaxCnt )
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

		/*if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << DST_STATE;
			kPacket << DS_CHARGE;
			kPacket << m_iCurComboCnt;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/
		return;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState	= true;
		m_bEnableAttackState	= false;
		m_bEnableDefenseState	= false;		
		m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;
		m_iSkillInput	= -1;
	}
	else
	{
		CheckCancelReserveByConditionalSpecialDash( pOwner );
	}
}

void	ioTaoistDash::ChangeToCharging( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_ChargingAniList.empty() )
		return;

	ioHashString szCurChargingAni;

	int iChargingCnt = m_ChargingAniList.size();
	int iCurIndex = m_iCurComboCnt;

	if( COMPARE( iCurIndex, 0, iChargingCnt ) )
		szCurChargingAni = m_ChargingAniList[iCurIndex];
	else
		szCurChargingAni = m_ChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( szCurChargingAni, FLOAT100 );

	m_dwChargingStartTime = FRAMEGETTIME();

	//m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;
	//m_dwPreReserveEnableTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;	
	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;

	m_TaoistDashState = TDS_CHARGE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_TaoistDashState );
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistDash::ChangeToNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	const	AttackAttribute*	pAttr	= NULL;

	pAttr	= pOwner->GetAttackAttribute( AT_DASH_ATTACK );
	if( !pAttr )
		return;
	
	int	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate	= pAttr->m_fAttackAniRate;

	//m_dwActionEndTime	= 0;
	m_dwReserveEnableTime	= 0;
	//m_dwPreReserveEnableTime	= 0;

	m_iSkillInput	= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState	= false;
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		pAttr->m_dwPreDelay );
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, (float)pAttr->m_dwPreDelay, true );

	DWORD	dwCurTime	= FRAMEGETTIME();
	DWORD	dwDuration	= pAttr->m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime	= dwCurTime;
	m_dwActionEndTime	= dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, pAttr->m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	DWORD	dwTrackingTime	= dwCurTime + pAttr->m_dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_szInputEventAnimation	= pAttr->m_AttackAnimation;
	m_fInputEventAnimationRate	= fTimeRate;

	m_bSetCombo	= true;
	m_TaoistDashState	= TDS_NORMAL_ATTACK;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, pAttr->m_dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, pAttr->m_dwPreDelay );

	//pOwner->ClearDirDoubleClick();
	//pOwner->ClearDirKey();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_TaoistDashState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistDash::ChangeToComboAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	int	iMaxCnt	= pOwner->GetExtraDashAttackMax();

	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	const	AttackAttribute*	pAttr	= NULL;

	pAttr	= pOwner->GetAttackAttribute( AT_EXTRA_DASH_ATTACK, m_iCurComboCnt );

	if( !pAttr )
		return;

	int	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate	= pAttr->m_fAttackAniRate;
	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;

	//m_dwActionEndTime = 0;
	//m_dwPreReserveEnableTime = 0;
	m_dwReserveEnableTime	= 0;

	m_iSkillInput	= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState	= false;
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD	dwCurTime	= FRAMEGETTIME();
	DWORD	dwDuration	= dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime	= dwCurTime;
	m_dwActionEndTime	= dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_szInputEventAnimation	= pAttr->m_AttackAnimation;
	m_fInputEventAnimationRate	= fTimeRate;

	m_bSetCombo	= true;
	m_TaoistDashState	= TDS_COMBO_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_TaoistDashState );
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurComboCnt++;
}

void	ioTaoistDash::ChangeToChargingAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_ChargingAttack.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float	fTimeRate	= m_ChargingAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_ChargingAttack.m_dwPreDelay;

	//m_dwActionEndTime = 0;
	//m_dwPreReserveEnableTime = 0;
	m_dwReserveEnableTime	= 0;

	m_iSkillInput	= -1;
	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState	= false;
	m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_ChargingAttack.m_vWeaponInfoList,
		FTT_EXTEND_DASH_ATTACK,
		fTimeRate,
		dwPreDelay );
	pOwner->SetNoWoundState( false );

	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)dwPreDelay, true );

	DWORD	dwCurTime	= FRAMEGETTIME();
	DWORD	dwDuration	= dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_ChargingAttack.m_dwEndDelay;

	m_dwCancelCheckTime = dwCurTime;
	m_dwActionStartTime	= dwCurTime;
	m_dwActionEndTime	= dwCurTime + dwDuration;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetReservedSliding( m_ChargingAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= FRAMEGETTIME() + dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_szInputEventAnimation	= m_ChargingAttack.m_AttackAnimation;
	m_fInputEventAnimationRate	= fTimeRate;

	m_bSetCombo	= true;
	m_TaoistDashState	= TDS_CHARGE_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_TaoistDashState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistDash::ProcessCharging( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime + m_dwChargingTime < dwCurTime )
		{
			ChangeToChargingAttack( pOwner );
		}
	}
	else
	{
		if( m_bSetCombo )
			ChangeToComboAttack( pOwner );
		else
			ChangeToNormalAttack( pOwner );
	}
}

void	ioTaoistDash::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	DWORD	dwEndTime	= m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
		pOwner->ClearDirDoubleClick();
	else if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->CheckExtendAttackEndJump() )
			return;
		
		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
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
			// 예약체크
			int	iMaxCnt	= pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt < iMaxCnt )
				CheckKeyReserve( pOwner );

			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void	ioTaoistDash::ProcessComboAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	DWORD	dwEndTime	= m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( CheckAniDash_Input_Speical_Dash( pOwner ) )
		return;

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( pOwner->CheckExtendAttackEndJump() )
			return;
		
		if( m_dwReserveEnableEndTime == 0 && m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			int iMaxCnt = pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt > iMaxCnt )
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
			// 예약체크
			int	iMaxCnt	= pOwner->GetExtraDashAttackMax();
			if( m_bSetCombo && m_iCurComboCnt < iMaxCnt )
				CheckKeyReserve( pOwner );

			return;
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
		return;
	}
}

void	ioTaoistDash::ProcessChargingAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwActionEndTime < FRAMEGETTIME() )
	{
		m_TaoistDashState	= TDS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

bool	ioTaoistDash::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return	false;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return	false;

	if( m_szInputEventAnimation.IsEmpty() )
		return	false;

	int	iAniID	= pGrp->GetAnimationIdx( m_szInputEventAnimation );

	std::list<float>	vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return	false;

	DWORD	dwCurTime	= FRAMEGETTIME();
	std::list< float >::iterator	iter	= vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD	dwTime	= (DWORD)*iter;
		dwTime	*= m_fInputEventAnimationRate;

		dwTime	+= m_dwActionStartTime;

		//다른 로직에서 input_jump_s의 기능이 key_reserve를 체크하지 않는데 해당로직에서만 체크하고 있어 버그로 판단하여 수정
		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{				
				pOwner->SetJumpState();
				return	true;
			}

			return	false;
		}

		if( dwTime > dwCurTime )
			return	false;

		iter++;
	}

	return	false;
}

bool	ioTaoistDash::CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return	false;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return	false;

	if( m_szInputEventAnimation.IsEmpty() )
		return	false;

	DWORD	dwCurTime	= FRAMEGETTIME();
	int	iAniID	= pGrp->GetAnimationIdx( m_szInputEventAnimation );		
	DWORD	dwInputSpecialDashTime	= (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime	*= m_fInputEventAnimationRate;
		dwInputSpecialDashTime	+= m_dwActionStartTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		ioAttackableItem*	pAttack	= pOwner->GetPriorityItem( SPT_ATTACK );
		ioTaoistItem*	pTaoistItem	= ToTaoistItem( pAttack );
		if( pTaoistItem && pTaoistItem->IsConditionalSpecialDashReserve( pOwner ) && m_eSpecialDashCurrDirkey != ioUserKeyInput::DKI_NONE &&
			pTaoistItem->GetCurBullet() >= pTaoistItem->GetCancelDashNeedBullet() )
		{			
			pTaoistItem->SetChangeConditionalSpecialDash( pOwner, m_eSpecialDashCurrDirkey, ioAttackableItem::CCSD_DASH_ATTACK );

			m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
			return	true;
		}
		return	false;
	}

	return	false;
}

void	ioTaoistDash::CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsDirKeyDoubleClick() && m_eSpecialDashCurrDirkey == ioUserKeyInput::DKI_NONE )
	{
		m_bEnableJumpState	= false;
		m_bEnableAttackState	= false;
		m_bEnableDefenseState	= false;		
		m_eSpecialDashCurrDirkey	= pOwner->CheckCurDirKey();
		m_iSkillInput	= -1;
		pOwner->ClearDirDoubleClick();
	}
}

void	ioTaoistDash::CheckExtraReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool	bReserveSkill	= false;
	int	iSkillNum	= pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput	= iSkillNum;
		bReserveSkill	= true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState	= true;
		m_bEnableAttackState	= false;
		m_bEnableJumpState	= false;		
		m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;

		m_iSkillInput	= -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState	= true;
		m_bEnableAttackState	= false;
		m_bEnableDefenseState	= false;		
		m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;
		m_iSkillInput	= -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState	= true;
		m_bEnableDefenseState	= false;
		m_bEnableJumpState	= false;		
		m_eSpecialDashCurrDirkey	= ioUserKeyInput::DKI_NONE;

		m_iSkillInput	= -1;
	}
}

void	ioTaoistDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	ioZoneEffectWeapon*	pZone	= ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar*	pChar	= pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3	vDir	= pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}