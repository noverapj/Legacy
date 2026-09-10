
#include "stdafx.h"

#include "ioChangeMeshBuff.h"

#include "ioBaseChar.h"

ioChangeMeshBuff::ioChangeMeshBuff()
{
}

ioChangeMeshBuff::ioChangeMeshBuff( const ioChangeMeshBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
m_bInvisibleCharMesh( rhs.m_bInvisibleCharMesh ),
m_vChangeMeshList( rhs.m_vChangeMeshList ),
m_vChangeMeshListW( rhs.m_vChangeMeshListW ),
m_bMaintainHideFaceMesh( rhs.m_bMaintainHideFaceMesh )
{
}

ioChangeMeshBuff::~ioChangeMeshBuff()
{
}

void ioChangeMeshBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );
	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
	m_bInvisibleCharMesh = rkLoader.LoadBool_e( "invisible_char_mesh", false );

	m_vChangeMeshList.clear();
	m_vChangeMeshListW.clear();
	int iCnt = 0;
	iCnt = rkLoader.LoadInt_e( "change_mesh_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "change_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vChangeMeshList.push_back( szBuf );
	}

	iCnt = rkLoader.LoadInt_e( "change_female_mesh_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "change_female_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vChangeMeshListW.push_back( szBuf );
	}

	//¨ú¨¡¢¥¨Ï¨¬n¨ö¨¬¢Ò¡× A©¬¡Æ¢®
	m_bMaintainHideFaceMesh = rkLoader.LoadBool_e( "maintain_hide_face_mesh", false );
}

ioBuff* ioChangeMeshBuff::Clone()
{
	return new ioChangeMeshBuff( *this );
}

void ioChangeMeshBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bInvisibleCharMesh )
		pOwner->SetInvisibleState( true, true, true );

	if( pOwner->IsMale() )
	{
		int iMeshCnt = m_vChangeMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vChangeMeshList[i];
			pOwner->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vChangeMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vChangeMeshListW[i];
			pOwner->AddEntity( szMeshName );
		}
	}
}

bool ioChangeMeshBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	if( m_bInvisibleCharMesh )
		pOwner->SetInvisibleState( true, true, true );

	if( pOwner->IsMale() )
	{
		int iMeshCnt = m_vChangeMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vChangeMeshList[i];
			pOwner->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vChangeMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vChangeMeshListW[i];
			pOwner->AddEntity( szMeshName );
		}
	}

	return true;
}

bool ioChangeMeshBuff::GetMaintainHideFaceMeshOption()
{
	return m_bMaintainHideFaceMesh;
}

void ioChangeMeshBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

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

bool ioChangeMeshBuff::CheckOwnerStateCheck()
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

	if( m_bDisableDropDownState )
	{
		if( eState == CS_DROP_ZONE_DOWN )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioChangeMeshBuff::CheckRestoreStateCheck()
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

void ioChangeMeshBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	if( m_bInvisibleCharMesh )
	{
		m_pOwner->SetInvisibleState( false, true );

		ioHelmetItem *pHelmet = ToHelmetItem( m_pOwner->GetEquipedItem( ES_HELMET ) );
		if( pHelmet )
		{
			ioHelmetItem::InvisibleFaceType eType = pHelmet->GetInvisibleFaceType();

			switch( eType )
			{
			case ioHelmetItem::IFT_ALL:
				m_pOwner->ShowFaceMesh( false );
				break;
			case ioHelmetItem::IFT_MALE:
				if( m_pOwner->IsMale() )
					m_pOwner->ShowFaceMesh( false );
				break;
			case ioHelmetItem::IFT_FEMALE:
				if( !m_pOwner->IsMale() )
					m_pOwner->ShowFaceMesh( false );
				break;
			}
		}
	}

	if( m_pOwner->IsMale() )
	{
		int iMeshCnt = m_vChangeMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vChangeMeshList[i];
			m_pOwner->DestroyEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vChangeMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vChangeMeshListW[i];
			m_pOwner->DestroyEntity( szMeshName );
		}
	}

	ioBuff::EndBuff();
}

