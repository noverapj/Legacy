

#include "stdafx.h"

#include "ioNpcChar.h"

#include "ioPositionBuff2.h"
#include "ioPlayStage.h"

ioPositionBuff2::ioPositionBuff2()
{
}

ioPositionBuff2::ioPositionBuff2( const ioPositionBuff2 &rhs )
	: ioBuff( rhs ),
	m_bDisableDownState( rhs.m_bDisableDownState ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
	//m_dwDummyNum( rhs.m_dwDummyNum ),
	m_stEraseBuff( rhs.m_stEraseBuff ),
	m_stControlBuff( rhs.m_stControlBuff ),
	m_nTotalAllowCnt( rhs.m_nTotalAllowCnt ),
	m_CheckBuffName( rhs.m_CheckBuffName ),
	m_ExceptCallBuffName( rhs.m_ExceptCallBuffName )
{
	m_bSafeCreate = true;
}

ioPositionBuff2::~ioPositionBuff2()
{
}

void ioPositionBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	//m_dwDummyNum = (DWORD)rkLoader.LoadInt_e( "find_dummy_num", 0 );
	rkLoader.LoadString_e( "erase_buff_name", "", szBuf, MAX_PATH );
	m_stEraseBuff = szBuf;

	rkLoader.LoadString_e( "control_buff", "", szBuf, MAX_PATH );
	m_stControlBuff = szBuf;

	m_nTotalAllowCnt = rkLoader.LoadInt_e( "total_allow_cnt", 0 );
	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_CheckBuffName = szBuf;

	rkLoader.LoadString_e( "except_call_buff", "", szBuf, MAX_PATH );
	m_ExceptCallBuffName = szBuf;
}

ioBuff* ioPositionBuff2::Clone()
{
	return new ioPositionBuff2( *this );
}

void ioPositionBuff2::StartBuff( ioBaseChar *pOwner )
{
	m_nFirstDummyID = -1;

	ioBuff::StartBuff( pOwner );

	CheckBuffCnt();

	if( m_bSafeCreate && m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioPositionBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}


void ioPositionBuff2::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_LOADING &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_VIEW )
	{
		pOwner->SetState( CS_LOCKUP_BUFF );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		ioBaseChar *pCreater = m_pOwner->GetBaseChar(m_CreateChar);
		if( pCreater == NULL )//|| m_dwDummyNum == 0 )
		{
			SetReserveEndBuff();
			return;
		}

		if( !IsLive() )
			return;
	}
}


void ioPositionBuff2::ProcessBuff( float fTimePerSec )
{
	if( m_bSafeCreate == false )
	{
		SetReserveEndBuff();
		return;
	}

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

	if( m_CreateChar.IsEmpty() )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreater = m_pOwner->GetBaseChar(m_CreateChar);
	if( pCreater == NULL )//|| m_dwDummyNum == 0 )
	{
		SetReserveEndBuff();
		return;
	}

	if( !IsLive() )
		return;


	if( m_stControlBuff.IsEmpty() || !pCreater->HasBuff(m_stControlBuff) )
	{
		SetReserveEndBuff();
		return;
	}

	D3DXVECTOR3 vPos = pCreater->GetWorldPosition();
	//vPos.y += 50.0f;
	m_pOwner->SetWorldPosition( vPos );
}


void ioPositionBuff2::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( m_bSafeCreate )
	{
		if( pOwner )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );

			if( !m_stEraseBuff.IsEmpty() && pOwner->HasBuff(m_stEraseBuff) )
				pOwner->RemoveBuff( m_stEraseBuff );
		}

		ioBuff::EndBuff();
	}
	else
	{
		if( pOwner && !m_stEraseBuff.IsEmpty() && pOwner->HasBuff(m_stEraseBuff) )
			pOwner->RemoveBuff( m_stEraseBuff );

		EndCall();
	}
}

void ioPositionBuff2::CheckBuffCnt()
{
	m_bSafeCreate = true;

	ioPlayStage *pStage = m_pOwner->GetCreator();
	if( !pStage )	
		return;

	const GameEntityList &rkTargetList = pStage->GetGameEntityList();
	GameEntityList::const_iterator iter;
	int nCnt = 0;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = ToBaseChar(*iter);
		ioNpcChar  *pNcp = ToNpcChar(*iter);
		if( pChar && pChar->HasBuff( m_CreateChar, m_CheckBuffName ) )
			nCnt++;

		if( pNcp && pNcp->HasBuff( m_CreateChar, m_CheckBuffName ) )
			nCnt++;

		if( nCnt >= m_nTotalAllowCnt )// > 가 아닌 >= 인것은 현재 버프는 아직 cnt가 안되기 때문에 -1의미로 한것
		{
			m_bSafeCreate = false;
			return;
		}
	}
}


void ioPositionBuff2::SetSafeCreate( bool b )
{
	m_bSafeCreate = b;
}


void ioPositionBuff2::EndCall()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( m_pOwner )
	{
		for( int iSlot = ES_WEAPON ; iSlot <= ES_CLOAK; ++iSlot )
		{
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill( m_pOwner->GetEquipedSkill( iSlot ) );
			if( pPassiveSkill && pPassiveSkill->GetName() == m_SkillName && pPassiveSkill->IsBuffEndGaugeClear( GetName() ) )
				pPassiveSkill->SetCurPassiveGauge( 0.0f );
		}
	}

	CheckInvisibleWeapon( true );
	CheckInvisibleCloak( true );
	CheckInvisibleHelmet( true );

	CheckInvisibleState( true );

	CheckChangeSkill( false );
	CheckChangeColor( false );

	m_pOwner->EndEffect( m_BuffEffect, false );
	m_pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );

	if( m_bDescEqualDuration )
		m_pOwner->EndEmoticonByName( m_DescName );

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}

	if( IsBadStateBuff() )
	{
		if( !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
	}
	else
	{
		if( !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
	}

	DestroyAreaWeapon();

	if( !m_ExceptCallBuffName.IsEmpty() )
		m_pOwner->ReserveAddNewBuff( m_ExceptCallBuffName, m_CreateChar, m_ItemName, NULL );

	//CheckBuffEndBuff();
	CheckCallWeaponEnd();
}