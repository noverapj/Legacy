

#include "StdAfx.h"
#include "ioTimeGrowthEndWnd.h"

//////////////////////////////////////////////////////////////////////////
ioTimeGrowthEndWnd::ioTimeGrowthEndWnd()
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pIconBack = NULL;

	InitInfoWnd();
}

ioTimeGrowthEndWnd::~ioTimeGrowthEndWnd()
{
	m_vEndInfoList.clear();

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
}

void ioTimeGrowthEndWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void ioTimeGrowthEndWnd::InitInfoWnd()
{
	m_iCurInfoCnt = 0;
	m_iMaxInfoCnt = 0;
	m_vEndInfoList.clear();

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
}

void ioTimeGrowthEndWnd::ShowEndInfoWnd()
{
	if( m_vEndInfoList.empty() )
		return;
	
	m_iCurInfoCnt = 1;

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );

	int iCurIndex = m_iCurInfoCnt - 1;
	if( COMPARE( iCurIndex, 0, (int)m_vEndInfoList.size() ) )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_vEndInfoList[iCurIndex].m_IconName );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( m_vEndInfoList[iCurIndex].m_SubIconName );

		HideChildWnd( ID_FAIL );
	}

	if( !IsShow() )
		ShowWnd();
}

void ioTimeGrowthEndWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioTimeGrowthEndWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 102,
		ICON_Y		= 98,

		DESC_X		= 102,
		DESC_Y		= 145,

		LINE_GAP	= 17,
	};

	if( m_pIconBack )
		m_pIconBack->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	// Text
	int iCurArray = m_iCurInfoCnt - 1;
	if( !COMPARE( iCurArray, 0, (int)m_vEndInfoList.size() ) )
		return;

	ioComplexStringPrinter kPrinter[3];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), m_vEndInfoList[iCurArray].m_ClassName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2), m_vEndInfoList[iCurArray].m_GrowthName.c_str() );
	
	if( m_vEndInfoList[iCurArray].m_bConfirm )
	{
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "+1" );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );
		kPrinter[2].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );
	}
	else
	{
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "+0" );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );	
		kPrinter[2].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(4) );
	}

	for( int i=0; i < 3; ++i )
	{
		if( kPrinter[i].IsEmpty() )
			continue;

		kPrinter[i].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+(LINE_GAP*i), TAT_CENTER );
	}
}

void ioTimeGrowthEndWnd::iwm_hide()
{
	InitInfoWnd();
}

void ioTimeGrowthEndWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SUCCESS:
	case ID_FAIL:
		if( cmd == IOBN_BTNUP )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				int iClassType = -1;
				int iCurIndex = m_iCurInfoCnt - 1;
				if( COMPARE( iCurIndex, 0, (int)m_vEndInfoList.size() ) )
				{
					iClassType = m_vEndInfoList[iCurIndex].m_iClassType;
				}

				pInvenWnd->ShowSoldierTabDirect( iClassType );
			}

			HideWnd();
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			NextInfoWnd();
		}
		break;
	}
}

void ioTimeGrowthEndWnd::AddEndInfo( int iClassType,
									 const ioHashString& szClassName,
									 const ioHashString& szIconName,
									 const ioHashString& szSubIconName,
									 const ioHashString& szGrowthName,
									 int iLevel,
									 bool bConfirm )
{
	TimeEndInfo kInfo;
	kInfo.m_iClassType = iClassType;
	kInfo.m_ClassName = szClassName;
	kInfo.m_GrowthName = szGrowthName;
	kInfo.m_IconName = szIconName;
	kInfo.m_SubIconName = szSubIconName;
	kInfo.m_iLevel = iLevel;
	kInfo.m_bConfirm = bConfirm;

	m_vEndInfoList.push_back( kInfo );

	m_iMaxInfoCnt = m_vEndInfoList.size();
}

void ioTimeGrowthEndWnd::NextInfoWnd()
{
	m_iCurInfoCnt++;
	if( m_iCurInfoCnt > m_iMaxInfoCnt )
	{
		HideWnd();
		return;
	}

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );

	int iCurIndex = m_iCurInfoCnt - 1;
	if( COMPARE( iCurIndex, 0, (int)m_vEndInfoList.size() ) )
	{
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_vEndInfoList[iCurIndex].m_IconName );
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( m_vEndInfoList[iCurIndex].m_SubIconName );
		HideChildWnd( ID_FAIL );
	}
}