#include "StdAfx.h"
#include "ioWeaponFollowedOwnerBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"

ioWeaponFollowedOwnerBuff::ioWeaponFollowedOwnerBuff(void)
{
	m_dwFollowWeaponControlList.clear();
	m_bAlreadyCallWeapon = false;
}

ioWeaponFollowedOwnerBuff::ioWeaponFollowedOwnerBuff( const ioWeaponFollowedOwnerBuff &rhs ):
ioBuff( rhs ),
m_vFollowWeaponInfoList( rhs.m_vFollowWeaponInfoList ),
m_bEnableDropZoneCancel( rhs.m_bEnableDropZoneCancel )
{
	m_dwFollowWeaponControlList.clear();
	m_bAlreadyCallWeapon = false;
}

ioWeaponFollowedOwnerBuff::~ioWeaponFollowedOwnerBuff(void)
{
}

ioBuff* ioWeaponFollowedOwnerBuff::Clone()
{
	return new ioWeaponFollowedOwnerBuff( *this );
}

void ioWeaponFollowedOwnerBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	int iMaxWeaponCount = rkLoader.LoadInt_e( "follow_weapon_count", 0 );

	for ( int i=0; i<iMaxWeaponCount ; ++i )
	{
		FollowWeaponInfo kInfo;

		wsprintf_e( szKey, "follow_weapon_type%d", i+1 );
		kInfo.weaponinfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "follow_weapon_type%d_resist", i+1 );
		kInfo.weaponinfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "follow_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.weaponinfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "follow_weapon_type%d_wounded_time", i+1 );
		kInfo.weaponinfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "follow_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.weaponinfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "follow_weapon_type%d_start_angle", i+1 );
		kInfo.fStartAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "follow_weapon_type%d_start_range", i+1 );
		kInfo.fStartRange = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "follow_weapon_type%d_move_speed", i+1 );
		kInfo.fMoveSpeed = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "follow_weapon_type%d_remove_gap_length", i+1 );
		kInfo.fRemoveGapLength = rkLoader.LoadFloat( szKey, 0.0f );

		m_vFollowWeaponInfoList.push_back( kInfo );
	}

	m_bEnableDropZoneCancel = rkLoader.LoadBool_e( "enable_dropzone_cancel", false );
}

void ioWeaponFollowedOwnerBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwFollowWeaponControlList.clear();
	m_bAlreadyCallWeapon = false;
}

void ioWeaponFollowedOwnerBuff::FollowWeaponRotateMoveDir( D3DXVECTOR3& vOutMoveDir, D3DXVECTOR3 vCurMoveDir, float fAngle )
{
	fAngle = ioMath::ArrangeHead( fAngle );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
	vOutMoveDir = qtRot * vCurMoveDir;
}

void ioWeaponFollowedOwnerBuff::CreateFollowWeapon( ioBaseChar *pOwner, D3DXVECTOR3 vOwnerPos, D3DXQUATERNION qtRot, DWORD dwWeaponIndex )
{
	if ( !pOwner )
		return;

	if ( m_vFollowWeaponInfoList.empty() )
		return;

	D3DXVECTOR3 vOwnerDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	D3DXVECTOR3 vWeaponPos = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vWeaponDir = ioMath::VEC3_ZERO;
	
	
	DWORD dwStartIndex = dwWeaponIndex;
	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szItemName;
	if ( GetOwnerItem() )
		szItemName = GetOwnerItem()->GetName();
	for ( int i=0 ; i<(int)m_vFollowWeaponInfoList.size() ; ++i )
	{
		const FollowWeaponInfo& rkInfo = m_vFollowWeaponInfoList[i];

		FollowWeaponRotateMoveDir( vWeaponDir, vOwnerDir, rkInfo.fStartAngle );

		vWeaponPos = vOwnerPos;
		vWeaponPos += vWeaponDir * rkInfo.fStartRange;
		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime + rkInfo.weaponinfo.m_dwCallTime;
		kFireTime.iAttributeIdx = rkInfo.weaponinfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.weaponinfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.weaponinfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.weaponinfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.weaponinfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;
		ioWeapon* pWeapon = pOwner->ExtendFireExplicit( kFireTime, vWeaponPos, -vWeaponDir, szItemName );

		ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );
		if( pZoneEffect )
		{
			ioBaseChar *pOwner = pWeapon->GetOwner();
			if( pOwner )
			{
				pZoneEffect->SetCollisionDir( -vWeaponDir );
				pZoneEffect->SetZoneValue( false, true );
			}
		}

		FollowWeaponControl ControlInfo;
		ControlInfo.dwIndex = dwWeaponIndex;
		ControlInfo.fMoveSpeed = rkInfo.fMoveSpeed;
		ControlInfo.fRemoveGapLength = rkInfo.fRemoveGapLength;
		m_dwFollowWeaponControlList.push_back( ControlInfo );
		dwWeaponIndex++;
	}

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_CREATE_WEAPON;
		kPacket << dwStartIndex;
		kPacket << vOwnerPos;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWeaponFollowedOwnerBuff::ProcessBuff( float fTimePerSec )
{
	//State Check
	if ( !CheckOwnerState() )
	{
		SetReserveEndBuff();
		return;
	}

	if ( !m_bAlreadyCallWeapon && m_pOwner->IsNeedProcess() )
	{
		m_bAlreadyCallWeapon = true;
		DWORD dwWeaponIndex = m_pOwner->IncreaseWeaponIndexBase();
		CreateFollowWeapon( m_pOwner, m_pOwner->GetWorldPosition(), m_pOwner->GetTargetRot(), dwWeaponIndex );
		m_pOwner->IncreaseWeaponIndexBase();
	}

	//Duration Check
	ioBuff::ProcessBuff( fTimePerSec );

	if( !UpdateWeaponControl( fTimePerSec ) && m_bAlreadyCallWeapon )
	{
		SetReserveEndBuff();
		return;
	}

}

bool ioWeaponFollowedOwnerBuff::CheckOwnerState()
{
	if ( !m_pOwner )
		return false;
	CharState eState = m_pOwner->GetState();

	if( m_bEnableDelayState && eState != CS_DELAY )
		return false;

	if( m_bEnableWoundCancel )
	{
		switch( eState )
		{
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_WOUNDED:
			return false;
			break;
		}
	}

	if( m_bEnableDropZoneCancel && eState == CS_DROP_ZONE_DOWN )
		return false;

	return true;
}

bool ioWeaponFollowedOwnerBuff::UpdateWeaponControl( float fTimePerSec )
{
	if( m_dwFollowWeaponControlList.empty() )
		return false;

	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vWeaponPos = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
	ioWeapon *pWeapon = NULL;
	float fWeaponOwnerGap = 0.0f;
	bool bEnableWeapon = false;
	for ( int i=0 ; i<(int)m_dwFollowWeaponControlList.size() ; ++i )
	{
		const FollowWeaponControl& rkInfo = m_dwFollowWeaponControlList[i];
		pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, rkInfo.dwIndex );
		if ( !pWeapon )
			continue;
		
		bEnableWeapon = true;
		vWeaponPos = pWeapon->GetPosition();
		vMoveDir = vOwnerPos - vWeaponPos;
		vMoveDir.y=0;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		//Move
		vWeaponPos += vMoveDir * ( fTimePerSec * rkInfo.fMoveSpeed );
		pWeapon->SetPosition( vWeaponPos );
		pWeapon->SetMoveDir( vMoveDir );
		
		if( m_pOwner->IsNeedProcess() )
		{
			//Distance Check
			vDiff = vWeaponPos - vOwnerPos;
			vDiff.y=0;
			fWeaponOwnerGap = D3DXVec3Length( &vDiff );
			if ( fWeaponOwnerGap < rkInfo.fRemoveGapLength )
			{
				pWeapon->SetWeaponDead();

				if ( m_pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
					kPacket << m_pOwner->GetCharName();
					kPacket << GetName();
					kPacket << BS_DELETE_WEAPON;
					kPacket << rkInfo.dwIndex;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
		
	}

	return bEnableWeapon;
}

void ioWeaponFollowedOwnerBuff::EndBuff()
{
	ioBuff::EndBuff();
	ioWeapon *pWeapon = NULL;
	for ( int i=0 ; i<(int)m_dwFollowWeaponControlList.size() ; ++i )
	{
		const FollowWeaponControl& rkInfo = m_dwFollowWeaponControlList[i];
		pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, rkInfo.dwIndex );
		if ( !pWeapon )
			continue;

		pWeapon->SetWeaponDead();
	}
	m_dwFollowWeaponControlList.clear();
}

void ioWeaponFollowedOwnerBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	if ( iState == BS_CREATE_WEAPON)
	{
		DWORD dwWeaponIndex;
		D3DXVECTOR3 vPos;
		D3DXQUATERNION qtRot;
		rkPacket >> dwWeaponIndex;
		rkPacket >> vPos;
		rkPacket >> qtRot;
		CreateFollowWeapon( pOwner, vPos, qtRot, dwWeaponIndex );
	}
	else if ( iState == BS_DELETE_WEAPON )
	{
		DWORD dwWeaponIndex;
		rkPacket >> dwWeaponIndex;
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, dwWeaponIndex );

		if ( pWeapon )
			pWeapon->SetWeaponDead();
	}
}
