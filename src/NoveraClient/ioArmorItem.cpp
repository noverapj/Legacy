

#include "stdafx.h"

#include "ioArmorItem.h"

#include "ioBaseChar.h"

ioArmorItem::ioArmorItem()
{
	m_ArmorType = AT_UNARMOR;

	m_pSpringParam = NULL;
}

ioArmorItem::ioArmorItem( const ioArmorItem &rhs )
	: ioAttackableItem( rhs ),
	m_BodyMesh( rhs.m_BodyMesh ),
	m_HandMesh( rhs.m_HandMesh ),
	m_ArmorType( rhs.m_ArmorType ),
	m_iArmorSoundType( rhs.m_iArmorSoundType ),
	m_SpringBiped( rhs.m_SpringBiped )
{
	if( rhs.m_pSpringParam )
	{
		m_pSpringParam = new RopeSpringParam;
		*m_pSpringParam = *rhs.m_pSpringParam;
	}
	else
	{
		m_pSpringParam = NULL;
	}
}

ioArmorItem::~ioArmorItem()
{
	m_BodyMeshMap.clear();
	m_HandMeshMap.clear();

	SAFEDELETE( m_pSpringParam );
}

void ioArmorItem::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackableItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szTitle[MAX_PATH];

	int iDecoCnt = 0;
	m_BodyMeshMap.clear();
	m_HandMeshMap.clear();
	while( true )
	{
		sprintf_e( szTitle, "body_mesh%d", iDecoCnt );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		m_BodyMeshMap.insert( DecoInfoMap::value_type( iDecoCnt, szBuf ) );

		sprintf_e( szTitle, "hand_mesh%d", iDecoCnt );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );		
		m_HandMeshMap.insert( DecoInfoMap::value_type( iDecoCnt, szBuf ) );
		iDecoCnt++;
	}	
	SetItemType();      //Default.

	m_ArmorType = (ArmorType)rkLoader.LoadInt_e( "armor_type", AT_UNARMOR );
	m_iArmorSoundType = rkLoader.LoadInt_e( "armor_sound_type", 0 );

	//
	rkLoader.LoadString_e( "spring_biped", "", szBuf, MAX_PATH );

	m_SpringBiped = szBuf;
	SAFEDELETE( m_pSpringParam );

	if( !m_SpringBiped.IsEmpty() )
	{
		m_pSpringParam = new RopeSpringParam;
		m_pSpringParam->m_fGravity    = 400.0f;
		m_pSpringParam->m_fBoxWeights = 15.0f;

		m_pSpringParam->m_fDamping    = FLOAT05;
		m_pSpringParam->m_fAngDamping = FLOAT05;

		m_pSpringParam->m_fLengthFixRate = 0.4f;

		SwingParam kSwing;
		kSwing.m_fHSwingDOWN = -45.0f;
		kSwing.m_fHSwingUP = 45.0f;
		kSwing.m_fVSwingDOWN = -80.0f;
		kSwing.m_fVSwingUP = 30.0f;
		m_pSpringParam->m_SwingList.push_back( kSwing );

		kSwing.m_fVSwingDOWN = FLOAT10;
		kSwing.m_fVSwingUP = 70.0f;
		m_pSpringParam->m_SwingList.push_back( kSwing );
	}

	//
	m_fExtraGaugeAttack = rkLoader.LoadFloat_e( "skill_gauge_att_rate", 0.0f );
	m_fExtraGaugeAttackDef = rkLoader.LoadFloat_e( "skill_gauge_att_def_rate", 0.0f );
	m_fExtraGaugeWound = rkLoader.LoadFloat_e( "skill_gauge_wound_rate", 0.0f );
	m_fExtraGaugeWoundDef = rkLoader.LoadFloat_e( "skill_gauge_wound_def_rate", 0.0f );
}

const ioHashString& ioArmorItem::FindBodyMesh() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_BodyMeshMap.find( 0 );
	if( iter != m_BodyMeshMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioArmorItem::FindBodyMesh : Unknown Default" );
	return m_BodyMesh;
}

const ioHashString& ioArmorItem::FindHandMesh() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_HandMeshMap.find( 0 );
	if( iter != m_HandMeshMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioArmorItem::FindHandMesh : Unknown Default" );
	return m_HandMesh;
}

void ioArmorItem::SetItemType()
{
	ioItem::SetItemType();
	m_BodyMesh = FindBodyMesh();
	m_HandMesh = FindHandMesh();
}

ioItem::ItemType ioArmorItem::GetType() const
{
	return IT_ARMOR;
}

ioItem* ioArmorItem::Clone()
{
	return new ioArmorItem( *this );
}

void ioArmorItem::AddGlowLine( ioBaseChar *pOwner )
{
	bool bMale = pOwner->IsMale();

	ioHashString szCurBodyMesh = m_BodyMesh;
	if ( !m_CostumeInfo.m_szBodyMesh.IsEmpty() )
		szCurBodyMesh = m_CostumeInfo.m_szBodyMesh;

	ioHashString szNewBodyMesh = Help::ConvertName( szCurBodyMesh, bMale );

	D3DCOLORVALUE CurGlowColor = m_GlowColor;
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		CurGlowColor = m_CostumeInfo.m_GlowColor;

	if( !pOwner->AddGlowLine( szNewBodyMesh, CurGlowColor, 2.0f ) )
	{
		if( !bMale ) // 여자 실패시 다시 남자로 설정.
		{
			szNewBodyMesh = Help::ConvertName( szCurBodyMesh, true );
			if( !pOwner->AddGlowLine( szNewBodyMesh, CurGlowColor, 2.0f ) )
			{
				LOG.PrintTimeAndLog( 0, "ioArmorItem::AddGlowLine : Fail : Default Man :%s", szNewBodyMesh.c_str() );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioArmorItem::AddGlowLine : Fail : Man :%s", szNewBodyMesh.c_str() );
		}
	}

	if( HasHandMesh(pOwner) )
	{
		ioHashString szCurHandMesh = m_HandMesh;
		if ( !m_CostumeInfo.m_szHandMesh.IsEmpty() )
			szCurHandMesh = m_CostumeInfo.m_szHandMesh;

		ioHashString szNewHandMesh = Help::ConvertName( szCurHandMesh, bMale );

		if( !pOwner->AddGlowLine( szNewHandMesh, CurGlowColor, 2.0f ) )
		{
			if( !bMale ) // 여자 실패시 다시 남자로 설정.
			{
				szNewHandMesh = Help::ConvertName( szCurHandMesh, true );
				if( !pOwner->AddGlowLine( szNewHandMesh, CurGlowColor, 2.0f ) )
				{
					LOG.PrintTimeAndLog( 0, "ioArmorItem::AddGlowLine : Fail : Default Man (Hand) :%s", szNewHandMesh.c_str() );
				}
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioArmorItem::AddGlowLine : Fail : Man (Hand) :%s", szNewHandMesh.c_str() );
			}
		}
	}
}

void ioArmorItem::RemoveGlowLine( ioBaseChar *pOwner )
{
	bool bMale = pOwner->IsMale();

	ioHashString szCurBodyMesh = m_BodyMesh;
	if ( !m_CostumeInfo.m_szBodyMesh.IsEmpty() )
		szCurBodyMesh = m_CostumeInfo.m_szBodyMesh;

	ioHashString szNewBodyMesh = Help::ConvertName( szCurBodyMesh, bMale );

	if( !pOwner->RemoveGlowLine( szNewBodyMesh ) )
	{
		if( !bMale ) // 여자 실패시 다시 남자로 설정.
		{
			szNewBodyMesh = Help::ConvertName( szCurBodyMesh, true );
			if( !pOwner->RemoveGlowLine( szNewBodyMesh ) )
			{
				LOG.PrintTimeAndLog( 0, "ioArmorItem::RemoveGlowLine : Fail : Default Man :%s", szNewBodyMesh.c_str() );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioArmorItem::RemoveGlowLine : Fail : Man :%s", szNewBodyMesh.c_str() );
		}
	}

	if( HasHandMesh(pOwner) )
	{
		ioHashString szCurHandMesh = m_HandMesh;
		if ( !m_CostumeInfo.m_szHandMesh.IsEmpty() )
			szCurHandMesh = m_CostumeInfo.m_szHandMesh;

		ioHashString szNewHandMesh = Help::ConvertName( szCurHandMesh, bMale );
		if( !pOwner->RemoveGlowLine( szNewHandMesh ) )
		{
			if( !bMale ) // 여자 실패시 다시 남자로 설정.
			{
				szNewHandMesh = Help::ConvertName( szCurHandMesh, true );
				pOwner->RemoveGlowLine( szNewHandMesh );
			}
		}
	}
}

int ioArmorItem::GetArmorSoundType()
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		return m_CostumeInfo.m_nArmorSoundType;

	return m_iArmorSoundType;
}

ArmorType ioArmorItem::GetArmorType() const 
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
		return (ArmorType)m_CostumeInfo.m_nArmorType;

	return m_ArmorType;
}

bool ioArmorItem::HasSpringRope() const
{
	if ( !m_CostumeInfo.m_szSpring_Biped.IsEmpty() && m_CostumeInfo.m_pCostumeSpringParam )
		return true;

	if( m_SpringBiped.IsEmpty() || !m_pSpringParam )
		return false;

	return true;
}

const ioHashString& ioArmorItem::GetSpringBiped() const
{
	if ( !m_CostumeInfo.m_szSpring_Biped.IsEmpty() )
		return m_CostumeInfo.m_szSpring_Biped;

	return m_SpringBiped; 
}

const RopeSpringParam* ioArmorItem::GetSpringParam() const 
{ 
	if ( m_CostumeInfo.m_pCostumeSpringParam )
		return m_CostumeInfo.m_pCostumeSpringParam;

	return m_pSpringParam; 
}

void ioArmorItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_SkillGaugeType != GT_DAMAGE )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_EXCALIBUR )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_EXCALIBUR )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	m_fCurSkillGauge += fCurGauge;

	float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
	m_fCurSkillGauge = min( m_fCurSkillGauge, FLOAT100 );

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "Add DamageSkillGauge: %s, %f", pOwner->GetCharName().c_str(), fCurGauge );
	}
#endif

}

bool ioArmorItem::HasHandMesh( ioBaseChar *pOwner ) const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if ( !m_CostumeInfo.m_szHandMesh.IsEmpty() )
			return true;
		else
			return false;
	}

	if( !g_ItemMaker.IsBaseEquipMesh() )
		return !m_HandMesh.IsEmpty();

	if( !pOwner )
		return !m_HandMesh.IsEmpty();

	if( !GetHandMesh( pOwner ).IsEmpty() )
		return true;

	return false;
}

const ioHashString& ioArmorItem::GetBodyMesh( ioBaseChar *pOwner ) const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if ( !m_CostumeInfo.m_szBodyMesh.IsEmpty() )
			return m_CostumeInfo.m_szBodyMesh;
		else
			return m_BodyMesh;
	}

	if( !g_ItemMaker.IsBaseEquipMesh() )
		return m_BodyMesh;

	if( !pOwner )
		return m_BodyMesh;

	int iClassType = pOwner->GetCharacterInfo().m_class_type;
	int iParts = DEFAULT_BASIC_ITEM_CODE * (GetType() - 1);
	if( iParts < 0 )
		return m_BodyMesh;

	DWORD dwItemCode = iParts + iClassType;
	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
	if( !pItem )
		return m_BodyMesh;

	switch( pItem->GetType() )
	{
	case ioItem::IT_ARMOR:
		if( ToArmorItemConst(pItem) )
			return ToArmorItemConst(pItem)->GetBodyMesh(NULL);
		break;
	}

	return m_BodyMesh;
}

const ioHashString& ioArmorItem::GetHandMesh( ioBaseChar *pOwner ) const
{
	if ( m_CostumeInfo.m_nCostumeCode > 0 )
	{
		if ( !m_CostumeInfo.m_szHandMesh.IsEmpty() )
			return m_CostumeInfo.m_szHandMesh;
		else 
			return m_HandMesh;
	}

	if( !g_ItemMaker.IsBaseEquipMesh() )
		return m_HandMesh;

	if( !pOwner )
		return m_HandMesh;

	int iClassType = pOwner->GetCharacterInfo().m_class_type;
	int iParts = DEFAULT_BASIC_ITEM_CODE * (GetType() - 1);
	if( iParts < 0 )
		return m_HandMesh;

	DWORD dwItemCode = iParts + iClassType;
	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
	if( !pItem )
		return m_HandMesh;

	switch( pItem->GetType() )
	{
	case ioItem::IT_ARMOR:
		if( ToArmorItemConst(pItem) )
			return ToArmorItemConst(pItem)->GetHandMesh(NULL);
		break;
	}

	return m_HandMesh;
}