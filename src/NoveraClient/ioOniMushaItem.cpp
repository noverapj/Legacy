
#include "stdafx.h"

#include "ioOniMushaItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioOniMushaItem::ioOniMushaItem()
{
	m_fCurExtraGauge = 0.0f;
	m_bEquipObjectItem = false;

	Init();
}

ioOniMushaItem::ioOniMushaItem( const ioOniMushaItem &rhs )
	: ioWeaponItem( rhs ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_dwObjectItem( rhs.m_dwObjectItem ),
	  m_szPreGatherAniList( rhs.m_szPreGatherAniList ),
	  m_szGatherAni( rhs.m_szGatherAni ),
	  m_fGatherAniRate( rhs.m_fGatherAniRate ),
	  m_EquipAttribute( rhs.m_EquipAttribute ),
	  m_JumpEquipAttribute( rhs.m_JumpEquipAttribute ),
	  m_DashEquipAttribute( rhs.m_DashEquipAttribute ),
	  m_fRunSpeed( rhs.m_fRunSpeed ),
	  m_dwRunRotateTime( rhs.m_dwRunRotateTime ),
	  m_RunWaitAni( rhs.m_RunWaitAni ),
	  m_fRunWaitAniRate( rhs.m_fRunWaitAniRate ),
	  m_RunLoopAni( rhs.m_RunLoopAni ),
	  m_fRunLoopAniRate( rhs.m_fRunLoopAniRate ),
	  m_dwRunLoopTime( rhs.m_dwRunLoopTime ),
	  m_RunAttack( rhs.m_RunAttack ),
	  m_RunEndAttack( rhs.m_RunEndAttack ),
	  m_RunBuffList( rhs.m_RunBuffList ),
	  m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip ),
	  m_fChargeUseGauge( rhs.m_fChargeUseGauge )
{
	m_fCurExtraGauge = 0.0f;
	m_bEquipObjectItem = false;

	Init();
}

ioOniMushaItem::~ioOniMushaItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );

	m_dwObjectItem = static_cast<DWORD>( rkLoader.LoadInt_e( "object_item", 0 ) );
	
	int nPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i<nPreCount; i++ )
	{
		wsprintf_e( szKey, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szPreGatherAniList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_szGatherAni = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gather_animation_rate", FLOAT1 );

	LoadAttackAttribute( "equip", m_EquipAttribute, rkLoader );
	LoadAttackAttribute( "jump_equip", m_JumpEquipAttribute, rkLoader );
	LoadAttackAttribute( "dash_equip", m_DashEquipAttribute, rkLoader );

	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRunRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );
	m_dwRunLoopTime = (DWORD)rkLoader.LoadInt_e( "charge_run_duration", 0 );

	rkLoader.LoadString_e( "charge_run_wait_ani", "", szBuf, MAX_PATH );
	m_RunWaitAni = szBuf;
	m_fRunWaitAniRate = rkLoader.LoadFloat_e( "charge_run_wait_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_loop_ani", "", szBuf, MAX_PATH );
	m_RunLoopAni = szBuf;
	m_fRunLoopAniRate = rkLoader.LoadFloat_e( "charge_run_loop_ani_rate", FLOAT1 );
	
	LoadAttackAttribute( "charge_run_attack", m_RunAttack, rkLoader );
	LoadAttackAttribute( "charge_run_end_attack", m_RunEndAttack, rkLoader );
	LoadRunBuff( rkLoader, "charge_run", m_RunBuffList );

	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );

	m_fChargeUseGauge = rkLoader.LoadFloat_e( "use_charge_gauge", 0.0f );
}

ioItem* ioOniMushaItem::Clone()
{
	return new ioOniMushaItem( *this );
}

ioWeaponItem::WeaponSubType ioOniMushaItem::GetSubType() const
{
	return WST_ONIMUSHA_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioOniMushaItem::GetAttackStateText() const
{
	switch( m_ChargeState )
	{
	case CS_NONE:
		return "CS_NONE";
	case CS_CHARGING:
		return "CS_CHARGING";
	case CS_GATHERING:
		return "CS_GATHERING";
	case CS_ATTACK_FIRE:
		return "CS_ATTACK_FIRE";
	case CS_EQUIP_OBJECT:
		return "CS_EQUIP_OBJECT";
	case CS_END:
		return "CS_END";
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	Init();
	
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_nCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iPreGatherCnt = m_szPreGatherAniList.size();

	ioHashString szPreAni;
	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		szPreAni = m_szPreGatherAniList[iCurCombo];
		
		int iAniID = pGrp->GetAnimationIdx( szPreAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioOniMushaItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
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
	case CS_CHARGE_RUN:
		OnRunState( pOwner );
		break;
	}
}

bool ioOniMushaItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "%s - AttackAttribute Not Exist", __FUNCTION__ );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - JumpAttack Animation Not Exist", __FUNCTION__ );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioOniMushaItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioOniMushaItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
		
	if( m_bEquipObjectItem )
	{
		if( m_fExtraGaugeBaseDec <= 0.0f )
			return;
	}
	else
	{
		if( m_fExtraGaugeBaseInc <= 0.0f )
			return;
	}

	if( pOwner->IsSystemState() )
		return;
		
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( m_bEquipObjectItem )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	}
	else
	{
		switch( pOwner->GetState() )
		{
		case CS_DELAY:
		case CS_RUN:
		case CS_DASH:
			{
				float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioOniMushaItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

void ioOniMushaItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseObject( pOwner );

	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioOniMushaItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	m_bEquipObjectItem = false;
	m_ChargeState = CS_NONE;
}

void ioOniMushaItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case ST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;
	case ST_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case ST_OBJECT_EQUIP:
		{
			byte eObjectState;
			rkPacket >> eObjectState;
			ChangeEquipObjectState( pOwner, static_cast<EquipObjectState>( eObjectState ) );
		}
		break;
	case ST_RUN_START:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToRunState( pOwner );
		}
		break;
	case ST_RUN_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			pOwner->SetState( CS_DELAY );
		}
		break;
	case ST_INPUT_JUMP:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			Init();

			RemoveRunBuff( pOwner );
			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		break;
	case ST_INPUT_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			RemoveRunBuff( pOwner );
			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			ChangeToRunAttack( pOwner );
		}
		break;
	case ST_ROTATE:
		{
			int nDir;
			rkPacket >> nDir;
			rkPacket >> m_bLeftRot;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nDir;
			m_bTargetRot = false;

			D3DXVECTOR3 vNewPos;
			rkPacket >> vNewPos;

			D3DXQUATERNION qtNewRot;
			rkPacket >> qtNewRot;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtNewRot );
		}
		break;
	}	
}

void ioOniMushaItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	if( m_ChargeState == CS_EQUIP_OBJECT )
		ReleaseObject( pOwner );
	else
		m_ChargeState = CS_NONE;
}

void ioOniMushaItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_ONIMUSHA )
		{
			if( m_bEquipObjectItem )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_ONIMUSHA )
		{
			if( m_bEquipObjectItem )
				fTypeRate = m_fExtraGaugeAttackDefDec;
			else
				fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		{
			if( m_bEquipObjectItem )
				fTypeRate = m_fExtraGaugeWoundDec;
			else
				fTypeRate = m_fExtraGaugeWound;
		}
		break;
	case EGT_WOUNDED_DEF:
		{
			if( m_bEquipObjectItem )
				fTypeRate = m_fExtraGaugeWoundDefDec;
			else
				fTypeRate = m_fExtraGaugeWoundDef;
		}
		break;
	}

	if ( m_bEquipObjectItem )
		fTypeRate = -fTypeRate;

	m_fCurExtraGauge += fTypeRate;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

void ioOniMushaItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	RemoveRunBuff( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioOniMushaItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioOniMushaItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioOniMushaItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioOniMushaItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

float ioOniMushaItem::GetChargeUseGauge()
{
	return m_fChargeUseGauge;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::Init()
{
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate	= 0.0f;

	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_nCurCombo = 0;
	m_dwMotionEndTime = 0;

	m_dwRunLoopEndTime = 0;
	
	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
}

void ioOniMushaItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && m_fCurExtraGauge > m_fChargeUseGauge )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToGathering( pOwner );
	}
	else
		ChangeToAttackFire( pOwner );
}

void ioOniMushaItem::OnGathering( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	DWORD dwCurTime	= FRAMEGETTIME();

	if( ( m_fCurExtraGauge >= m_fNeedGauge && pOwner->IsAttackKeyDown() ) && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionEndTime < dwCurTime )
			ChangeEquipObjectState( pOwner );
	}
	else
		ChangeToRunState( pOwner );
}

void ioOniMushaItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_nCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_ATTACK );
		kPacket << m_nCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOniMushaItem::ChangeToGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szGatherAni );
	float fTimeRate = m_fGatherAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_GATHERING );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioOniMushaItem::ChangeToEquipObject( ioBaseChar* pOwner, EquipObjectState eState )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	AttackAttribute* pAttribute = NULL;
	switch( eState )	
	{
	case EOS_NORMAL:
		{
			pAttribute = &m_EquipAttribute;	
		}
		break;
	case EOS_JUMP:
		{
			pAttribute = &m_JumpEquipAttribute;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		break;
	case EOS_DASH:
		{	
			pAttribute = &m_DashEquipAttribute;
		}
		break;
	}

	if( !pAttribute )
		return false;

	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;

	pOwner->SetObjectEquipState( pAttribute );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioOniMushaItem::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGE_RUN:
			{
				if( m_RunState == RS_RUN )
					return true;
			}
			break;
		}
	}

	return false;
}

void ioOniMushaItem::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
		
	int nAniID = pGrp->GetAnimationIdx( m_RunWaitAni );
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fRunWaitAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * m_fRunWaitAniRate;

	pOwner->SetAttackMoveEnable( false );
	
	m_ChargeState = CS_CHARGE_RUN;
	m_RunState = RS_WAIT;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	
	AddRunBuff( pOwner );

	SetUseExtraGauge( m_fChargeUseGauge );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_RUN_START );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioOniMushaItem::ChangeToRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
			
	int nAniID = pGrp->GetAnimationIdx( m_RunLoopAni );
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	pGrp->SetLoopAni( nAniID, FLOAT100, FLOAT1, FLOAT1 / m_fRunLoopAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunLoopEndTime = dwCurTime + m_dwRunLoopTime;
	
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunLoopEndTime, m_bOnlyTeamCharColSkip );

	m_RunState = RS_RUN;
}

void ioOniMushaItem::ChangeToRunAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_RunAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = m_RunAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_RunAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_RunAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	SetCurAttackAniForInputJump( m_RunAttack.m_AttackAnimation, fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	m_RunState = RS_RUN_ATTACK;
}

void ioOniMushaItem::ChangeToRunEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	int nAniID = pGrp->GetAnimationIdx( m_RunEndAttack.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = m_RunEndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_RunEndAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_RunEndAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	SetCurAttackAniForInputJump( m_RunEndAttack.m_AttackAnimation, fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetCharCollisionSkipTime( 0, 0 );
	pOwner->SetChargingState( false );

	m_RunState = RS_END;

	RemoveRunBuff( pOwner );
}

void ioOniMushaItem::OnRunState( ioBaseChar *pOwner )
{
	switch( m_RunState )
	{
	case RS_WAIT:
		OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_RUN_ATTACK:
		OnRunAttack( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	}
}

void ioOniMushaItem::OnRunWait( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
		ChangeToRunning( pOwner );
}

void ioOniMushaItem::OnRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunLoopEndTime > 0 && m_dwRunLoopEndTime < dwCurTime )
	{
		m_dwRunLoopEndTime = 0;
		ChangeToRunEnd( pOwner );
	}
	else
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;

		if( pOwner->IsNeedProcess() )
			pStage->SetTargetLookAt( true );

		pOwner->SetCurMoveSpeed( m_fRunSpeed );

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
	}
}

void ioOniMushaItem::OnRunAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}
	
	if( m_dwMotionEndTime < dwCurTime )
		CheckReserveState( pOwner );
	else 
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			ReserveInputKey( pOwner, true, true, false );
	}
}

void ioOniMushaItem::OnRunEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	if( m_dwMotionEndTime < dwCurTime )
		CheckReserveState( pOwner );
	else 
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			ReserveInputKey( pOwner, true, true, false );
	}
}

void ioOniMushaItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	if ( pOwner->IsJumpKey() )
	{
		Init();

		RemoveRunBuff( pOwner );
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_INPUT_JUMP );
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		pOwner->SetJumpState();
		return;
	}
	else if ( pOwner->IsAttackKey() )
	{
		RemoveRunBuff( pOwner );
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_INPUT_ATTACK );
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		ChangeToRunAttack( pOwner );
		return;
	}

	CheckDirKeyInput( pOwner );
}

void ioOniMushaItem::CheckDirKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_bTargetRot = false;
			m_bLeftRot = false;

			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
				float fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fCurYaw = ioMath::ArrangeHead( fCurYaw );

				float fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( ST_ROTATE );
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<byte>( ST_ROTATE );
				kPacket << m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioOniMushaItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_dwRunRotateTime <= 0 || m_bTargetRot || m_CurDirKey == ioUserKeyInput::DKI_NONE ) 
		return;

	float fRate = g_FrameTimer.GetCurFrameGap() / m_dwRunRotateTime;
	float fRotateAngle = 360.0f * fRate;

	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( bEnableAttack && pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( bEnableDefence && pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( bEnableJump && pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioOniMushaItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if( m_bAttackKeyReserved )
	{
		Init();
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		Init();
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		Init();
		SetReserveJump( pOwner );
		return;
	}

	Init();
	pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_RUN_END );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioOniMushaItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioOniMushaItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioOniMushaItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

bool ioOniMushaItem::IsNoDropState( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_CHARGE_RUN )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::ChangeEquipObjectState( ioBaseChar *pOwner, EquipObjectState eState )
{
	if( !pOwner )
		return;

	if( !ChangeToEquipObject( pOwner, eState ) )
	{
		ChangeToRunState( pOwner );
		return;
	}

	m_ChargeState = CS_EQUIP_OBJECT;
	m_bEquipObjectItem = true;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && m_dwObjectItem > 0 )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kUdpPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kUdpPacket );
			kUdpPacket << static_cast<byte>( ST_OBJECT_EQUIP );
			kUdpPacket << static_cast<byte>( eState );
			P2PNetwork::SendToAllPlayingUser( kUdpPacket );

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
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pOwner->EquipItem( pObject );
			}
		}
	}
}

bool ioOniMushaItem::IsEnableEquipObject()
{
	if ( m_ChargeState == CS_EQUIP_OBJECT )
		return false;

	if ( m_bEquipObjectItem )
		return false;

	if ( m_fCurExtraGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioOniMushaItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
			return true;
	}

	return false;
}

void ioOniMushaItem::ReleaseObject( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
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

			m_bEquipObjectItem = false;
			m_ChargeState = CS_NONE;			
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::FillOnimushaInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << static_cast<BYTE>( m_ChargeState );
	rkPacket << m_fCurExtraGauge;
}

void ioOniMushaItem::SetOnimushaInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE eState = 0;
	float fGauge = 0.0f;

	rkPacket >> eState;
	rkPacket >> fGauge;

	if ( !pOwner )
		return;

	m_ChargeState = static_cast<ChargeState>( eState );
	SetCurExtraGauge( fGauge );

	ChangeEquipObjectState( pOwner, EOS_NONE );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && eState == CS_EQUIP_OBJECT )
	{
		ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
		ioObjectItem *pObject = ToObjectItem( pItem );
		if( pObject )
		{
			pObject->ReLoadProperty();			
			pObject->SetObjectCreateItem( GetName() );
			pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
			pOwner->EquipItem( pObject );
		}
	}
}

bool ioOniMushaItem::CheckReleaseObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;
	
	if ( !m_bEquipObjectItem )
		return false;

	if( GetCurBullet() > 0 )
		return false;

	switch ( pOwner->GetState() )
	{
	case CS_ATTACK:
	case CS_JUMP:
	case CS_EXTEND_DASH:
	case CS_OBJECT_EQUIP:
	case CS_ONIMUSHA_RUN_EXTRA_STATE:
		return false;
	}

	ReleaseObject( pOwner );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioOniMushaItem::LoadRunBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList )
{
	char szBuff[MAX_PATH], szKey[MAX_PATH];

	BuffList.clear();

	wsprintf_e( szKey, "%s_buff_cnt", szName );
	int nMaxCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szKey, "%s_buff%d", szName, i+1 );
		rkLoader.LoadString( szKey, "", szBuff, MAX_PATH );
		BuffList.push_back( szBuff );
	}
}

void ioOniMushaItem::AddRunBuff( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;
	
	if( m_RunBuffList.empty() ) 
		return;

	int nMaxCnt = m_RunBuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
		pOwner->AddNewBuff( m_RunBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioOniMushaItem::RemoveRunBuff( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;
	
	if( m_RunBuffList.empty() ) 
		return;

	int nMaxCnt = m_RunBuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
	{
		ioBuff* pBuff = pOwner->GetBuff( m_RunBuffList[i] );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}