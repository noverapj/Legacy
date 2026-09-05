
#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/ioINILoader.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"

#include "../TextColorTable.h"
#include "../NetworkWrappingFunc.h"
#include "../EtcHelpFunc.h"
#include "../Setting.h"

#include "../ioMyInfo.h"
#include "../ioItem.h"
#include "../ioItemMaker.h"
#include "../ioUserExtraItem.h"
#include "../ioExtraItemInfoManager.h"
#include "../ioItemCompoundManager.h"
#include "../ioEtcItem.h"
#include "../ioEtcItemManager.h"

#include "../WndID.h"
#include "ioSP2GUIManager.h"
#include "MyInventoryWnd.h"

#include "GlobalWnd.h"
#include "ManualWnd.h"

#include "ItemMultipleCompoundWnd.h"
#include "ItemMultipleCompoundResultWnd.h"

#include <strsafe.h>
#include "../io3DEngine/Safesprintf.h"

//////////////////////////////////////////////////////////////////////////
ItemMultipleCompoundResultWnd::ItemMultipleCompoundResultWnd()
{
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pIcon	= NULL;
	m_pEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_bFinalResult = false;

	m_pManual = NULL;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;
}

ItemMultipleCompoundResultWnd::~ItemMultipleCompoundResultWnd()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	SAFEDELETE(m_pNumText);
	SAFEDELETE(m_pPlusText);

	m_vWeaponIcon.clear();
	m_vArmorIcon.clear();
	m_vHelmetIcon.clear();
	m_vCloakIcon.clear();
	m_vIconList.clear();

	m_vRollingEffect.clear();
	m_vEndEfffect.clear();

	m_vWeaponGrade.clear();
	m_vArmorGrade.clear();
	m_vHelmetGrade.clear();
	m_vCloakGrade.clear();
	m_vGradeList.clear();
}

void ItemMultipleCompoundResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemMultipleCompoundResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "rolling_gap" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "rolling_duration" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "rolling_sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_End_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "End_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect.push_back( szEffect );
	}
}

void ItemMultipleCompoundResultWnd::iwm_create()
{
	LoadIconList();
}

void ItemMultipleCompoundResultWnd::LoadIconList()
{
	m_vWeaponIcon.clear();
	m_vArmorIcon.clear();
	m_vHelmetIcon.clear();
	m_vCloakIcon.clear();
	m_vUniqueIcon.clear();
	m_vIconList.clear();

	m_vWeaponGrade.clear();
	m_vArmorGrade.clear();
	m_vHelmetGrade.clear();
	m_vCloakGrade.clear();
	m_vUniqueGrade.clear();
	m_vGradeList.clear();

	ioINILoader_e kLoader( "config/sp2_item_compound_info.ini" );
	kLoader.SetTitle_e( "multiple_compound" );

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	int iIconCnt = kLoader.LoadInt_e( "weapon_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vWeaponIcon.reserve( iIconCnt );
		m_vWeaponGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "weapon_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vWeaponIcon.push_back( szIconName );

			wsprintf_e( szKey, "weapon_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vWeaponGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "armor_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vArmorIcon.reserve( iIconCnt );
		m_vArmorGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "armor_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vArmorIcon.push_back( szIconName );

			wsprintf_e( szKey, "armor_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vArmorGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "helmet_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vHelmetIcon.reserve( iIconCnt );
		m_vHelmetGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "helmet_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vHelmetIcon.push_back( szIconName );

			wsprintf_e( szKey, "helmet_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vHelmetGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "cloak_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vCloakIcon.reserve( iIconCnt );
		m_vCloakGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "cloak_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vCloakIcon.push_back( szIconName );
			
			wsprintf_e( szKey, "cloak_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vCloakGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "unique_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vUniqueIcon.reserve( iIconCnt );
		m_vUniqueGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "unique_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vUniqueIcon.push_back( szIconName );

			wsprintf_e( szKey, "unique_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vUniqueGrade.push_back( nGrade );
		}
	}
}

void ItemMultipleCompoundResultWnd::SetInfoAndShow( int iEtcType, int iItemCode, int iReinforce )
{
	m_vIconList.clear();

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();

	m_bFinalResult = false;

	m_iEtcType = iEtcType;
	m_iItemCode = iItemCode;
	m_iReinforce = iReinforce;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	GetCurIconList();

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void ItemMultipleCompoundResultWnd::GetCurIconList()
{
	int iItemType = m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

	// IconList
	m_vIconList.clear();
	if( m_iEtcType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 ) //유니크 장비 합성기
	{
		int iCnt = m_vUniqueIcon.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vUniqueIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vUniqueGrade.size() ) )
				m_vGradeList.push_back( m_vUniqueGrade[i] );
		}
	}
	else if( m_iEtcType != ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND &&
		m_iEtcType != ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 &&
		m_iEtcType != ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )	// 모두
	{
		int i = 0;
		int iCnt = m_vWeaponIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vWeaponIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vWeaponGrade.size() ) )
				m_vGradeList.push_back( m_vWeaponGrade[i] );
		}

		iCnt = m_vArmorIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vArmorIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vArmorGrade.size() ) )
				m_vGradeList.push_back( m_vArmorGrade[i] );
		}

		iCnt = m_vHelmetIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vHelmetIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vHelmetGrade.size() ) )
				m_vGradeList.push_back( m_vHelmetGrade[i] );
		}

		iCnt = m_vCloakIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vCloakIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vCloakGrade.size() ) )
				m_vGradeList.push_back( m_vCloakGrade[i] );
		}
	}
	else
	{
		switch( iItemType )
		{
		case ES_WEAPON:
			{
				int iCnt = m_vWeaponIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vWeaponIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vWeaponGrade.size() ) )
						m_vGradeList.push_back( m_vWeaponGrade[i] );
				}
			}
			break;
		case ES_ARMOR:
			{
				int iCnt = m_vArmorIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vArmorIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vArmorGrade.size() ) )
						m_vGradeList.push_back( m_vArmorGrade[i] );
				}
			}
			break;
		case ES_HELMET:
			{
				int iCnt = m_vHelmetIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vHelmetIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vHelmetGrade.size() ) )
						m_vGradeList.push_back( m_vHelmetGrade[i] );
				}
			}
			break;
		case ES_CLOAK:
			{
				int iCnt = m_vCloakIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vCloakIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vCloakGrade.size() ) )
						m_vGradeList.push_back( m_vCloakGrade[i] );
				}
			}
			break;
		}
	}
	//

	if( (int)m_vIconList.size() > 0 )
	{
		std::random_shuffle( m_vIconList.begin(), m_vIconList.end() );
	}
}

void ItemMultipleCompoundResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon(fTimePerSec);
	RollingEffect(fTimePerSec);
}

void ItemMultipleCompoundResultWnd::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		const ioItem *pItem = g_ItemMaker.GetItemConst( m_iItemCode, __FUNCTION__ );
		if( pItem && !m_bFinalResult )		// 한 번만 하기 위해
		{
			ioHashString szIcon = pItem->GetItemLargeIconName();
			if( !szIcon.IsEmpty() )
			{
				SAFEDELETE( m_pIcon );
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
			}

			m_ItemName = pItem->GetName();
			m_dwScaleStartTime = dwCurTime;
			m_nGradeType = pItem->GetGradeType();
		}

		// IconScale 설정
		float fScale = FLOAT1;
		m_fGradeScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
			m_fGradeScale = fScale;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bFinalResult )
		{
			m_bFinalResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( !m_szResultSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szResultSound );

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
		}
	}
	else if( (int)m_vIconList.size() > 0 )				// Rolling
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )		// 교체
		{
			SAFEDELETE( m_pIcon );

			m_iCurIconArray++;

			if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList.size() ) )
				m_iCurIconArray = 0;

			ioHashString szCurIcon = m_vIconList[m_iCurIconArray];
			if( !szCurIcon.IsEmpty() )
			{
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );
			}

			m_dwRollingCheckTime = dwCurTime;

			m_nGradeType = 0;
			if ( COMPARE( m_iCurIconArray, 0, (int)m_vGradeList.size() ) )
				m_nGradeType = m_vGradeList[m_iCurIconArray];
		}
	}
}

void ItemMultipleCompoundResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bFinalResult )
		iEffectListCnt = m_vEndEfffect.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bFinalResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vEndEfffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
			{
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void ItemMultipleCompoundResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( m_iEtcType );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void ItemMultipleCompoundResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bFinalResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y, m_fGradeScale );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	if( m_bFinalResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(2) );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bFinalResult && m_pNumText && m_pPlusText && m_iReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_iReinforce < 100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iReinforce, -TEXT_GAP );
	}


	// Text
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = "?????";
	ioHashString szDesc = STR(3);

	if( m_bFinalResult )
	{
		if( !m_ItemName.IsEmpty() )
			szName = m_ItemName;
	}

	// Name
	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	kPrinter[1].ClearList();
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
	kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
}

void ItemMultipleCompoundResultWnd::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void ItemMultipleCompoundResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iEtcType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_iEtcType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_iEtcType, pUserEtcItem );
			}
		}
		break;
	}
}

void ItemMultipleCompoundResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_iEtcType, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
UniqueItemMultipleCompoundResultWnd::UniqueItemMultipleCompoundResultWnd()
{
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pIcon	= NULL;
	m_pEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_bFinalResult = false;

	m_pManual = NULL;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	m_pLoopEffect = NULL;
	m_pResultEffect = NULL;
}

UniqueItemMultipleCompoundResultWnd::~UniqueItemMultipleCompoundResultWnd()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	SAFEDELETE(m_pNumText);
	SAFEDELETE(m_pPlusText);

	m_vWeaponIcon.clear();
	m_vArmorIcon.clear();
	m_vHelmetIcon.clear();
	m_vCloakIcon.clear();
	m_vIconList.clear();

	m_vRollingEffect.clear();
	m_vEndEfffect.clear();

	m_vWeaponGrade.clear();
	m_vArmorGrade.clear();
	m_vHelmetGrade.clear();
	m_vCloakGrade.clear();
	m_vGradeList.clear();
}

void UniqueItemMultipleCompoundResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void UniqueItemMultipleCompoundResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "rolling_gap" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "rolling_duration" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "rolling_sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_End_Effect" );
	m_vEndEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "End_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect.push_back( szEffect );
	}
}

void UniqueItemMultipleCompoundResultWnd::iwm_create()
{
	LoadIconList();
}

void UniqueItemMultipleCompoundResultWnd::LoadIconList()
{
	m_vWeaponIcon.clear();
	m_vArmorIcon.clear();
	m_vHelmetIcon.clear();
	m_vCloakIcon.clear();
	m_vUniqueIcon.clear();
	m_vIconList.clear();

	m_vWeaponGrade.clear();
	m_vArmorGrade.clear();
	m_vHelmetGrade.clear();
	m_vCloakGrade.clear();
	m_vUniqueGrade.clear();
	m_vGradeList.clear();

	ioINILoader_e kLoader( "config/sp2_item_compound_info.ini" );
	kLoader.SetTitle_e( "multiple_compound" );

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	int iIconCnt = kLoader.LoadInt_e( "weapon_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vWeaponIcon.reserve( iIconCnt );
		m_vWeaponGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "weapon_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vWeaponIcon.push_back( szIconName );

			wsprintf_e( szKey, "weapon_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vWeaponGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "armor_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vArmorIcon.reserve( iIconCnt );
		m_vArmorGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "armor_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vArmorIcon.push_back( szIconName );

			wsprintf_e( szKey, "armor_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vArmorGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "helmet_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vHelmetIcon.reserve( iIconCnt );
		m_vHelmetGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "helmet_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vHelmetIcon.push_back( szIconName );

			wsprintf_e( szKey, "helmet_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vHelmetGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "cloak_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vCloakIcon.reserve( iIconCnt );
		m_vCloakGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "cloak_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vCloakIcon.push_back( szIconName );

			wsprintf_e( szKey, "cloak_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vCloakGrade.push_back( nGrade );
		}
	}

	iIconCnt = kLoader.LoadInt_e( "unique_icon_cnt", 0 );
	if( iIconCnt > 0 )
	{
		m_vUniqueIcon.reserve( iIconCnt );
		m_vUniqueGrade.reserve( iIconCnt );

		ioHashString szIconName;
		for( i=0; i < iIconCnt; ++i )
		{
			wsprintf_e( szKey, "unique_icon_name%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szIconName = szBuf;
			m_vUniqueIcon.push_back( szIconName );

			wsprintf_e( szKey, "unique_icon_grade%d", i+1 );
			int nGrade = kLoader.LoadInt( szKey, 0 );
			m_vUniqueGrade.push_back( nGrade );
		}
	}
}

void UniqueItemMultipleCompoundResultWnd::SetInfoAndShow( int iEtcType, int iItemCode, int iReinforce )
{
	m_vIconList.clear();

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ShowWnd();

	m_bFinalResult = false;

	m_iEtcType = iEtcType;
	m_iItemCode = iItemCode;
	m_iReinforce = iReinforce;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	GetCurIconList();

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void UniqueItemMultipleCompoundResultWnd::GetCurIconList()
{
	int iItemType = m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

	// IconList
	m_vIconList.clear();
	if( m_iEtcType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 ) //유니크 장비 합성기
	{
		int iCnt = m_vUniqueIcon.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vUniqueIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vUniqueGrade.size() ) )
				m_vGradeList.push_back( m_vUniqueGrade[i] );
		}
	}
	else if( m_iEtcType != ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND &&
		m_iEtcType != ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 &&
		m_iEtcType != ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )	// 모두
	{
		int i = 0;
		int iCnt = m_vWeaponIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vWeaponIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vWeaponGrade.size() ) )
				m_vGradeList.push_back( m_vWeaponGrade[i] );
		}

		iCnt = m_vArmorIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vArmorIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vArmorGrade.size() ) )
				m_vGradeList.push_back( m_vArmorGrade[i] );
		}

		iCnt = m_vHelmetIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vHelmetIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vHelmetGrade.size() ) )
				m_vGradeList.push_back( m_vHelmetGrade[i] );
		}

		iCnt = m_vCloakIcon.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szName = m_vCloakIcon[i];
			m_vIconList.push_back( szName );

			if ( COMPARE( i, 0, (int)m_vCloakGrade.size() ) )
				m_vGradeList.push_back( m_vCloakGrade[i] );
		}
	}
	else
	{
		switch( iItemType )
		{
		case ES_WEAPON:
			{
				int iCnt = m_vWeaponIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vWeaponIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vWeaponGrade.size() ) )
						m_vGradeList.push_back( m_vWeaponGrade[i] );
				}
			}
			break;
		case ES_ARMOR:
			{
				int iCnt = m_vArmorIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vArmorIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vArmorGrade.size() ) )
						m_vGradeList.push_back( m_vArmorGrade[i] );
				}
			}
			break;
		case ES_HELMET:
			{
				int iCnt = m_vHelmetIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vHelmetIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vHelmetGrade.size() ) )
						m_vGradeList.push_back( m_vHelmetGrade[i] );
				}
			}
			break;
		case ES_CLOAK:
			{
				int iCnt = m_vCloakIcon.size();
				for( int i=0; i < iCnt; ++i )
				{
					ioHashString szName = m_vCloakIcon[i];
					m_vIconList.push_back( szName );

					if ( COMPARE( i, 0, (int)m_vCloakGrade.size() ) )
						m_vGradeList.push_back( m_vCloakGrade[i] );
				}
			}
			break;
		}
	}
	//

	if( (int)m_vIconList.size() > 0 )
	{
		std::random_shuffle( m_vIconList.begin(), m_vIconList.end() );
	}
}

void UniqueItemMultipleCompoundResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon(fTimePerSec);
	RollingEffect(fTimePerSec);
}

void UniqueItemMultipleCompoundResultWnd::iwm_show()
{
	m_pLoopEffect = GetEffect("LoopEffect");
	if( m_pLoopEffect )
		m_pLoopEffect->SetSize( 200, 200 );

	m_pResultEffect = GetEffect("ResultEffect");
	if( m_pResultEffect )
		m_pResultEffect->SetSize( 200, 200 );
}

void UniqueItemMultipleCompoundResultWnd::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		// Icon 설정
		const ioItem *pItem = g_ItemMaker.GetItemConst( m_iItemCode, __FUNCTION__ );
		if( pItem && !m_bFinalResult )		// 한 번만 하기 위해
		{
			ioHashString szIcon = pItem->GetItemLargeIconName();
			if( !szIcon.IsEmpty() )
			{
				SAFEDELETE( m_pIcon );
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
			}

			m_ItemName = pItem->GetName();
			m_dwScaleStartTime = dwCurTime;
			m_nGradeType = pItem->GetGradeType();
		}

		// IconScale 설정
		float fScale = FLOAT1;
		m_fGradeScale = FLOAT1;
		DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
		if( dwGapTime < m_dwIconScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
			fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
			m_fGradeScale = fScale;
		}

		if( m_pIcon )
			m_pIcon->SetScale( fScale );

		if( !m_bFinalResult )
		{
			m_bFinalResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( !m_szResultSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szResultSound );

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
		}
	}
	else if( (int)m_vIconList.size() > 0 )				// Rolling
	{
		if( m_dwRollingCheckTime == 0 || dwCurTime - m_dwRollingCheckTime > m_dwRollingTime )		// 교체
		{
			SAFEDELETE( m_pIcon );

			m_iCurIconArray++;

			if( !COMPARE( m_iCurIconArray, 0, (int)m_vIconList.size() ) )
				m_iCurIconArray = 0;

			ioHashString szCurIcon = m_vIconList[m_iCurIconArray];
			if( !szCurIcon.IsEmpty() )
			{
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szCurIcon );
			}

			m_dwRollingCheckTime = dwCurTime;

			m_nGradeType = 0;
			if ( COMPARE( m_iCurIconArray, 0, (int)m_vGradeList.size() ) )
				m_nGradeType = m_vGradeList[m_iCurIconArray];
		}
	}
}

void UniqueItemMultipleCompoundResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	enum { EFFECT_X_OFFSET = 130, EFFECT_Y_OFFSET = 170};

	int iEffectListCnt = m_vRollingEffect.size();
	if( m_bFinalResult )
		iEffectListCnt = m_vEndEfffect.size();

	if( iEffectListCnt == 0 )
	{
		if( m_pResultEffect )
			m_pResultEffect->Stop();

		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_bFinalResult )
	{
		if( m_iCurEffectArray == iEffectListCnt-1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_pResultEffect )
		{
			if( !m_pResultEffect->IsLive() )
			{
				if( m_pLoopEffect )
					m_pLoopEffect->Stop();

				m_pResultEffect->Start();
			}

			m_pResultEffect->SetPosX( EFFECT_X_OFFSET - m_pResultEffect->GetWidth() / 2 );
			m_pResultEffect->SetPosY( EFFECT_Y_OFFSET - m_pResultEffect->GetHeight() / 2 );
		}

		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );
			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			{
				if( m_pResultEffect )
					m_pResultEffect->Stop();	

				m_iCurEffectArray = 0;
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}

	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			if( m_pLoopEffect )
			{
				if( !m_pLoopEffect->IsLive() )
				{
					m_pLoopEffect->Start();
				}

				m_pLoopEffect->SetPosX(EFFECT_X_OFFSET - m_pLoopEffect->GetWidth() / 2 );
				m_pLoopEffect->SetPosY(EFFECT_Y_OFFSET - m_pLoopEffect->GetHeight() / 2 );
			}

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void UniqueItemMultipleCompoundResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( m_iEtcType );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void UniqueItemMultipleCompoundResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_bFinalResult )
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y, m_fGradeScale );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Effect
	if( m_pEffect )
	{
		m_pEffect->SetScale( 2.0f );
		m_pEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_ADD );
	}

	if( m_bFinalResult )
	{
		PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, "장비합성을 성공했습니다" );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, "합성중입니다. 잠시만 기다려주세요." );
	}

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 14,
		TEXT_GAP	= 4,
	};

	if( m_bFinalResult && m_pNumText && m_pPlusText && m_iReinforce > 0 )
	{
		int iCurPos = 0;
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;
		int iDefaultPos = iXPos+ICON_X;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iReinforce < 10 )
			iTotalSize = TEXT_SIZE * 2 - TEXT_GAP;
		else if( m_iReinforce < FLOAT100 )
			iTotalSize = TEXT_SIZE * 3 - TEXT_GAP * 2;
		else
			iTotalSize = TEXT_SIZE * 4 - TEXT_GAP * 3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (TEXT_SIZE - TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iReinforce, -TEXT_GAP );
	}


	// Text
	ioComplexStringPrinter kPrinter[2];

	char szHack1[] = {'?','?','?','?','?','\0'};
	ioHashString szName = szHack1;
	ioHashString szDesc = "영구사용";

	if( m_bFinalResult )
	{
		if( !m_ItemName.IsEmpty() )
			szName = m_ItemName;
	}

	// Name
	kPrinter[0].ClearList();
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );
	kPrinter[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	kPrinter[1].ClearList();
	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, szDesc.c_str() );
	kPrinter[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+23, TAT_CENTER );
}

void UniqueItemMultipleCompoundResultWnd::iwm_hide()
{
	if( m_pLoopEffect )
		m_pLoopEffect->Stop();
	if(m_pResultEffect)
		m_pResultEffect->Stop();

	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void UniqueItemMultipleCompoundResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iEtcType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_iEtcType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_iEtcType, pUserEtcItem );
			}
		}
		break;
	}
}

void UniqueItemMultipleCompoundResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_iEtcType, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
