

#include "stdafx.h"

#include "ioSetItemInfo.h"
#include "ioHeroManager.h"
#include <strsafe.h>

ioSetItemInfo::ioSetItemInfo()
{
	m_dwSetCode = 0;
	m_dwRequireRightCode = -1;
	m_ePackageType = PT_NORMAL;
	m_iMortmainSellValue = 0;
	m_iPresetSex		= 0;

	m_vSetItemCodeList.reserve( MAX_EQUIP_SLOT );
	m_vNeedLevelInfoList.reserve( 10 );
}

ioSetItemInfo::~ioSetItemInfo()
{
	m_vSetItemCodeList.clear();
	m_vNeedLevelInfoList.clear();
}

void ioSetItemInfo::LoadInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString( "name", "", szBuf, MAX_PATH * 2 );
	m_SetName = szBuf;

	m_dwSetCode = rkLoader.LoadInt( "set_code", 0 );
	m_dwRequireRightCode = rkLoader.LoadInt( "need_right_code", -1 );
	m_ePackageType       = (PackageType)rkLoader.LoadInt( "package_type", PT_NORMAL );

	enum { MAX_NEED_LEVEL = 100, };
	for ( int i = 0; i < MAX_NEED_LEVEL; i++ )
	{
		NeedLevelInfo kInfo;
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof(szKeyName), "need_level_type%d", i+1 );
		kInfo.m_eNeedLevelType = (NeedLevelType) rkLoader.LoadInt( szKeyName, -1 );
		if( kInfo.m_eNeedLevelType == NLT_NONE )
			break;
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf( szKeyName, sizeof(szKeyName), "need_level%d", i+1 );
		kInfo.m_iNeedLevel = rkLoader.LoadInt( szKeyName, -1);
		if( kInfo.m_iNeedLevel == -1 )
			break;
		m_vNeedLevelInfoList.push_back( kInfo );
	}
	m_iMortmainSellValue = rkLoader.LoadInt( "mortain_sellvalue", 0 );

	m_iPresetSex = rkLoader.LoadInt( "preset_gender", 0 );
	LoadPreset( "man", m_ManPresetDeco, rkLoader );
	LoadPreset( "woman", m_WomanPresetDeco, rkLoader );
}

void ioSetItemInfo::LoadInfo( int iIndex )
{
	LSC_heroitem_info *pHeroInfo = NULL;
	pHeroInfo = g_ioHeroManager.GetAt( iIndex );
	if( pHeroInfo )
	{
		m_dwSetCode						= GetCheckValue( pHeroInfo->item_code + (int)SET_ITEM_CODE );
		m_iPresetSex					= GetCheckValue( pHeroInfo->preset_gender );
		m_iMortmainSellValue			= GetCheckValue( pHeroInfo->value_1 );
		m_ManPresetDeco.m_iFace			= GetCheckValue( pHeroInfo->preset_man_face );
		m_ManPresetDeco.m_iHair			= GetCheckValue( pHeroInfo->preset_man_hair );
		m_ManPresetDeco.m_iSkinColor	= GetCheckValue( pHeroInfo->preset_man_skincolor );
		m_ManPresetDeco.m_iHairColor	= GetCheckValue( pHeroInfo->preset_man_haircolor );
		m_ManPresetDeco.m_iUnderwear	= GetCheckValue( pHeroInfo->preset_man_underwear );
		m_WomanPresetDeco.m_iFace		= GetCheckValue( pHeroInfo->preset_woman_face );
		m_WomanPresetDeco.m_iHair		= GetCheckValue( pHeroInfo->preset_woman_hair );
		m_WomanPresetDeco.m_iSkinColor	= GetCheckValue( pHeroInfo->preset_woman_skincolor );
		m_WomanPresetDeco.m_iHairColor	= GetCheckValue( pHeroInfo->preset_woman_haircolor );
		m_WomanPresetDeco.m_iUnderwear	= GetCheckValue( pHeroInfo->preset_woman_underwear );
	}
}

int ioSetItemInfo::GetSetItemCnt() const
{
	return m_vSetItemCodeList.size();
}

DWORD ioSetItemInfo::GetSetItemCode( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetSetItemCnt() ) )
		return m_vSetItemCodeList[iIndex];

	return 0;
}

int ioSetItemInfo::GetNeedLevelInfoListCnt() const
{
	return m_vNeedLevelInfoList.size();	
}

ioSetItemInfo::NeedLevelType ioSetItemInfo::GetNeedLevelType( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetNeedLevelInfoListCnt() ) )
		return m_vNeedLevelInfoList[iIndex].m_eNeedLevelType;

	return NLT_NONE;
}

int ioSetItemInfo::GetNeedLevel( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetNeedLevelInfoListCnt() ) )
		return m_vNeedLevelInfoList[iIndex].m_iNeedLevel;

	return -1;
}

int ioSetItemInfo::GetPresetDecoCode( int iGendedrType, int iDecoParts ) const
{
	// 1 : man
	// 2 : woman
	PresetDeco stTemp;
	if( iGendedrType == GT_MAN )
		stTemp = m_ManPresetDeco;
	else
		stTemp = m_WomanPresetDeco;

	switch( iDecoParts )
	{
	case IDT_FACE:
		return stTemp.m_iFace;
	case IDT_HAIR:
		return stTemp.m_iHair;
	case IDT_SKIN:
		return stTemp.m_iSkinColor;
	case IDT_HAIR_COLOR:
		return stTemp.m_iHairColor;
	case IDT_UNDERWEAR:
		return stTemp.m_iUnderwear;
	}

	return -1;
}

int ioSetItemInfo::GetCheckValue( int iValue )
{
	if( iValue <= 0 || iValue >= MAX_INT_VALUE)
		return 1;
	else 
		return iValue;
}

void ioSetItemInfo::SetItemCodeList( DWORDVec vSetItemCodeList )
{
	m_vSetItemCodeList = vSetItemCodeList;
}

void ioSetItemInfo::LoadPreset( const char *szDeco, PresetDeco &stPreset, ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH]="";

	StringCbPrintf( szBuf, sizeof(szBuf), "preset_%s_face", szDeco );
	stPreset.m_iFace = rkLoader.LoadInt( szBuf, 1 );

	StringCbPrintf( szBuf, sizeof(szBuf), "preset_%s_hair", szDeco );
	stPreset.m_iHair = rkLoader.LoadInt( szBuf, 1 );

	StringCbPrintf( szBuf, sizeof(szBuf), "preset_%s_skincolor", szDeco );
	stPreset.m_iSkinColor = rkLoader.LoadInt( szBuf, 1 );

	StringCbPrintf( szBuf, sizeof(szBuf), "preset_%s_haircolor", szDeco );
	stPreset.m_iHairColor = rkLoader.LoadInt( szBuf, 1 );

	StringCbPrintf( szBuf, sizeof(szBuf), "preset_%s_underwear", szDeco );
	stPreset.m_iUnderwear = rkLoader.LoadInt( szBuf, 1 );
}