
#include "stdafx.h"

#include "ioHide2Buff.h"
#include "ItemDefine.h"
#include "ioKisaragiJump.h"
#include "ioDoubleJump.h"

ioHide2Buff::ioHide2Buff()
{
	m_dwStartTime = 0;
	m_fCurSpeed = 0.0f;
	m_bSetVisible = false;
}

ioHide2Buff::ioHide2Buff( const ioHide2Buff &rhs )
: ioBuff( rhs ),
 m_fHideRate( rhs.m_fHideRate ),
 m_fAddSpeed( rhs.m_fAddSpeed )
{
	m_dwStartTime = 0;
	m_fCurSpeed = 0.0f;
	m_bSetVisible = false;
}

ioHide2Buff::~ioHide2Buff()
{
}

void ioHide2Buff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fHideRate = rkLoader.LoadFloat_e( "hide_rate", FLOAT1 );
	m_fAddSpeed = rkLoader.LoadFloat_e( "speed_add_value", 0.0f );
}

ioBuff* ioHide2Buff::Clone()
{
	return new ioHide2Buff( *this );
}

void ioHide2Buff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetHide( true );

	m_fCurSpeed = m_fAddSpeed;
	pOwner->SetExtraMoveSpeed( m_fCurSpeed );
	m_dwStartTime = FRAMEGETTIME();
}

bool ioHide2Buff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	SetHide( true );

	m_fCurSpeed = m_fAddSpeed;
	pOwner->SetExtraMoveSpeed( m_fCurSpeed );
	m_dwStartTime = FRAMEGETTIME();
	
	return true;
}

void ioHide2Buff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	CheckOwnerState();
}

void ioHide2Buff::EndBuff()
{
	SetHide( false );
	m_pOwner->RemoveExtraMoveSpeed( m_fCurSpeed );

	if( m_pOwner->GetState() == CS_DELAY )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( m_pOwner->GetState() == CS_RUN )
	{
		m_pOwner->SetState( CS_RUN );
	}

	ioBuff::EndBuff();
}

void ioHide2Buff::SetHide( bool bHide )
{
	if( !m_pOwner )
		return;

	if( bHide && !m_bSetVisible )
	{
		int iAlphaRate = m_fHideRate * MAX_ALPHA_RATE;
		m_pOwner->SetAlphaRateDirect( iAlphaRate );
		m_pOwner->SetHideChar( true );
		m_bSetVisible = true;
	}
	else if( !bHide && m_bSetVisible )
	{
		m_pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
		m_pOwner->SetHideChar( false );
		m_bSetVisible = false;
	}
}

void ioHide2Buff::CheckOwnerState()
{
	if( !m_pOwner )
		return;

	bool bCharHide = true;

	CharState eState = m_pOwner->GetState();
	switch( eState )
	{
	case CS_ATTACK:
	case CS_DASH_ATTACK:
	case CS_USING_SKILL:
	case CS_WOUNDED:
	case CS_COUNTER_ATTACK:
	case CS_FISHING:
	case CS_EXCAVATING:
	case CS_CONSTRUCT_HOUSE:
	case CS_BUFF_AIR_ATTACK:
	case CS_NEW_JUMP_ATTACK:
	case CS_DEFENSE_ATTACK:
	case CS_GRAPPLING:
	case CS_GRAPPLING_EXPANSION:
	case CS_GRAPPLING_WOUNDED:
	case CS_GRAPPLING_PUSHED:
	case CS_GRAPPLING_WOUNDED_SKILL:
	case CS_GRAPPLING_WOUNDED_SWING:
	case CS_LAND_AFTER_STATE:
	case CS_BLOW_EXTEND_WOUND:
	case CS_BLOW_DASH_ATTACK:
	case CS_JUMP_GRAPPLING:
	case CS_JUMP_GRAPPLING_WOUNDED:
	case CS_ADHESIVE_BUFF:
	case CS_GRAPPLING_WOUNDED_ITEM:
	case CS_GRAPPLING_WOUNDED_BUFF:
	case CS_CMD_STATE:
	case CS_EXTEND_DASH:
	case CS_CHANGE_WAIT:
	case CS_ROULETTE:
		{
			bCharHide = false;
		}
		break;
	case CS_JUMP:
		{
			if( !CheckHideCharOnJumpState() )
				bCharHide = false;
		}
		break;
	case CS_BOUND_BLOW_WOUNDED:
	case CS_BLOW_WOUNDED:
		{
			if( m_pOwner->GetBlowWoundedState() == BW_BLOWING ||
				m_pOwner->GetBlowWoundedState() == BW_BOUND_BLOWING )
				bCharHide = false;
		}
		break;
	case CS_WITCH_FLY:
	case CS_NAKORURU_FLY:
	case CS_MECHANICS_FLY:
	case CS_LUCIFER_FLY:
	case CS_PHANTOM_FLY:
	case CS_DESTROYER_FLY:
	case CS_MICHAEL_FLY:
	case CS_DEVIL_FLY:
	case CS_SUPER_DIMENSION_DASH:
	case CS_JUMPER_SPECIAL:
	case CS_MEGA_AGENT_FLY:
	case CS_SANTA_SPECIAL:
	case CS_SMILE_JOKER_FLY:
	case CS_SLOPE_SPECIAL:
		{
			if( !CheckHideCharOnFlyState() )
				bCharHide = false;
		}
		break;
	}

	SetHide( bCharHide );
}

bool ioHide2Buff::CheckHideCharOnJumpState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	bool bHideChar = true;

	JumpState eJumpState = pOwner->GetJumpState();
	JumpType  eJumpType = pOwner->GetJumpType();
	switch( eJumpState )
	{
	case JS_JUMPPING:
		{
			ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
			if( !pItem )
				return false;

			switch( eJumpType )
			{
			case JT_KISARAGI_JUMP:
				{
					ioKisaragiJump *pJump = ToKisaragiJump( pItem->GetCurExtendJump() );
					if( pJump && pJump->GetJumpState() == ioKisaragiJump::KJS_DASH )
						bHideChar = false;
				}
				break;
			case JT_DOUBLE_JUMP:
				{
					ioDoubleJump *pJump = ToDoubleJump( pItem->GetCurExtendJump() );
					if( pJump && pJump->GetCurJumpState() == ioDoubleJump::DJS_DASH )
						bHideChar = false;
				}
				break;
			}
		}
		break;
	case JS_AIM_JUMP:
	case JS_AIM_JUMP2:
	case JS_JUMP_ATTACK:
		bHideChar = false;
		break;
	case JS_LANDING:
		if( pOwner->GetLandingType() == CLT_LAND_ATTACK ||
			pOwner->GetLandingType() == CLT_EXTEDN_LAND_ATTTACK )
			bHideChar = false;
		break;
	case JS_COMBO_DASH:
		bHideChar = false;
		break;
	}

	return bHideChar;
}

bool ioHide2Buff::CheckHideCharOnFlyState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;
	
	bool bHideChar = true;
	
	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );

	// 위치
	ioWitchItem *pWitch = ToWitchItem( pItem );
	if( pWitch && pWitch->GetWitchFlyState() == ioWitchItem::WFS_ATTACK )
		bHideChar = false;

	// 메카닉
	ioMechanicsItem *pMechanics = ToMechanicsItem( pItem );
	if( pMechanics && pMechanics->GetMechanicsFlyState() == ioMechanicsItem::MFS_ATTACK )
		bHideChar = false;

	// 루시퍼
	ioLuciferItem *pLucifer = ToLuciferItem( pItem );
	if( pLucifer &&
		( pLucifer->GetFlyState() == ioLuciferItem::LFS_FLY_ATTACK ||
		  pLucifer->GetFlyState() == ioLuciferItem::LFS_FLY_GRAP ||
		  pLucifer->GetFlyState() == ioLuciferItem::LFS_LAND_ATTACK ) )
		bHideChar = false;

	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pItem );
	if( pMegaAgent &&
		( pMegaAgent->GetFlyState() == ioMegaAgentItem::MFS_ATTACK ||
		  pMegaAgent->GetFlyState() == ioMegaAgentItem::MFS_CHARGING ||
		  pMegaAgent->GetFlyState() == ioMegaAgentItem::MFS_CHARGE_ATTACK ) )
		bHideChar = false;

	//데빌
	ioDevilItem *pDevilItem = ToDevilItem( pItem );
	if( pDevilItem &&
		( pDevilItem->GetFlyState() != ioDevilItem::DFS_NONE ) )
		bHideChar = false;

	// 귀신
	ioPhantomItem *pPhantom = ToPhantomItem( pItem );
	if( pPhantom &&
		( pPhantom->GetPhantomFlyState() == ioPhantomItem::PFS_TRACE_MOVE ||
		  pPhantom->GetPhantomFlyState() == ioPhantomItem::PFS_POSSESSION ) )
		bHideChar = false;

	// 나코루루
	ioNakoruruItem *pNakoruru = ToNakoruruItem( pItem );
	if( pNakoruru && pNakoruru->GetWitchFlyState() == ioNakoruruItem::WFS_ATTACK )
		bHideChar = false;

	// 디스트로이어
	ioDestroyerItem *pDestroy = ToDestroyerItem( pItem );
	if( pDestroy && 
		( pDestroy->GetDestroyerFlyState() == ioDestroyerItem::CFS_ATTACK ||
		  pDestroy->GetDestroyerFlyState() == ioDestroyerItem::CFS_CHARGING ||
		  pDestroy->GetDestroyerFlyState() == ioDestroyerItem::CFS_CHARGED ||
		  pDestroy->GetDestroyerFlyState() == ioDestroyerItem::CFS_CHARGE_ATTACK ) )
		bHideChar = false;

	// 미카엘
	ioMichaelItem *pMichael = ToMichaelItem( pItem );
	if( pMichael &&
		(pMichael->GetMichaelFlyState() == ioMichaelItem::MFS_ATTACK_MOVE ||
		 pMichael->GetMichaelFlyState() == ioMichaelItem::MFS_GRAPPLING_LOOP ||
		 pMichael->GetMichaelFlyState() == ioMichaelItem::MFS_GRAPPLING_MOVE ||
		 pMichael->GetMichaelFlyState() == ioMichaelItem::MFS_END_ATTACK ) )
		bHideChar = false;

	ioMephistoItem* pMephistoItem = ToMephistoItem( pItem );
	if( pMephistoItem && pMephistoItem->IsFlyState() )
		return false;

	return bHideChar;
}