

#include "StdAfx.h"

#include "../ioDateHelp.h"
#include "../ioFishingManager.h"
#include "FishingInventoryWnd.h"
#include "ioFishingMessageBox.h"

//////////////////////////////////////////////////////////////////////////
FishingAutoSellQuestion::FishingAutoSellQuestion()
{
	m_pPreEdit	= NULL;
	m_pOwnerWnd = NULL;

	m_iFirstNumber = m_iSecondNumber = 0;
}

FishingAutoSellQuestion::~FishingAutoSellQuestion()
{
}

void FishingAutoSellQuestion::SetOwnerWnd( ioWnd *pWnd )
{
	m_pOwnerWnd = pWnd;
}

void FishingAutoSellQuestion::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_QUESTION_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}	

	m_iFirstNumber  = rand() % 89 + 10;
	m_iSecondNumber = rand() % 9 + 1;
}

void FishingAutoSellQuestion::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_QUESTION_EDIT );
	if( pEdit )
		pEdit->KillKeyFocus();
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void FishingAutoSellQuestion::CheckQuestion()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_QUESTION_EDIT );
	if( !pEdit ) return;
	if( !m_pOwnerWnd ) return;

	int iTotalNumber = m_iFirstNumber + m_iSecondNumber;
	int iAnswer = atoi( pEdit->GetText() );

	HideWnd();
	if( iTotalNumber == iAnswer )
	{
		ioFishingMessageBox *pSellWnd = dynamic_cast<ioFishingMessageBox*>(m_pOwnerWnd);
		if( pSellWnd->IsShow() )
			pSellWnd->StartAutoSell();
	}
	else
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );
		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );	
		kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3), m_iFirstNumber, m_iSecondNumber, m_iFirstNumber + m_iSecondNumber );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

		ioFishingMessageBox *pSellWnd = dynamic_cast<ioFishingMessageBox*>(m_pOwnerWnd);
		if( pSellWnd->IsShow() )
			pSellWnd->AutoSellQuestionFailed();
	}
}

void FishingAutoSellQuestion::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			CheckQuestion();
		}
		break;
	case ID_CANCEL:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();			
		}
		break;
	case ID_QUESTION_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				CheckQuestion();
			}						
		}
		break;
	}
}

void FishingAutoSellQuestion::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
	g_FontMgr.PrintText( iXPos + 125, iYPos + 53, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.PrintText( iXPos + 33, iYPos + 76, FONT_SIZE_14, STR(2), m_iFirstNumber, m_iSecondNumber );
}
//////////////////////////////////////////////////////////////////////////
ioFishingMessageBox::ioFishingMessageBox()
{
	m_pPrevWnd = NULL;

	m_pRedBackEffect = NULL;
	m_pBlueBackEffect = NULL;

	int i = 0;
	for(i = 0;i < BACK_ARROW;i++)
		m_pBackArrow[i] = NULL;
	for(i = 0;i < MAX_GRADE;i++)
		m_pGradeIconList[i] = NULL;
	for(i = 0;i < MAX_GRADE_BAR;i++)
		m_pGradeBar[i] = NULL;

	m_pIcon	= NULL;
	m_pPrevIcon = NULL;
	m_pPrevGrade = NULL;
	m_pNextIcon = NULL;
	m_pGrade = NULL;

	m_pIconBack = NULL;
	m_pNoneItemIcon = NULL;

	m_bResult = false;
	m_bFinalResult = false;
	m_bAutoSell = false;
	m_dwAutoSellTime = 0;
	m_bAlarm = false;

	m_iItemType = -1;
	m_iItemIndex = 0;
	m_iResultGrade = -1;
	m_iResultPeso = 0;
	m_iMaxItemCnt = 0;
	m_iCurGrade = -1;
	m_fCurGradeRate = FLOAT1;

	m_dwRollingStartTime = 0;
	m_dwGradeScaleStartTime = 0;
	m_iStartIndex = 0;
	m_iCurPage = 0;
	m_iAutoSellCount = 0;
}

ioFishingMessageBox::~ioFishingMessageBox()
{
	SAFEDELETE( m_pRedBackEffect );
	SAFEDELETE( m_pBlueBackEffect );

	int i = 0;
	for(i = 0;i < BACK_ARROW;i++)
		SAFEDELETE( m_pBackArrow[i] );
	for(i = 0;i < MAX_GRADE;i++)
		SAFEDELETE( m_pGradeIconList[i] );
	for(i = 0;i < MAX_GRADE_BAR;i++)
		SAFEDELETE( m_pGradeBar[i] );

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pNextIcon );
	SAFEDELETE( m_pPrevIcon );

	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNoneItemIcon );

	ClearList();
}

void ioFishingMessageBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "red_back_effect" )
	{
		SAFEDELETE( m_pRedBackEffect );
		m_pRedBackEffect = pImage;
	}
	else if( szType == "blue_back_effect" )
	{
		SAFEDELETE( m_pBlueBackEffect );
		m_pBlueBackEffect = pImage;
	}
	else if( szType == "Arrow1" )
	{
		SAFEDELETE( m_pBackArrow[0] );
		m_pBackArrow[0] = pImage;
	}
	else if( szType == "Arrow2" )
	{
		SAFEDELETE( m_pBackArrow[1] );
		m_pBackArrow[1] = pImage;
	}
	else if( szType == "Arrow3" )
	{
		SAFEDELETE( m_pBackArrow[2] );
		m_pBackArrow[2] = pImage;
	}
	else if( szType == "Arrow4" )
	{
		SAFEDELETE( m_pBackArrow[3] );
		m_pBackArrow[3] = pImage;
	}
	else if( szType == "GradeIcon1" )
	{
		SAFEDELETE( m_pGradeIconList[0] );
		m_pGradeIconList[0] = pImage;
	}
	else if( szType == "GradeIcon2" )
	{
		SAFEDELETE( m_pGradeIconList[1] );
		m_pGradeIconList[1] = pImage;
	}
	else if( szType == "GradeIcon3" )
	{
		SAFEDELETE( m_pGradeIconList[2] );
		m_pGradeIconList[2] = pImage;
	}
	else if( szType == "GradeIcon4" )
	{
		SAFEDELETE( m_pGradeIconList[3] );
		m_pGradeIconList[3] = pImage;
	}
	else if( szType == "GradeIcon5" )
	{
		SAFEDELETE( m_pGradeIconList[4] );
		m_pGradeIconList[4] = pImage;
	}
	else if( szType == "GradeIcon6" )
	{
		SAFEDELETE( m_pGradeIconList[5] );
		m_pGradeIconList[5] = pImage;
	}
	else if( szType == "GradeIcon7" )
	{
		SAFEDELETE( m_pGradeIconList[6] );
		m_pGradeIconList[6] = pImage;
	}
	else if( szType == "GradeIcon8" )
	{
		SAFEDELETE( m_pGradeIconList[7] );
		m_pGradeIconList[7] = pImage;
	}
	else if( szType == "GradeIcon9" )
	{
		SAFEDELETE( m_pGradeIconList[8] );
		m_pGradeIconList[8] = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "NoneItemIcon" )
	{
		SAFEDELETE( m_pNoneItemIcon );
		m_pNoneItemIcon = pImage;
	}
	else if( szType == "GradeBar1" )
	{
		SAFEDELETE( m_pGradeBar[0] );
		m_pGradeBar[0] = pImage;
	}
	else if( szType == "GradeBar2" )
	{
		SAFEDELETE( m_pGradeBar[1] );
		m_pGradeBar[1] = pImage;
	}
	else if( szType == "GradeBar3" )
	{
		SAFEDELETE( m_pGradeBar[2] );
		m_pGradeBar[2] = pImage;
	}
	else if( szType == "GradeBar4" )
	{
		SAFEDELETE( m_pGradeBar[3] );
		m_pGradeBar[3] = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ioFishingMessageBox::ParseExtraInfo( ioXMLElement &xElement )
{
	for(int i=0; i < MAX_GRADE ; i++)
	{
		char szKeyName[MAX_PATH]="";
		ioHashString szTemp;

		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GradeImg%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_vGradeList.push_back( szTemp );
	}

	m_dwRollingTime = xElement.GetIntAttribute_e( "Rolling_Time" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );

	m_fGradeScale = xElement.GetFloatAttribute_e( "Grade_Scale" );
	m_dwGradeScaleTime = xElement.GetIntAttribute_e( "Grade_Scale_Time" );

	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
}

void ioFishingMessageBox::ClearList()
{
	m_bResult = false;
	m_iItemType = -1;
	m_iItemIndex = 0;
	m_iResultGrade = -1;
	m_iResultPeso = 0;

	m_iCurGrade = -1;

	m_dwRollingStartTime = 0;
}

void ioFishingMessageBox::SetPrintMessage( int iStartArray, ioWnd *pPrevWnd )
{
	int iItemType =  g_MyInfo.GetFishingItemType( iStartArray );

	if( iItemType == -1 )
		return;
	
	ClearList();
	m_pPrevWnd = pPrevWnd;
	m_iItemType = iItemType;
	m_bResult = false;
	m_bFinalResult = false;
	m_dwRollingStartTime = 0;
	m_dwGradeScaleStartTime = 0;
	m_iStartIndex = iStartArray;
	// 현재 선택된 아이템 
	ioHashString szIcon = g_FishingMgr.GetFishingItemIconName( m_iItemType );
	if( !szIcon.IsEmpty() )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIcon );
	}

	// 이전에 판매한 아이템
	m_pPrevGrade = NULL;
	SAFEDELETE( m_pPrevIcon );
	if( !m_SellDataList.empty() )
	{
		int iLastArray = m_SellDataList.size() - 1;
		int iPrevItemType = m_SellDataList[iLastArray].m_iItemType;
		ioHashString szPrevIcon = g_FishingMgr.GetFishingItemIconName( iPrevItemType );
		if( !szPrevIcon.IsEmpty() )
		{
			m_pPrevIcon = g_UIImageSetMgr.CreateImageByFullName( szPrevIcon );
		}

		int iGradeIndex= m_SellDataList[iLastArray].m_iGradeIndex - 1;
		if( COMPARE( iGradeIndex, 0, MAX_GRADE ) )
		{
			m_pPrevGrade = m_pGradeIconList[iGradeIndex];
		}
	}

	// 다음 아이템
	SAFEDELETE( m_pNextIcon );
	int iNextItemType = g_MyInfo.GetFishingItemType( iStartArray + 1 );
	if( iNextItemType != -1 )
	{
		ioHashString szNextIcon = g_FishingMgr.GetFishingItemIconName( iNextItemType );
		if( !szNextIcon.IsEmpty() )
		{
			m_pNextIcon = g_UIImageSetMgr.CreateImageByFullName( szNextIcon );
		}
	}

	if( !m_bAutoSell )
	{
		SetWndPos( (Setting::Width()/2) - GetWidth()/2, ((Setting::Height()/2) - GetHeight()/2) - 16 );

		int iXRand = rand() % max( 1, GetXPos() );
		int iYRand = rand() % max( 1, GetYPos() );

		if( (iXRand%2) )
			iXRand = -iXRand;

		if( (iYRand%2) )
			iYRand = -iYRand;

		int iXPos, iYPos;
		iXPos = GetXPos() + iXRand;
		iYPos = GetYPos() + iYRand;

		SetWndPos( iXPos, iYPos );
		ReBatchButton( NORMAL_BTN );
	}
	else
	{
		// 자동 판매는 다음 아이템 표시를 일반과 다르게 해줘야한다.
		if( m_iAutoSellCount <= 1 )
			SAFEDELETE( m_pNextIcon );

		if( m_pPrevWnd )
			m_pPrevWnd->iwm_command( this, IOBN_BTNUP, MAKELONG( IDYES, m_iStartIndex ) );
	}

	if( !IsShow() )
	{
		m_iCurPage = 0;
		ShowWnd();
	}	
	UpdateMaxItemCnt();

	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
	}
}

void ioFishingMessageBox::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( m_bResult && !m_bFinalResult )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwGapTime = dwCurTime - m_dwRollingStartTime;

		if( m_dwRollingTime == 0 ||
			m_dwRollingTotalTime == 0 ||
			dwGapTime > m_dwRollingTotalTime )
		{
			m_bFinalResult = true;
			UpdateMaxItemCnt();
		}

		int iGradeSize = m_vGradeList.size();
		m_iCurGrade = m_iResultGrade-1;

		if( !m_bFinalResult && iGradeSize > 0 )
			m_iCurGrade = (dwGapTime / m_dwRollingTime) % iGradeSize;

		m_pGrade = NULL;
		ioHashString szGrade;
		int iGradeIndex = m_iCurGrade;
		if( COMPARE( iGradeIndex, 0, iGradeSize ) )
		{
			szGrade = m_vGradeList[iGradeIndex];
			m_pGrade = m_pGradeIconList[iGradeIndex];
			m_fCurGradeRate = FLOAT1;
		}

		if( m_bFinalResult )
		{
			m_dwGradeScaleStartTime = FRAMEGETTIME();

			FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
			if( pWnd )
			{
				ioHashString szIcon = g_FishingMgr.GetFishingItemIconName( m_iItemType );
				pWnd->UpdateItemData( FishingInventoryWnd::SELL_TYPE, szIcon, szGrade );

				g_QuestMgr.QuestCompleteTerm( QC_FISHING_SELL_PESO, m_iResultPeso );
			}

			g_SoundMgr.StopSound( m_szRollingSound, 0 );

			ReBatchButton(RESULT_BTN);
		}
	}
	else if( m_bFinalResult )
	{
		float fScale = FLOAT1;
		DWORD dwGapTime = FRAMEGETTIME() - m_dwGradeScaleStartTime;
		if( dwGapTime < m_dwGradeScaleTime )
		{
			float fScaleRate = (float)dwGapTime / (float)m_dwGradeScaleTime;
			fScale = m_fGradeScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
		}
		m_fCurGradeRate = fScale;

		if( m_bAutoSell )
		{
			if( m_dwAutoSellTime > 0 )
			{
				if( FRAMEGETTIME() - m_dwAutoSellTime <= 3000 )
				{
					ioWnd *pAutoSellStop = FindChildWnd( ID_AUTO_SELL_STOP );
					if( pAutoSellStop )
					{
						int iSec = ( 3000 - (FRAMEGETTIME() - m_dwAutoSellTime) ) / 1000;
						char szTitle[MAX_PATH] = "";
						SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iSec + 1 );
						pAutoSellStop->SetTitleText( szTitle );
					}
				}
				else
				{
					m_dwAutoSellTime = 0;
					SetPrintMessage( m_iStartIndex, m_pPrevWnd );
					ioWnd *pAutoSellStop = FindChildWnd( ID_AUTO_SELL_STOP );
					if( pAutoSellStop )
					{
						pAutoSellStop->SetTitleText( STR(2) );
					}
				}
			}
		}
	}
}

void ioFishingMessageBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 170,
		ICON_Y		= 98,

		DESC_TXT_X	= 170,
		DESC_TXT_Y	= 144,

		DESC_X		= 170,
		DESC_Y		= 198,
	};

	// Back
	if( m_bFinalResult && m_pBlueBackEffect )
		m_pBlueBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	else if( !m_bFinalResult && m_pRedBackEffect )
		m_pRedBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	// Icon
	if( m_pIcon )
	{
		m_pIcon->SetScale( FLOAT1 );
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Grade
	if( m_bFinalResult && m_pGrade )
	{
		m_pGrade->SetScale( m_fCurGradeRate );
		m_pGrade->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Arrow
	for(int i = 0;i < BACK_ARROW;i++)
	{
		if( m_pBackArrow[i] )
			m_pBackArrow[i]->Render( iXPos, iYPos );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	// GradeDesc
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	if( !m_bResult && !m_bFinalResult )
	{
		g_FontMgr.PrintText( iXPos + DESC_TXT_X, iYPos + DESC_TXT_Y, FONT_SIZE_17, "?????" );
	}
	else if( !m_bFinalResult )
	{
		int iGradeDesc = m_iCurGrade + 1;
		ioHashString szDesc = g_FishingMgr.GetFishingItemGradeDesc( iGradeDesc );
		g_FontMgr.PrintText( iXPos + DESC_TXT_X, iYPos + DESC_TXT_Y, FONT_SIZE_17, szDesc.c_str() );
	}
	else
	{
		ioHashString szDesc = g_FishingMgr.GetFishingItemGradeDesc( m_iResultGrade );
		g_FontMgr.PrintText( iXPos + DESC_TXT_X, iYPos + DESC_TXT_Y, FONT_SIZE_17, szDesc.c_str() );
	}

	// 아이템 이름
	ioHashString szName = g_FishingMgr.GetFishingItemName( m_iItemType );
	g_FontMgr.PrintText( iXPos + DESC_TXT_X, iYPos + DESC_TXT_Y + 22, FONT_SIZE_17, szName.c_str() );

	// 아이템 설명
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	ioHashString szDesc = g_FishingMgr.GetFishingItemDesc( m_iItemType );
	g_FontMgr.PrintText( iXPos + DESC_X, iYPos + DESC_Y, FONT_SIZE_13, szDesc.c_str() );

	// 행동 지침 -> 진행중 -> 결과 
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	if( !m_bResult && !m_bFinalResult )
	{
		g_FontMgr.PrintText( iXPos + DESC_X, iYPos + DESC_Y + 20, FONT_SIZE_13, STR(1) );
	}
	else if( !m_bFinalResult )
	{
		g_FontMgr.PrintText( iXPos + DESC_X, iYPos + DESC_Y + 20, FONT_SIZE_13, STR(2) );
	}
	else
	{
		char szConvertNum[MAX_PATH] = "";
		Help::ConvertNumToStrComma( (int)m_iResultPeso, szConvertNum, sizeof( szConvertNum ) );
		ioHashString szGradeDesc = g_FishingMgr.GetFishingItemGradeName( m_iResultGrade );
		g_FontMgr.PrintText( iXPos + DESC_X, iYPos + DESC_Y + 20, FONT_SIZE_13, STR(3), szGradeDesc.c_str(), szConvertNum );
	}

	OnRenderPrevSellItem( iXPos + 71, iYPos + 73 );
	OnRenderNextSellItem( iXPos + 269, iYPos + 73 );
	OnRenderSellItemList( iXPos + 17, iYPos + 264 );
}

void ioFishingMessageBox::OnRenderPrevSellItem( int iXPos, int iYPos )
{
	if( m_SellDataList.empty() || !m_pPrevIcon )
	{
		if( m_pNoneItemIcon )
		{
			m_pNoneItemIcon->SetScale( 0.60f );
			m_pNoneItemIcon->Render( iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			m_pNoneItemIcon->Render( iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
	else
	{
		if( m_pPrevIcon )
		{
			if( m_pIconBack )
			{
				m_pIconBack->SetScale( 0.60f );
				m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}
			m_pPrevIcon->SetScale( 0.60f );
			m_pPrevIcon->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( m_pPrevGrade )
			{
				m_pPrevGrade->SetScale( 0.60f );
				m_pPrevGrade->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}			
		}
	}
}

void ioFishingMessageBox::OnRenderNextSellItem( int iXPos, int iYPos )
{
	if( m_pNextIcon )
	{
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.60f );
			m_pIconBack->Render( iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		m_pNextIcon->SetScale( 0.60f );
		m_pNextIcon->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if( m_pNoneItemIcon )
	{
		m_pNoneItemIcon->SetScale( 0.60f );
		m_pNoneItemIcon->Render( iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pNoneItemIcon->Render( iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
}

void ioFishingMessageBox::OnRenderSellItemList( int iXPos, int iYPos )
{
	int iMaxCount = m_iCurPage + 5;
	int iMaxSellList = m_SellDataList.size();

	// 결과중인 아이템은 표시하지 않음
	if( m_bResult && !m_bFinalResult )
		iMaxSellList -= 1;

	for(int iStartArray = m_iCurPage;iStartArray < iMaxCount;iStartArray++)
	{
		if( COMPARE( iStartArray, 0, iMaxSellList ) )
		{
			SellDataList &rkData = m_SellDataList[iStartArray];
			
			char szGrade[MAX_PATH] = "";
			int iBarArray = 0;
			switch( rkData.m_iGradeIndex )
			{
			case 1:        // SSS
				iBarArray = 0;
				sprintf( szGrade, "SSS" );
				break;
			case 2:        // SS
				iBarArray = 0;
				sprintf_e( szGrade, "SS" );
				break;
			case 3:        // S
				iBarArray = 0;
				sprintf_e( szGrade, "S" );
				break;
			case 4:        // A
				iBarArray = 1;
				sprintf_e( szGrade, "A" );
				break;
			case 5:        // B
				iBarArray = 2;
				sprintf_e( szGrade, "B" );
				break;
			case 6:        // C
				iBarArray = 3;
				sprintf_e( szGrade, "C" );
				break;
			case 7:        // D
				iBarArray = 3;
				sprintf_e( szGrade, "D" );
				break;
			case 8:        // E
				iBarArray = 3;
				sprintf_e( szGrade, "E" );
				break;
			case 9:        // F
				iBarArray = 3;
				sprintf_e( szGrade, "F" );
				break;
			}

			// 등급
			if( COMPARE( iBarArray, 0, MAX_GRADE_BAR ) )
			{
				if( m_pGradeBar[iBarArray] )
					m_pGradeBar[iBarArray]->Render( iXPos, iYPos );
			}
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iXPos + 19, iYPos, FONT_SIZE_12, szGrade );

			// 아이템 이름
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			ioHashString szName = g_FishingMgr.GetFishingItemName( rkData.m_iItemType );
			g_FontMgr.PrintText( iXPos + 43, iYPos, FONT_SIZE_12, szName.c_str() );

			// 아이템 페소
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( rkData.m_iSellPeso, szConvertNum, sizeof( szConvertNum ) );
			g_FontMgr.PrintText( iXPos + 280, iYPos, FONT_SIZE_13, STR(1), szConvertNum );
		}
		iYPos += 19;
	}

	int i = 0;
	int iTotalSellPeso = 0;
	for(i = 0;i < iMaxSellList;i++)
	{
		if( COMPARE( i, 0, iMaxSellList ) )
		{
			SellDataList &rkData = m_SellDataList[i];
			iTotalSellPeso += rkData.m_iSellPeso;
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iTotalSellPeso, szConvertNum, sizeof( szConvertNum ) );
	g_FontMgr.PrintText( iXPos + 153, iYPos + 2, FONT_SIZE_13, STR(2), szConvertNum, iMaxSellList, iMaxSellList + m_iMaxItemCnt );
}

void ioFishingMessageBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_AUTO_SELL:
		if( cmd == IOBN_BTNUP )
		{
			FishingAutoSellQuestion *pSellQuestion = dynamic_cast<FishingAutoSellQuestion*>(g_GUIMgr.FindWnd( FISHING_AUTO_SELL_QUESTION ));
			if( pSellQuestion )
			{
				pSellQuestion->SetOwnerWnd( this );
				pSellQuestion->ShowWnd();
			}
		}
		break;
	case ID_AUTO_SELL_STOP:
		if( cmd == IOBN_BTNUP )
		{
			m_bAutoSell = false;
			ReBatchButton( RESULT_BTN );
		}
		break;
	case ID_SELL:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pPrevWnd )
				m_pPrevWnd->iwm_command( this, IOBN_BTNUP, MAKELONG( IDYES, m_iStartIndex ) );
		}
		break;
	case ID_NEXT_SELL:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pPrevWnd )
				m_pPrevWnd->iwm_command( this, IOBN_BTNUP, MAKELONG( IDRETRY, m_iStartIndex ) );
		}
		break;	
	case ID_CANCEL:
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			if( m_pPrevWnd )
				m_pPrevWnd->iwm_command( this, IOBN_BTNUP, MAKELONG( IDNO, 0 ) );
		}
		break;
	}
}

void ioFishingMessageBox::iwm_hide()
{
	if( m_pPrevWnd && !m_pPrevWnd->IsShow() )
		m_pPrevWnd->ShowWnd();
	ClearList();

	m_bAutoSell = false;
	m_SellDataList.clear();

	g_QuestMgr.ProcessCompleteReserve();
}

void ioFishingMessageBox::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( pScroll->HasWndStyle( IWS_INACTIVE ) ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPage - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPage + 1 );
	}
}

void ioFishingMessageBox::iwm_vscroll(DWORD dwID, int curPos )
{
	m_iCurPage = curPos;
}

void ioFishingMessageBox::ReBatchButton( int iType )
{
	switch( iType )
	{
	case NORMAL_BTN:
		ShowChildWnd( ID_AUTO_SELL );
		ShowChildWnd( ID_SELL );
		ShowChildWnd( ID_CANCEL );
		
		ShowChildWnd( ID_EXIT );

		SetChildActive( ID_AUTO_SELL );
		SetChildActive( ID_SELL );
		SetChildActive( ID_CANCEL );
		SetChildActive( ID_EXIT );

		HideChildWnd( ID_CLOSE );
		HideChildWnd( ID_NEXT_SELL );
		HideChildWnd( ID_AUTO_SELL_STOP );
		break;
	case ROLLING_BTN:
		{
			if( !m_bAutoSell )
			{
				ShowChildWnd( ID_AUTO_SELL );
				ShowChildWnd( ID_SELL );
				ShowChildWnd( ID_CANCEL );
			}

			ShowChildWnd( ID_EXIT );

			SetChildInActive( ID_AUTO_SELL );
			SetChildInActive( ID_SELL );
			SetChildInActive( ID_CANCEL );
			SetChildInActive( ID_EXIT );
			SetChildInActive( ID_AUTO_SELL_STOP );

			HideChildWnd( ID_CLOSE );
			HideChildWnd( ID_NEXT_SELL );
		}
		break;
	case RESULT_BTN:
		{
			SetChildActive( ID_AUTO_SELL );
			SetChildActive( ID_SELL );
			SetChildActive( ID_CANCEL );
			SetChildActive( ID_EXIT );
			SetChildActive( ID_AUTO_SELL_STOP );

			HideChildWnd( ID_AUTO_SELL_STOP );
			HideChildWnd( ID_AUTO_SELL );
			HideChildWnd( ID_SELL );
			HideChildWnd( ID_CANCEL );

			ShowChildWnd( ID_EXIT );

			if( m_bAutoSell && m_iAutoSellCount > 0 )
			{	
				ShowChildWnd( ID_AUTO_SELL_STOP );
				m_dwAutoSellTime = FRAMEGETTIME();
			}
			else
			{
				m_bAutoSell = false;
				m_dwAutoSellTime = 0;
				ShowChildWnd( ID_CLOSE );
				ShowChildWnd( ID_NEXT_SELL );
			}
		}
		break;
	}
}

void ioFishingMessageBox::OnResultInfo( int iItemType, int iItemIndex, int iGrade, bool bAlarm, __int64 iPeso )
{
	m_pGrade = NULL;
	m_bResult = true;
	m_bFinalResult = false;

	m_bAlarm = bAlarm;

	m_iResultGrade = iGrade;
	m_iResultPeso  = iPeso;
	m_iItemIndex   = iItemIndex;
	m_dwRollingStartTime = FRAMEGETTIME();
	m_dwGradeScaleStartTime = 0;

	ReBatchButton( ROLLING_BTN );
	if( !m_szRollingSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	}
	m_iAutoSellCount--;
	SellDataList kSellData;
	kSellData.m_iItemType   = iItemType;
	kSellData.m_iGradeIndex = iGrade;
	kSellData.m_iSellPeso   = iPeso;
	m_SellDataList.push_back( kSellData );

	g_QuestMgr.QuestCompleteTermReserve( QC_FISHING_SELL_PESO, m_iResultPeso );
}

void ioFishingMessageBox::UpdateMaxItemCnt()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{		
		pScroll->SetScrollRange( 0, m_SellDataList.size() );
		pScroll->SetScrollPos( pScroll->GetMaxPos() );
	}
	m_iMaxItemCnt = g_MyInfo.GetCurFishingItemCnt();
}

void ioFishingMessageBox::StartAutoSell()
{
	m_iAutoSellCount = (g_MyInfo.GetCurFishingItemCnt() - m_iStartIndex);
	m_bAutoSell = true;
	if( m_pPrevWnd )
		m_pPrevWnd->iwm_command( this, IOBN_BTNUP, MAKELONG( IDYES, m_iStartIndex ) );
}

void ioFishingMessageBox::StopAutoSell()
{
	m_bAutoSell = false;
}

void ioFishingMessageBox::AutoSellQuestionFailed()
{
	HideWnd();
	if( m_pPrevWnd )
		m_pPrevWnd->iwm_command( this, IOBN_BTNUP, MAKELONG( IDCANCEL, 0 ) );
}
//////////////////////////////////////////////////////////////////////////
ioFishingItemMessageBox::ioFishingItemMessageBox()
{
	m_pPrevWnd = NULL;
	m_pLine    = NULL;
}

ioFishingItemMessageBox::~ioFishingItemMessageBox()
{
	ClearList();

	SAFEDELETE( m_pLine );
}

void ioFishingItemMessageBox::ClearList()
{
	for( int i=0; i<2; i++ )
	{
		m_Printer[i].ClearList();
	}
}

void ioFishingItemMessageBox::SetPrintMessage( ioComplexStringPrinter *MsgPrinter, ioWnd *pPrevWnd )
{
	m_pPrevWnd = pPrevWnd;

	ClearList();
	
	int i = 0;
	for(i = 0;i < 2; i++)
	{
		if( MsgPrinter[i].IsEmpty() )
			break;

		MsgPrinter[i].CopyFullText( m_Printer[i] );
	}	

	int iMaxWidth  = 178;
	int iMaxHeight = 0;
	int iMaxLine   = 0;
	for(i = 0;i < 2;i++)
	{
		if( m_Printer[i].IsEmpty() )
			break;

		int iCurLineWidth = m_Printer[i].GetFullWidth();
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
		iMaxLine++;
	}
	iMaxWidth += 44;
	iMaxHeight = 94 + ( 20 * iMaxLine );

	
	SetSize( iMaxWidth, iMaxHeight );
	SetWndPos( (Setting::Width() - iMaxWidth ) / 2, ( (Setting::Height() - iMaxHeight ) / 2 ) - 16 );

	if( m_pLine )
	{
		m_pLine->SetSize( iMaxWidth-2, m_pLine->GetHeight() );
	}

	ReBatchButton( 0 );

	if( !IsShow() )
		ShowWnd();
}

void ioFishingItemMessageBox::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + GetWidth() / 2;
	int iYPos = GetDerivedPosY() + 45;

	for( int i=0 ; i<2 ; i++ )
	{
		if( m_Printer[i].IsEmpty() )
			break;

		m_Printer[i].PrintFullText( iXPos, iYPos + 20 * i, TAT_CENTER );
	}

	if( m_pLine )
	{
		m_pLine->Render( GetDerivedPosX(), GetDerivedPosY(), UI_RENDER_NORMAL );
	}
}

void ioFishingItemMessageBox::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_YES:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->ShowEtcItemTab();
			}
		}
		break;
	case ID_NO:
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void ioFishingItemMessageBox::iwm_hide()
{
	if( m_pPrevWnd && !m_pPrevWnd->IsShow() )
		m_pPrevWnd->ShowWnd();
}

bool ioFishingItemMessageBox::iwm_esc()
{
	ioMessageBoxBtn *pNoBtn = (ioMessageBoxBtn*)FindChildWnd( ID_NO );
	if( !pNoBtn ) return false;
	if( !pNoBtn->IsShow() ) return false;

	iwm_command( pNoBtn, IOBN_BTNUP, 0 );
	return true;
}

bool ioFishingItemMessageBox::iwm_spacebar()
{
	ioMessageBoxBtn *pYesBtn = (ioMessageBoxBtn*)FindChildWnd( ID_YES );
	if( !pYesBtn ) return false;
	if( !pYesBtn->IsShow() ) return false;

	iwm_command( pYesBtn, IOBN_BTNUP, 0 );
	return true;
}

void ioFishingItemMessageBox::ReBatchButton( int iType )
{
	ioMessageBoxBtn *pYesBtn = (ioMessageBoxBtn*)FindChildWnd( ID_YES );
	ioMessageBoxBtn *pNoBtn  = (ioMessageBoxBtn*)FindChildWnd( ID_NO );
	ioWnd *pXBtn = FindChildWnd( ID_X );
	if( pYesBtn && pNoBtn && pXBtn )
	{
		pYesBtn->SetSize( ( GetWidth()/2 - 10 ), 27 );
		pYesBtn->SetWndPos( 9, GetHeight() - 36 );
		pNoBtn->SetSize( ( GetWidth()/2 - 10 ), 27 );
		pNoBtn->SetWndPos( GetWidth() - pYesBtn->GetWidth() - 9, GetHeight() - 36 );
		pXBtn->SetWndPos( GetWidth() - 31, 6 );
	}

	ShowChildWnd( ID_YES );
	ShowChildWnd( ID_NO );
	ShowChildWnd( ID_X );
}

void ioFishingItemMessageBox::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlackLine" )
	{
		m_pLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}
//////////////////////////////////////////////////////////////////////////
ioFishingLevelUpWnd::ioFishingLevelUpWnd()
{
	m_pPrevWnd = NULL;

	m_pIconBack	= NULL;
	m_pLevelUpIcon = NULL;
	m_pLevelUpAddSlotIcon = NULL;

	m_bAddSlot = false;
	m_iCurLevel = 0;
}

ioFishingLevelUpWnd::~ioFishingLevelUpWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pLevelUpIcon );
	SAFEDELETE( m_pLevelUpAddSlotIcon );
}

void ioFishingLevelUpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "LevelUpIcon" )
	{
		SAFEDELETE( m_pLevelUpIcon );
		m_pLevelUpIcon = pImage;
	}
	else if( szType == "LevelUpAddSlotIcon" )
	{
		SAFEDELETE( m_pLevelUpAddSlotIcon );
		m_pLevelUpAddSlotIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioFishingLevelUpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szUpSound = xElement.GetStringAttribute_e( "Level_Up_Sound" );
}

void ioFishingLevelUpWnd::SetPrintMessage( int iCurLevel, bool bAddSlot, ioWnd *pPrevWnd )
{
	m_bAddSlot  = bAddSlot;
	m_iCurLevel = iCurLevel;

	m_pPrevWnd = pPrevWnd;

	if( !m_szUpSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_szUpSound );
	}

	if( !IsShow() )
		ShowWnd();
}

void ioFishingLevelUpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ioFishingLevelUpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		m_pIconBack->Render( iXPos + 125, iYPos + 98, UI_RENDER_MULTIPLY );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(1) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(2), m_iCurLevel );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(3) );
	kPrinter.PrintFullText( iXPos + 125, iYPos + 146, TAT_CENTER );
    
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_bAddSlot )
	{
		if( m_pLevelUpAddSlotIcon )
			m_pLevelUpAddSlotIcon->Render( iXPos + 125, iYPos + 98 );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 169, FONT_SIZE_13, STR(4) );
	}
	else
	{
		if( m_pLevelUpIcon )
			m_pLevelUpIcon->Render( iXPos + 125, iYPos + 98 );
		g_FontMgr.PrintText( iXPos + 125, iYPos + 169, FONT_SIZE_13, STR(5) );
	}
}

void ioFishingLevelUpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void ioFishingLevelUpWnd::iwm_show()
{
	ShowChildWnd( ID_ALPHA_WND );
}

void ioFishingLevelUpWnd::iwm_hide()
{
	HideChildWnd( ID_ALPHA_WND );

	if( m_pPrevWnd && !m_pPrevWnd->IsShow() )
		m_pPrevWnd->ShowWnd();

	if( !m_szUpSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szUpSound, 0 );
	}
}
//////////////////////////////////////////////////////////////////////////
FishingBuyWnd::FishingBuyWnd()
{
	m_pIcon = NULL;
}

FishingBuyWnd::~FishingBuyWnd()
{
	SAFEDELETE( m_pIcon );
}

bool FishingBuyWnd::SetInfo( const ioHashString &rsIcon, const ioHashString &rsTitle, const ioHashStringVec &rvDescVec )
{
	if( rsIcon.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s icon is empty", __FUNCTION__ );
		return false;
	}

	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( rsIcon );

	m_vDescVec.clear();
	m_vDescVec = rvDescVec;
	SetTitleText( rsTitle.c_str() );

	return true;
}

void FishingBuyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{
			// 상점 특별아이템 열기
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_FISHING_ROD, 0 );
				HideWnd();
			}
		}
		break;		
	}
}

void FishingBuyWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X_OFFSET = 151,
		ICON_Y_OFFSET = 98,

		DESC_X_OFFSET = 151,
		DESC_Y_OFFSET = 144,
		DESC_Y_GAP    = 20,
	};

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iSize = m_vDescVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( i == 0 )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		ioHashString &rsDesc = m_vDescVec[i];
		g_FontMgr.PrintText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET + (DESC_Y_GAP*i), FONT_SIZE_13, rsDesc.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
FishingDropZoneInduceWnd::FishingDropZoneInduceWnd()
{
	m_AlarmType = AT_NOT_FISHING_ZONE;

	for( int i = 0; i < AT_MAX_ALARM_TYPE; i++ )
		m_dwAlarmPopUpEndDate[ i ] = 0;

	m_bCheckPopup = false;

	m_pNotFishingAlarmType		= NULL;
	m_pNewFishingAlarmType		= NULL;
}

FishingDropZoneInduceWnd::~FishingDropZoneInduceWnd()
{
	SAFEDELETE( m_pNotFishingAlarmType );
	SAFEDELETE( m_pNewFishingAlarmType );
}

void FishingDropZoneInduceWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "fishing_alarm_01" )
	{
		SAFEDELETE( m_pNotFishingAlarmType );
		m_pNotFishingAlarmType = pImage;
	}
	else if( szType == "fishing_alarm_02" )
	{
		SAFEDELETE( m_pNewFishingAlarmType );
		m_pNewFishingAlarmType = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void FishingDropZoneInduceWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioHashString sSmallTabName;
	int          iType = 0;
	for (int i = 0; i <= AT_NEW_FISHING_CONTENTS_ALARM; i++)
	{
		char szName[MAX_PATH]="";
		wsprintf_e( szName, "alarm_popup_%d_end_date", i + 1 );
		m_dwAlarmPopUpEndDate[ i ] = (DWORD)xElement.GetIntAttribute( szName );
	}
}

void FishingDropZoneInduceWnd::iwm_show()
{
	ioWnd::iwm_show();
}

void FishingDropZoneInduceWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_NORMAL_CLOSE:
	case ID_WIDE_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			if( m_AlarmType == AT_NEW_FISHING_CONTENTS_ALARM )
				g_GUIMgr.EtcValueSaveInt( "FishingDropZoneInduceWnd", "once_popup", true );
		}
		break;
	case ID_GO_TO_SHOP:
		{
			if( cmd == IOBN_BTNUP )
			{
				NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
				if( pShopWnd )
				{
					pShopWnd->ShowWnd(); 

					pShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, 5000314, 0 );

					HideWnd();
				}
			}
		}
		break;
	}
}

void FishingDropZoneInduceWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_AlarmType == AT_NOT_FISHING_ZONE )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 178, FONT_SIZE_11, STR(1) );
																 
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );				 
		g_FontMgr.PrintText( iXPos + 16, iYPos + 198, FONT_SIZE_11, STR(2) );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 218, FONT_SIZE_11, STR(3) );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 238, FONT_SIZE_11, STR(4) );

		if( m_pNotFishingAlarmType )
		{
			m_pNotFishingAlarmType->SetScale( FLOAT1 );
			m_pNotFishingAlarmType->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	else if ( m_AlarmType == AT_NO_FISHING_PLACE_TO_INSTALL )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 178, FONT_SIZE_13, STR(5) );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 198, FONT_SIZE_13, STR(6) );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 218, FONT_SIZE_13, STR(7) );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 238, FONT_SIZE_13, STR(8) );

		if( m_pNotFishingAlarmType )
		{
			m_pNotFishingAlarmType->SetScale( FLOAT1 );
			m_pNotFishingAlarmType->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	else if ( m_AlarmType == AT_NEW_FISHING_CONTENTS_ALARM )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 178, FONT_SIZE_13, STR(9) );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 198, FONT_SIZE_11, STR(10) );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 218, FONT_SIZE_11, STR(11) );
		g_FontMgr.PrintText( iXPos + 16, iYPos + 238, FONT_SIZE_11, STR(12) );

		if( m_pNewFishingAlarmType )
		{
			m_pNewFishingAlarmType->SetScale( FLOAT1 );
			m_pNewFishingAlarmType->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

bool FishingDropZoneInduceWnd::SetAlarmType( AlarmType eAlarmType )
{
	m_AlarmType = eAlarmType;

	if( m_AlarmType == AT_NEW_FISHING_CONTENTS_ALARM )
	{
		m_bCheckPopup = (bool)g_GUIMgr.EtcValueLoadInt( "FishingDropZoneInduceWnd", "once_popup" );
		if( m_bCheckPopup )
			return false;
	}

	if( m_dwAlarmPopUpEndDate[ m_AlarmType ] != 0 && !IsShow() )
	{
		CTime kCurrTime = DateHelp::GetCurrTime();
		DWORD dwCurtime = DateHelp::ConvertDwDate( kCurrTime.GetYear(), kCurrTime.GetMonth(), kCurrTime.GetDay(), kCurrTime.GetHour(), kCurrTime.GetMinute() );

		m_dwAlarmPopUpEndDate[ m_AlarmType ] -= 2000000000;
		m_dwAlarmPopUpEndDate[ m_AlarmType ] *= 100;

		if( dwCurtime >= m_dwAlarmPopUpEndDate[ m_AlarmType ] )
		{
			return false;
		}
	}
	else if( m_dwAlarmPopUpEndDate[ m_AlarmType ] != 0 && IsShow() )
		return false;

	ioButton* pNormalCloseBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_NORMAL_CLOSE ) );
	ioButton* pWideCloseBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_WIDE_CLOSE ) );

	if( m_AlarmType == AT_NOT_FISHING_ZONE )
	{
		SetTitleText( STR(1) );
		
		if( pNormalCloseBtn )
			pNormalCloseBtn->ShowWnd();
		if( pWideCloseBtn )
			pWideCloseBtn->HideWnd();
	}
	else if( m_AlarmType == AT_NO_FISHING_PLACE_TO_INSTALL )
	{
		SetTitleText( STR(2) );

		if( pNormalCloseBtn )
			pNormalCloseBtn->ShowWnd();
		if( pWideCloseBtn )
			pWideCloseBtn->HideWnd();
	}
	else if( m_AlarmType == AT_NEW_FISHING_CONTENTS_ALARM )
	{
		SetTitleText( STR(3) );

		if( pNormalCloseBtn )
			pNormalCloseBtn->HideWnd();
		if( pWideCloseBtn )
			pWideCloseBtn->ShowWnd();
	}

	return true;
}