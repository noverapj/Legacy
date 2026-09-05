

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioLookatWeaponBuff.h"

ioLookatWeaponBuff::ioLookatWeaponBuff()
{
}

ioLookatWeaponBuff::ioLookatWeaponBuff( const ioLookatWeaponBuff &rhs )
	: ioBuff( rhs ),
      m_RemoveBuffList( rhs.m_RemoveBuffList ),
	  m_bLookWeapon( rhs.m_bLookWeapon ),
	  m_bLookAreaWeapon( rhs.m_bLookAreaWeapon ),
	  m_fMoveSpeed( rhs.m_fMoveSpeed )
{
	m_bLookInit = false;
}

ioLookatWeaponBuff::~ioLookatWeaponBuff()
{
	m_RemoveBuffList.clear();
}

ioBuff* ioLookatWeaponBuff::Clone()
{
	return new ioLookatWeaponBuff( *this );
}

void ioLookatWeaponBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_bLookWeapon = rkLoader.LoadBool_e( "look_weapon", false );
	m_bLookAreaWeapon = rkLoader.LoadBool_e( "look_area_weapon", false );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );

	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	char szRemoveBuf[MAX_PATH] = "";
	for (int i=0; i<nCnt; ++i)
	{
		wsprintf_e( szRemoveBuf, "remove_buff%d_name",i+1);
		rkLoader.LoadString( szRemoveBuf, "", szBuf, MAX_PATH );
		m_RemoveBuffList.push_back( szBuf );
	}
}

void ioLookatWeaponBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );	
	m_bLookInit = false;
}

bool ioLookatWeaponBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
	{
		return false;
	}
	m_bLookInit = false;

	return true;
}

void ioLookatWeaponBuff::ProcessBuff( float fTimePerSec )
{
	bool bFinish = false;

	if ( m_bLookWeapon )
	{
		//무기 번호가 있다면
		if ( m_dwCreateWeaponIndex != 0 )
		{
			ioBaseChar *pAttack = GetCreator()->GetBaseChar( m_CreateChar );

			if ( !pAttack )
				return;

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttack, m_dwCreateWeaponIndex );
			if ( !m_bLookInit && pWeapon )
			{
				LookAtWeaponPos( pWeapon->GetPosition() );
				m_bLookInit = true;
			}
			else if ( !pWeapon )
			{
				bFinish = true;
			}
		}		
		else 
			bFinish = true;
	}
	if ( m_bLookAreaWeapon )
	{
		if ( m_dwCreateAreawaponIndex != 0 )
		{
			ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwCreateAreawaponIndex );
			if ( !m_bLookInit && pAreaWeapon )
			{
				LookAtWeaponPos( pAreaWeapon->GetStartPos() );
				m_bLookInit = true;
			}	
			else if ( !pAreaWeapon )
			{ 
				bFinish = true;
			}
		}
		else 
			bFinish = true;
	}
	
	if ( (!m_bLookWeapon && !m_bLookAreaWeapon) || bFinish )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioLookatWeaponBuff::EndBuff()
{	
	ioBuff::EndBuff();

	ioBaseChar *pOwner = GetOwner();

	if ( pOwner )
	{
		for ( int i=0; i<(int)m_RemoveBuffList.size(); ++i )
		{
			m_pOwner->RemoveBuff( m_RemoveBuffList[i], true );
		}
		m_RemoveBuffList.clear();
	}
}

void ioLookatWeaponBuff::LookAtWeaponPos( D3DXVECTOR3 vPos )
{
	D3DXQUATERNION qtTargetRot;
	D3DXVECTOR3 vTargetDir = m_pOwner->GetWorldPosition() - vPos;
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vXZDir( vTargetDir.x, 0.0f, vTargetDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	ioMath::CalcDirectionQuaternion( qtTargetRot, vXZDir );

	m_pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	m_pOwner->SetCurMoveSpeed( m_fMoveSpeed );
}