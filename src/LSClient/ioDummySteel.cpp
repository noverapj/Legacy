
#include "stdafx.h"
#include "ioDummySteel.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "BuffDefine.h"

ioDummySteel::ioDummySteel( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_bStartInit = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetDownKey = false;
	m_bSetUpKey = false;
	m_State = DCS_START;
	m_iCurCombo = 0;
}

ioDummySteel::~ioDummySteel()
{
}

void ioDummySteel::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioDummySteel::LoadInitProperty( ioINILoader &rkLoader )
{
	//오프셋
	m_vCreateWeaponOffset.x = rkLoader.LoadFloat_e( "attack_offset_x", 0.0f );
	m_vCreateWeaponOffset.y = rkLoader.LoadFloat_e( "attack_offset_y", 0.0f );
	m_vCreateWeaponOffset.z = rkLoader.LoadFloat_e( "attack_offset_z", 0.0f );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_szExplosionSound = szBuf;

	m_dwWeaponDuration = (DWORD)rkLoader.LoadInt_e( "weapon_duration", 0 );
	
	int iComboCnt = rkLoader.LoadInt_e( "weapon_combo_cnt", 0 );
	m_vWeaponInfoList.clear();

	if( iComboCnt > 0 )
	{
		m_vWeaponInfoList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			WeaponInfo kInfo;
			kInfo.Init();

			wsprintf_e( szKey, "weapon_combo%d", i+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "weapon_combo%d_resist", i+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "weapon_combo%d_wounded", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "weapon_combo%d_wounded_time", i+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "weapon_combo%d_wounded_loop_ani", i+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			m_vWeaponInfoList.push_back( kInfo );
		}
	}

	m_bNoStartInit = rkLoader.LoadBool_e( "no_start_init", false );
	m_bNoChangeHeight = rkLoader.LoadBool_e( "no_change_height", false );
}

void ioDummySteel::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_LOOP_ATTACK:
		ProcessLoopAttackState( fTimePerSec );
		break;
	}
}

void ioDummySteel::ProcessStartState( float fTimePerSec )
{	
	DWORD dwCurTime = FRAMEGETTIME();

	if ( !m_bStartInit && !m_bNoStartInit )
	{
		SetAttachWeapon();
		
		D3DXVECTOR3 vPos = GetWorldPosition();
		float fMapHeight = GetBottomHeight();

		vPos.y = fMapHeight + m_fDummyCharHeightOffset;		
		SetWorldPosition( vPos );

		m_bStartInit = true;
				
		if( m_HP.m_fCurValue > 0.0f && fMapHeight <= 0.0f )
		{
			m_HP.m_fCurValue = 0.0f;
			m_LastAttacker = m_OwnerName;
			SetDieState( true );
		}
	}
	
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioDummySteel::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;
		SetLoopState();
	}
}

void ioDummySteel::SetLoopState()
{
	m_State = DCS_LOOP_ATTACK;
	SetNextCombo();
}

void ioDummySteel::ProcessLoopAttackState( float fTimePerSec )
{	
	CheckCombo();
}

void ioDummySteel::SetNextCombo()
{
	m_dwLoopStartTime = FRAMEGETTIME();

	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;

	const WeaponInfoList &rkWeaponInfoList = m_vWeaponInfoList;
	int iMaxCombo = rkWeaponInfoList.size();
	if( !COMPARE( m_iCurCombo, 0, iMaxCombo ) )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition() + ( GetWorldOrientation() * m_vCreateWeaponOffset );
	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_vWeaponInfoList[ m_iCurCombo ].m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_vWeaponInfoList[ m_iCurCombo ].m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_vWeaponInfoList[ m_iCurCombo ].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_vWeaponInfoList[ m_iCurCombo ].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_vWeaponInfoList[ m_iCurCombo ].m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + m_iCurCombo;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	pOwner->SetWeaponIndexBase( m_dwBaseWeaponIndex );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
	}

	m_iCurCombo++;	
}

void ioDummySteel::CheckCombo()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwDelayStartTime != 0 )
		return;

	const WeaponInfoList &rkWeaponInfoList = m_vWeaponInfoList;
	if( !COMPARE( m_iCurCombo, 0, (int)rkWeaponInfoList.size() ) && m_dwLoopStartTime + m_dwWeaponDuration < dwCurTime )
	{
		SetDieState( true );
		return;
		
	}

	if ( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwWeaponDuration < dwCurTime )
	{
		SetNextCombo();
	}
}

void ioDummySteel::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;	

	int iType;
	rkPacket >> iType;

}

void ioDummySteel::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{
//	ioDummyChar::TranslateByConveyer( vMove );
//
//	if( m_State != DCS_DIE )
//	{
//		m_nConveyerMoveFrameIdx = g_FrameTimer.GetFrameCounter();
//	}
}

//더미 죽는 상태 설정
void ioDummySteel::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if ( !m_szExplosionSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szExplosionSound );
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioDummySteel::IsActivateWeapon()
{
	if( m_State == DCS_START )
		return false;

	return true;
}

bool ioDummySteel::IsNoChangeHeight()
{
	return m_bNoChangeHeight;
}