#include "StdAfx.h"

#include "MissionWnd.h"

#include "../ioMissionManager.h"
#include "../ioExtendSoundManager.h"
#include "../ioUserSpiritManager.h"
//////////////////////////////////////////////////////////////////////////
MissionQuestMainWnd::MissionQuestMainWnd(void)
{
	m_dwTabID = ID_MISSION_BTN;
}

MissionQuestMainWnd::~MissionQuestMainWnd(void)
{
}

void MissionQuestMainWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/questlistwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/mainmissionwnd.xml", this );
}

void MissionQuestMainWnd::iwm_show()
{
	ChangeRadioWnd( ID_MISSION_BTN );
	ChangeTitle( ID_MISSION_BTN );
}

void MissionQuestMainWnd::iwm_hide()
{
	HideChildWnd( ID_MISSIONLIST_WND );
	HideChildWnd( ID_QUESTLIST_WND );
}

void MissionQuestMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_MISSION_BTN:
	case ID_QUEST_BTN:
		if( cmd == IOBN_BTNDOWN )
		{			
			ChangeRadioWnd( dwID );
		}
		break;
	}
}

void MissionQuestMainWnd::ChangeTitle( DWORD dwID )
{
	char szTitle[MAX_PATH] = "";
	if ( dwID == ID_MISSION_BTN )
		SetTitleText( "미션" );
	else if ( dwID == ID_QUEST_BTN )
		SetTitleText( "퀘스트" );
}

void MissionQuestMainWnd::ChangeRadioWnd( DWORD dwID )
{
	m_dwTabID = dwID;
	CheckRadioButton( ID_MISSION_BTN, ID_QUEST_BTN, dwID );

	HideChildWnd( ID_MISSIONLIST_WND );
	HideChildWnd( ID_QUESTLIST_WND );

	switch( m_dwTabID )
	{
	case ID_MISSION_BTN:
		ShowChildWnd( ID_MISSIONLIST_WND );
		ChangeTitle( ID_MISSION_BTN );
		break;
	case ID_QUEST_BTN:
		ShowChildWnd( ID_QUESTLIST_WND );
		ChangeTitle( ID_QUEST_BTN );
		break;
	}
	UpdateRadioBtn();
}

void MissionQuestMainWnd::UpdateRadioBtn()
{
	MissionRadioBtn *MissionBtn = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_MISSION_BTN ) );
	if( MissionBtn )
	{
		bool bStartSparkle = false;
		//현재 탭이 퀘스트일 때
		if ( m_dwTabID != ID_MISSION_BTN )
		{
			for ( int i=MDT_DAILY ; i<MDT_MAX ; ++i )
			{
				//보상 받기가 가능한 미션이 있을 경우
				if ( g_MissionManager.GetAttinCountToType( i ) > 0 )
				{
					bStartSparkle = true;
					break;
				}
			}
		}
		//보상 받기가 가능한 미션이 있고
		if ( bStartSparkle )
		{
			//현재 이펙트 상태가 아니라면
			if ( !MissionBtn->IsSparkling() )
				MissionBtn->SetStartSparkle();	//이펙트 스타트
		}
		//보상 받기가 가능한 미션이 없거나 현재 탭이 미션일 경우
		else
		{
			//이펙트 상태이면 End 시켜준다.
			if ( MissionBtn->IsSparkling() )
				MissionBtn->SetEndSparkle();	//이펙트 종료
		}
	}	
}

void MissionQuestMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	if ( g_MissionManager.IsChangeMission() )
		UpdateRadioBtn();
}
//////////////////////////////////////////////////////////////////////////////////
MainMissionWnd::MainMissionWnd() :
m_dwTabID( 0 ),
m_iMissionDateType( 0 ),
m_iCurPage( 0 ),
m_iMaxPage( 0 )
{
}

MainMissionWnd::~MainMissionWnd()
{
}

void MainMissionWnd::iwm_show()
{
	ChangeRadioBtn( ID_DAILY_MISSION_BTN );
}

void MainMissionWnd::iwm_hide()
{
	m_iCurPage = m_iMaxPage = m_iMissionDateType = m_dwTabID = 0;
}

void MainMissionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_MISSION_INFO_BTN1:
	case ID_MISSION_INFO_BTN2:
	case ID_MISSION_INFO_BTN3:
	case ID_MISSION_INFO_BTN4:
	case ID_MISSION_INFO_BTN5:
	case ID_MISSION_INFO_BTN6:
		{
			if( cmd == IOWN_OVERED )
			{
				MissionInfoBtn *pInfo = dynamic_cast<MissionInfoBtn*>( FindChildWnd( dwID ) );
				if ( pInfo )
				{
					if ( pInfo->GetMissionCode() != 0 )
					{
						MissionToolTip *pTooltip = dynamic_cast<MissionToolTip*>( FindChildWnd( ID_MISSION_TOOLTIP ) );
						if( pTooltip )
						{
							if( pTooltip->SetInfo( g_MissionManager.GetMissionToCode( pInfo->GetMissionCode() ) , pInfo->GetPosX(), pInfo->GetPosY() ) )
								ShowChildWnd( ID_MISSION_TOOLTIP );
						}
					}
				}
			}
			else if( cmd == IOWN_LEAVED )
				HideChildWnd( ID_MISSION_TOOLTIP );
		}
		break;
	case ID_DAILY_MISSION_BTN:
	case ID_WEEKLY_MISSION_BTN:
	case ID_MONTHLY_MISSION_BTN:
		if( cmd == IOBN_BTNDOWN )
		{			
			ChangeRadioBtn( dwID );
		}
		break;
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_iCurPage > 0 )
			{
				m_iCurPage--;
				UpdateMissionInfo();
			}
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_iCurPage < m_iMaxPage - 1 )
			{
				m_iCurPage++;
				UpdateMissionInfo();
			}
		}
		break;
	}
}

void MainMissionWnd::ChangeRadioBtn( DWORD dwID )
{
	if( m_dwTabID == dwID )
		return;

	m_dwTabID = dwID;
	CheckRadioButton( ID_DAILY_MISSION_BTN, ID_MONTHLY_MISSION_BTN, dwID );
	if ( m_dwTabID == ID_DAILY_MISSION_BTN )
		m_iMissionDateType = MDT_DAILY;
	else if ( m_dwTabID == ID_WEEKLY_MISSION_BTN )
		m_iMissionDateType = MDT_WEEKLY;
	else if ( m_dwTabID == ID_MONTHLY_MISSION_BTN )
		m_iMissionDateType = MDT_MONTHLY;

	m_iCurPage = m_iMaxPage = 0;
	InitMissionInfoBtn();
	UpdateMissionInfo();
	UpdateRadioBtn();
}

void MainMissionWnd::UpdateMissionInfo()
{
	int iMissionCnt = g_MissionManager.GetMissionCountToType( m_iMissionDateType );
	m_iMaxPage =  iMissionCnt/ MAX_MISSION_CNT;
	if ( iMissionCnt % MAX_MISSION_CNT > 0 )
		m_iMaxPage++;


	if ( !COMPARE( m_iCurPage, 0, m_iMaxPage ) )
	{
		m_iCurPage = max( 0, m_iCurPage );
		m_iCurPage = min( m_iCurPage, max( 0, (m_iMaxPage - 1) ) );
	}


	int iStartArray =  m_iCurPage * MAX_MISSION_CNT;
	for(int i = 0;i < MAX_MISSION_CNT ;i++,iStartArray++)
	{
		MissionInfoBtn *pMissionInfomBtn = dynamic_cast<MissionInfoBtn*>( FindChildWnd( ID_MISSION_INFO_BTN1 + i ) );
		if( pMissionInfomBtn )
			pMissionInfomBtn->SetMission( g_MissionManager.GetMissionToArray( m_iMissionDateType, iStartArray ) );
	}

	m_PagePrinter.ClearList();

	m_PagePrinter.SetTextStyle( TS_NORMAL );
	m_PagePrinter.SetBkColor( 0, 0, 0 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PagePrinter.AddTextPiece( FONT_SIZE_13, "%d", m_iCurPage + 1 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	if ( m_iMaxPage == 0 )
		m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage + 1 );
	else
		m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage );
}

void MainMissionWnd::InitMissionInfoBtn()
{
	for( DWORD dwID = ID_MISSION_INFO_BTN1; dwID <= ID_MISSION_INFO_BTN6; ++dwID )
	{
		MissionInfoBtn *pBtn = dynamic_cast<MissionInfoBtn*>( FindChildWnd( dwID ) );
		if( pBtn )
		{
			if( dwID < static_cast<int>( ID_MISSION_INFO_BTN1 + MAX_MISSION_CNT ) )
				pBtn->ShowWnd();
			else
				pBtn->HideWnd();
		}
	}
}

void MainMissionWnd::UpdateRadioBtn()
{
	char szTitle[MAX_PATH] = "";

	MissionRadioBtn *pDailyMission = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_DAILY_MISSION_BTN ) );
	if( pDailyMission )
	{
		//타이틀 세팅
		int iMaxMission = g_MissionManager.GetMissionCountToType( MDT_DAILY );
		SafeSprintf( szTitle, sizeof( szTitle ), "일일미션(%d)", iMaxMission );
		pDailyMission->SetTitleText( szTitle );

		bool bStartSparkle = false;
		//현재 탭이 일일미션이 아니라면
		if ( m_dwTabID != ID_DAILY_MISSION_BTN )
		{
			//현재 보상받기가 가능한 일일 미션이 존재하는 경우
			if ( g_MissionManager.GetAttinCountToType( MDT_DAILY ) > 0 )
				bStartSparkle = true;
		}

		if ( bStartSparkle )
		{
			// 이펙트 중이 아닌경우
			if ( !pDailyMission->IsSparkling() )
				pDailyMission->SetStartSparkle();	//이펙트 시작
		}
		else
		{
			//현재 이펙트 중이라면
			if ( pDailyMission->IsSparkling() )
				pDailyMission->SetEndSparkle();		//이펙트 종료
		}
	}

	MissionRadioBtn *pWeeklyMission = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_WEEKLY_MISSION_BTN ) );
	if( pWeeklyMission )
	{
		//타이틀 세팅
		int iMaxMission = g_MissionManager.GetMissionCountToType( MDT_WEEKLY );
		SafeSprintf( szTitle, sizeof( szTitle ), "주간미션(%d)", iMaxMission );
		pWeeklyMission->SetTitleText( szTitle );

		bool bStartSparkle = false;
		//현재 탭이 주간미션이 아니라면
		if ( m_dwTabID != ID_WEEKLY_MISSION_BTN )
		{
			//현재 보상받기가 가능한 주간 미션이 존재하는 경우
			if ( g_MissionManager.GetAttinCountToType( MDT_WEEKLY ) > 0 )
				bStartSparkle = true;
		}

		if ( bStartSparkle )
		{
			// 이펙트 중이 아닌경우
			if ( !pWeeklyMission->IsSparkling() )
				pWeeklyMission->SetStartSparkle();	//이펙트 시작
		}
		else
		{
			//현재 이펙트 중이라면
			if ( pWeeklyMission->IsSparkling() )
				pWeeklyMission->SetEndSparkle();		//이펙트 종료
		}
	}

	MissionRadioBtn *pMonthlyMission = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_MONTHLY_MISSION_BTN ) );
	if( pMonthlyMission )
	{
		int iMaxMission = g_MissionManager.GetMissionCountToType( MDT_MONTHLY );
		SafeSprintf( szTitle, sizeof( szTitle ), "월간미션(%d)", iMaxMission );
		pMonthlyMission->SetTitleText( szTitle );

		bool bStartSparkle = false;
		//현재 탭이 월간미션이 아니라면
		if ( m_dwTabID != ID_MONTHLY_MISSION_BTN )
		{
			//현재 보상받기가 가능한 월간 미션이 존재하는 경우
			if ( g_MissionManager.GetAttinCountToType( MDT_MONTHLY ) > 0 )
				bStartSparkle = true;
		}

		if ( bStartSparkle )
		{
			// 이펙트 중이 아닌경우
			if ( !pMonthlyMission->IsSparkling() )
				pMonthlyMission->SetStartSparkle();	//이펙트 시작
		}
		else
		{
			//현재 이펙트 중이라면
			if ( pMonthlyMission->IsSparkling() )
				pMonthlyMission->SetEndSparkle();		//이펙트 종료
		}
	}
}

void MainMissionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if ( g_MissionManager.IsChangeMission() )
	{
		UpdateRadioBtn();
		UpdateMissionInfo();
	}
}

void MainMissionWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{ 
		CUR_PAGE_X = 230,
		CUR_PAGE_Y = 393,
	};

	m_PagePrinter.PrintFullText( iXPos + CUR_PAGE_X, iYPos + CUR_PAGE_Y, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////////////
MissionInfoBtn::MissionInfoBtn()
{
	m_pIconBack = NULL;
	m_pIconEmpty = NULL;
	m_pCompleteImg = NULL;
	m_pGauegeBack = NULL;
	m_pGauegeBar  = NULL;
	m_pTextNum = NULL;
	m_pTextPercent = NULL;
	m_pToolTip = NULL;
	m_pMissionIcon = NULL;
	m_pYellowBackFrm = NULL;
	m_pNormalBackFrm = NULL;
	m_pCostumeMark = NULL;
	ClearData();
}

MissionInfoBtn::~MissionInfoBtn()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconEmpty );
	SAFEDELETE( m_pCompleteImg );
	SAFEDELETE( m_pGauegeBack );
	SAFEDELETE( m_pGauegeBar );
	SAFEDELETE( m_pTextNum );
	SAFEDELETE( m_pTextPercent );
	SAFEDELETE( m_pToolTip );
	SAFEDELETE( m_pYellowBackFrm );
	SAFEDELETE( m_pNormalBackFrm );
	SAFEDELETE( m_pCostumeMark );

	ClearData();
	ClearIconMap();
}

void MissionInfoBtn::ClearData()
{
	m_dwMissionMagicCode = 0;
	m_iMaxValue = 0;
	m_iCurValue = 0;
	m_byClearType = MIS_NONE;
	m_AttainShowStyle = MASS_VALUE;
	for( int i=0; i < MissionInfoBtn::TITLE_CNT; ++i )
		m_TitlePrinter[i].ClearList();

	m_fGaugeBarRate = 0.0f; 
	m_pCurFrm = NULL;
	m_fLineCut = 166.0f;
	m_bIsCostume = false;
}

void MissionInfoBtn::ClearIconMap()
{
	MissionIconMap::iterator iCreate = m_MissionIconMap.begin();
	for( ; iCreate != m_MissionIconMap.end() ; ++iCreate )
		SAFEDELETE( iCreate->second );
	m_MissionIconMap.clear();
}

void MissionInfoBtn::iwm_hide()
{
	ClearData();
}

void MissionInfoBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else if( szType == "CompleteImg" )
	{
		SAFEDELETE( m_pCompleteImg );
		m_pCompleteImg = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGauegeBack );
		m_pGauegeBack = pImage;
	}
	else if( szType == "GaugeBar" )
	{
		SAFEDELETE( m_pGauegeBar );
		m_pGauegeBar = pImage;
	}
	else if( szType == "TextNum" )
	{
		SAFEDELETE( m_pTextNum );
		m_pTextNum = pImage;
	}
	else if( szType == "TextPercentNum" )
	{
		SAFEDELETE( m_pTextPercent );
		m_pTextPercent = pImage;
	}
	else if( szType == "ToolTip" )
	{
		SAFEDELETE( m_pToolTip );
		m_pToolTip = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}
}

void MissionInfoBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "YellowBack" )
		{
			SAFEDELETE( m_pYellowBackFrm );
			m_pYellowBackFrm = pFrame;
		}
		else if( szType == "NormalBack" )
		{
			SAFEDELETE( m_pNormalBackFrm );
			m_pNormalBackFrm = pFrame;
		}
		else
			ioButton::AddRenderFrame( szType, pFrame );
	}
}

void MissionInfoBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_ATTAIN_BTN:
		if( cmd == IOBN_BTNUP )
		{
			MissionAttainStateWnd* pMissionAttainStateWnd = dynamic_cast<MissionAttainStateWnd*>( g_GUIMgr.FindWnd( MISSION_ATTAIN_STATE_WND ) );
			if( pMissionAttainStateWnd )
			{
				if( !pMissionAttainStateWnd->IsShow() )
				{
					SP2Packet kPacket( CTPK_MISSION_COMPENSATION_RECV );
					kPacket << m_dwMissionMagicCode;
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "미션 진행중 입니다." );
		}
		break;
	}
}

void MissionInfoBtn::SetMission( const ioMission* const rkMissionData )
{
	ClearData();
	SetActive();

	if ( rkMissionData )
	{
		//Magic Code
		m_dwMissionMagicCode = rkMissionData->GetMagicCode();			

		//Clear Type
		m_byClearType = rkMissionData->GetClearMissionType();

		// 최종 달성치
		m_iMaxValue = rkMissionData->GetMaxAchieveValue();
		// 현재 달성치
		m_iCurValue = rkMissionData->GetCurAchieveValue();

		//Icon
		m_pMissionIcon = GetMissionIcon( rkMissionData->GetIconName() );
		m_bIsCostume = rkMissionData->IsMissionCostume();

		//Title
		SetMissionTitle( rkMissionData );

		//Frame
		m_AttainShowStyle = rkMissionData->GetAttinShowStyle();
		if ( rkMissionData->GetBackFrameType() == MBFT_YELLOW && m_byClearType != MIS_COMPLETE )
			m_pCurFrm = m_pYellowBackFrm;
		else
			m_pCurFrm = m_pNormalBackFrm;

		// 달성치 표기 세팅
		if( m_AttainShowStyle == MASS_GAUGE )
		{
			float fXScale;
			if ( m_iMaxValue > 0 )
				fXScale = m_iCurValue/(float)m_iMaxValue;
			else
				fXScale = 0.0f;
			m_fGaugeBarRate = min( FLOAT1, fXScale );
			m_fLineCut = 99.0f;
		}
		else
			m_fLineCut = 166.0f;

		MissionInfoRewardBtn *pBtn = dynamic_cast<MissionInfoRewardBtn*>( FindChildWnd( ID_ATTAIN_BTN ) );
		if ( pBtn )
		{
			if ( m_byClearType == MIS_PROGRESS )
			{
				pBtn->SetActive();
				pBtn->AddWndStyle( IWS_EXACTIVE );
			}
			else if ( m_byClearType == MIS_ATTAIN )
			{
				pBtn->SetActive();
				pBtn->RemoveWndStyle( IWS_EXACTIVE );
			}
			else
				pBtn->SetInActive();

			pBtn->ShowWnd();
		}
	}
	else
	{
		SetInActive();
		MissionInfoRewardBtn *pBtn = dynamic_cast<MissionInfoRewardBtn*>( FindChildWnd( ID_ATTAIN_BTN ) );
		if ( pBtn )
			pBtn->HideWnd();
	}
}

void MissionInfoBtn::SetMissionTitle( const ioMission* const rkMissionData )
{
	if ( !rkMissionData || rkMissionData->GetMagicCode() == 0 )
		return;
	const ioMission::vTitleText& TotalTitleText = rkMissionData->GetTitlePrinter();


	int iMaxTitleLine = (int)TotalTitleText.size();
	if ( iMaxTitleLine > MissionInfoBtn::TITLE_CNT )
	{
		LOG.PrintTimeAndLog( 0, "MissionInfoBtn::SetMissionTitle - Title Line Longer than Two Line" );
		return;
	}
	for ( int iLine = 0 ; iLine < iMaxTitleLine ; ++iLine )
	{
		const ioMission::vTitleLine& TitleColorText = TotalTitleText[iLine];
		for ( int iColorText = 0 ; iColorText<(int)TitleColorText.size() ; ++iColorText )
		{
			m_TitlePrinter[iLine].SetTextStyle( TS_NORMAL );
			m_TitlePrinter[iLine].SetBkColor( 0, 0, 0 );

			//미션 완료 상태인 경우는 Text색을 TCT_DEFAULT_LIGHTGRAY로 고정!
			if ( m_byClearType == MIS_COMPLETE )
				m_TitlePrinter[iLine].SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				m_TitlePrinter[iLine].SetTextColor( TitleColorText[iColorText].m_dwColor );
			m_TitlePrinter[iLine].AddTextPiece( FONT_SIZE_13, TitleColorText[iColorText].m_szText.c_str() );

			float ftextwidth = m_TitlePrinter[iLine].GetFullWidth();
			if ( m_fLineCut <= ftextwidth )
				break;
		}
	}
	float fLineCut = 166.0f;
}

ioUIRenderImage* MissionInfoBtn::GetMissionIcon( const ioHashString &szIconName )
{
	if ( szIconName.IsEmpty() )
		return NULL;

	MissionIconMap::iterator iter = m_MissionIconMap.find( szIconName );
	if( iter != m_MissionIconMap.end() )
		return iter->second;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
	{
		m_MissionIconMap.insert( MissionIconMap::value_type( szIconName, pIcon ) );
		return pIcon;
	}

	return NULL;
}

void MissionInfoBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		//미션 없는 빈슬롯
		EMPTY_ICON_POS_X				= 36,
		EMPTY_ICON_POS_Y				= 35,

		EMPTY_TEXT_POS_X				= 74,
		EMPTY_TEXT_POS_Y				= 18,
		EMPTY_TEXT_OFFSET_Y_LING_GAP	= 16,

		//미션이 있는 경우
		ICON_POS_X						= 36,
		ICON_POS_Y						= 33,

		TITLE_POS_X						= 74,
		ONE_TITLE_POS_Y					= 27,
		TWO_TITLE_POS_Y					= 18,
		TITLE__OFFSET_Y_LING_GAP		= 16,
	};

	//Empty Slot
	if ( m_dwMissionMagicCode == 0 )
	{
		if( m_pIconEmpty )
		{
			m_pIconEmpty->SetScale( 0.75f, 0.75f );
			m_pIconEmpty->Render( iXPos + EMPTY_ICON_POS_X, iYPos + EMPTY_ICON_POS_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( 178, 168, 130 );
		g_FontMgr.PrintText( iXPos + EMPTY_TEXT_POS_X, iYPos + EMPTY_TEXT_POS_Y, FONT_SIZE_13, "내용없음" );
		g_FontMgr.PrintText( iXPos + EMPTY_TEXT_POS_X, iYPos + EMPTY_TEXT_POS_Y + EMPTY_TEXT_OFFSET_Y_LING_GAP, FONT_SIZE_13, "미션 대기중" );
		return;
	}

	// 아이콘 출력
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.65f, 0.65f );
		m_pIconBack->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if ( m_byClearType == MIS_COMPLETE )
	{
		if( m_pMissionIcon )
		{
			m_pMissionIcon->SetScale( 0.65f, 0.65f );
			m_pMissionIcon->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
		}

		// UI코스튬 관련 (코스튬 마크)
		if( m_bIsCostume && m_pCostumeMark )
		{
			m_pCostumeMark->SetScale( 0.65f, 0.65f );
			m_pCostumeMark->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
		}

		if( m_pCompleteImg && m_byClearType == MIS_COMPLETE )
			m_pCompleteImg->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
	}
	else
	{
		if( m_pMissionIcon )
		{
			m_pMissionIcon->SetScale( 0.65f, 0.65f );
			m_pMissionIcon->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		// UI코스튬 관련 (코스튬 마크)
		if( m_bIsCostume && m_pCostumeMark )
		{
			m_pCostumeMark->SetScale( 0.65f, 0.65f );
			m_pCostumeMark->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	//Title이 한줄이라면
	if ( m_TitlePrinter[1].IsEmpty() )
	{
		if ( !m_TitlePrinter[0].IsEmpty() )
			m_TitlePrinter[0].PrintFullTextWidthCut( iXPos + TITLE_POS_X, iYPos + ONE_TITLE_POS_Y, TAT_LEFT, m_fLineCut );
	}
	else
	{
		for ( int i=0 ; i<TITLE_CNT ; ++i )
		{
			if ( !m_TitlePrinter[i].IsEmpty() )
				m_TitlePrinter[i].PrintFullTextWidthCut( iXPos + TITLE_POS_X, iYPos + TWO_TITLE_POS_Y + (TITLE__OFFSET_Y_LING_GAP * i ), TAT_LEFT, m_fLineCut );
		}
	}

	OnRenderAttain( iXPos, iYPos );
}

void MissionInfoBtn::OnRenderAttain( int iXPos, int iYPos )
{
	enum
	{
		VALUE_TEXT_X					= 312,
		VALUE_TEXT_Y					= 26,

		GAUGE_POS_X						= 180,
		GAUGE_POS_Y						= 26,

		GAUGE_TEXT_POS_X_1				= 236,
		GAUGE_TEXT_POS_X_2				= 232,
		GAUGE_TEXT_POS_X_3				= 229,
		GAUGE_TEXT_POS_Y				= 29,
		GAUGE_TEXT_NUM_SIZE				= 7,
	};

	if ( m_AttainShowStyle == MASS_VALUE )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		// 진행률 출력
		float fLineCut = 62.0f;
		if( m_byClearType == MIS_COMPLETE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintTextWidthCut( iXPos + VALUE_TEXT_X, iYPos + VALUE_TEXT_Y, FONT_SIZE_13, fLineCut, "%d/%d", m_iCurValue, m_iMaxValue );
	}
	else if ( m_AttainShowStyle == MASS_GAUGE )
	{
		if ( m_pGauegeBack)
			m_pGauegeBack->Render( iXPos + GAUGE_POS_X, iYPos + GAUGE_POS_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		if ( m_pGauegeBar )
		{
			int iGaugeBarWidth = int( m_pGauegeBar->GetWidth() * m_fGaugeBarRate );
			if ( m_byClearType == MIS_COMPLETE )
				m_pGauegeBar->RenderWidthCut( iXPos + GAUGE_POS_X, iYPos + GAUGE_POS_Y, 0, iGaugeBarWidth, UI_RENDER_GRAY_LIGHT );
			else
				m_pGauegeBar->RenderWidthCut( iXPos + GAUGE_POS_X, iYPos + GAUGE_POS_Y, 0, iGaugeBarWidth );
		}

		//Percentage Render
		float fPercentage = m_fGaugeBarRate * FLOAT100 + 0.05f;
		int iPercentage = (int)fPercentage;
		int ICurXPos = iXPos;
		int iOffset = 0;
		//한자리
		if( iPercentage < 10 )
		{
			iOffset = GAUGE_TEXT_NUM_SIZE;
			ICurXPos += GAUGE_TEXT_POS_X_1;
		}
		//두자리
		else if( iPercentage < 100 )
		{
			iOffset = GAUGE_TEXT_NUM_SIZE * 2;
			ICurXPos += GAUGE_TEXT_POS_X_2;
		}
		//세자리
		else
		{
			iOffset = GAUGE_TEXT_NUM_SIZE * 3;
			ICurXPos += GAUGE_TEXT_POS_X_3;
		}
		UIRenderType UiType = UI_RENDER_NORMAL;
		if ( m_byClearType == MIS_COMPLETE )
			UiType = UI_RENDER_NORMAL;
		else
			UiType = UI_RENDER_ADD;
		m_pTextNum->RenderNum( ICurXPos, iYPos + GAUGE_TEXT_POS_Y, iPercentage, 0.0f, FLOAT1, UiType );
		ICurXPos += iOffset;
		m_pTextPercent->Render( ICurXPos, iYPos + GAUGE_TEXT_POS_Y, UiType );
	}
}

void MissionInfoBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_pCurFrm )
		m_pCurFrm->Render( iXPos, iYPos );

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void MissionInfoBtn::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_pCurFrm )
		m_pCurFrm->Render( iXPos, iYPos );
	OnDrawOveredAdd( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void MissionInfoBtn::OnDrawPushed( int iXPos, int iYPos )
{
	if( m_pCurFrm )
		m_pCurFrm->Render( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
}
//////////////////////////////////////////////////////////////////////////
MissionInfoRewardBtn::MissionInfoRewardBtn():
m_dwAlphaLoopStartTime( 0 ),
m_dwAlphaLoopTicTime( 0 ),
m_pRewardOutLine( NULL ),
m_byOutLineScaleType( 0 ),
m_fAlphaSpeed( 0.0f ),
m_fCurAlpha( 0.0f )
{
}

MissionInfoRewardBtn::~MissionInfoRewardBtn()
{
	SAFEDELETE( m_pRewardOutLine );
}

void MissionInfoRewardBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "RewardOutLine" )
		{
			SAFEDELETE( m_pRewardOutLine );
			m_pRewardOutLine = pFrame;
		}
		else
			ioButton::AddRenderFrame( szType, pFrame );
	}
}

void MissionInfoRewardBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_dwAlphaLoopTicTime	= (DWORD)xElement.GetIntAttribute_e( "AlphaLoopTime" );
	m_byOutLineScaleType	= (BYTE)xElement.GetIntAttribute_e( "OutLineType" );
	m_fAlphaSpeed			= xElement.GetFloatAttribute_e( "AlphaSpeed" );
}

void MissionInfoRewardBtn::iwm_create()
{
	if ( m_pRewardOutLine )
	{
		if ( m_byOutLineScaleType == LOBBY_OUTLINE_TYPE )
			m_pRewardOutLine->SetSize( 98, 21 );
		else if ( m_byOutLineScaleType == POPUP_OUTLINE_TYPE )
			m_pRewardOutLine->SetSize( 79, 33 );
	}
}

void MissionInfoRewardBtn::iwm_show()
{
	m_fCurAlpha = MAX_ALPHA_RATE;
	m_dwAlphaLoopStartTime = FRAMEGETTIME();

	if ( m_byOutLineScaleType == POPUP_OUTLINE_TYPE )
	{
		if ( IsExDisabled() )
			m_pUITitle->SetOffset( 38, 6 );
		else
			m_pUITitle->SetOffset( 40, 7 );
	}
}

void MissionInfoRewardBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioButton::OnProcess( fTimePerSec );

	if( !IsDisabled() && !IsExDisabled() )
	{
		if ( m_pRewardOutLine )
		{
			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwCurHalfTicTime = m_dwAlphaLoopTicTime/2;
			DWORD dwGapTime = dwCurTime - m_dwAlphaLoopStartTime;
			if( dwGapTime < dwCurHalfTicTime )
			{
				m_fCurAlpha -= m_fAlphaSpeed * fTimePerSec;
				m_fCurAlpha = max( 0.0f, m_fCurAlpha );
			}
			else
			{
				m_fCurAlpha += m_fAlphaSpeed * fTimePerSec;
				m_fCurAlpha = min( 255.0f, m_fCurAlpha );
				if ( dwGapTime > m_dwAlphaLoopTicTime )
				{
					m_fCurAlpha = 255.0f ;
					m_dwAlphaLoopStartTime = dwCurTime;
				}
			}
		}
	}
}

void MissionInfoRewardBtn::OnRender()
{
	ioButton::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( !IsDisabled() && !IsExDisabled() )
	{
		if ( m_pRewardOutLine )
		{
			int iAlpha = (int)m_fCurAlpha;
			m_pRewardOutLine->SetAlpha( (BYTE)iAlpha );
			m_pRewardOutLine->Render( iXPos, iYPos - 1 );
		}
	}
}

void MissionInfoRewardBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	if ( m_byOutLineScaleType == LOBBY_OUTLINE_TYPE )
		ioButton::OnDrawDisabled( iXPos, iYPos );
	else if ( m_byOutLineScaleType == POPUP_OUTLINE_TYPE )
	{
		if( m_pDisableFrm )
			m_pDisableFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
		else if( m_pNormalFrm )
			m_pNormalFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

		if( m_pDisable )
			m_pDisable->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
		else if( m_pNormal )
			m_pNormal->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

		RenderImageList( iXPos, iYPos, UI_RENDER_GRAY );
	}
}
//////////////////////////////////////////////////////////////////////////
LobbyMainMissionWnd::LobbyMainMissionWnd() :
m_dwTabID( 0 ),
m_iMissionDateType( 0 ),
m_iCurPage( 0 ),
m_iMaxPage( 0 )
{
}

LobbyMainMissionWnd::~LobbyMainMissionWnd()
{
}

void LobbyMainMissionWnd::iwm_show()
{
	ChangeRadioBtn( ID_DAILY_MISSION_BTN );
}

void LobbyMainMissionWnd::iwm_hide()
{
	m_iCurPage = m_iMaxPage = m_iMissionDateType = m_dwTabID = 0;
	m_PagePrinter.ClearList();
}

void LobbyMainMissionWnd::iwm_wheel( int zDelta )
{
	if( zDelta == WHEEL_DELTA )
	{
		if( m_iCurPage > 0 )
		{
			m_iCurPage--;
			UpdateMissionInfo();
		}
	}
	else if( zDelta == -WHEEL_DELTA )
	{
		if( m_iCurPage < m_iMaxPage - 1 )
		{
			m_iCurPage++;
			UpdateMissionInfo();
		}
	}
}

void LobbyMainMissionWnd::InitMissionInfoBtn()
{
	for( DWORD dwID = ID_MISSION_INFO_BTN1; dwID <= ID_MISSION_INFO_BTN5; ++dwID )
	{
		LobbyMissionInfoBtn *pBtn = dynamic_cast<LobbyMissionInfoBtn*>( FindChildWnd( dwID ) );
		if( pBtn )
			pBtn->ShowWnd();
	}
}

void LobbyMainMissionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_MISSION_INFO_BTN1:
	case ID_MISSION_INFO_BTN2:
	case ID_MISSION_INFO_BTN3:
	case ID_MISSION_INFO_BTN4:
	case ID_MISSION_INFO_BTN5:
		{
			if( cmd == IOWN_OVERED )
			{
				LobbyMissionInfoBtn *pInfo = dynamic_cast<LobbyMissionInfoBtn*>( FindChildWnd( dwID ) );
				if ( pInfo )
				{
					if ( pInfo->GetMissionCode() != 0 )
					{
						MissionToolTip *pTooltip = dynamic_cast<MissionToolTip*>( FindChildWnd( ID_MISSION_TOOLTIP ) );
						if( pTooltip )
						{
							if( pTooltip->SetInfo( g_MissionManager.GetMissionToCode( pInfo->GetMissionCode() ) , pInfo->GetPosX(), pInfo->GetPosY() ) )
								ShowChildWnd( ID_MISSION_TOOLTIP );
						}
					}
				}
			}
			else if( cmd == IOWN_LEAVED ) 
				HideChildWnd( ID_MISSION_TOOLTIP );
		}
		break;
	case ID_DAILY_MISSION_BTN:
	case ID_WEEKLY_MISSION_BTN:
	case ID_MONTHLY_MISSION_BTN:
		if( cmd == IOBN_BTNDOWN )
		{			
			ChangeRadioBtn( dwID );
		}
		break;
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_iCurPage > 0 )
			{
				m_iCurPage--;
				UpdateMissionInfo();
			}
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_iCurPage < m_iMaxPage - 1 )
			{
				m_iCurPage++;
				UpdateMissionInfo();
			}
		}
		break;
	}
}

void LobbyMainMissionWnd::ChangeRadioBtn( DWORD dwID )
{
	if( m_dwTabID == dwID )
		return;

	m_dwTabID = dwID;
	CheckRadioButton( ID_DAILY_MISSION_BTN, ID_MONTHLY_MISSION_BTN, dwID );
	if ( m_dwTabID == ID_DAILY_MISSION_BTN )
		m_iMissionDateType = MDT_DAILY;
	else if ( m_dwTabID == ID_WEEKLY_MISSION_BTN )
		m_iMissionDateType = MDT_WEEKLY;
	else if ( m_dwTabID == ID_MONTHLY_MISSION_BTN )
		m_iMissionDateType = MDT_MONTHLY;

	m_iCurPage = m_iMaxPage = 0;
	InitMissionInfoBtn();
	UpdateMissionInfo();
	UpdateRadioBtn();
}

void LobbyMainMissionWnd::UpdateMissionInfo()
{
	int iMissionCnt = g_MissionManager.GetMissionCountToType( m_iMissionDateType );
	m_iMaxPage =  iMissionCnt/ LOBBY_MAX_MISSION_CNT;
	if ( iMissionCnt % LOBBY_MAX_MISSION_CNT > 0 )
		m_iMaxPage++;

	if ( !COMPARE( m_iCurPage, 0, m_iMaxPage ) )
	{
		m_iCurPage = max( 0, m_iCurPage );
		m_iCurPage = min( m_iCurPage, max( 0, (m_iMaxPage - 1) ) );
	}
	
	int iStartArray =  m_iCurPage * LOBBY_MAX_MISSION_CNT;
	for(int i = 0;i < LOBBY_MAX_MISSION_CNT ;i++,iStartArray++)
	{
		LobbyMissionInfoBtn *pMissionInfomBtn = dynamic_cast<LobbyMissionInfoBtn*>( FindChildWnd( ID_MISSION_INFO_BTN1 + i ) );
		if( pMissionInfomBtn )
			pMissionInfomBtn->SetMission( g_MissionManager.GetMissionToArray( m_iMissionDateType, iStartArray ) );
	}

	m_PagePrinter.ClearList();

	m_PagePrinter.SetTextStyle( TS_NORMAL );
	m_PagePrinter.SetBkColor( 0, 0, 0 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PagePrinter.AddTextPiece( FONT_SIZE_13, "%d", m_iCurPage + 1 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	if ( m_iMaxPage == 0 )
		m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage + 1 );
	else
		m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage );
}

void LobbyMainMissionWnd::UpdateRadioBtn()
{
	MissionRadioBtn *pDailyMission = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_DAILY_MISSION_BTN ) );
	if( pDailyMission )
	{
		bool bStartSparkle = false;
		//현재 탭이 일일미션이 아니라면
		if ( m_dwTabID != ID_DAILY_MISSION_BTN )
		{
			//현재 보상받기가 가능한 일일 미션이 존재하는 경우
			if ( g_MissionManager.GetAttinCountToType( MDT_DAILY ) > 0 )
				bStartSparkle = true;
		}

		if ( bStartSparkle )
		{
			// 이펙트 중이 아닌경우
			if ( !pDailyMission->IsSparkling() )
				pDailyMission->SetStartSparkle();	//이펙트 시작
		}
		else
		{
			//현재 이펙트 중이라면
			if ( pDailyMission->IsSparkling() )
				pDailyMission->SetEndSparkle();		//이펙트 종료
		}
	}

	MissionRadioBtn *pWeeklyMission = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_WEEKLY_MISSION_BTN ) );
	if( pWeeklyMission )
	{
		bool bStartSparkle = false;
		//현재 탭이 주간미션이 아니라면
		if ( m_dwTabID != ID_WEEKLY_MISSION_BTN )
		{
			//현재 보상받기가 가능한 주간 미션이 존재하는 경우
			if ( g_MissionManager.GetAttinCountToType( MDT_WEEKLY ) > 0 )
				bStartSparkle = true;
		}

		if ( bStartSparkle )
		{
			// 이펙트 중이 아닌경우
			if ( !pWeeklyMission->IsSparkling() )
				pWeeklyMission->SetStartSparkle();	//이펙트 시작
		}
		else
		{
			//현재 이펙트 중이라면
			if ( pWeeklyMission->IsSparkling() )
				pWeeklyMission->SetEndSparkle();		//이펙트 종료
		}
	}

	MissionRadioBtn *pMonthlyMission = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_MONTHLY_MISSION_BTN ) );
	if( pMonthlyMission )
	{
		bool bStartSparkle = false;
		//현재 탭이 월간미션이 아니라면
		if ( m_dwTabID != ID_MONTHLY_MISSION_BTN )
		{
			//현재 보상받기가 가능한 월간 미션이 존재하는 경우
			if ( g_MissionManager.GetAttinCountToType( MDT_MONTHLY ) > 0 )
				bStartSparkle = true;
		}

		if ( bStartSparkle )
		{
			// 이펙트 중이 아닌경우
			if ( !pMonthlyMission->IsSparkling() )
				pMonthlyMission->SetStartSparkle();	//이펙트 시작
		}
		else
		{
			//현재 이펙트 중이라면
			if ( pMonthlyMission->IsSparkling() )
				pMonthlyMission->SetEndSparkle();		//이펙트 종료
		}
	}
}

void LobbyMainMissionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if ( g_MissionManager.IsChangeMission() )
	{
		UpdateRadioBtn();
		UpdateMissionInfo();
	}
}

void LobbyMainMissionWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{ 
		CUR_PAGE_X = 105,
		CUR_PAGE_Y = 334,
	};

	m_PagePrinter.PrintFullText( iXPos + CUR_PAGE_X, iYPos + CUR_PAGE_Y, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
LobbyMissionInfoBtn::LobbyMissionInfoBtn()
{
	m_pIconBack = NULL;
	m_pIconEmpty = NULL;
	m_pMissionIcon = NULL;
	m_pCompleteImg = NULL;
	m_pYellowBackFrm = NULL;
	m_pNormalBackFrm = NULL;
	m_pGauegeBack = NULL;
	m_pGauegeBar  = NULL;
	m_pTextNum = NULL;
	m_pTextPercent = NULL;
	m_pToolTip = NULL;
	m_pCostumeMark = NULL;
	m_fLineCut = 99.0f;
	ClearData();
}

LobbyMissionInfoBtn::~LobbyMissionInfoBtn()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconEmpty );
	SAFEDELETE( m_pCompleteImg );
	SAFEDELETE( m_pYellowBackFrm );
	SAFEDELETE( m_pNormalBackFrm );
	SAFEDELETE( m_pGauegeBack );
	SAFEDELETE( m_pGauegeBar );
	SAFEDELETE( m_pTextNum );
	SAFEDELETE( m_pTextPercent );
	SAFEDELETE( m_pToolTip );
	SAFEDELETE( m_pCostumeMark );

	ClearData();
	ClearIconMap();
}

void LobbyMissionInfoBtn::ClearData()
{
	m_dwMissionMagicCode = 0;
	m_iMaxValue = 0;
	m_iCurValue = 0;
	m_byClearType = MIS_NONE;
	m_AttainShowStyle = MASS_VALUE;
	for( int i=0; i < LobbyMissionInfoBtn::TITLE_CNT; ++i )
		m_TitlePrinter[i].ClearList();

	m_fGaugeBarRate = 0.0f; 
	m_pCurFrm = NULL;
	m_bIsCostume = false;
}

void LobbyMissionInfoBtn::ClearIconMap()
{
	MissionIconMap::iterator iCreate = m_MissionIconMap.begin();
	for( ; iCreate != m_MissionIconMap.end() ; ++iCreate )
		SAFEDELETE( iCreate->second );
	m_MissionIconMap.clear();
}

void LobbyMissionInfoBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else if( szType == "CompleteImg" )
	{
		SAFEDELETE( m_pCompleteImg );
		m_pCompleteImg = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGauegeBack );
		m_pGauegeBack = pImage;
	}
	else if( szType == "GaugeBar" )
	{
		SAFEDELETE( m_pGauegeBar );
		m_pGauegeBar = pImage;
	}
	else if( szType == "TextNum" )
	{
		SAFEDELETE( m_pTextNum );
		m_pTextNum = pImage;
	}
	else if( szType == "TextPercentNum" )
	{
		SAFEDELETE( m_pTextPercent );
		m_pTextPercent = pImage;
	}
	else if( szType == "ToolTip" )
	{
		SAFEDELETE( m_pToolTip );
		m_pToolTip = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}
}

void LobbyMissionInfoBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "YellowBack" )
		{
			SAFEDELETE( m_pYellowBackFrm );
			m_pYellowBackFrm = pFrame;
		}
		else if( szType == "NormalTitle" )
		{
			SAFEDELETE( m_pNormalBackFrm );
			m_pNormalBackFrm = pFrame;
		}
		else
			ioButton::AddRenderFrame( szType, pFrame );
	}
}

void LobbyMissionInfoBtn::iwm_hide()
{
	ClearData();
	MissionInfoRewardBtn* pBtn = dynamic_cast<MissionInfoRewardBtn*>( FindChildWnd( ID_ATTAIN_BTN ) );
	if ( pBtn )
		pBtn->HideWnd();
}

void LobbyMissionInfoBtn::SetMission( const ioMission* const rkMissionData ) 
{
	ClearData();
	MissionInfoRewardBtn* pBtn = dynamic_cast<MissionInfoRewardBtn*>( FindChildWnd( ID_ATTAIN_BTN ) );
	if ( pBtn || pBtn->IsShow() )
		pBtn->HideWnd();
	SetActive();

	if ( rkMissionData )
	{
		//Magic Code
		m_dwMissionMagicCode = rkMissionData->GetMagicCode();			

		//Clear Type
		m_byClearType = rkMissionData->GetClearMissionType();
		if ( m_byClearType == MIS_ATTAIN )
		{
			//보상받기 Button
			MissionInfoRewardBtn* pBtn = dynamic_cast<MissionInfoRewardBtn*>( FindChildWnd( ID_ATTAIN_BTN ) );
			if ( pBtn )
				pBtn->ShowWnd();
		}
		
		// 최종 달성치
		m_iMaxValue = rkMissionData->GetMaxAchieveValue();
		// 현재 달성치
		m_iCurValue = rkMissionData->GetCurAchieveValue();

		//Icon
		m_pMissionIcon = GetMissionIcon( rkMissionData->GetIconName() );
		m_bIsCostume = rkMissionData->IsMissionCostume();

		//Title
		SetMissionTitle( rkMissionData );

		//Frame
		m_AttainShowStyle = rkMissionData->GetAttinShowStyle();
		if ( rkMissionData->GetBackFrameType() == MBFT_YELLOW && m_byClearType != MIS_COMPLETE )
			m_pCurFrm = m_pYellowBackFrm;
		else
			m_pCurFrm = m_pNormalBackFrm;

		// 달성치 표기 세팅
		if( m_AttainShowStyle == MASS_GAUGE )
		{
			float fXScale;
			if ( m_iMaxValue > 0 )
				fXScale = m_iCurValue/(float)m_iMaxValue;
			else
				fXScale = 0.0f;
			m_fGaugeBarRate = min( FLOAT1, fXScale );
		}
	}
	else
		SetInActive();
}

void LobbyMissionInfoBtn::SetMissionTitle( const ioMission* const rkMissionData )
{
	if ( !rkMissionData || rkMissionData->GetMagicCode() == 0 )
		return;

	enum{ ATTAIN_PRINT_LINE = 1 };
	const ioMission::vTitleText& TotalTitleText = rkMissionData->GetTitlePrinter();
	int iMaxTitleLine = (int)TotalTitleText.size();
	// 보상받기 상태의 경우는 Title이 두줄이여도 Text가 무조건 한줄로 출력이 되어야만 한다.
	if ( m_byClearType == MIS_ATTAIN )
		iMaxTitleLine = min( ATTAIN_PRINT_LINE, iMaxTitleLine );
	if ( iMaxTitleLine > LobbyMissionInfoBtn::TITLE_CNT )
	{
		LOG.PrintTimeAndLog( 0, "LobbyMissionInfoBtn::SetMissionTitle - Title Line Longer than Two Line" );
		return;
	}
	for ( int iLine = 0 ; iLine < iMaxTitleLine ; ++iLine )
	{
		const ioMission::vTitleLine& TitleColorText = TotalTitleText[iLine];
		for ( int iColorText = 0 ; iColorText<(int)TitleColorText.size() ; ++iColorText )
		{
			m_TitlePrinter[iLine].SetTextStyle( TS_NORMAL );
			m_TitlePrinter[iLine].SetBkColor( 0, 0, 0 );

			//미션 완료 상태인 경우는 Text색을 TCT_DEFAULT_LIGHTGRAY로 고정!
			if ( m_byClearType == MIS_COMPLETE )
				m_TitlePrinter[iLine].SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			else
				m_TitlePrinter[iLine].SetTextColor( TitleColorText[iColorText].m_dwColor );
			m_TitlePrinter[iLine].AddTextPiece( FONT_SIZE_11, TitleColorText[iColorText].m_szText.c_str() );
			
			float ftextwidth = m_TitlePrinter[iLine].GetFullWidth();
			if ( m_fLineCut <= ftextwidth )
				break;
		}
	}
}

ioUIRenderImage* LobbyMissionInfoBtn::GetMissionIcon( const ioHashString &szIconName )
{
	if ( szIconName.IsEmpty() )
		return NULL;

	MissionIconMap::iterator iter = m_MissionIconMap.find( szIconName );
	if( iter != m_MissionIconMap.end() )
		return iter->second;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
	{
		m_MissionIconMap.insert( MissionIconMap::value_type( szIconName, pIcon ) );
		return pIcon;
	}

	return NULL;
}

void LobbyMissionInfoBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_ATTAIN_BTN:
		if( cmd == IOBN_BTNUP )
		{
			MissionAttainStateWnd* pMissionAttainStateWnd = dynamic_cast<MissionAttainStateWnd*>( g_GUIMgr.FindWnd( MISSION_ATTAIN_STATE_WND ) );
			if( pMissionAttainStateWnd )
			{
				if( !pMissionAttainStateWnd->IsShow() )
				{
					SP2Packet kPacket( CTPK_MISSION_COMPENSATION_RECV );
					kPacket << m_dwMissionMagicCode;
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		break;
	}
}

void LobbyMissionInfoBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		//미션 없는 빈슬롯
		EMPTY_ICON_POS_X				= 31,
		EMPTY_ICON_POS_Y				= 29,

		EMPTY_TEXT_POS_X				= 63,
		EMPTY_TEXT_POS_Y				= 16,
		EMPTY_TEXT_OFFSET_Y_LING_GAP	= 14,

		//미션이 있는 경우
		ICON_POS_X						= 32,
		ICON_POS_Y						= 31,

		TITLE_POS_X						= 63,
		ONE_TITLE_POS_Y					= 16,
		TWO_TITLE_POS_Y					= 9,
		TITLE__OFFSET_Y_LING_GAP		= 14,
	};

	//Empty Slot
	if ( m_dwMissionMagicCode == 0 )
	{
		if( m_pIconEmpty )
		{
			m_pIconEmpty->SetScale( 0.7f, 0.7f );
			m_pIconEmpty->Render( iXPos + EMPTY_ICON_POS_X, iYPos + EMPTY_ICON_POS_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
		g_FontMgr.PrintText( iXPos + EMPTY_TEXT_POS_X, iYPos + EMPTY_TEXT_POS_Y, FONT_SIZE_11, "내용없음" );
		g_FontMgr.PrintText( iXPos + EMPTY_TEXT_POS_X, iYPos + EMPTY_TEXT_POS_Y + EMPTY_TEXT_OFFSET_Y_LING_GAP, FONT_SIZE_11, "미션 대기중" );
		return;
	}

	// 아이콘 출력
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.6f, 0.6f );
		m_pIconBack->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if ( m_byClearType == MIS_COMPLETE )
	{
		if( m_pMissionIcon )
		{
			m_pMissionIcon->SetScale( 0.6f, 0.6f );
			m_pMissionIcon->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
		}

		// UI코스튬 관련 (코스튬 마크)
		if( m_bIsCostume && m_pCostumeMark )
		{
			m_pCostumeMark->SetScale( 0.6f, 0.6f );
			m_pCostumeMark->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
		}

		if( m_pCompleteImg && m_byClearType == MIS_COMPLETE )
			m_pCompleteImg->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
	}
	else
	{
		if( m_pMissionIcon )
		{
			m_pMissionIcon->SetScale( 0.6f, 0.6f );
			m_pMissionIcon->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		// UI코스튬 관련 (코스튬 마크)
		if( m_bIsCostume && m_pCostumeMark )
		{
			m_pCostumeMark->SetScale( 0.6f, 0.6f );
			m_pCostumeMark->Render( iXPos + ICON_POS_X, iYPos + ICON_POS_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	//Title이 한줄이라면
	
	if ( m_TitlePrinter[1].IsEmpty() )
	{
		if ( !m_TitlePrinter[0].IsEmpty() )
			m_TitlePrinter[0].PrintFullTextWidthCut( iXPos + TITLE_POS_X, iYPos + ONE_TITLE_POS_Y, TAT_LEFT, m_fLineCut );
	}
	else
	{
		for ( int i=0 ; i<TITLE_CNT ; ++i )
		{
			if ( !m_TitlePrinter[i].IsEmpty() )
				m_TitlePrinter[i].PrintFullTextWidthCut( iXPos + TITLE_POS_X, iYPos + TWO_TITLE_POS_Y + (TITLE__OFFSET_Y_LING_GAP * i ), TAT_LEFT, m_fLineCut );
		}
	}

	OnRenderAttain( iXPos, iYPos );
}

void LobbyMissionInfoBtn::OnRenderAttain( int iXPos, int iYPos )
{
	enum
	{
		VALUE_TEXT_X					= 63,
		VALUE_TEXT_Y_1					= 30,
		VALUE_TEXT_Y_2					= 37,

		GAUGE_POS_X						= 63,
		GAUGE_POS_Y						= 31,
		GAUGE_POS_Y_2					= 37,

		GAUGE_TEXT_POS_X_1				= 102,
		GAUGE_TEXT_POS_X_2				= 99,
		GAUGE_TEXT_POS_X_3				= 95,
		GAUGE_TEXT_POS_Y_1				= 32,
		GAUGE_TEXT_POS_Y_2				= 38,
		GAUGE_TEXT_NUM_SIZE				= 7,
	};

	if ( m_AttainShowStyle == MASS_VALUE )
	{
		float fLineCut = 99.0f;
		// 진행률 출력
		if( m_byClearType == MIS_PROGRESS || m_byClearType == MIS_COMPLETE )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			if( m_byClearType == MIS_PROGRESS )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			int iTextPosY = VALUE_TEXT_Y_1;
			//두줄이라면
			if ( !m_TitlePrinter[1].IsEmpty() )
				iTextPosY = VALUE_TEXT_Y_2;
			g_FontMgr.PrintTextWidthCut( iXPos + VALUE_TEXT_X, iYPos + iTextPosY, FONT_SIZE_11, fLineCut, " %d/%d", m_iCurValue, m_iMaxValue );
		}
	}
	else if ( m_AttainShowStyle == MASS_GAUGE )
	{
		if( m_byClearType == MIS_PROGRESS || m_byClearType == MIS_COMPLETE )
		{
			int iGaugeBarYPos = GAUGE_POS_Y;
			int iGaugeTextYPos = GAUGE_TEXT_POS_Y_1;
			//타이틀이 두줄이라면
			if ( !m_TitlePrinter[1].IsEmpty() )
			{
				iGaugeBarYPos = GAUGE_POS_Y_2;
				iGaugeTextYPos = GAUGE_TEXT_POS_Y_2;
			}
			if ( m_pGauegeBack)
				m_pGauegeBack->Render( iXPos + GAUGE_POS_X, iYPos + iGaugeBarYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			if ( m_pGauegeBar )
			{
				int iGaugeBarWidth = int( m_pGauegeBar->GetWidth() * m_fGaugeBarRate );
				if ( m_byClearType == MIS_COMPLETE )
					m_pGauegeBar->RenderWidthCut( iXPos + GAUGE_POS_X, iYPos + iGaugeBarYPos, 0, iGaugeBarWidth, UI_RENDER_GRAY_LIGHT );
				else
					m_pGauegeBar->RenderWidthCut( iXPos + GAUGE_POS_X, iYPos + iGaugeBarYPos, 0, iGaugeBarWidth );
			}

			//Percentage Render
			float fPercentage = m_fGaugeBarRate * FLOAT100 + 0.05f;
			int iPercentage = (int)fPercentage;
			int ICurXPos = iXPos;
			int iCurYPos = iYPos + iGaugeTextYPos;
			int iOffset = 0;
			//한자리
			if( iPercentage < 10 )
			{
				iOffset = GAUGE_TEXT_NUM_SIZE;
				ICurXPos += GAUGE_TEXT_POS_X_1;
			}
			//두자리
			else if( iPercentage < 100 )
			{
				iOffset = GAUGE_TEXT_NUM_SIZE * 2;
				ICurXPos += GAUGE_TEXT_POS_X_2;
			}
			//세자리
			else
			{
				iOffset = GAUGE_TEXT_NUM_SIZE * 3;
				ICurXPos += GAUGE_TEXT_POS_X_3;
			}
			UIRenderType UiType = UI_RENDER_NORMAL;
			if ( m_byClearType == MIS_COMPLETE )
				UiType = UI_RENDER_NORMAL;
			else
				UiType = UI_RENDER_ADD;
			m_pTextNum->RenderNum( ICurXPos, iCurYPos, iPercentage, 0.0f, FLOAT1, UiType );
			ICurXPos += iOffset;
			m_pTextPercent->Render( ICurXPos, iCurYPos, UiType );
		}
	}
}

void LobbyMissionInfoBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_pCurFrm )
		m_pCurFrm->Render( iXPos, iYPos );

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void LobbyMissionInfoBtn::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_pCurFrm )
		m_pCurFrm->Render( iXPos, iYPos );
	OnDrawOveredAdd( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void LobbyMissionInfoBtn::OnDrawPushed( int iXPos, int iYPos )
{
	if( m_pCurFrm )
		m_pCurFrm->Render( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
}
//////////////////////////////////////////////////////////////////////////////////
MissionAttainStateWnd::MissionAttainStateWnd()
{
	m_pBrownBackFrm = NULL;
	m_pBrownBottomFrm = NULL;

	m_pMissionIconBack1 = NULL;
	m_pMissionIconBack2 = NULL;
	m_pCompleteText = NULL;

	m_pRewardIcon = NULL;
	m_pSubIcon = NULL;
	m_pCostumeMark = NULL;

	m_dwHideDelayTime   = 0;
	m_iHelpMentSize     = 0;
	m_iRewardSize       = 0;

	m_dwState = STATE_NONE;
	m_dwCompleteTime = 0;
	m_iRewardIconWhiteAlpha = 0;

	m_iMissionPresentCode = 0;

	m_bIsCostume = false;
}

MissionAttainStateWnd::~MissionAttainStateWnd()
{
	SAFEDELETE( m_pBrownBackFrm );
	SAFEDELETE( m_pBrownBottomFrm );

	SAFEDELETE( m_pMissionIconBack1 );
	SAFEDELETE( m_pMissionIconBack2 );
	SAFEDELETE( m_pCompleteText );

	SAFEDELETE( m_pRewardIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pCostumeMark );

	m_iMissionPresentCode = 0;
}

void MissionAttainStateWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "BrownBack" )
		{
			SAFEDELETE( m_pBrownBackFrm );
			m_pBrownBackFrm = pFrame;
		}
		else if( szType == "BrownBottomFrm" )
		{
			SAFEDELETE( m_pBrownBottomFrm );
			m_pBrownBottomFrm = pFrame;
		}
		else
		{	
			ioWnd::AddRenderFrame( szType, pFrame );
		}
	}
}

void MissionAttainStateWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MissionIconBack1" )
	{
		SAFEDELETE( m_pMissionIconBack1 );
		m_pMissionIconBack1 = pImage;
	}
	else if( szType == "MissionIconBack2" )
	{
		SAFEDELETE( m_pMissionIconBack2 );
		m_pMissionIconBack2 = pImage;
	}
	else if( szType == "CompleteText" )
	{
		SAFEDELETE( m_pCompleteText );
		m_pCompleteText = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void MissionAttainStateWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );
	m_szRewardSnd = xElement.GetStringAttribute_e( "Reward" );
}

void MissionAttainStateWnd::iwm_show()
{
	ShowChildWnd( ID_ALPHA_WND );
	m_dwState = STATE_NONE;
	m_dwCompleteTime = 0;
	m_iRewardIconWhiteAlpha = 0;
}

void MissionAttainStateWnd::iwm_hide()
{
	m_dwHideDelayTime = FRAMEGETTIME();
	HideChildWnd( ID_ALPHA_WND );

	SAFEDELETE( m_pRewardIcon );
	SAFEDELETE( m_pSubIcon );

	m_bIsCostume = false;
}

bool MissionAttainStateWnd::iwm_spacebar()
{
	ioWnd *pAlphaWnd = FindChildWnd( ID_ALPHA_WND );
	if( pAlphaWnd && pAlphaWnd->IsShow() )
		return false;	

	iwm_command( FindChildWnd( ID_REWARD_BTN ), IOBN_BTNUP, 0 );
	return true;
}

void MissionAttainStateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_REWARD_BTN:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_ALPHA_WND:
		if( cmd == IOWN_HIDE )
		{
			// 알파 끝!! Complete 애니 시작
			m_dwState = STATE_SCALE;
			m_dwCompleteTime = FRAMEGETTIME();
			m_iRewardIconWhiteAlpha = 0;
		}
		break;
	}
}

void MissionAttainStateWnd::ProcessComplete()
{
	if( m_dwCompleteTime == 0 ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCompleteTime;
	switch( m_dwState )
	{
	case STATE_SCALE:
		{
			if( dwGapTime <= 100 )
			{
				if( m_pCompleteText )
				{
					float fScalePer = FLOAT1 - (float)dwGapTime / FLOAT100;
					m_pCompleteText->SetScale( FLOAT1 + fScalePer );
				}
			}
			else
			{
				m_dwState = STATE_ALPHA;
				m_dwCompleteTime = FRAMEGETTIME();
				m_iRewardIconWhiteAlpha = 0;

				if( m_pCompleteText )
				{
					m_pCompleteText->SetScale( FLOAT1 );

					if( g_ExSoundMgr.IsSound( ExSound::EST_MISSION_COMPLETE ) )
					{
						g_ExSoundMgr.PlaySound( ExSound::EST_MISSION_COMPLETE );
					}
					else
					{
						if( !m_szRewardSnd.IsEmpty() )
							g_SoundMgr.PlaySound( m_szRewardSnd );
					}
				}
			}
		}
		break;
	case STATE_ALPHA:
		{
			if( dwGapTime <= 1000 )
			{
				float fAlphaPer = FLOAT1 - (float)dwGapTime / FLOAT1000;
				m_iRewardIconWhiteAlpha = max( 0, MAX_ALPHA_RATE * fAlphaPer );
			}
		}
		break;
	}
}

void MissionAttainStateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessComplete();
}

ioUIRenderImage *MissionAttainStateWnd::GetRewardIcon( int iPresentType, int iPresentValue1, int iPresentValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentIconText(iPresentType, iPresentValue1, iPresentValue2);	
	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
		return pIcon;

	return NULL;
}

ioUIRenderImage *MissionAttainStateWnd::GetRewardSubIcon( int iPresentType, int iPresentValue1, int iPresentValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentSubIconText(iPresentType, iPresentValue1, iPresentValue2);	
	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	if( pIcon )
		return pIcon;

	return NULL;
}

bool MissionAttainStateWnd::OnRenderShowAndHide()
{
	if( !HasExWndStyle( IWS_EX_SHOW_HIDE_ACTION ) ) 
		return false;

	const DWORD dwCheckTime = 50;
	if( m_dwWndAniTime == 0 ) return false;
	if( FRAMEGETTIME() - m_dwWndAniTime > dwCheckTime )
	{
		m_dwWndAniTime = 0;
		if( m_pBrownBackFrm )
			m_pBrownBackFrm->SetSize( GetWidth(), GetHeight() );
		return false;
	}

	// 확대 & 축소를 20% ~ 100% 사이만 진행 시킨다.
	DWORD dwGapTime = FRAMEGETTIME() - m_dwWndAniTime;
	float fTimeRate = ( (float)dwGapTime / dwCheckTime ) * FLOAT08;
	if( IsShow() )
	{
		fTimeRate = min( FLOAT08, fTimeRate ) + 0.2f;
	}
	else
	{
		fTimeRate = max( 0.0f, FLOAT08 - fTimeRate ) + 0.2f;
	}

	if( m_pBrownBackFrm )
	{
		int iWidth = GetWidth() * fTimeRate;
		int iHeight= GetHeight() * fTimeRate;
		int iXPos  = GetDerivedPosX() + ( ( GetWidth() - iWidth ) / 2 );
		int iYPos  = GetDerivedPosY() + ( ( GetHeight() - iHeight ) / 2 );
		m_pBrownBackFrm->SetSize( iWidth, iHeight );
		m_pBrownBackFrm->Render( iXPos, iYPos );
	}		

	return true;
}

void MissionAttainStateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnRenderIcon( iXPos + 104, iYPos + 98 );
	OnRenderRewardText( iXPos, iYPos );
}

void MissionAttainStateWnd::OnRenderIcon( int iXPos, int iYPos )
{
	if( m_pMissionIconBack1 )
		m_pMissionIconBack1->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	
	if( m_pMissionIconBack2 )
		m_pMissionIconBack2->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	
	if( m_pRewardIcon )
	{
		m_pRewardIcon->SetAlpha( MAX_ALPHA_RATE );
		m_pRewardIcon->Render( iXPos, iYPos );
	}
	if( m_pSubIcon )
	{
		m_pSubIcon->SetAlpha( MAX_ALPHA_RATE );
		m_pSubIcon->Render( iXPos, iYPos );
	}
	// UI코스튬 관련 (코스튬 마크)
	if( m_bIsCostume && m_pCostumeMark )
	{
		m_pCostumeMark->SetAlpha( MAX_ALPHA_RATE );
		m_pCostumeMark->Render( iXPos, iYPos );
	}

	if( m_dwState != STATE_NONE ) 
	{
		if( m_pRewardIcon )
		{
			m_pRewardIcon->SetColor( TCT_DEFAULT_WHITE );
			m_pRewardIcon->SetAlpha( (BYTE)m_iRewardIconWhiteAlpha );
			m_pRewardIcon->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}
		if( m_pSubIcon )
		{
			m_pSubIcon->SetColor( TCT_DEFAULT_WHITE );
			m_pSubIcon->SetAlpha( (BYTE)m_iRewardIconWhiteAlpha );
			m_pSubIcon->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}
		// UI코스튬 관련 (코스튬 마크)
		if( m_bIsCostume && m_pCostumeMark )
		{
			m_pCostumeMark->SetColor( TCT_DEFAULT_WHITE );
			m_pCostumeMark->SetAlpha( (BYTE)m_iRewardIconWhiteAlpha );
			m_pCostumeMark->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}

		if( m_pCompleteText )
			m_pCompleteText->Render( iXPos, iYPos );
	}
}

void MissionAttainStateWnd::OnRenderRewardText( int iXPos, int iYPos )
{
	enum
	{
		REWARD_TEXT_POS_X		= 103,
		REWARD_TEXT_POS_Y		= 144,
		REWARD_TEXT_LINE_GAP	= 16,

		WND_TEXT_LINE_GAP		= 16,
		DEFAULT_BOTTOM_POS_Y	= 175,
		
	};

	int iLine = max( 0, ( m_RewardTextVec.size() - 1 ) );
	float fTextLineSize = WND_TEXT_LINE_GAP * iLine;
	m_pBrownBottomFrm->Render( iXPos, iYPos + DEFAULT_BOTTOM_POS_Y + fTextLineSize, UI_RENDER_MULTIPLY );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	float fLineCut = 197.0f;
	for ( int i=0 ; i<(int)m_RewardTextVec.size() ; ++i )
	{
		int iCurYPos = iYPos + REWARD_TEXT_POS_Y + ( REWARD_TEXT_LINE_GAP * i );
		g_FontMgr.PrintTextWidthCut( iXPos + REWARD_TEXT_POS_X, iCurYPos, FONT_SIZE_13, fLineCut, m_RewardTextVec[i].c_str() );	
	}
}

void MissionAttainStateWnd::ShowMissionAttainWnd( const ioMission* const rkMissionData )
{
	if ( rkMissionData )
	{
		SAFEDELETE( m_pRewardIcon );
		SAFEDELETE( m_pSubIcon );

		m_iMissionPresentCode = rkMissionData->GetPresentNumber();
		bool bPresentDirect = false;
		int iPresentType, iPresentValue1, iPresentValue2;
		g_QuestMgr.GetPresentData( m_iMissionPresentCode, iPresentType, iPresentValue1, iPresentValue2, bPresentDirect );
		if( iPresentType != 0 )
		{
			m_pRewardIcon = GetRewardIcon( iPresentType, iPresentValue1, iPresentValue2 );
			m_pSubIcon = GetRewardSubIcon( iPresentType, iPresentValue1, iPresentValue2 );

			if ( iPresentType == PRESENT_COSTUME || iPresentType == PRESENT_COSTUME_BOX )
				m_bIsCostume = true;
		}
		SetWndReSize( rkMissionData );
		ShowWnd();
	}
	else
		HideWnd();
}

void MissionAttainStateWnd::SetWndReSize( const ioMission* const rkMissionData )
{
	if ( !rkMissionData )
		return;

	enum
	{ 
		DEFAULT_WND_SIZE_Y		= 240,
		WND_TEXT_LINE_GAP		= 16,

		DEFAULT_BUTTON_POS_Y	= 192,
	};
	m_RewardTextVec.clear();
	m_RewardTextVec = rkMissionData->GetRewardTextVec();
	int iLine = max( 0, ( m_RewardTextVec.size() - 1 ) );
	float fTextLineSize = WND_TEXT_LINE_GAP * iLine;

	SetSize( GetWidth(), DEFAULT_WND_SIZE_Y + fTextLineSize );
	SetWndPos( (Setting::Width() / 2) - (GetWidth() / 2), (Setting::Height() / 2) - (GetHeight() / 2) - 20 );

	ioWnd *pRewardBtn = FindChildWnd( ID_REWARD_BTN );
	if( pRewardBtn )
		pRewardBtn->SetWndPos( pRewardBtn->GetXPos(), DEFAULT_BUTTON_POS_Y + fTextLineSize );
}
//////////////////////////////////////////////////////////////////////////////////
MissionToolTip::MissionToolTip():
m_fMaxLimitRowSize( 200.0f )
{
	Clear();
}

MissionToolTip::~MissionToolTip()
{
}

void MissionToolTip::Clear()
{
	m_szPresent.ClearList();
	m_szAttainProgression.ClearList();
	m_szTitleVec.clear();
	m_szSubTitleVec.clear();
}

void MissionToolTip::iwm_hide()
{
	Clear();
}

bool MissionToolTip::SetInfo( const ioMission* const rkMissionData, const int& iXPos, const int& iYPos )
{
	if ( !rkMissionData || rkMissionData->GetMagicCode() == 0 )
		return false;
	enum 
	{ 
		INCREASE_WIDTH = 14,
		INCREASE_HEIGHT = 10,
		LINE_SIZE = 16, 
	};
	Clear();
	
	//Title
	const ioMission::vTitleText& TotalTitleText = rkMissionData->GetTitlePrinter();
	int iMaxTitleLine = (int)TotalTitleText.size();
	if ( iMaxTitleLine > MissionToolTip::MAX_INFO )
	{
		LOG.PrintTimeAndLog( 0, "MissionToolTip::SetInfo - Title Line Longer than MAX_INFO Line" );
		return false;
	}

	char szBuf[MAX_PATH] = "";
	float fLastRowSize = 0;
	for ( int iLine = 0 ; iLine <iMaxTitleLine ; ++iLine )
	{
		ZeroMemory( szBuf, sizeof(szBuf) );
		const ioMission::vTitleLine& TitleColorText = TotalTitleText[iLine];
		for ( int iColorText = 0 ; iColorText<(int)TitleColorText.size() ; ++iColorText )
			strcat( szBuf, TitleColorText[iColorText].m_szText.c_str() );

		int iCutLine = 0;
		//Tool Tip은 PrintTextWidthCut을 하면 안되기 때문에 Text의 사이즈 만큼 자르고 밑으로 출력해야한다.
		while( iCutLine < (int)strlen( szBuf ) )
		{
			char szDst[MAX_PATH] = "";
			ZeroMemory( szDst, sizeof( szDst ) );
			iCutLine += Help::StringCutFun( FONT_SIZE_12, m_fMaxLimitRowSize, TS_NORMAL, szDst, sizeof(szDst), &szBuf[iCutLine] );
			if ( iLine == 0)
			{
				m_szTitleVec.push_back( szDst );
				fLastRowSize = g_FontMgr.GetTextWidth( szDst, TS_NORMAL, FONT_SIZE_12 );
			}
			else if( iLine == 1)
			{
				m_szSubTitleVec.push_back( szDst );
			}
		}
	}

	// Attain Progression
	if( rkMissionData->GetAttinShowStyle() == MASS_VALUE )
	{
		m_szAttainProgression.SetTextStyle( TS_NORMAL );
		m_szAttainProgression.SetBkColor( 0, 0, 0 );
		ZeroMemory( szBuf, sizeof(szBuf) );
		sprintf_s_e( szBuf, " %d/%d", rkMissionData->GetCurAchieveValue(), rkMissionData->GetMaxAchieveValue() );
		//기본 진행상태는 퀘스트 처럼 타이틀 맨 마지막줄 옆에 붙에 세팅을 한다.
		//이 경우 타이틀 마지막 줄은 진행상태 텍스트에 포함시키고 m_szTitleVec에서 삭제시킨다.
		//마지막줄의 길이와 진행상태표시 길이가 제한길이보다 크다면 타이틀 다음 줄에 표시한다.
		if ( !m_szTitleVec.empty() && fLastRowSize + g_FontMgr.GetTextWidth( szBuf, TS_NORMAL, FONT_SIZE_12 ) < m_fMaxLimitRowSize )
		{
			ioHashString ioLastText = m_szTitleVec.back();
			m_szAttainProgression.SetTextColor( TCT_DEFAULT_DARKGRAY );
			m_szAttainProgression.AddTextPiece( FONT_SIZE_12, ioLastText.c_str() );
			m_szTitleVec.pop_back();
		}
		m_szAttainProgression.SetTextColor( TCT_DEFAULT_RED );
		m_szAttainProgression.AddTextPiece( FONT_SIZE_12, szBuf );
	}
	else if( rkMissionData->GetAttinShowStyle() == MASS_GAUGE )
	{
		m_szAttainProgression.SetTextStyle( TS_NORMAL );
		m_szAttainProgression.SetBkColor( 0, 0, 0 );
		ZeroMemory( szBuf, sizeof(szBuf) );

		//Calculate Percentage
		int iMaxValue = rkMissionData->GetMaxAchieveValue();
		int iCurValue = rkMissionData->GetCurAchieveValue();
		float fPercentage = 0.0f;
		if ( iMaxValue > 0 )
			fPercentage = iCurValue/(float)iMaxValue;
		fPercentage = min( FLOAT1, fPercentage );
		fPercentage = fPercentage * FLOAT100 + 0.05f;
		int iPercentage = fPercentage;

		sprintf_s_e( szBuf, " %d％", iPercentage );
		//기본 진행상태는 퀘스트 처럼 타이틀 맨 마지막줄 옆에 붙에 세팅을 한다.
		//이 경우 타이틀 마지막 줄은 진행상태 텍스트에 포함시키고 m_szTitleVec에서 삭제시킨다.
		//마지막줄의 길이와 진행상태표시 길이가 제한길이보다 크다면 타이틀 다음 줄에 표시한다.
		if ( !m_szTitleVec.empty() && fLastRowSize + g_FontMgr.GetTextWidth( szBuf, TS_NORMAL, FONT_SIZE_12 ) < m_fMaxLimitRowSize )
		{
			ioHashString ioLastText = m_szTitleVec.back();
			m_szAttainProgression.SetTextColor( TCT_DEFAULT_DARKGRAY );
			m_szAttainProgression.AddTextPiece( FONT_SIZE_12, ioLastText.c_str() );
			m_szTitleVec.pop_back();
		}
		m_szAttainProgression.SetTextColor( TCT_DEFAULT_RED );
		m_szAttainProgression.AddTextPiece( FONT_SIZE_12, szBuf );
	}
	
	//보상
	if ( rkMissionData->GetShowToolTipReward() )
	{
		DWORD dwRewardIndex = rkMissionData->GetPresentNumber();
		if ( dwRewardIndex == 0 )
		{
			LOG.PrintTimeAndLog( 0, "MissionToolTip::SetInfo - Not exist present index" );
			return false;
		}
		bool bPresentDirect = false;
		int iPresentType, iPresentValue1, iPresentValue2;
		g_QuestMgr.GetPresentData( dwRewardIndex, iPresentType, iPresentValue1, iPresentValue2, bPresentDirect );

		if( iPresentType == 0 ) return false;

		char szPresentName[MAX_PATH] = "";
		if( iPresentType == PRESENT_PESO )
			sprintf_e( szPresentName, "보상 : %s", g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );
		else
		{
			if( iPresentType == PRESENT_MEDALITEM )
				SafeSprintf( szPresentName, sizeof( szPresentName ), "보상 : %s [메달] %s", g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str(), g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );
			else
				sprintf_e( szPresentName, "보상 : %s %s", g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str(), g_PresentMgr.GetPresentValue2Text( iPresentType, iPresentValue1, iPresentValue2 ).c_str() );
		}

		m_szPresent.SetTextStyle( TS_NORMAL );
		m_szPresent.SetBkColor( 0, 0, 0 );
		m_szPresent.SetTextColor( TCT_DEFAULT_GRAY );
		m_szPresent.AddTextPiece( FONT_SIZE_12, szPresentName );
	}
	
	//첫줄은 진행상황도 같이 출력해야 함으로.
	int iWidth = 0;
	for ( int i=0; i<(int)m_szTitleVec.size() ; ++i )
		iWidth = max( g_FontMgr.GetTextWidth( m_szTitleVec[i].c_str(), TS_NORMAL, FONT_SIZE_12 ), iWidth );
	for ( int i=0; i<(int)m_szSubTitleVec.size() ; ++i )
		iWidth = max( g_FontMgr.GetTextWidth( m_szSubTitleVec[i].c_str(), TS_NORMAL, FONT_SIZE_12 ), iWidth );
	iWidth = max( m_szAttainProgression.GetFullWidth(),iWidth );
	iWidth = max( m_szPresent.GetFullWidth(),iWidth );
	iWidth = min( (int)m_fMaxLimitRowSize, iWidth );

	int iColumn = 0;
	if ( !m_szTitleVec.empty() )
		iColumn += (int)m_szTitleVec.size();	//타이틀 행 길이
	if ( !m_szSubTitleVec.empty() )
		iColumn += (int)m_szSubTitleVec.size();	//서브 타이틀 행 길이
	if ( !m_szAttainProgression.IsEmpty() )
		iColumn++;	//진행상황( 한줄)
	if ( !m_szPresent.IsEmpty() )
		iColumn++;	//보상받기( 한줄)
	int iHeight = iColumn * LINE_SIZE;

	SetSize( iWidth + INCREASE_WIDTH, iHeight + INCREASE_HEIGHT );
	SetWndPos( iXPos + 8, iYPos + 8 );

	return true;
}

void MissionToolTip::OnProcess( float fTimePerSec )
{
	if ( IsShow() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( !pMouse ) return;
		pMouse->SetToolTipCursor();
		int iXPos = 0;
		int iYPos = 0;
		Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
		SetScreenWndPos( iXPos, iYPos );
		GoToTop();
	}
}

void MissionToolTip::OnRender()
{
	ioWnd::OnRender();
	enum
	{
		TEXT_OFFSET_X			= 7,
		TEXT_OFFSET_Y			= 6,
		TEXT_LINE_GAP			= 16,
	};

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	
	int iColumn = 0;
	for ( int iLine = 0 ; iLine<(int)m_szTitleVec.size() ; ++iLine )
		g_FontMgr.PrintText( iXPos + TEXT_OFFSET_X , iYPos + TEXT_OFFSET_Y + (TEXT_LINE_GAP * iColumn++ ), FONT_SIZE_12, m_szTitleVec[iLine].c_str() );

	if ( !m_szAttainProgression.IsEmpty() )
		m_szAttainProgression.PrintFullText( iXPos + TEXT_OFFSET_X, iYPos + TEXT_OFFSET_Y + (TEXT_LINE_GAP * iColumn++ ), TAT_LEFT );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	for ( int iLine = 0 ; iLine<(int)m_szSubTitleVec.size() ; ++iLine )
		g_FontMgr.PrintText( iXPos + TEXT_OFFSET_X , iYPos + TEXT_OFFSET_Y + (TEXT_LINE_GAP * iColumn++ ), FONT_SIZE_12, m_szSubTitleVec[iLine].c_str() );

	if( !m_szPresent.IsEmpty() )
		m_szPresent.PrintFullText( iXPos + TEXT_OFFSET_X, iYPos + TEXT_OFFSET_Y + (TEXT_LINE_GAP * iColumn++ ), TAT_LEFT );
}
//////////////////////////////////////////////////////////////////////////////////
MissionRadioBtn::MissionRadioBtn():
m_bUseSparkle( false ),
m_bOver( false ),
m_dwOverTimer( 0 ),
m_dwOverTicTime( 0 ),
m_byOverFrmPrintCount( 0 )
{
}

MissionRadioBtn::~MissionRadioBtn()
{
}

void MissionRadioBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioRadioButton::ParseExtraInfo( xElement );
	m_dwOverTicTime = xElement.GetIntAttribute_e( "OverTicTime" );
	m_byOverFrmPrintCount = (BYTE)xElement.GetIntAttribute_e( "OverFrmPrintCount" );
}

void MissionRadioBtn::SetStartSparkle()
{
	m_bUseSparkle = true;
	m_bOver = false;
	m_dwOverTimer = FRAMEGETTIME();
}

void MissionRadioBtn::SetEndSparkle()
{
	m_bUseSparkle = false;
	m_bOver = false;
}

void MissionRadioBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioButton::OnProcess( fTimePerSec );

	if ( m_bUseSparkle )
	{
		if( FRAMEGETTIME() - m_dwOverTimer > m_dwOverTicTime )
		{
			m_dwOverTimer = FRAMEGETTIME();
			m_bOver = !m_bOver;
		}
	}
}

void MissionRadioBtn::OnRender()
{
	ioRadioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_bOver )
	{
		for ( int i=0 ; i<(int)m_byOverFrmPrintCount ; ++i )
		{
			OnDrawOveredAdd( iXPos, iYPos );
		}
	}
}