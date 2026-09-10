

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioEscapeDropZoneBuff.h"

ioEscapeDropZoneBuff::ioEscapeDropZoneBuff()
{
}

ioEscapeDropZoneBuff::ioEscapeDropZoneBuff( const ioEscapeDropZoneBuff &rhs )
: ioBuff( rhs ),
  m_RedCircle( rhs.m_RedCircle ),
  m_BlueCircle( rhs.m_BlueCircle ),
  m_ReadyCircle( rhs.m_ReadyCircle ),
  m_EscapeEndBuff( rhs.m_EscapeEndBuff ),
  m_EscapeSound( rhs.m_EscapeSound ),
  m_dwCircleDuration( rhs.m_dwCircleDuration ),
  m_fCircleRange( rhs.m_fCircleRange ),
  m_fCircleOffset( rhs.m_fCircleOffset ),
  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
  m_fDamageRate( rhs.m_fDamageRate )
{
}

ioEscapeDropZoneBuff::~ioEscapeDropZoneBuff()
{
}

void ioEscapeDropZoneBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "escape_end_buff", "", szBuf, MAX_PATH );
	m_EscapeEndBuff = szBuf;
	rkLoader.LoadString_e( "escape_sound", "", szBuf, MAX_PATH );
	m_EscapeSound = szBuf;

	m_dwCircleDuration = (DWORD)rkLoader.LoadInt_e( "magic_circle_duration", 0 );
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleOffset = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );

	m_fDamageRate = rkLoader.LoadFloat_e( "system_damage_rate", FLOAT1 );
}

ioBuff* ioEscapeDropZoneBuff::Clone()
{
	return new ioEscapeDropZoneBuff( *this );
}

void ioEscapeDropZoneBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	m_vLastMapPosition = ioMath::VEC3_ZERO;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwMagicCircleStartTime = 0;
	m_BuffState = BS_ON_MAP;
	m_dwCurMapEffect = -1;
}

bool ioEscapeDropZoneBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	m_vLastMapPosition = ioMath::VEC3_ZERO;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwMagicCircleStartTime = 0;
	m_BuffState = BS_ON_MAP;
	m_dwCurMapEffect = -1;

	return true;
}

void ioEscapeDropZoneBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
	case OT_DELAY:
	case OT_ACTIVE:
		break;
	case OT_PASSIVE:
		break;
	}

	switch( m_BuffState )
	{
	case BS_ON_MAP:
		ProcessOnMap();
		break;
	case BS_ON_DROP_ZONE:
		ProcessOnDropZone();
		break;
	}
	
}

bool ioEscapeDropZoneBuff::CheckRestoreStateCheck()
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

void ioEscapeDropZoneBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioEscapeDropZoneBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	rkPacket >> m_vCirclePos;

	m_BuffState = BS_ON_MAP;
	SetTeleport( pOwner );
}

void ioEscapeDropZoneBuff::ProcessOnMap()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f );
	if( fMapHeight > 0.0f )
	{
		vCurPos.y = pOwner->GetBottomHeight();
		m_vLastMapPosition = vCurPos;
	}
	else if( fMapHeight <= 0.0f )
	{
		if( pOwner->GetState() == CS_DROP_ZONE_DOWN && pOwner->GetBlowWoundedState() == BW_BLOW_ENDED )
		{
			SetEscapeDropZone();
		}
	}
}

void ioEscapeDropZoneBuff::SetEscapeDropZone()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	m_BuffState = BS_ON_DROP_ZONE;
	m_dwMagicCircleStartTime = FRAMEGETTIME();

	if( !pOwner->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_vLastMapPosition, m_fCircleOffset );

		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
			pStage->SetCameraToOwnerExplicit( pOwner );
	}

	pOwner->SetHideChar( true );
}

void ioEscapeDropZoneBuff::ProcessOnDropZone()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	pOwner->CheckChangeDirectionForMagicCircle();
	pOwner->UpdateMagicCirclePosByStartPos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

	if( m_dwMagicCircleStartTime + m_dwCircleDuration < FRAMEGETTIME() )
	{
		m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffset, true );
		m_BuffState = BS_ON_MAP;
		SetTeleport( pOwner );
	}
	else if( pOwner->IsAttackKey() )
	{
		m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffset, true );
		m_BuffState = BS_ON_MAP;
		SetTeleport( pOwner );
	}


	//À±ÅÂÃ¶ Ãß°¡( ¸àÆ¼½º 0001039 )
	if( pOwner->GetState() != CS_DROP_ZONE_DOWN )
	{
		m_BuffState = BS_ON_MAP;
		pOwner->DestroyMagicCircle();
		pOwner->SetHideChar( false );
	}
}

void ioEscapeDropZoneBuff::SetTeleport( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_BuffState = BS_END;

	pOwner->SetWorldPosition( m_vCirclePos );
	pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );

	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetTeam() == TEAM_RED )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}
	else if( pOwner->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
	}
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pOwner->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
		else
			pMapEffect = pOwner->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}

	if( pMapEffect )
	{
		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}

	for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
	{
		ioSkill *pSkill = pOwner->GetEquipedSkill(i);
		ioPassiveSkill *pPassiveSkill = ToPassiveSkill(pSkill);

		if( !pPassiveSkill )
			continue;

		if( GetSkillName().IsEmpty() ||  pPassiveSkill->GetName() != GetSkillName() )
			continue;

		if( pPassiveSkill->UsePassiveGauge() )
		{
			pPassiveSkill->ReduceCurPassiveGauge( pPassiveSkill->GetMaxPassiveGauge() );
		}
	}

	if( !m_EscapeEndBuff.IsEmpty() )
	{
		pOwner->ReserveAddNewBuff( m_EscapeEndBuff, m_CreateChar, m_ItemName, NULL );
	}

	if( !m_EscapeSound.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			g_SoundMgr.PlaySound( m_EscapeSound, pGrp->GetParentSceneNode() );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetMagicCircleTarget( "" );
	pOwner->EndMapEffect( m_dwCurMapEffect );
	pOwner->DestroyMagicCircle();
}