
#include "StdAfx.h"

#include "ItemMaterialCompoundWnd.h"
#include "SelectMaterialItemBtn.h"
#include "ItemCompoundWnd.h"
#include "MyItemSelecteWnd.h"

ItemMaterialCompoundWnd::ItemMaterialCompoundWnd(void)
{
	m_nReUseSlot = 0;
	m_pManual = NULL;
	m_pExtraSubBack = NULL;
	m_pManualMark = NULL;
	m_pGaugeMainBack = NULL;
	m_pGaugeBack  = NULL;
	m_pGauge      = NULL;
	m_pMagicCircle = NULL;
	m_pItemBack = NULL;

	InitData();
}

ItemMaterialCompoundWnd::~ItemMaterialCompoundWnd(void)
{
	SAFEDELETE( m_pExtraSubBack );
	SAFEDELETE( m_pManualMark );
	SAFEDELETE( m_pGaugeMainBack );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pMagicCircle );
	SAFEDELETE( m_pItemBack );
}

void ItemMaterialCompoundWnd::iwm_show()
{
	HideChildWnd( ID_MATERIALITEM_WND );
	HideChildWnd( ID_FAIL_MANUAL_BTN );

	if ( m_pManualMark )
		m_pManualMark->SetColor( 128, 128, 128 );

	InitData();

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
	
	//강화 가능 아이템 얻기
	LoadTotalItemList();	

	//강화 가능 아이템이 없다면 버튼 비활성화
	if( m_vTotalItemList.empty() )
		SetChildInActive( ID_TARGETITEM_BTN );
	else
		SetChildActive( ID_TARGETITEM_BTN );

	//재료 초기화
	SelectMaterialItemBtn *pMaterialItemIcon = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(ID_MATERIALITEM_ICON));
	if( pMaterialItemIcon )
	{
		pMaterialItemIcon->InitData();
		pMaterialItemIcon->ShowWnd();
	}

	SelectExtraItemBtn *pMaterialItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_MATERIALITEM_BTN));
	if( pMaterialItemBtn )
	{
		pMaterialItemBtn->SetSelectMaterial( 0, "" );
		pMaterialItemBtn->SetCurTitleColor( TCT_DEFAULT_DARKGRAY );
	}

	//다시 사용 가능하다면 슬롯에 재설정
	if ( m_nReUseSlot > 0 )
	{
		if ( SlotReload() )
			UpdateItemData();
		else 
			TargetItemInit();
	}
	else 
		TargetItemInit();	
}

void ItemMaterialCompoundWnd::iwm_hide()
{
	InitData();
	HideChildWnd( ID_MATERIALITEM_WND );
	HideChildWnd( ID_MATERIALITEM_ICON );
}

void ItemMaterialCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemCompound();
		}
		break;
	case ID_TARGETITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int nItemListSize = m_vTotalItemList.size();
			
			MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
			if ( pSelectWnd && nItemListSize > 0 )
			{
				ExtraItemList rkList;
				GetTargetSeletInfo( rkList );
				pSelectWnd->ShowItemSeletWnd( rkList, this );
			}
		}
		break;
	case ID_MATERIALITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			//리스트 설정
			int nItemListSize = m_vTotalItemList.size();
			NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_MATERIALITEM_WND));
			if( pListWnd && pListWnd->IsShow() )
			{
				pListWnd->HideWnd();
			}
			else if( pListWnd && nItemListSize > 0 )
			{
				pListWnd->CreatePullDown( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 25 );
				// 조건이 달라지면 리스트 따로 구하고...
				vNewShopPullDownItem kItemList;
				GetMaterialList( kItemList );

				if( !kItemList.empty() )
				{
					int iSize = kItemList.size();
					for(int i = 0;i < iSize; i++)
					{
						pListWnd->InsertItem( kItemList[i] );
					}
					pListWnd->ShowWnd();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				}
			}
		}
		break;
	case ID_MATERIALITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			//재료 선택 완료시
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				pPullDownWnd->HideWnd();
				MaterialListBtnUp( pPullDownWnd->GetOpenBtn(), param );
			}
		}
		break;
	case ID_FAIL_MANUAL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideChildWnd( ID_FAIL_TOOL_TIP );
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_nHelpTipIdx );
		}
		else if( cmd == IOWN_OVERED )
		{
			if ( m_pManualMark )
				m_pManualMark->SetColor( 206, 78, 0 );

			ShowChildWnd( ID_FAIL_TOOL_TIP );
		}
		else if( cmd == IOWN_LEAVED )
		{
			if ( m_pManualMark )
				m_pManualMark->SetColor( 128, 128, 128 );

			HideChildWnd( ID_FAIL_TOOL_TIP );
		}
		break;
	case MYITEM_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
			if( pTarget && param > 0 )
			{
				pTarget->SetSelectExtraItemSlot( param );
				UpdateItemData();
			}
		}
		break;
	}
}

void ItemMaterialCompoundWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ExtraSubBack" )
	{
		SAFEDELETE( m_pExtraSubBack );
		m_pExtraSubBack = pImage;
	}
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else if( szType == "MainBack" )
	{
		SAFEDELETE( m_pGaugeMainBack );
		m_pGaugeMainBack = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else if( szType == "magic_circle" )
	{
		SAFEDELETE( m_pMagicCircle );
		m_pMagicCircle = pImage;
	}
	else if( szType == "ItemBack" )
	{
		SAFEDELETE( m_pItemBack );
		m_pItemBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemMaterialCompoundWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_nExtraSubManualIdx = xElement.GetIntAttribute_e( "extra_sub_manual" );
	m_nHelpTipIdx = xElement.GetIntAttribute_e( "help_tip_idx" );
	m_szSelectItemMsg = xElement.GetStringAttribute_e( "SelectItemMsg" );
	m_szLackMaterialMsg = xElement.GetStringAttribute_e( "LackMaterialMsg" );
}

void ItemMaterialCompoundWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessMagicCircle();

	ioWnd::OnProcess( fTimePerSec );
}

void ItemMaterialCompoundWnd::CreateMagicCircle()
{
	DWORD dwCurTime = FRAMEGETTIME();

	MagicEffectInfo kInfo;
	kInfo.m_dwCreateTime = dwCurTime;

	m_vMagicEffectInfoList.push_back( kInfo );
}

void ItemMaterialCompoundWnd::ProcessMagicCircle()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMagicEffectCreateTime == 0 ||
		m_dwMagicEffectCreateTime+1000 < dwCurTime )
	{
		CreateMagicCircle();
		m_dwMagicEffectCreateTime = dwCurTime;
	}

	// process
	vMagicEffectInfo::iterator iter = m_vMagicEffectInfoList.begin();
	while( iter != m_vMagicEffectInfoList.end() )
	{
		DWORD dwGap = dwCurTime - (*iter).m_dwCreateTime;
		if( dwGap >= 1500 )
		{
			iter = m_vMagicEffectInfoList.erase( iter );
			continue;
		}

		float fRate = FLOAT1 - ((float)dwGap / 1500.0f);
		float fCurEffectRate = (float)sin(D3DX_PI * FLOAT05 * fRate);

		(*iter).m_iCurAlphaRate = (float)MAX_ALPHA_RATE * (0.5f*fCurEffectRate);
		(*iter).m_fCurScaleRate = 1.25f - (0.25f*fCurEffectRate);

		++iter;
	}
}

void ItemMaterialCompoundWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	OnRenderMagicCircle();

	if( m_pItemBack )
		m_pItemBack->Render( iXPos + 96, iYPos + 94 );

	SuccessRateDescRender();
	RenderFailExp( iXPos, iYPos );
	PrintManual( iXPos, iYPos, FONT_SIZE_12 );
}

void ItemMaterialCompoundWnd::OnRenderMagicCircle()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pMagicCircle )
	{
		m_pMagicCircle->SetAlpha( MAX_ALPHA_RATE );
		m_pMagicCircle->SetScale( FLOAT1 );
		m_pMagicCircle->Render( iXPos+129, iYPos+167 );

		// 퍼지는 이펙트
		vMagicEffectInfo::iterator iter = m_vMagicEffectInfoList.begin();
		while( iter != m_vMagicEffectInfoList.end() )
		{
			m_pMagicCircle->SetAlpha( (*iter).m_iCurAlphaRate );
			m_pMagicCircle->SetScale( (*iter).m_fCurScaleRate );
			m_pMagicCircle->Render( iXPos+129, iYPos+167 );

			++iter;
		}
	}
}

void ItemMaterialCompoundWnd::SuccessRateDescRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		INFO_LEFT_X_OFFSET = 33,
		INFO_LEFT_Y_OFFSET = 415,

		INFO_RIGHT_X_OFFSET = 225,
		INFO_RIGHT_Y_OFFSET = 415,
	};

	if( m_bIsSelectMaterial )
	{
		SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
		if( !pTarget ) 
			return;

		int nTargetSlot = pTarget->GetSelectExtraItemSlot();
		if ( nTargetSlot > 0)
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, STR(1) );

			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET, FONT_SIZE_12, "%.2f％", m_fSuccessRate );
		}
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		if( m_bIsSelectItem )
			g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, m_szLackMaterialMsg.c_str() );
		else
			g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, m_szSelectItemMsg.c_str() );
	}
}

void ItemMaterialCompoundWnd::RenderFailExp( int iXPos, int iYPos )
{
	if( !m_pManualMark || !m_pGaugeMainBack || !m_pGaugeBack || !m_pGauge || m_nMaxFailExp <= 0 )
		return;

	enum 
	{		
		GAUGE_X_OFFSET = 57,
		GAUGE_Y_OFFSET = 392,
		GAUGE_BACK_OFFSET = 2,

		TEXT_X_OFFSET  = 130,
		TEXT_Y_OFFSET  = 374,

		MANUAL_X_OFFSET = 207,
		MANUAL_Y_OFFSET = 390,
	};
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, " %.2f％", m_fFailExpRate * FLOAT100 );	
	kPrinter.PrintFullText( iXPos + TEXT_X_OFFSET, iYPos + TEXT_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();

	//게이지
	int nGaugeWidth = m_pGauge->GetWidth() * (float)min( FLOAT1, m_fFailExpRate );
	m_pGaugeMainBack->Render( iXPos + GAUGE_X_OFFSET - GAUGE_BACK_OFFSET, iYPos + GAUGE_Y_OFFSET - GAUGE_BACK_OFFSET );
	m_pGaugeBack->Render( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET );
	m_pGauge->RenderWidthCut( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET, 0, nGaugeWidth );
	m_pManualMark->Render( iXPos + MANUAL_X_OFFSET, iYPos + MANUAL_Y_OFFSET, UI_RENDER_NORMAL_ADD_COLOR );
}

void ItemMaterialCompoundWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	enum
	{
		DEFAULT_MANUAL_X_OFFSET = 28,
		DEFAULT_MANUAL_Y_OFFSET = 281,

		SUB_MANUAL_X_OFFSET = 28,
		SUB_MANUAL_Y_OFFSET = 281,

		EXTRA_SUB_MANUAL_X_OFFSET = 28,
		EXTRA_SUB_MANUAL_Y_OFFSET = 333,

		EXTRA_SUB_BACK_X_OFFSET = 27,
		EXTRA_SUB_BACK_Y_OFFSET = 332,
	};

	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
	if( !pItem ) 
		return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	int nXpos = a_iXPos + DEFAULT_MANUAL_X_OFFSET;
	int nYpos = a_iYPos + DEFAULT_MANUAL_Y_OFFSET;

	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		nYpos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( nXpos, nYpos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}

	if ( m_bIsSelectItem )
	{
		if ( m_nCurReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
		{
			iSubManual = m_nExtraSubManualIdx - 1;
			nXpos = a_iXPos + EXTRA_SUB_MANUAL_X_OFFSET;
			nYpos = a_iYPos + EXTRA_SUB_MANUAL_Y_OFFSET;

			int iMaxLine = m_pManual->GetMaxLine( iSubManual );
			for(int iLine = 1; iLine < iMaxLine; iLine++)
			{
				m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
				nYpos += m_pManual->GetSpaceLine( iSubManual, iLine);
				kPrinter.PrintFullTextWidthCut( nXpos, nYpos, TAT_LEFT, DESC_WIDTH );
				kPrinter.ClearList();
			}
			m_pExtraSubBack->Render( a_iXPos+EXTRA_SUB_BACK_X_OFFSET, a_iYPos+EXTRA_SUB_BACK_Y_OFFSET );
		}
	}
}

void ItemMaterialCompoundWnd::SetMaterialCode( int nMaterialCode )
{
	if ( !m_bIsSelectItem )
		return;

	SelectMaterialItemBtn *pMaterialItemIcon = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(ID_MATERIALITEM_ICON));
	if( !pMaterialItemIcon )
		return;

	if( nMaterialCode == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
	{
		ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
		if( !pUserEtc )
			return;

		int iHaveCount = 0;
		ioUserEtcItem::ETCITEMSLOT rkEtcItem;
		if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, rkEtcItem ) )
			iHaveCount = rkEtcItem.m_iValue1;

		int iNeedCount = g_CompoundMaterialMgr.GetNeedMaterialCount( m_nCurReinforce, m_dwEtcItemCode );
		if( iHaveCount >= iNeedCount )
		{
			pMaterialItemIcon->SetData( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iNeedCount );
			m_bIsSelectMaterial = true;
			m_bSelectAdditive = true;
			ApplySuccessRate();
		}
	}
	else
	{
		int iHavePeso = g_MyInfo.GetMoney();
		int iNeedPeso = g_CompoundMaterialMgr.GetNeedPeso( m_nCurReinforce, m_dwEtcItemCode );

		if( iHavePeso >= iNeedPeso )
		{
			pMaterialItemIcon->SetData( 0, iNeedPeso );
			m_bIsSelectMaterial = true;
			m_bSelectAdditive = false;
			ApplySuccessRate();
		}
	}
	if( m_bIsSelectMaterial )
		SetChildActive( ID_COMPOUND_BTN );
	else
		SetChildInActive( ID_COMPOUND_BTN );
}

void ItemMaterialCompoundWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	ShowWnd();
}

void ItemMaterialCompoundWnd::InitData()
{
	m_vTotalItemList.clear();
	m_nCurReinforce = 0;
	m_fSuccessRate = 0.0f;
	m_bIsSelectItem = false;
	m_bIsSelectMaterial = false;
	m_nMaxFailExp = 0;
	m_fFailExpRate = 0.0f;
	m_nCurFailExp = 0;
	m_dwMagicEffectCreateTime = 0;

	SetChildInActive( ID_COMPOUND_BTN );
}

void ItemMaterialCompoundWnd::LoadTotalItemList()
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
	{
		HideWnd();
		return;
	}

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		MaterialCompoundTargetItemInfo kInfo;
		kInfo.m_iSlotIndex = kSlot.m_iIndex;
		kInfo.m_iItemCode = kSlot.m_iItemCode;
		kInfo.m_iReinforce = kSlot.m_iReinforce;

		m_vTotalItemList.push_back( kInfo );
	}

	std::sort( m_vTotalItemList.begin(), m_vTotalItemList.end(), MaterialCompoundItemInfoSort() );
}

//강화 아이템 초기화
void ItemMaterialCompoundWnd::TargetItemInit()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget )
		pTarget->InitData();

	InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
	if( pTargetIconBtn )
		pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );

	SetChildInActive( ID_MATERIALITEM_BTN );
}

void ItemMaterialCompoundWnd::GetTargetList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalItemList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalItemList.begin(), m_vTotalItemList.end(), MaterialCompoundItemInfoSort() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalItemList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMaterialMgr.GetMaxReinforce() )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = kTargetSlot.m_iIndex;

		char szName[MAX_PATH]="";
		if( kTargetSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), STR(1), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kTargetSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), STR(2), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName = szName;
		rkList.push_back( kItem );
	}
}

void ItemMaterialCompoundWnd::GetTargetSeletInfo( ExtraItemList &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalItemList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalItemList.begin(), m_vTotalItemList.end(), MaterialCompoundItemInfoSort() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalItemList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMaterialMgr.GetMaxReinforce() )
			continue;

		rkList.push_back( kTargetSlot );
	}
}

bool ItemMaterialCompoundWnd::SlotReload()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget )
		return false;

	vNewShopPullDownItem kItemList;
	GetTargetList( kItemList );	

	vNewShopPullDownItem::iterator iter = kItemList.begin();
	for( ; iter != kItemList.end(); ++iter )
	{
		const NewShopPullDownItem& rkNewShopItem = *iter;
		if( rkNewShopItem.m_iMagicCode == m_nReUseSlot )
		{			
			pTarget->SetSelectExtraItemSlot( m_nReUseSlot );
			return true;
		}
	}

	return false;
}

void ItemMaterialCompoundWnd::UpdateItemData()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) 
		return;
	
	int nTargetSlot = pTarget->GetSelectExtraItemSlot();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( nTargetSlot, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );
			}

			//현재 강화 레벨 설정
			m_nCurReinforce = kSlot.m_iReinforce;		
			
			m_bIsSelectItem = true;
			m_bIsSelectMaterial = false;
			m_bSelectAdditive = false;

			//실패 경험치 얻기
			m_nMaxFailExp = g_CompoundMaterialMgr.GetMaxFailExp( m_nCurReinforce, m_dwEtcItemCode );
			m_nCurFailExp = (int)kSlot.m_failExp;

			if ( m_nMaxFailExp > 0 )
			{
				m_fFailExpRate = ( (float)m_nCurFailExp / m_nMaxFailExp );
				ShowChildWnd( ID_FAIL_MANUAL_BTN );
			}
			else
				HideChildWnd( ID_FAIL_MANUAL_BTN );

			m_fFailExpRate = min( FLOAT1, m_fFailExpRate );

			SelectMaterialItemBtn *pMaterialItemIcon = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(ID_MATERIALITEM_ICON));
			if( !pMaterialItemIcon )
				return;

			SelectExtraItemBtn *pMaterialItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_MATERIALITEM_BTN));
			if( pMaterialItemBtn )
			{
				pMaterialItemBtn->SetInActive();
				pMaterialItemBtn->SetSelectMaterial( 0, "" );
				pMaterialItemBtn->SetCurTitleColor( TCT_DEFAULT_DARKGRAY );
			}

			int iHavePeso = g_MyInfo.GetMoney();
			int iNeedPeso = g_CompoundMaterialMgr.GetNeedPeso( m_nCurReinforce, m_dwEtcItemCode );
			
			ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
			if( !pUserEtc )
				return;

			int iHaveCount = 0;
			ioUserEtcItem::ETCITEMSLOT rkEtcItem;
			if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, rkEtcItem ) )
				iHaveCount = rkEtcItem.m_iValue1;

			int iNeedCount = g_CompoundMaterialMgr.GetNeedMaterialCount( m_nCurReinforce, m_dwEtcItemCode );
			if( iHaveCount >= iNeedCount )
			{
				MaterialListBtnUp( pMaterialItemBtn, 0 );
				pMaterialItemIcon->SetData( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iNeedCount );
				m_bIsSelectMaterial = true;
				m_bSelectAdditive = true;
				ApplySuccessRate();
				SetChildActive( ID_MATERIALITEM_BTN );
			}
			else if( m_nCurReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				pMaterialItemIcon->SetRecommand( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iNeedCount );
			else if( iHavePeso >= iNeedPeso )
			{
				MaterialListBtnUp( pMaterialItemBtn, 1 );
				pMaterialItemIcon->SetData( 0, iNeedPeso );
				m_bIsSelectMaterial = true;
				ApplySuccessRate();
				SetChildActive( ID_MATERIALITEM_BTN );
			}
			else
				pMaterialItemIcon->SetRecommand( 0, iNeedPeso );

			//경고 문구 출력
			int nFixedReinforce = g_CompoundMaterialMgr.GetFixedFailReinforce();
			if ( m_nCurReinforce >= nFixedReinforce )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), nFixedReinforce, nFixedReinforce );
		}
	}
	if( m_bIsSelectMaterial )
		SetChildActive( ID_COMPOUND_BTN );
	else
		SetChildInActive( ID_COMPOUND_BTN );
}

void ItemMaterialCompoundWnd::SendItemCompound()
{
	if( !m_bIsSelectMaterial )
		return;

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) 
		return;
	
	bool bTarget;
	bTarget = false;

	int nTargetSlot = pTarget->GetSelectExtraItemSlot();
	if ( nTargetSlot <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( nTargetSlot, kSlot ) )
			bTarget = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	m_nReUseSlot = nTargetSlot;

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemCode;
	kPacket << nTargetSlot;
	if( m_bSelectAdditive )
		kPacket << (int)MATERIAL_SPIRIT;
	else
		kPacket << (int)MATERIAL_PESO;

	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemMaterialCompoundWnd::GetMaterialList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return;

	char szCnt[MAX_PATH]="";	

	int iHaveCount = 0;
	ioUserEtcItem::ETCITEMSLOT rkEtcItem;
	if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, rkEtcItem ) )
		iHaveCount = rkEtcItem.m_iValue1;

	int iNeedCount = g_CompoundMaterialMgr.GetNeedMaterialCount( m_nCurReinforce, m_dwEtcItemCode );
	if( iHaveCount >= iNeedCount )
	{
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE );
		if( !pItem ) 
			return;

		NewShopPullDownItem kItem;
		wsprintf_e( szCnt, "%d개", iHaveCount );

		kItem.m_dwTextColor = TCT_DEFAULT_GREEN;
		kItem.m_dwCntTextColor = TCT_DEFAULT_GREEN;
		kItem.m_szName      = pItem->GetName();
		kItem.m_iMagicCode	= ioEtcItem::EIT_ETC_ADDICTIVE_PIECE;
		kItem.m_bCount = true;
		kItem.m_szCnt = szCnt;
		rkList.push_back( kItem );
	}

	if( m_nCurReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
		return;

	int iHavePeso = g_MyInfo.GetMoney();
	int iNeedPeso = g_CompoundMaterialMgr.GetNeedPeso( m_nCurReinforce, m_dwEtcItemCode );
	if( iHavePeso >= iNeedPeso && iNeedPeso > 0 )
	{
		NewShopPullDownItem kItem;
		Help::ConvertNumToStrComma( iNeedPeso, szCnt, sizeof(szCnt) );

		kItem.m_dwTextColor = TCT_DEFAULT_GREEN;
		kItem.m_dwCntTextColor = TCT_DEFAULT_GREEN;
		kItem.m_szName      = "페소";
		kItem.m_iMagicCode	= 0;
		kItem.m_bCount = true;
		kItem.m_szCnt = szCnt;
		rkList.push_back( kItem );
	}
}

void ItemMaterialCompoundWnd::MaterialListBtnUp( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetMaterialList( kItemList );	

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		iListArray = kItemList.size() - 1;		

	ioUITitle *pTitle = pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	int nSelectMaterialCode = rkItem.m_iMagicCode;
	SetMaterialCode( nSelectMaterialCode );

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_MATERIALITEM_BTN));
	if( pTarget )
	{
		pTarget->SetSelectMaterial( nSelectMaterialCode, rkItem.m_szName );
		pTarget->SetCurTitleColor( rkItem.m_dwTextColor );
	}
}

void ItemMaterialCompoundWnd::ApplySuccessRate()
{	
	float fSuccessRate = 0.0f;

	//최대 실패경험치가 존재할 때
	if ( m_nMaxFailExp > 0 )
	{
		//성공율 얻기
		fSuccessRate = g_CompoundMaterialMgr.GetSuccessConst( m_dwEtcItemCode );
		fSuccessRate = ((float)( m_nCurFailExp + fSuccessRate ) / m_nMaxFailExp * FLOAT100 );
	}
	else if ( m_nMaxFailExp == 0 )
		fSuccessRate = g_CompoundMaterialMgr.GetReinforceSuccessRate( m_nCurReinforce, m_dwEtcItemCode );

	fSuccessRate += g_CompoundMaterialMgr.GetPCRoomBonusReinforceSuccessRate( m_nCurReinforce, m_dwEtcItemCode );

	m_fSuccessRate = fSuccessRate;

	m_fSuccessRate = min( FLOAT100, m_fSuccessRate );
	m_fSuccessRate = max( 0.0f, m_fSuccessRate );
}

bool ItemMaterialCompoundWnd::GetCurAlchemicItemInfo( OUT AlchemicToolTipInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( COMPARE(dwItemInfoID, ID_MATERIALITEM_ICON, ID_MATERIALITEM_ICON+1) )
	{
		SelectMaterialItemBtn *pItem = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(dwItemInfoID));
		if( pItem )
		{
			int nCode = pItem->GetCode();
			if( nCode <= 0 )
				return false;

			rkInfo.m_iCode = nCode;
			rkInfo.m_iType = AlchemicItemToolTip::ATT_ETCITEM;
			return true;
		}
	}
	return false;
}