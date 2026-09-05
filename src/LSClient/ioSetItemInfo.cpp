#include "stdafx.h"

#include "ioDecorationPrice.h"
#include "iosalemanager.h"

ioSetItemInfo::ioSetItemInfo()
{
	m_bIsDummyInfo = false;
	m_dwSetCode = 0;
	m_dwRequireRightCode = 0;
	m_ePackageType = PT_NORMAL;

	m_vSetItemCodeList.reserve( MAX_EQUIP_SLOT );
	m_vNeedLevelInfoList.reserve( 10 );
	m_dwSetFocusColor = 0xff000000;

	m_eCharAttackType = CAT_NONE;

	m_iItemManual           = 0;
	m_iShopOrder            = 0;
	m_iRecommendedShopOrder = 0;
	m_iShopMarkType         = 0;		// SHOP_MARK_TYPE_NONE
}

ioSetItemInfo::~ioSetItemInfo()
{
	m_vSetItemCodeList.clear();
	m_vNeedLevelInfoList.clear();
}

void ioSetItemInfo::LoadInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH * 2];
	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH * 2 );
	m_SetName = szBuf;

	m_bIsDummyInfo = rkLoader.LoadBool_e( "is_dummy", false );

	m_dwSetCode = rkLoader.LoadInt_e( "set_code", 0 );

	m_iVoiceType = rkLoader.LoadInt_e( "voice_type", 0 );
	m_CharSexType = (int)rkLoader.LoadInt_e( "char_sex_type", CST_ALL );
	
	m_dwRequireRightCode = rkLoader.LoadInt_e( "need_right_code", -1 );
	m_ePackageType       = (PackageType)rkLoader.LoadInt_e( "package_type", PT_NORMAL );

	m_eCharAttackType = (CharAttackType) rkLoader.LoadInt_e( "char_attack_type", -1 );

	enum { MAX_NEED_LEVEL = 100, };
	for ( int i = 0; i < MAX_NEED_LEVEL; i++ )
	{
		NeedLevelInfo kInfo;
		char szKeyName[MAX_PATH]="";
		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "need_level_type%d", i+1 );
		kInfo.m_eNeedLevelType = (NeedLevelType) rkLoader.LoadInt( szKeyName, -1 );
		if( kInfo.m_eNeedLevelType == NLT_NONE )
			break;
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "need_level%d", i+1 );
		kInfo.m_iNeedLevel = rkLoader.LoadInt( szKeyName, -1);
		if( kInfo.m_iNeedLevel == -1 )
			break;
		m_vNeedLevelInfoList.push_back( kInfo );
	}

	m_iItemManual				= rkLoader.LoadInt_e( "item_manual", 0 );
	m_iShopMarkType				= rkLoader.LoadInt_e( "shop_mark_type", 0 ); // SHOP_MARK_TYPE_NONE
	m_iShopOrder				= rkLoader.LoadInt_e( "shop_order", ORDER_DEFAULT );
	m_iRecommendedShopOrder		= rkLoader.LoadInt_e( "recommended_shop_order", ORDER_DEFAULT );

	m_dwLimitDate				= rkLoader.LoadInt_e( "limit_date", 0 );

	rkLoader.LoadString_e( "setitem_skill_name", "", szBuf, MAX_PATH*2 );
	m_SetItemSkillName = szBuf;

	rkLoader.LoadString_e( "sub_icon", "", szBuf, MAX_PATH*2 );
	m_szSubIconName = szBuf;

	int iRed, iGreen, iBlue;
	iRed   = rkLoader.LoadInt_e( "set_focus_line_color_r", 255 );
	iGreen = rkLoader.LoadInt_e( "set_focus_line_color_g", 255 );
	iBlue  = rkLoader.LoadInt_e( "set_focus_line_color_b", 255 );

	m_dwSetFocusColor = D3DCOLOR_ARGB( 0xff, iRed, iGreen, iBlue );

	m_PackageManDef.m_iFaceDeco = rkLoader.LoadInt_e( "package_man_face", 1 );
	m_PackageManDef.m_iHairDeco = rkLoader.LoadInt_e( "package_man_hair", 1 );
	m_PackageManDef.m_iHairColorDeco = rkLoader.LoadInt_e( "package_man_hair_color", 1 );
	m_PackageManDef.m_iSkinColorDeco = rkLoader.LoadInt_e( "package_man_skin_color", 1 );
	m_PackageManDef.m_iUnderwearDeco = rkLoader.LoadInt_e( "package_man_underwear", 1 );
	
	m_PackageWomanDef.m_iFaceDeco = rkLoader.LoadInt_e( "package_woman_face", 1 );
	m_PackageWomanDef.m_iHairDeco = rkLoader.LoadInt_e( "package_woman_hair", 1 );
	m_PackageWomanDef.m_iHairColorDeco = rkLoader.LoadInt_e( "package_woman_hair_color", 1 );
	m_PackageWomanDef.m_iSkinColorDeco = rkLoader.LoadInt_e( "package_woman_skin_color", 1 );
	m_PackageWomanDef.m_iUnderwearDeco = rkLoader.LoadInt_e( "package_woman_underwear", 1 );

	enum { MAX_LOOP = 10, };
	for (int j = 0; j < MAX_LOOP ; j++)
	{
		g_SaleMgr.LoadINI( true, rkLoader, ioSaleManager::IT_CLASS, m_dwSetCode - SET_ITEM_CODE, j, j ); // m_dwSetCode - SET_ITEM_CODE : classtype	
	}
}

int ioSetItemInfo::GetClassType() const
{
	return m_dwSetCode - SET_ITEM_CODE;
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

ConstItemVector ioSetItemInfo::GetSetItemList() const
{
	ConstItemVector vItemPtrList;

	int iItemCnt = GetSetItemCnt();
	for( int i=0 ; i<iItemCnt ; i++ )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( m_vSetItemCodeList[i], __FUNCTION__ );
		if( pItem )
		{
			vItemPtrList.push_back( pItem );
		}
	}

	return vItemPtrList;
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

int ioSetItemInfo::GetPackageDefaultDecoCode( int iDecoType, int iSex ) const
{
	int iDecoCode = 1;	
	switch( iDecoType )
	{
	case UID_FACE:
		{
			if( iSex == RDT_HUMAN_MAN )
				iDecoCode = m_PackageManDef.m_iFaceDeco;
			else if( iSex == RDT_HUMAN_WOMAN )
				iDecoCode = m_PackageWomanDef.m_iFaceDeco;
		}
		break;
	case UID_HAIR:
		{
			if( iSex == RDT_HUMAN_MAN )
				iDecoCode = m_PackageManDef.m_iHairDeco;
			else if( iSex == RDT_HUMAN_WOMAN )
				iDecoCode = m_PackageWomanDef.m_iHairDeco;
		}
		break;
	case UID_HAIR_COLOR:
		{
			if( iSex == RDT_HUMAN_MAN )
				iDecoCode = m_PackageManDef.m_iHairColorDeco;
			else if( iSex == RDT_HUMAN_WOMAN )
				iDecoCode = m_PackageWomanDef.m_iHairColorDeco;
		}
		break;
	case UID_SKIN_COLOR:
		{
			if( iSex == RDT_HUMAN_MAN )
				iDecoCode = m_PackageManDef.m_iSkinColorDeco;
			else if( iSex == RDT_HUMAN_WOMAN )
				iDecoCode = m_PackageWomanDef.m_iSkinColorDeco;
		}
		break;
	case UID_UNDERWEAR:
		{
			if( iSex == RDT_HUMAN_MAN )
				iDecoCode = m_PackageManDef.m_iUnderwearDeco;
			else if( iSex == RDT_HUMAN_WOMAN )
				iDecoCode = m_PackageWomanDef.m_iUnderwearDeco;
		}
		break;
	}
	return iDecoCode;
}


DWORD ioSetItemInfo::GetLimitDate() const
{
	return m_dwLimitDate;
}