

#include "stdafx.h"

#include "ioWeaponAttachBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioWeaponAttachBuff::ioWeaponAttachBuff()
{
}

ioWeaponAttachBuff::ioWeaponAttachBuff( const ioWeaponAttachBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_vMapCollisionBuff( rhs.m_vMapCollisionBuff ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation )
{
}

ioWeaponAttachBuff::~ioWeaponAttachBuff()
{
}

void ioWeaponAttachBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";
	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;

	int iSize = rkLoader.LoadInt_e( "map_collision_buff_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "map_collision_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vMapCollisionBuff.push_back( szBuf );
	}
}

ioBuff* ioWeaponAttachBuff::Clone()
{
	return new ioWeaponAttachBuff( *this );
}

void ioWeaponAttachBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetAction();
}

bool ioWeaponAttachBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	SetAction();

	return true;
}

void ioWeaponAttachBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	if( !CheckWeaponAttach() )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioWeaponAttachBuff::SetAction()
{
	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );

		m_pOwner->SetState( CS_ATTACH_WEAPON );
	}
}

bool ioWeaponAttachBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioWeaponAttachBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

bool ioWeaponAttachBuff::CheckWeaponAttach()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACH_WEAPON )
		return false;

	if( m_dwCreateWeaponIndex == 0 )
		return false;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return false;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, m_dwCreateWeaponIndex );
	if( !pWeapon )
		return false;

	if( !pWeapon->IsLive() )
		return false;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return false;

	D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vNewPos = pWeapon->GetPosition();
	D3DXVECTOR3 vMove = vNewPos - vPos;
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );

	bool bCol = false;
	if( vNewPos.y <= fMapHeight )
	{
		if( vPos.y <= fMapHeight )
			vPos.y = fMapHeight;

		pOwner->SetWorldPosition( vPos );
		bCol = true;
	}

	if( !bCol && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMove, bCol ) )
	{
		pOwner->Translate( vMove );

		vPos = pOwner->GetWorldPosition();

		if( pOwner->CheckAxisLimit( &vNewPos ) )
			pOwner->SetWorldPosition( vPos );
	}

	if( bCol )
	{
		if( pOwner->IsNeedProcess() )
		{
			ioHashStringVec vBuffList;
			vBuffList.clear();

			int iSize = m_vMapCollisionBuff.size();
			for( int i=0; i<iSize; ++i )
			{
				//pOwner->ReserveAddNewBuff( m_vMapCollisionBuff[i], "", "", NULL );
				ioBuff *pBuff = pOwner->AddNewBuff( m_vMapCollisionBuff[i], "", "", NULL );
				if( pBuff )
					vBuffList.push_back( m_vMapCollisionBuff[i] );
			}

			int iBuffCnt = vBuffList.size();
			if(iBuffCnt > 0 )
			{
				if( pOwner->IsNeedSendNetwork() )
				{
					if( Help::CheckEnableTimeGapUser(4) )
					{
						LOG.PrintTimeAndLog( 0, "PC -BuffAddBuff1 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
					}

					SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
					kPacket << pOwner->GetCharName();
					kPacket << "";
					kPacket << "";
					kPacket << pOwner->GetRandomSeed();
					kPacket << false;		// 타격횟수 체크
					kPacket << 0.0f;
					kPacket << 0.0f;
					kPacket << 0.0f;
					kPacket << COWT_NONE;
					kPacket << iBuffCnt;

					for( i=0; i<iBuffCnt; ++i )
					{
						kPacket << vBuffList[i];
					}

					kPacket << false;					// Use Force

					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}

		return false;
	}

	return true;
}

void ioWeaponAttachBuff::EndBuff()
{
	ioBuff::EndBuff();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( pOwner->GetState() == CS_ATTACH_WEAPON )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			pOwner->SetDashFailStart();
		}
	}
}