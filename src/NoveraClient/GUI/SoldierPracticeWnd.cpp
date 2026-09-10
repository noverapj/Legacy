

#include "StdAfx.h"

#include "SoldierPracticeWnd.h"

PracticeHelpWnd::PracticeHelpWnd()
{
	m_iStep = 0;
	m_iClassType = 0;
	m_dwTextTickTime	= 0;
	m_dwCurTextTickTime = 0;
	m_iCurTextTickCount	= 0;
	m_iCurTextLineCount	= 0;
	m_iSelectCharArray = -1;
	m_pPlayStage = NULL;

	m_pDevK = NULL;
	m_pDevKBack = NULL;
}

PracticeHelpWnd::~PracticeHelpWnd()
{
	SAFEDELETE( m_pDevK );
	SAFEDELETE( m_pDevKBack );
	m_vPracticeHelp.clear();
	m_vStringTickList.clear();
}

void PracticeHelpWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void PracticeHelpWnd::SetPracticeHelp( int iClassType, int iSelectCharArray )
{
	if( m_pPlayStage == NULL )
		return;

	if( g_TutorialMgr.IsTutorial() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	if( m_pPlayStage->GetModeType() != MT_HEADQUARTERS )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	if( iClassType == -1 ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	m_iClassType = iClassType;
	m_iSelectCharArray = iSelectCharArray;
	m_vPracticeHelp.clear();
	m_vStringTickList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_practice_help.ini" );
	sprintf_e( szBuf, "Set%d", m_iClassType );
	kLoader.SetTitle( szBuf );

	int iMaxStep = kLoader.LoadInt_e( "max_step", 0 );
	if( iMaxStep == 0 )
		return;

	for(int i = 0;i < iMaxStep;i++)
	{
		PracticeHelp kHelp;
		sprintf_e( szKey, "step%d_title", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kHelp.m_szTitle = szBuf;

		sprintf_e( szKey, "step%d_help", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kHelp.m_szHelp = szBuf;

		m_vPracticeHelp.push_back( kHelp );
	}
	ShowWnd();
	SetStep( 0 );
}

void PracticeHelpWnd::SetStep( int iStep )
{
	if( !COMPARE( iStep, 0, (int)m_vPracticeHelp.size() ) ) return;

	m_iStep = iStep;
	PracticeHelp &kHelp = m_vPracticeHelp[m_iStep];
	SetTitleText( kHelp.m_szTitle.c_str() );

	if( (int)m_vPracticeHelp.size() == m_iStep + 1 )
	{
		HideChildWnd( ID_NEXT );
		ShowChildWnd( ID_CLOSE );
		SetChildInActive( ID_CLOSE );
	}
	else
	{
		HideChildWnd( ID_CLOSE );
		ShowChildWnd( ID_NEXT );
		SetChildInActive( ID_NEXT );
	}

	if( iStep == 0 )
		SetChildInActive( ID_PREV );
	else
		SetChildActive( ID_PREV );

	m_dwCurTextTickTime = FRAMEGETTIME();
	m_iCurTextTickCount = 0;
	m_iCurTextLineCount = 0;

	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
	{
		SplitDescription( kHelp.m_szHelp );

		if( m_DescList.empty() ) return;

		m_vStringTickList.clear();				
		int iSize = m_DescList.size();
		for(int i = 0;i < iSize;i++)
		{
			StringTick kTick;
			m_vStringTickList.push_back( kTick );
		}
	}
}

void PracticeHelpWnd::SplitDescription( const ioHashString &szDesc )
{
	m_DescList.clear();
	m_DescList = ioStringConverter::Split( szDesc.c_str(), "#" );
}

void PracticeHelpWnd::iwm_show()
{
	if( g_GUIMgr.IsShow( EXERCISE_SOLDIER_EVENT_WND ) )
		g_GUIMgr.HideWnd( EXERCISE_SOLDIER_EVENT_WND );
}

void PracticeHelpWnd::iwm_hide()
{
	if( !m_pPlayStage ) return;

	ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
	if( pHeadquartersMode && pHeadquartersMode->GetCharState() == ioHeadquartersMode::STATE_UNITE_CHAR )
	{
		if( pHeadquartersMode->GetRecordPlayUserCnt() == 1 )
		{
			SP2Packet kPacket( CTPK_HEADQUARTERS_STATE_CHANGE );
			kPacket << (int)ioHeadquartersMode::STATE_OPEN_CHAR;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
	}
	if( m_iSelectCharArray != -1 )
	{
		ioBaseChar *pOwnerChar = m_pPlayStage->GetOwnerChar();
		if( pOwnerChar )
			pOwnerChar->SetChangeWaitState( m_iSelectCharArray, true );
	}
	// 해외 이벤트
	if( !g_GUIMgr.IsShow( EXERCISE_SOLDIER_EVENT_WND ) )
		g_GUIMgr.ShowWnd( EXERCISE_SOLDIER_EVENT_WND );

	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
		pMouse->SetTraining( false );

	g_GUIMgr.ShowWnd( HELP_WND );
}

bool PracticeHelpWnd::iwm_esc()
{
	return SetNextAction();
}

void PracticeHelpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			// 완료
			g_QuestMgr.QuestCompleteTerm( QC_SOLDIER_PRACTICE_SUCCESS, m_iClassType );
		}
		break;
	case ID_NEXT:
		if( cmd == IOBN_BTNUP )
		{
			// 다음 단계
			SetStep( m_iStep + 1 );			
		}
		break;
	case ID_PREV:
		if( cmd == IOBN_BTNUP )
		{
			// 이전 단계
			SetStep( max( 0, m_iStep - 1 ) );			
		}
		break;
	}
}

void PracticeHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevK" )
	{
		SAFEDELETE( m_pDevK );
		m_pDevK = pImage;
	}
	else if( szType == "DevKBack" )
	{
		SAFEDELETE( m_pDevKBack );
		m_pDevKBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PracticeHelpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwTextTickTime = xElement.GetIntAttribute_e( "TextTickTime" );
	m_szTextTickSnd  = xElement.GetStringAttribute_e( "TextTickSnd" );	
}

void PracticeHelpWnd::ProcessTextTick()
{
	if( m_dwCurTextTickTime == 0 ) return;
	if( !COMPARE( m_iCurTextLineCount, 0, (int)m_DescList.size() ) ) return;
	if( FRAMEGETTIME() - m_dwCurTextTickTime < m_dwTextTickTime ) return;

	m_dwCurTextTickTime = FRAMEGETTIME();

	if( !m_szTextTickSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szTextTickSnd );

	char szHelp[MAX_PATH] = "";
	memset( szHelp, 0, sizeof( szHelp ) );
	sprintf( szHelp, "%s", m_DescList[m_iCurTextLineCount].c_str() );

	int iMaxLen = strlen( szHelp );

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
	if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE) szHelp[m_iCurTextTickCount] ) )
#else
	if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE) szHelp[m_iCurTextTickCount] ) )
#endif

#else
	if( IsDBCSLeadByte( (BYTE) szHelp[m_iCurTextTickCount] ) )
#endif

		m_iCurTextTickCount+=2;
	else 
		m_iCurTextTickCount+=1;

	memcpy( m_vStringTickList[m_iCurTextLineCount].m_szCopyDesc, szHelp, m_iCurTextTickCount );

	if( m_iCurTextTickCount >= iMaxLen )
	{
		m_iCurTextTickCount = 0;
		m_iCurTextLineCount++;
	}

	if( m_iCurTextLineCount >= (int)m_DescList.size() )
	{
		m_dwCurTextTickTime = 0;
		m_iCurTextTickCount = 0;
		m_iCurTextLineCount = 0;

		SetChildActive( ID_NEXT );
		SetChildActive( ID_CLOSE );
	}
}

void PracticeHelpWnd::ProcessIoStringTick()
{
	if( m_dwCurTextTickTime == 0 ) return;
	if( FRAMEGETTIME() - m_dwCurTextTickTime < m_dwTextTickTime ) return;

	m_dwCurTextTickTime = FRAMEGETTIME();

	if( !m_szTextTickSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szTextTickSnd );

	char szHelp[MAX_BUFFER_SIZE] = "";
	memset( szHelp, 0, sizeof( szHelp ) );
	sprintf( szHelp, "%s", m_vPracticeHelp[m_iStep].m_szHelp.c_str() );

	int iMaxLen = strlen( szHelp );

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
	if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE) szHelp[m_iCurTextTickCount] ) )
#else
	if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE) szHelp[m_iCurTextTickCount] ) )
#endif

#else
	if( IsDBCSLeadByte( (BYTE) szHelp[m_iCurTextTickCount] ) || (szHelp[m_iCurTextTickCount] == 92 && szHelp[m_iCurTextTickCount+1] == 110) )
#endif

		m_iCurTextTickCount+=2;
	else 
		m_iCurTextTickCount+=1;

	char szTickHelp[MAX_BUFFER_SIZE] = "";
	memcpy( szTickHelp, szHelp, m_iCurTextTickCount );
	m_szIoString = szTickHelp;

	if( m_iCurTextTickCount >= iMaxLen )
	{
		m_dwCurTextTickTime = 0;
		m_iCurTextTickCount = 0;
		m_iCurTextLineCount = 0;

		SetChildActive( ID_NEXT );
		SetChildActive( ID_CLOSE );
	}
}

bool PracticeHelpWnd::iwm_spacebar()
{
	return SetNextAction();
}

void PracticeHelpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	
		return;  

	if( m_pPlayStage )
	{
		ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
		if( pOwner && pOwner->GetState() != CS_CHANGE_WAIT )
		{
			if( m_iSelectCharArray == -1 && m_iClassType != g_MyInfo.GetClassType() )
			{
				HideWnd();
			}		
		}
	}	
	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
		ProcessTextTick();
	else
		ProcessIoStringTick();
}

void PracticeHelpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	if( m_pDevKBack && m_pDevK )
	{
		m_pDevKBack->SetScale( 0.85f );
		m_pDevKBack->Render( iXPos + 40, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		m_pDevK->SetScale( 0.85f );
		m_pDevK->Render( iXPos + 40, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	//////////////////////////////////////////////////////////////////////////
	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
		int i = 0;
		int iInfoSize = m_vStringTickList.size();      
		for(;i < iInfoSize;i++)
			g_FontMgr.PrintText( iXPos + 77, iYPos + 47 + ( 17 * i ), FONT_SIZE_12, m_vStringTickList[i].m_szCopyDesc );
	}
	else
	{
		IoString str;
		str.Set(0, 0, m_szIoString.c_str(), FONT_SIZE_12, 290, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
		str.OnRender( iXPos + 77, iYPos + 47 );
	}
}

bool PracticeHelpWnd::SetNextAction()
{
	if( !IsShow() )
		return false;

	ioButton *pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_NEXT ) );
	if( pButton && pButton->IsShow() && !pButton->IsDisabled() )
	{
		iwm_command( pButton, IOBN_BTNUP, 0 );
		return true;
	}

	pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_CLOSE ) );
	if( pButton && pButton->IsShow() && !pButton->IsDisabled() )
	{
		iwm_command( pButton, IOBN_BTNUP, 0 );
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
int SoldierGradeInfo::GetSortIndex()
{
	switch( m_iGradeType )
	{
	case 4:		return 0;
	case 2:		return 1;
	case 6:		return 2;
	case 3:		return 3;
	}
	return INT_MAX;
}

SoldierPracticeSelectBtn::SoldierPracticeSelectBtn()
{
	m_pBaseBG = NULL;
	m_pNoneBG = NULL;
	m_pBlankBG = NULL;
	m_pStateBlueFrm = NULL;
	m_pCustomOverAdd	= NULL;
	m_pNone			= NULL;
	m_pIconBack		= NULL;
	m_pSoldierIcon  = NULL;
	m_pSoldierSubIcon = NULL;

	m_bHaveClass = false;

	m_cInfo.Init();
	m_szClassName.Clear();
}

SoldierPracticeSelectBtn::~SoldierPracticeSelectBtn()
{
	SAFEDELETE( m_pBaseBG );
	SAFEDELETE( m_pNoneBG );
	SAFEDELETE( m_pBlankBG );
	SAFEDELETE( m_pStateBlueFrm );
	SAFEDELETE( m_pCustomOverAdd );
	SAFEDELETE( m_pNone );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
}

void SoldierPracticeSelectBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BaseBG" )
	{
		SAFEDELETE( m_pBaseBG );
		m_pBaseBG = pFrame;
	}
	else if( szType == "NoneBG" )
	{
		SAFEDELETE( m_pNoneBG );
		m_pNoneBG = pFrame;
	}
	else if( szType == "BlankBG" )
	{
		SAFEDELETE( m_pBlankBG );
		m_pBlankBG = pFrame;
	}
	else if( szType == "CustomOverAdd" )
	{
		SAFEDELETE( m_pCustomOverAdd );
		m_pCustomOverAdd = pFrame;
	}
	else if( szType == "StateBlueFrm" )
	{
		SAFEDELETE( m_pStateBlueFrm );
		m_pStateBlueFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void SoldierPracticeSelectBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "None" )
	{
		SAFEDELETE( m_pNone );
		m_pNone = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else 
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void SoldierPracticeSelectBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szHaveText = xElement.GetStringAttribute_e( "HaveText" );
}

void SoldierPracticeSelectBtn::SetClassType( SoldierGradeInfo cInfo )
{
	m_cInfo = cInfo;

	if( g_MyInfo.GetCharIndexByClassType( m_cInfo.m_iClassType ) == 0 )
	{		
		m_bHaveClass = false;
	}
	else
	{
		m_bHaveClass = true;
	}

	SAFEDELETE( m_pSoldierIcon );
	SAFEDELETE( m_pSoldierSubIcon );
	m_szClassName.Clear();
	if( m_cInfo.m_iClassType != -1 )
	{
		SetActive();
		m_pSoldierIcon = g_MyInfo.GetMySoldierIcon( m_cInfo.m_iClassType );
		m_pSoldierSubIcon = g_MyInfo.GetSoldierSubIcon( m_cInfo.m_iClassType );
		m_szClassName = g_MyInfo.GetClassName( m_cInfo.m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_cInfo.m_iClassType ) );
	}
	else
	{
		SetInActive();
	}
}

void SoldierPracticeSelectBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( IsDisabled() )
	{
		if( m_pBlankBG )
			m_pBlankBG->Render( iXPos, iYPos );
		if( m_pNone )
			m_pNone->Render( iXPos + 55, iYPos + 42, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_bHaveClass )
		{
			if( m_pBaseBG )
				m_pBaseBG->Render( iXPos, iYPos );
		}
		else
		{
			if( m_pNoneBG )
				m_pNoneBG->Render( iXPos, iYPos );
		}		
		if( m_bOver || m_bClicked )
		{
			if( m_pCustomOverAdd )
				m_pCustomOverAdd->Render( iXPos, iYPos, UI_RENDER_ADD );
		}
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.97f );
			m_pIconBack->Render( iXPos + 56, iYPos + 44, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			g_UIImageRenderMgr.RenderGradeIconBack( m_cInfo.m_iGradeType, iXPos + 56, iYPos + 44, 0.97f );
		}
		if( m_pSoldierIcon )
		{
			m_pSoldierIcon->SetScale( 0.97f );
			m_pSoldierIcon->Render( iXPos + 56, iYPos + 44, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		if( m_pSoldierSubIcon )
		{
			m_pSoldierSubIcon->SetScale( 0.97f );
			m_pSoldierSubIcon->Render( iXPos + 56, iYPos + 44, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		if( m_bHaveClass )
		{
			if( m_pStateBlueFrm )
				m_pStateBlueFrm->Render( iXPos + 9, iYPos + 9 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iXPos + 29, iYPos + 11, FONT_SIZE_11, m_szHaveText.c_str() );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		g_FontMgr.PrintTextWidthCut( iXPos + 54, iYPos + 97, FONT_SIZE_12, 93, m_szClassName.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
SoldierPracticeSelectWnd::SoldierPracticeSelectWnd()
{
	m_pPlayStage = NULL;
	m_pTabLine = NULL;
	m_iCurPage = 0;
	m_iMaxPage = 0;
	m_iCurTab = 0;
}

SoldierPracticeSelectWnd::~SoldierPracticeSelectWnd()
{
	SAFEDELETE( m_pTabLine );
}

void SoldierPracticeSelectWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void SoldierPracticeSelectWnd::SetBtnPos()
{
	int iRealXPos = 17;
	int iRealYPos = 86;
	for(int i = 0;i < MAX_SOLDIER_BTN;i++)
	{
		ioWnd *pSoldier = FindChildWnd( ID_SOLDIER_BTN1 + i );
		if( pSoldier )
			pSoldier->SetWndPos( iRealXPos, iRealYPos );

		if( 0 == ( i + 1 ) % MAX_HORZ )
		{
			iRealXPos  = 17;
			iRealYPos  += 122;
		}
		else
		{
			iRealXPos += 112;
		}
	}

	iRealXPos = 17;
	iRealYPos = 49;
	for( int i = 0; i < TAB_COUNT; ++i )
	{
		ioWnd *pWnd = FindChildWnd( ID_TAB1 + i );
		if( !pWnd )
			continue;

		pWnd->SetWndPos( iRealXPos, iRealYPos );

		if( i == 0 )
			iRealXPos += 111;
		else
			iRealXPos += 112;
	}
}

void SoldierPracticeSelectWnd::UpdateSetPage( int iCurPage )
{
	m_iCurPage = iCurPage;

	int iListSIze = m_CharList.size();
	int iStartPos = iCurPage * MAX_SOLDIER_BTN;
	for(int i = ID_SOLDIER_BTN1;i < ID_SOLDIER_BTN18 + 1;i++,iStartPos++)
	{
		SoldierPracticeSelectBtn *pBtn = dynamic_cast<SoldierPracticeSelectBtn*>( FindChildWnd( i ) );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				pBtn->SetClassType( m_CharList[iStartPos] );
			}
			else
			{
				SoldierGradeInfo cInfo;
				pBtn->SetClassType( cInfo );
			}
		}
	}	
}

void SoldierPracticeSelectWnd::iwm_create()
{
	for(int i = 0;i < MAX_SOLDIER_BTN;i++)
	{
		g_GUIMgr.AddWndByTemplate( ioHashString("SoldierPracticeSelectBtn"), this, ID_SOLDIER_BTN1 + i );
	}
	for( int i = 0; i < TAB_COUNT; ++i )
	{
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("ioRadioButton"), this, ID_TAB1 + i );
		if( !pWnd )
			continue;

		if( i == 0 || i == TAB_COUNT - 1 )
			pWnd->SetSize( 110, 27 );

		pWnd->SetTitleText( m_vTabInfoVec[i].m_szText.c_str() );
	}
	CheckRadioButton( ID_TAB1, ID_TAB6, ID_TAB1 );
}

void SoldierPracticeSelectWnd::iwm_show()
{
	SetBtnPos();
	CreateList();
}

void SoldierPracticeSelectWnd::CreateList()
{
	m_CharList.clear();    
	SoldierGradeInfoVec HaveList;
	int iTabIndex = -1;
	if( COMPARE( m_iCurTab, 0, (int)m_vTabInfoVec.size() ) )
		iTabIndex = m_vTabInfoVec[m_iCurTab].m_iIndex;

	int iMax = g_ClassPrice.MaxClassPrice();
	for( int i = 0; i <  iMax; i++ )
	{
		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo || pInfo->IsDummyInfo() ) 
			continue;

		SoldierGradeInfo cInfo;
		cInfo.m_iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
		cInfo.m_iGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( cInfo.m_iClassType );		
		if( iTabIndex != -1 && iTabIndex != cInfo.m_iGradeType )
			continue;

		if( g_MyInfo.GetCharIndexByClassType( cInfo.m_iClassType ) != 0 )
		{
			HaveList.insert( HaveList.begin(), cInfo );
			continue;
		}

		if( g_ClassPrice.IsSoldierTraingActive() && !g_ClassPrice.IsActiveClass( i ) )
		{
			// inactive 용병이지만 표시해야 하는 경우가 있다 ( 해외 가챠 캡슐용병등 )
			bool bContinue = true;
			int iSize = m_vExtraClassVec.size();
			for (int j = 0; j < iSize ; j++)
			{
				int &rClassType = m_vExtraClassVec[j];
				if( cInfo.m_iClassType == rClassType )
				{
					bContinue = false;
					break;
				}
			}
			if( bContinue )
				continue;
		}
		m_CharList.push_back( cInfo );
	}
	std::sort( m_CharList.begin(), m_CharList.end(), SoldierPracticeSort() );
	std::sort( HaveList.begin(), HaveList.end(), SoldierPracticeSort() );

	m_CharList.insert( m_CharList.begin(), HaveList.begin(), HaveList.end() );
	HaveList.clear();

	m_iMaxPage = max( 0, (int)m_CharList.size() - 1 ) / MAX_SOLDIER_BTN;
	UpdateSetPage( 0 );
}

void SoldierPracticeSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	}
	if( COMPARE( dwID, ID_TAB1, ID_TAB6 + 1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			CheckRadioButton( ID_TAB1, ID_TAB6, dwID );

			m_iCurTab = dwID - ID_TAB1;
			CreateList();
		}
	}
	if( COMPARE( dwID, ID_SOLDIER_BTN1, ID_SOLDIER_BTN18 + 1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
			if( pHeadquartersMode && pHeadquartersMode->GetRecordPlayUserCnt() > 1 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			SoldierPracticeSelectBtn *pSoldier = dynamic_cast<SoldierPracticeSelectBtn*>(pWnd);
			int iClassType = pSoldier->GetClassType();
			if( iClassType == -1 ) return;
			if( !m_pPlayStage )
				return;

			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( !pOwner )	return;

			if( pHeadquartersMode && pHeadquartersMode->GetCharState() != ioHeadquartersMode::STATE_UNITE_CHAR )
			{
				SP2Packet kPacket( CTPK_HEADQUARTERS_STATE_CHANGE );
				kPacket << (int)ioHeadquartersMode::STATE_UNITE_CHAR;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}

			int iSelectCharArray = -1;
			if( g_MyInfo.GetCharIndexByClassType( iClassType ) == 0 )
			{
				iSelectCharArray = pOwner->GetSelectCharArray();
				pOwner->SetChangeWaitState( iClassType, true, true );
			}
			else
			{
				int iCharArray = g_MyInfo.GetClassArray( iClassType );
				if( iCharArray != pOwner->GetSelectCharArray() )
				{
					pOwner->SetChangeWaitState( iCharArray, true );
				}

#if defined( USE_GA )
				char chLabel[32] = {0,};

				if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
					sprintf_e( chLabel, "%d", iClassType );
				else
					SafeSprintf( chLabel, sizeof(chLabel), "%1", iClassType );

				// PLAY_HQ_TRAINING
				g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
					, "Play"
					, "Trainig"
					, chLabel
					, 1
					, "%2FPLAY%2FHQ%2FTRAINING" );
#endif
			}
			PracticeHelpWnd *pPracticeHelp = dynamic_cast<PracticeHelpWnd *>( g_GUIMgr.FindWnd(PRACTICE_HELP_WND) );
			if( pPracticeHelp )
			{
				pPracticeHelp->SetPracticeHelp( iClassType, iSelectCharArray );
			}
			g_GUIMgr.HideWnd( HELP_WND );
			HideWnd();
		}
	}
}

void SoldierPracticeSelectWnd::iwm_wheel( int zDelta )
{
	if( g_App.IsMouseBusy() )
		return;

	if( zDelta == WHEEL_DELTA )
		UpdateSetPage( max( 0, m_iCurPage - 1 ) );
	else if( zDelta == -WHEEL_DELTA )
		UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
}

void SoldierPracticeSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pTabLine )
	{
		for( int i = 0; i < TAB_COUNT - 1; i++ )
			m_pTabLine->Render( iXPos + 127 + 112 * i, iYPos + 74 );
	}
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(2), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 352, iYPos + 461, TAT_CENTER );
}

bool SoldierPracticeSelectWnd::QuestGuide( int iClassType, float &rfXPos, float &rfYPos )
{
	int i = 0;
	for(i = ID_SOLDIER_BTN1;i < ID_SOLDIER_BTN18 + 1;i++)
	{
		SoldierPracticeSelectBtn *pSelectBtn = dynamic_cast<SoldierPracticeSelectBtn*>( FindChildWnd( i ) );
		if( pSelectBtn )
		{
			if( pSelectBtn->GetClassType() == iClassType )
			{
				rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
				rfYPos = pSelectBtn->GetDerivedPosY();
				return true;
			}
		}
	}	

	// 리스트에 없으면..
	int iClassPos = 0;
	for(iClassPos = 0;iClassPos < (int)m_CharList.size();iClassPos++)
	{
		if( m_CharList[iClassPos].m_iClassType == iClassType )
			break;
	}
	if( iClassPos == (int)m_CharList.size() && m_iCurTab != 0 )
	{
		ioWnd *pTabBtn = FindChildWnd( ID_TAB1 );
		if( pTabBtn )
		{
			rfXPos = pTabBtn->GetDerivedPosX() + ( pTabBtn->GetWidth() / 2 );
			rfYPos = pTabBtn->GetDerivedPosY();
			return true;
		}
	}
	if( m_iCurPage < (iClassPos / MAX_SOLDIER_BTN) )
	{
		// Right Btn
		ioWnd *pRightBtn = FindChildWnd( ID_RIGHT_PAGE );
		if( pRightBtn )
		{
			rfXPos = pRightBtn->GetDerivedPosX() + ( pRightBtn->GetWidth() / 2 );
			rfYPos = pRightBtn->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		// Left Btn
		ioWnd *pLeftBtn = FindChildWnd( ID_LEFT_PAGE );
		if( pLeftBtn )
		{
			rfXPos = pLeftBtn->GetDerivedPosX() + ( pLeftBtn->GetWidth() / 2 );
			rfYPos = pLeftBtn->GetDerivedPosY();
			return true;
		}
	}

	return false;
}

void SoldierPracticeSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TabLine" )
	{
		SAFEDELETE( m_pTabLine );
		m_pTabLine = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void SoldierPracticeSelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_vExtraClassVec.clear();
	int iMax = xElement.GetIntAttribute_e( "MaxClass" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		wsprintf( szName, "ClassType%d", i+1 );
		int iClassType = xElement.GetIntAttribute( szName );
		if( iClassType  <= 0 )
			continue;

		m_vExtraClassVec.push_back( iClassType );
	}

	m_vTabInfoVec.clear();
	int iTabCount = xElement.GetIntAttribute_e( "TabCount" );
	for (int i = 0; i < iMax ; i++)
	{
		TabInfo cInfo;
		char szName[MAX_PATH]="";
		wsprintf( szName, "TabText%d", i+1 );
		cInfo.m_szText = xElement.GetStringAttribute( szName );
		wsprintf( szName, "TabIndex%d", i+1 );
		cInfo.m_iIndex = xElement.GetIntAttribute( szName );

		m_vTabInfoVec.push_back( cInfo );
	}
}