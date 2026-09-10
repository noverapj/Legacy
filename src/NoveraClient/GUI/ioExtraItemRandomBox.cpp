
#include "StdAfx.h"

#include "ioExtraItemRandomBox.h"
#include "../ioExtendSoundManager.h"
//////////////////////////////////////////////////////////////////////////
ioExtraItemRandomBox::ioExtraItemRandomBox()
{
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_pStateGreenFrm = NULL;
	m_pStateOrangeFrm = NULL;

	m_pIcon	= NULL;
	m_pEffect = NULL;

	m_bResult = false;
	m_bFinalResult = false;

	m_iPeriodTime = -1;

	m_pManual = NULL;
	m_iTradeType = EET_DISABLE;

	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;
}

ioExtraItemRandomBox::~ioExtraItemRandomBox()
{
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pFinalBackEffect);

	SAFEDELETE(m_pNumText);
	SAFEDELETE(m_pPlusText);

	SAFEDELETE(m_pStateGreenFrm);
	SAFEDELETE(m_pStateOrangeFrm);

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);
}

void ioExtraItemRandomBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ioExtraItemRandomBox::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateGreenFrm" )
		{
			SAFEDELETE( m_pStateGreenFrm );
			m_pStateGreenFrm = pFrame;
		}
		else if( szType == "StateOrangeFrm" )
		{
			SAFEDELETE( m_pStateOrangeFrm );
			m_pStateOrangeFrm = pFrame;
		}
		else
		{	
			ioWnd::AddRenderFrame( szType, pFrame );
		}
	}
}

void ioExtraItemRandomBox::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTime = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
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

void ioExtraItemRandomBox::ShowRandomBox( int iMachineCode )
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	m_bResult = false;
	m_bFinalResult = false;
	m_iPeriodTime = -1;

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingCheckTime = 0;
	m_dwRollingEffectCheckTime = 0;
	m_nGradeType = 0;
	m_fGradeScale = FLOAT1;

	m_vIconList.clear();
	m_vGradeList.clear();

	ItemListInfoSortVector vTotalList;
	g_ExtraItemInfoMgr.GetRandomItemList( iMachineCode, vTotalList );
	for(ItemListInfoSortVector::iterator iter = vTotalList.begin(); iter != vTotalList.end(); ++iter)
	{
		ItemListInfoSort &rkInfo = (*iter);
		const ioItem *pItem = g_ItemMaker.GetItemConst( rkInfo.m_iItemCode, __FUNCTION__ );
		if( !pItem )
			continue;
		m_vIconList.push_back( pItem->GetItemLargeIconName() );
		m_vGradeList.push_back( pItem->GetGradeType() );
	}

	if( (int)m_vIconList.size() == 1)
	{
		m_dwRollingEndTime = dwCurTime + 200;
	}

	m_iCurIconArray = -1;
	m_iCurEffectArray = -1;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pEffect);

	ReBatchButton( ROLLING_BTN );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void ioExtraItemRandomBox::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	RollingIcon( fTimePerSec );
	RollingEffect( fTimePerSec );
}

void ioExtraItemRandomBox::RollingIcon( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bResult && m_dwRollingEndTime < dwCurTime )			// RollingEnd
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

			if( g_ExSoundMgr.IsSound( ExSound::EST_GASHAPON_GET ) )
			{
				g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
			}
			else
			{
				if( !m_szResultSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szResultSound );
			}

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;

			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_ACQUIRE, m_iItemCode, m_iPeriodTime );
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
			m_fGradeScale = FLOAT1;
			if ( COMPARE( m_iCurIconArray, 0, (int)m_vGradeList.size() ) )
				m_nGradeType = m_vGradeList[m_iCurIconArray];
		}
	}
}

void ioExtraItemRandomBox::RollingEffect( float fTimePerSec )
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

void ioExtraItemRandomBox::OnRender()
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

	// Reinforce
	enum
	{
		REINFORCE_Y	= 14,

		TEXT_SIZE	= 20,
		PLUS_SIZE	= 14,
		TEXT_GAP	= 4,

		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
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
			iTotalSize = PLUS_SIZE + TEXT_SIZE - TEXT_GAP;
		else if( m_iReinforce < 100 )
			iTotalSize = PLUS_SIZE + TEXT_SIZE*2 - TEXT_GAP*2;
		else
			iTotalSize = PLUS_SIZE + TEXT_SIZE*3 - TEXT_GAP*3;

		iCurPos = iDefaultPos - (iTotalSize / 2);
		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (PLUS_SIZE-TEXT_GAP);
		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iReinforce, -TEXT_GAP );
	}

	if( m_bFinalResult )
	{
		int iNewXPos = iXPos+ICON_X;
		int iNewYPos = iYPos+ICON_Y;

		int iMarkX = iNewXPos;
		int iMarkY = iNewYPos;
		int iTextX = iMarkX;
		int iTextY = iMarkY;

		char szName[MAX_PATH]="";
		if( m_iTradeType == EET_NORMAL )
		{
			int iMarkX = iNewXPos-45;
			int iMarkY = iNewYPos-41;

			if( m_pStateOrangeFrm )
				m_pStateOrangeFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

			iTextX = iMarkX+27;
			iTextY = iMarkY+Y_TEXT_OFFSET;
			SafeSprintf( szName, sizeof( szName ), STR(7) );
		}
		else if( m_iTradeType == EET_ENABLE )
		{
			int iMarkX = iNewXPos-45;
			int iMarkY = iNewYPos-41;

			if( m_pStateGreenFrm )
				m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

			iTextX = iMarkX+27;
			iTextY = iMarkY+Y_TEXT_OFFSET;
			SafeSprintf( szName, sizeof( szName ), STR(8) );
		}

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
	}

	if( m_bFinalResult )
	{
		//PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(3) );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12, STR(5) );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos+DESC_X, iYPos+DESC_Y+18, FONT_SIZE_12, STR(6) );
	}


	// Text
	ioComplexStringPrinter kPrinter[2];

	ioHashString szName = "?????";
	ioHashString szDesc = "?????";

	if( m_bFinalResult )
	{
		char szBuf[MAX_PATH] = "";

		if( !m_ItemName.IsEmpty() )
			szName = m_ItemName;

		if( m_iPeriodTime == 0 )
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1) );
			szDesc = szBuf;
		}
		else if( ( m_iPeriodTime / 24 ) > 0 )
		{
			SafeSprintf( szBuf, sizeof( szBuf), STR(2), m_iPeriodTime/24 );
			szDesc = szBuf;
		}
		else
		{
			SafeSprintf( szBuf, sizeof( szBuf), STR(4), m_iPeriodTime );
			szDesc = szBuf;
		}
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

void ioExtraItemRandomBox::iwm_hide()
{
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void ioExtraItemRandomBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void ioExtraItemRandomBox::ReBatchButton( int iType )
{
	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		ShowChildWnd(ID_GO);
		break;
	}
}

void ioExtraItemRandomBox::OnResultInfo( int iItemCode, int iPeriodTime, int iReinforce, int iTradeType )
{
	m_bResult = true;
	m_iItemCode = iItemCode;
	m_iPeriodTime = iPeriodTime;
	m_iReinforce = iReinforce;
	m_iTradeType = iTradeType;
}

void ioExtraItemRandomBox::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	int iSubManual = 0;
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