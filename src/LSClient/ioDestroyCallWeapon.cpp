#include "StdAfx.h"
#include "ioDestroyCallWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"

ActionStopType ioDestroyCallWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioDestroyCallWeapon::m_DieActionStopType = AST_NONE;

ioDestroyCallWeapon::ioDestroyCallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pWeaponEffect = NULL;
	m_bStartInit = false;
	m_dwCheckDelayTime = FRAMEGETTIME();
	m_vMoveDir = ioMath::VEC3_ZERO;
}

ioDestroyCallWeapon::~ioDestroyCallWeapon()
{
}

void ioDestroyCallWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );
	
	char szbuf[MAX_PATH];

	//레드 팀 이펙트
	rkLoader.LoadString_e( "weapon_effect_red", "", szbuf, MAX_PATH );
	m_szWeaponRedEffect = szbuf;
	//블루 팀 이펙트
	rkLoader.LoadString_e( "weapon_effect_blue", "", szbuf, MAX_PATH );
	m_szWeaponBlueEffect = szbuf;

	m_dwDelayTime = rkLoader.LoadInt_e( "dead_delay_time", 0 );
	m_bSetItemFunc = rkLoader.LoadBool_e( "set_item_func", false );
}

bool ioDestroyCallWeapon::InitResource()
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

bool ioDestroyCallWeapon::CreateWeaponEffect( const ioHashString &szFileName )
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

void ioDestroyCallWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pWeaponEffect )
	{
		g_EffectFactory.DestroyEffect( m_pWeaponEffect );
		m_pWeaponEffect = NULL;
	}
}

void ioDestroyCallWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwDelayTime > 0 && m_dwDelayTime + m_dwCheckDelayTime < dwCurTime )
	{
		if ( m_bSetItemFunc )
			SetItemFunc();
		SetWeaponDead();
		return;
	}

	if ( !m_bStartInit )
	{
		//범위 무기 생성
		CreateAttachAreaWeapon();
		m_bStartInit = true;
	}

	if( m_pWeaponEffect )
	{
		m_pWeaponEffect->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioDestroyCallWeapon::SetWeaponDead()
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

ioWeapon::WeaponType ioDestroyCallWeapon::GetType() const
{
	return WT_DESTROY_CALL;
}

ActionStopType ioDestroyCallWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioDestroyCallWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioDestroyCallWeapon::SetMoveDir( const D3DXVECTOR3 &vMoveDir )
{
	m_vMoveDir = vMoveDir;
}

D3DXVECTOR3 ioDestroyCallWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioDestroyCallWeapon::SetItemFunc()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioWeaponItem *pWeaponItem = pOwner->GetWeapon();
	if ( !pWeaponItem )
		return;

	ioNephilimItem *pNephilimItem = ToNephilimItem( pWeaponItem );
	if ( pNephilimItem )
		pNephilimItem->CreateAreaWeapon( pOwner, GetPosition(), GetOrientation() );
}