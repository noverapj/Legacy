#include "StdAfx.h"

#include "ioSelfControlBuff.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioSelfControlBuff::ioSelfControlBuff(void)
{
	m_State = SCS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_fSkillTotalMoveAmount = 0.0f;
}

ioSelfControlBuff::ioSelfControlBuff( const ioSelfControlBuff &rhs )
	: ioBuff( rhs ),
	m_fCurMoveSpeed( rhs.m_fCurMoveSpeed ),
	m_szMoveAni( rhs.m_szMoveAni ),
	m_dwMoveRotateTime( rhs.m_dwMoveRotateTime )
{
	m_State = SCS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_fSkillTotalMoveAmount = 0.0f;
}

ioSelfControlBuff::~ioSelfControlBuff(void)
{
}

void ioSelfControlBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_szMoveAni = szBuf;

	m_fCurMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );

	m_dwMoveRotateTime = (DWORD)rkLoader.LoadInt_e( "move_rotate_speed", 0 );
}

ioBuff* ioSelfControlBuff::Clone()
{
	return new ioSelfControlBuff( *this );
}

void ioSelfControlBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	if ( pOwner )
	{
		pOwner->SetSelfControlInfo( m_szMoveAni );

		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );	
		m_State = SCS_CONTROL;
	}
}

void ioSelfControlBuff::ProcessBuff( float fTimePerSec )
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
		{
			CheckCallingAttribute();
			CheckCallingAreaWeapon();
			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				m_pOwner->SetCurMoveSpeed( 0 );
				SetReserveEndBuff();
				return;
			}

			if ( !CheckWeaponDead( m_pOwner ) )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				m_pOwner->SetCurMoveSpeed( 0 );
				SetReserveEndBuff();
			}
		}		
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
	
	ProcessBuffAction( fTimePerSec );

	if( m_pOwner->GetState() != CS_BUFF_ACTION )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		m_pOwner->SetCurMoveSpeed( 0 );
		SetReserveEndBuff();
		return;
	}
}


void ioSelfControlBuff::ProcessBuffAction( float fTimePerSec )
{	
	switch( m_State )
	{
	case SCS_CONTROL:
		{
			float fMoveAMt = m_fCurMoveSpeed * fTimePerSec;
			m_fSkillTotalMoveAmount += fMoveAMt;

			D3DXVECTOR3 vMoveAmt = m_pOwner->GetMoveDir() * fMoveAMt;
			D3DXVECTOR3 vPos = m_pOwner->GetWorldPosition();
			vPos += vMoveAmt;
			m_pOwner->SetWorldPosition( vPos );
			CheckKeyInput( m_pOwner );
			ProcessRotate( m_pOwner, m_dwMoveRotateTime );
		}
		break;
	}
}

void ioSelfControlBuff::EndBuff()
{
	if( m_pOwner->GetState() == CS_BUFF_ACTION )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		m_pOwner->SetCurMoveSpeed( 0 );
	}

	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if ( pCreator )
	{
		int iSize = m_vWeaponIndexList.size();
		for( int i=0; i < iSize; ++i )
		{
			int iWeaponIndex = m_vWeaponIndexList[i];

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, iWeaponIndex );
			if( pWeapon && pWeapon->IsLive() )
			{
				pWeapon->SetWeaponDead();
			}
		}
	}

	ioBuff::EndBuff();
}

bool ioSelfControlBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if ( pOwner )
	{
		pOwner->SetSelfControlInfo( m_szMoveAni );

		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );	
		m_State = SCS_CONTROL;
	}

	return true;
}


void ioSelfControlBuff::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{	
		m_CurDirKey = eNewDirKey;

		if ( m_pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << m_pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SS_KEY;
			kPacket << pChar->GetTargetRot();				
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioSelfControlBuff::ProcessRotate( ioBaseChar *pChar, float fRotateTime )
{
	if( !pChar ) return;
	if( fRotateTime <= 0 ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pChar->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pChar->SetTargetRotToRotate( qtNewRot, true );
}

void ioSelfControlBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	switch( iState )
	{
	case SS_KEY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true, false );
			int nCurKey;
			rkPacket >>	nCurKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;
		}
		break;
	}
}

bool ioSelfControlBuff::CheckWeaponDead( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwTopTime = m_CallAttributeList[0].m_dwAttributeCallTime;
	dwTopTime += m_dwBuffStartTime;
	ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
	if ( !pCreator ) return false;
	if ( dwCurTime >= dwTopTime )
	{
		int iSize = m_vWeaponIndexList.size();
		for( int i=0; i < iSize; ++i )
		{
			int iWeaponIndex = m_vWeaponIndexList[i];

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pCreator, iWeaponIndex );
			if( pWeapon && pWeapon->IsLive() )
			{
				if( ToZoneEffectWeapon( pWeapon ) )
					return true;
			}
		}
		return false;
	}
	return true;
}

void ioSelfControlBuff::CallingAttribute()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	float fGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( m_fCallAttributeEnableHeight > 0.0f && m_fCallAttributeEnableHeight < fGap )
	{
		m_CallAttributeList.erase( m_CallAttributeList.begin() );
		return;
	}

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( pCreator )
	{
		DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase();
		m_vWeaponIndexList.push_back( dwWeaponIndex );
		pCreator->BuffFireExplicit2( m_CallAttributeList[0].m_WeaponInfo, m_Name, dwWeaponIndex, m_OwnerName );
	}

	m_CallAttributeList.erase( m_CallAttributeList.begin() );

}

void ioSelfControlBuff::CheckCallingAttribute()
{
	if( m_CallAttributeList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwTopTime = m_CallAttributeList[0].m_dwAttributeCallTime;
	DWORD dwRandomTime = m_CallAttributeList[0].m_dwRandomTime;
	dwTopTime += m_dwBuffStartTime;

	if( dwRandomTime == 0 )
	{
		if( dwCurTime >= dwTopTime )
			CallingAttribute();
	}
	else
	{
		if( dwCurTime >= (dwRandomTime+m_dwBuffStartTime) )
			CallingAttribute();
	}
}
