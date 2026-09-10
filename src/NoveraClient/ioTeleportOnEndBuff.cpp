

#include "stdafx.h"

#include "ioTeleportOnEndBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioTeleportOnEndBuff::ioTeleportOnEndBuff()
{
	m_vCheckPosition = ioMath::VEC3_ZERO;
	m_dwTeleportTargetEffectID = -1;
	m_bCheckOverDist = false;
}

ioTeleportOnEndBuff::ioTeleportOnEndBuff( const ioTeleportOnEndBuff &rhs )
: ioBuff( rhs ),
  m_TeleportTargetEffect( rhs.m_TeleportTargetEffect ),
  m_TeleportStartEffect( rhs.m_TeleportStartEffect ),
  m_TeleportEndEffect( rhs.m_TeleportEndEffect ),
  m_fJumpPower( rhs.m_fJumpPower ),
  m_bBuffEndBuffOnTimeEnd( rhs.m_bBuffEndBuffOnTimeEnd ),
  m_BuffEndSound( rhs.m_BuffEndSound ),
  m_fLimitDistance( rhs.m_fLimitDistance ),
  m_bReturnToOverDist( rhs.m_bReturnToOverDist )
{
	m_vCheckPosition = ioMath::VEC3_ZERO;
	m_dwTeleportTargetEffectID = -1;
	m_bCheckOwnerItem = true;
	m_bCheckOverDist = false;
}

ioTeleportOnEndBuff::~ioTeleportOnEndBuff()
{
}

void ioTeleportOnEndBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "teleport_target_effect", "", szBuf, MAX_PATH );
	m_TeleportTargetEffect = szBuf;
	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_TeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_TeleportEndEffect = szBuf;
	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	
	m_bBuffEndBuffOnTimeEnd = rkLoader.LoadBool_e( "buff_end_buff_on_time_end", false );
	rkLoader.LoadString_e( "buff_end_sound", "", szBuf, MAX_PATH );
	m_BuffEndSound = szBuf;

	m_fLimitDistance = rkLoader.LoadFloat_e( "limit_distance", 0.0f );
	m_bReturnToOverDist = rkLoader.LoadBool_e( "return_to_over_distance", false );
}

ioBuff* ioTeleportOnEndBuff::Clone()
{
	return new ioTeleportOnEndBuff( *this );
}

void ioTeleportOnEndBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();

	m_vCheckPosition = pOwner->GetWorldPosition();
	if( pStage && m_vCheckPosition != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_TeleportTargetEffect, m_vCheckPosition, vScale );
		if( pMapEffect )
			m_dwTeleportTargetEffectID = pMapEffect->GetUniqueID();
	}

	m_bCheckOwnerItem = true;
	m_bCheckOverDist = false;
}

bool ioTeleportOnEndBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_vCheckPosition != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_TeleportTargetEffect, m_vCheckPosition, vScale );
		if( pMapEffect )
			m_dwTeleportTargetEffectID = pMapEffect->GetUniqueID();
	}

	m_bCheckOwnerItem = true;
	m_bCheckOverDist = false;
	return true;
}

void ioTeleportOnEndBuff::ProcessBuff( float fTimePerSec )
{
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

	CheckDistance();
}

void ioTeleportOnEndBuff::EndBuff()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	ioBaseChar *pOwner = GetOwner();

	CheckInvisibleWeapon( true );
	CheckInvisibleCloak( true );
	CheckInvisibleHelmet( true );
	CheckChangeSkill( false );
	CheckChangeColor( false );

	if( pOwner )
	{
		pOwner->EndEffect( m_BuffEffect, false );
		pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );
	}

	if( pOwner && m_bDescEqualDuration )
		pOwner->EndEmoticonByName( m_DescName );

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}

	if( IsBadStateBuff() )
	{
		if( pOwner && !pOwner->IsOwnerChar() )
			pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
	}
	else
	{
		if( pOwner && !pOwner->IsOwnerChar() )
			pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
	}

	DestroyAreaWeapon();

	if( m_bBuffEndBuffOnTimeEnd )
	{
		if( pOwner && pOwner->GetCurItemSkillGauge( m_ItemName ) <= 0 )
			CheckBuffEndBuff();
	}
	else
		CheckBuffEndBuff();

	CheckCallWeaponEnd();

	if( !pOwner )	return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	if( m_dwTeleportTargetEffectID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwTeleportTargetEffectID );
		if( pMapEffect )
		{
			pMapEffect->EndEffectForce();
			m_dwTeleportTargetEffectID = -1;
		}
	}

	if( m_vCheckPosition == ioMath::VEC3_ZERO )
		return;

	if( pOwner->IsSystemState() )
		return;
			
	if( !pOwner->IsEnableTarget() )
		return;

	if( !m_bCheckOwnerItem )
	{
		return;
	}

	if( !m_bReturnToOverDist && m_bCheckOverDist )
		return;

	if( m_bRemoveEquipSkillBuff )
	{
		return;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	pStage->CreateMapEffect( m_TeleportStartEffect, vPos, vScale );
	pStage->CreateMapEffect( m_TeleportEndEffect, m_vCheckPosition, vScale );

	pOwner->SetWorldPosition( m_vCheckPosition );
	
	if( pOwner->IsNeedProcess() && pOwner->IsOwnerChar() )
		pStage->SetCameraToOwnerExplicit( pOwner );
	
	float fHeightGap = m_vCheckPosition.y - m_pOwner->GetBottomHeight();
	if( fHeightGap >= 5.0f )
	{
		pOwner->SetSKillEndJumpState( m_fJumpPower, false, false, false );
	}
	else
	{
		pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp && !m_BuffEndSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_BuffEndSound, pGrp->GetParentSceneNode() );
	}

	m_vCheckPosition = ioMath::VEC3_ZERO;
}

void ioTeleportOnEndBuff::CheckDistance()
{
	if( !m_pOwner )	return;
	if( m_vCheckPosition == ioMath::VEC3_ZERO )	return;

	D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vDist = vPos - m_vCheckPosition;
	float fLength = D3DXVec3Length( &vDist );

	if( fLength > m_fLimitDistance )
	{
		SetReserveEndBuff();
		m_bCheckOverDist = true;
	}
}

void ioTeleportOnEndBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_vCheckPosition;
}

void ioTeleportOnEndBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_vCheckPosition;
}