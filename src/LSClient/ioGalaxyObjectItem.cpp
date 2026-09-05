

#include "stdafx.h"

#include "ioGalaxyObjectItem.h"
#include "ioBaseChar.h"
#include "WeaponAttribute.h"
#include "ioDoubleJump2.h"

#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioGalaxyObjectItem::ioGalaxyObjectItem()
{
}

ioGalaxyObjectItem::ioGalaxyObjectItem( const ioGalaxyObjectItem &rhs )
	: ioObjectItem( rhs ),
	m_GatherMeteoEffect( rhs.m_GatherMeteoEffect ),
	m_NormalAttributeList( rhs.m_NormalAttributeList ),
	m_JumpAttributeList( rhs.m_JumpAttributeList )
{
	m_fCurrScaleRate		= FLOAT1;
	m_GatherMeteoEffectID	= 0;
	m_fHeightOffSet			= 0;
}

ioGalaxyObjectItem::~ioGalaxyObjectItem()
{
}

void ioGalaxyObjectItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "gather_meteo_effect", "", szBuf, MAX_PATH );
	m_GatherMeteoEffect = szBuf;

	LoadNoramlAttribute( rkLoader );
	LoadJumpAttribute( rkLoader );
}

void ioGalaxyObjectItem::LoadNoramlAttribute( ioINILoader &rkLoader )
{
	m_NormalAttributeList.clear();

	int iCount = rkLoader.LoadInt_e( "normal_max_count", 0 );

	char szBuf[MAX_PATH];
	for( int i = 0; i < iCount; ++i )
	{
		GalaxyAttack Attack;
		sprintf_s_e( szBuf, "normal_charged%d", i+1 );
		LoadAttackAttribute( szBuf, Attack.m_Attribute, rkLoader );

		sprintf_s_e( szBuf, "normal_charged%d_range_min", i+1 );
		Attack.m_iRangeMin = rkLoader.LoadInt( szBuf, 0 );

		sprintf_s_e( szBuf, "normal_charged%d_range_max", i+1 );
		Attack.m_iRangeMax = rkLoader.LoadInt( szBuf, 0 );

		m_NormalAttributeList.push_back( Attack );
	}
}

void ioGalaxyObjectItem::LoadJumpAttribute( ioINILoader &rkLoader )
{
	m_JumpAttributeList.clear();

	int iCount = rkLoader.LoadInt_e( "jump_max_count", 0 );

	char szBuf[MAX_PATH];
	for( int i = 0; i < iCount; ++i )
	{
		GalaxyAttack Attack;
		sprintf_s_e( szBuf, "jump_charged%d", i+1 );
		LoadAttackAttribute( szBuf, Attack.m_Attribute, rkLoader );

		sprintf_s_e( szBuf, "jump_charged%d_range_min", i+1 );
		Attack.m_iRangeMin = rkLoader.LoadInt( szBuf, 0 );

		sprintf_s_e( szBuf, "jump_charged%d_range_max", i+1 );
		Attack.m_iRangeMax = rkLoader.LoadInt( szBuf, 0 );

		m_JumpAttributeList.push_back( Attack );
	}
}

ioItem* ioGalaxyObjectItem::Clone()
{
	return new ioGalaxyObjectItem( *this );
}

const AttackAttribute* ioGalaxyObjectItem::GetCurrNoramlAttackAttribute() const
{
	for( GalaxyAttackList::const_iterator iter = m_NormalAttributeList.begin(); iter != m_NormalAttributeList.end(); ++iter )
	{
		if( m_iCurrFireIndex >= iter->m_iRangeMin && m_iCurrFireIndex <= iter->m_iRangeMax )
			return &iter->m_Attribute;			
	}	

	return NULL;
}

const AttackAttribute* ioGalaxyObjectItem::GetCurrJumpAttackAttribute() const
{
	for( GalaxyAttackList::const_iterator iter = m_JumpAttributeList.begin(); iter != m_JumpAttributeList.end(); ++iter )
	{
		if( m_iCurrFireIndex >= iter->m_iRangeMin && m_iCurrFireIndex <= iter->m_iRangeMax )
			return &iter->m_Attribute;			
	}

	return NULL;
}

void ioGalaxyObjectItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon )
		return;

	ioObjectItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
}

ioObjectItem::ObjectSubType ioGalaxyObjectItem::GetObjectSubType() const
{
	return OST_GALAXY;
}

void ioGalaxyObjectItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );
	if( !pOwner )
		return;
	
	ioDoubleJump2* pJump = ToDoubleJump2( m_pExtendJump );
	const AttackAttribute* pAttr = GetCurrJumpAttackAttribute();
	if( pJump && pAttr )
	{
		pJump->AddExternalComboAttack( pAttr );
	}

	float fOffset = m_fHeightOffSet;
	if( pOwner->IsNeedProcess() )
		fOffset += 50.0f;
	
	if( !m_GatherMeteoEffect.IsEmpty() )
	{
		ioPlayStage* pStage = pOwner->GetCreator();
		if( pStage )
		{
			D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y + fOffset, pOwner->GetWorldPosition().z );
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherMeteoEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
			if( pMapEffect )
			{
				m_GatherMeteoEffectID = pMapEffect->GetUniqueID();
			}
		}
	}
}

void ioGalaxyObjectItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	ioDoubleJump2* pJump = ToDoubleJump2( m_pExtendJump );
	if( pJump )
	{
		pJump->ClearExternalComboAttackList();
	}

	if( m_GatherMeteoEffectID != 0 )
	{
		pOwner->EndMapEffect( m_GatherMeteoEffectID );
		m_GatherMeteoEffectID = 0;
	}
}

const AttackAttribute* ioGalaxyObjectItem::GetAttackAttribute( AttackType eType, int iSubIdx ) const
{
	return GetCurrNoramlAttackAttribute();
}

void ioGalaxyObjectItem::OnProcessState(  ioPlayStage *pStage, ioBaseChar *pOwner )
{
	ioObjectItem::OnProcessState( pStage, pOwner );

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_GatherMeteoEffectID );
	if( pMapEffect )
	{
		float fOffset = m_fHeightOffSet;
		if( pOwner->IsNeedProcess() )
			fOffset += 50.0f;

		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y + fOffset, pOwner->GetWorldPosition().z );
		pMapEffect->SetWorldPosition( vPos );
	}
}