#include "StdAfx.h"
#include "ioAttachAeraWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioAttachAeraWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioAttachAeraWeapon::m_DieActionStopType = AST_NONE;

ioAttachAeraWeapon::ioAttachAeraWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pWeaponEffect = NULL;
	m_bStartInit = false;
}

ioAttachAeraWeapon::~ioAttachAeraWeapon()
{
}

void ioAttachAeraWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szbuf[MAX_PATH];

	//레드 팀 이펙트
	rkLoader.LoadString_e( "weapon_effect_red", "", szbuf, MAX_PATH );
	m_szWeaponRedEffect = szbuf;
	//블루 팀 이펙트
	rkLoader.LoadString_e( "weapon_effect_blue", "", szbuf, MAX_PATH );
	m_szWeaponBlueEffect = szbuf; 

}

bool ioAttachAeraWeapon::InitResource()
{
	DestroyResource();
	ioBaseChar* pOwner = GetOwner();

	if ( !pOwner)
		return false;

	if ( pOwner->GetTeam() == TEAM_BLUE )
	{
		if( !m_szWeaponBlueEffect.IsEmpty() )
		{
			CreateWeaponEffect( m_szWeaponBlueEffect );
		}
	}
	else if ( pOwner->GetTeam() == TEAM_RED )
	{
		if( !m_szWeaponRedEffect.IsEmpty() )
		{
			CreateWeaponEffect( m_szWeaponRedEffect );
		}
	}
	else 
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			CreateWeaponEffect( m_szWeaponBlueEffect );
		else
			CreateWeaponEffect( m_szWeaponRedEffect );
	}

	return true;
}

bool ioAttachAeraWeapon::CreateWeaponEffect( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	
		return false;

	m_pWeaponEffect = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pWeaponEffect )	
		return false;

	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

	GetParentSceneNode()->AttachObject( m_pWeaponEffect );
	GetParentSceneNode()->SetScale( vScale );

	AddEffectEvent( m_pWeaponEffect );

	return true;
}

void ioAttachAeraWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pWeaponEffect )
	{
		g_EffectFactory.DestroyEffect( m_pWeaponEffect );
		m_pWeaponEffect = NULL;
	}
}

void ioAttachAeraWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if ( !m_bStartInit )
	{
		CreateAttachAreaWeapon();
		m_bStartInit = true;
	}

	if( m_pWeaponEffect )
	{
		m_pWeaponEffect->Update( fTimePerSec * FLOAT1000 );
	}

	if ( m_bOwnerAttach )
	{
		ProcessAttachMove( fTimePerSec, pStage );
	}	
}

void ioAttachAeraWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();

	if ( g_WeaponMgr.GetSingletonPtr() && g_AreaWeaponMgr.GetSingletonPtr() )
	{
		//새 무기 생성
		CheckCreateWeaponList();
		//범위 무기 삭제
		DestroyAttachAreaWeapon();
	}
}

ioWeapon::WeaponType ioAttachAeraWeapon::GetType() const
{
	return WT_ATTACH_AREA;
}

ActionStopType ioAttachAeraWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioAttachAeraWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioAttachAeraWeapon::ProcessAttachMove( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();

	if( m_iDummyIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
		if( pDummy )
		{
			D3DXQUATERNION qtRot = pDummy->GetWorldOrientation();

			D3DXVECTOR3 vPos = pDummy->GetWorldPosition();

			SetPosition( vPos );
			SetOrientation( qtRot );
		
			UpdateAttachAreaWeapon( pStage );
		}
	}
}

void ioAttachAeraWeapon::CheckOwnerAttach( bool bAttach )
{
	m_bOwnerAttach = bAttach;
}