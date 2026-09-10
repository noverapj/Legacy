#include "StdAfx.h"
#include "RollBookWnd.h"

#include "../ioDateHelp.h"
#include "../ioDecorationPrice.h"
#include "../ioPlayStage.h"
#include "../ioLadderTeamMgr.h"
#include "../ioBattleRoomMgr.h"
#include "../ioUserSpiritManager.h"

CRollBookRewardInfo::CRollBookRewardInfo():
m_iValue1( 0 ),
m_iValue2( 0 ),
m_iPresentType( 0 ),
m_bHidden( false ),
m_bReceive( false ),
m_bEmphasize( false ),
m_pPresentIcon( NULL ),
m_pSubIcon( NULL )
{
	m_szPresentName.Clear();
}

CRollBookRewardInfo::~CRollBookRewardInfo()
{
	SAFEDELETE( m_pPresentIcon );
	SAFEDELETE( m_pSubIcon );
}

bool CRollBookRewardInfo::Init( ioINILoader& rkLoader, const int& iRewardNum, const int& iLastDay )
{
	char szBuf[MAX_PATH] = "";
	char szValue[MAX_PATH] = "";

	//Load Reward Value
	sprintf_s_e( szBuf, "reward%d_values", iRewardNum + 1 );
	rkLoader.LoadString( szBuf, "", szValue, MAX_PATH );
	StringVector kString = ioStringConverter::Split( szValue, "." );
	for ( int i=0 ; i<(int)kString.size() ; ++i )
	{
		if ( i== 0 )
			m_iPresentType =atoi( kString[i].c_str() );
		if ( i== 1 )
			m_iValue1 =atoi( kString[i].c_str() );
		if ( i== 2 )
			m_iValue2 =atoi( kString[i].c_str() );
	}
	if ( IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Reward Index(%d) Values is empty",__FUNCTION__ , iRewardNum + 1 );
		return false;
	}

	//Load Icon
	m_pPresentIcon = GetRewardIconByValues( m_iPresentType, m_iValue1, m_iValue2 );
	if ( m_pPresentIcon )
		m_pPresentIcon->SetScale( 0.7f, 0.7f );
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Present Name is Not Exist : Index(%d) ",__FUNCTION__ , iRewardNum + 1 );
	}

	m_pSubIcon = GetRewardSubIconByValues( m_iPresentType, m_iValue1, m_iValue2 );
	if( m_pSubIcon )
		m_pSubIcon->SetScale( 0.7f, 0.7f );

	//Load Reward Text
	ZeroMemory( szBuf, sizeof( szBuf ) );
	sprintf_s_e( szBuf, "%s %s", g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iValue1, m_iValue2 ).c_str(),
		g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iValue1, m_iValue2 ).c_str() );
	m_szPresentName = szBuf;

	//Load Is Hidden
	sprintf_s_e( szBuf, "reward%d_hidden", iRewardNum + 1 );
	m_bHidden = rkLoader.LoadBool( szBuf, false );

	//Load Is emphasize
	sprintf_s_e( szBuf, "reward%d_emphasize", iRewardNum + 1 );
	m_bEmphasize = rkLoader.LoadBool( szBuf, false );

	//Set Recieve
	if ( iRewardNum <= iLastDay )
		m_bReceive = true;

	// Set Pos
	m_iIconPosX = ICON_ROW_X + ICON_ROW_X_OFFSET * ( iRewardNum % MAX_ROW );
	m_iIconPosY = ICON_COLUMN_Y + ICON_COLUMN_Y_OFFSET * ( iRewardNum / MAX_ROW );

	return true;
}

ioUIRenderImage* CRollBookRewardInfo::GetRewardIconByValues( const int& iPresentType, const int& iPresentValue1, const int& iPresentValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentIconText( iPresentType, iPresentValue1, iPresentValue2 );
	if ( !szIconName.IsEmpty() )
		return g_UIImageSetMgr.CreateImageByFullName( szIconName );

	return NULL;
}

ioUIRenderImage* CRollBookRewardInfo::GetRewardSubIconByValues( const int& iPresentType, const int& iPresentValue1, const int& iPresentValue2 )
{
	ioHashString szIconName = g_PresentMgr.GetPresentSubIconText( iPresentType, iPresentValue1, iPresentValue2 );
	if ( !szIconName.IsEmpty() )
		return g_UIImageSetMgr.CreateImageByFullName( szIconName );

	return NULL;
}

bool CRollBookRewardInfo::IsEmpty() const
{
	if( m_iValue1 == 0 && m_iValue2 == 0 && m_iPresentType == 0 )
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
CTime RollBookWnd::m_RequestDateTime = CTime( 0 );
ModeType RollBookWnd::m_CurModeType = MT_NONE;
int RollBookWnd::m_TableIndex = 0;
int RollBookWnd::m_TablePosition = 0;
bool RollBookWnd::m_bBattleRoomCheck = false;
bool RollBookWnd::m_bLadderRoomCheck = false;
bool RollBookWnd::m_bDirectlyShow	 = false;


RollBookWnd::RollBookWnd(void) :
m_pRewardIconBG( NULL ),
m_pGainIcon( NULL ),
m_pCostumeMark( NULL ),
//m_pPlayStage( NULL ),
m_pCurFrm( NULL ),
m_pNormalUserTitleFrm( NULL ),
m_pReturnUserTitleFrm( NULL ),
m_pNewUserTitleFrm( NULL ),
m_pEmphasizeEffect( NULL ),
m_pHidden( NULL ),
m_iTableType( RBT_NONE ),
m_iOverTooltipIndex( -1 ),
m_bRequestTime( false ),
m_dwProcessCheckTime( 0 ),
m_dwProcessCheckTic( 0 ),

m_dwEmphasizeEffectStartTime( 0 ),
m_dwEmphasizeEffectDuration( 0 ),
m_fCurEmphasizeEffectAlpha( 0 ),
m_fEmphasizeEffectAlphaSpeed( 0 ),
m_fCurEmphasizeEffectScale( 0.0f),
m_fEmphasizeEffectScaleSpeed( 0.0f ),

m_iTodayRewardIndex( -1 ),
m_fTodayRewardStartScale( 0.0f ),
m_fCurTodayRewardScale( 0.0f ),
m_fTodayRewardScaleSpeed( 0.0f )
{
	ClearList();
	m_szRewardSnd.Clear();
}

RollBookWnd::~RollBookWnd(void)
{
	ClearList();
	SAFEDELETE( m_pRewardIconBG );
	SAFEDELETE( m_pGainIcon );
	SAFEDELETE( m_pCostumeMark );

	SAFEDELETE( m_pNormalUserTitleFrm );
	SAFEDELETE( m_pReturnUserTitleFrm );
	SAFEDELETE( m_pNewUserTitleFrm );
	SAFEDELETE( m_pEmphasizeEffect );
	SAFEDELETE( m_pHidden );

}

void RollBookWnd::ClearList()
{
	for ( int i=0; i<(int)m_CurRollBookRewardList.size(); ++i )
		SAFEDELETE( m_CurRollBookRewardList[i] );
	m_CurRollBookRewardList.clear();

	for ( int i=0; i<MAX_SUB_TITLE ; ++i )
		m_SubText[i].ClearList();
	m_Title.ClearList();

	m_iTableType = RBT_NONE;
	m_iTodayRewardIndex = -1;
}

void RollBookWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "RewardIconBG" )
	{
		SAFEDELETE( m_pRewardIconBG );
		m_pRewardIconBG = pImage;

		if( m_pRewardIconBG )
			m_pRewardIconBG->SetScale( 0.7f, 0.7f );
	}
	else if( szType == "GainIcon" )
	{
		SAFEDELETE( m_pGainIcon );
		m_pGainIcon = pImage;
	}
	else if ( szType == "EmphasizeEffect")
	{
		SAFEDELETE( m_pEmphasizeEffect );
		m_pEmphasizeEffect = pImage;
	}
	else if ( szType == "HiddenIcon" )
	{
		SAFEDELETE( m_pHidden );
		m_pHidden = pImage;
		if ( m_pHidden )
			m_pHidden->SetScale( 0.7f );
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
		if ( m_pCostumeMark )
			m_pCostumeMark->SetScale( 0.7f );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RollBookWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "NormalTitle" )
		{
			SAFEDELETE( m_pNormalUserTitleFrm );
			m_pNormalUserTitleFrm = pFrame;
		}
		else if( szType == "ReturnTitle" )
		{
			SAFEDELETE( m_pReturnUserTitleFrm );
			m_pReturnUserTitleFrm = pFrame;
		}
		else if( szType == "NewTitle" )
		{
			SAFEDELETE( m_pNewUserTitleFrm );
			m_pNewUserTitleFrm = pFrame;
		}
		else
			ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void RollBookWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );
	m_fTodayRewardStartScale = xElement.GetFloatAttribute_e( "TodayRewardStartScale" );
	m_fTodayRewardScaleSpeed = xElement.GetFloatAttribute_e( "TodayRewardScaleSpeed" );

	m_dwEmphasizeEffectDuration = (DWORD)xElement.GetIntAttribute_e( "EmphasizeEffectDuration" );
	m_fEmphasizeEffectAlphaSpeed = xElement.GetFloatAttribute_e( "EmphasizeEffectAlphaSpeed" );
	m_fEmphasizeEffectScaleSpeed = xElement.GetFloatAttribute_e( "EmphasizeEffectScaleSpeed" );

	m_szRewardSnd = xElement.GetStringAttribute_e( "Reward" );
}

void RollBookWnd::iwm_show()
{
	ErrorReport::SetPosition( 6272, 15 );
	if ( m_CurRollBookRewardList.empty() )
	{
		ioINILoader_e kLoader( "config/sp2_roll_book.ini" );
		LoadTableDataToINI( kLoader, m_TableIndex, m_TablePosition );
	}
	ErrorReport::SetPosition( 6272, 16 );

	//EmphasizeEffect Setting
	m_dwEmphasizeEffectStartTime = FRAMEGETTIME();
	m_fCurEmphasizeEffectAlpha = MAX_ALPHA_RATE;
	m_fCurEmphasizeEffectScale = FLOAT1;

	ErrorReport::SetPosition( 6272, 17 );
	RollBookToolTip *pToolTip = dynamic_cast<RollBookToolTip*>( FindChildWnd( ID_TOOL_TIP ) );
	if ( pToolTip && !pToolTip->IsShow() )
	{
		pToolTip->SetSize( 0, 0 );
		pToolTip->ShowWnd();
	}

	if ( !HasWndStyle( IWS_MODAL ) && !HasWndStyle( IWS_CANMOVE ) )
		AddWndStyle( IWS_CANMOVE );
	ErrorReport::SetPosition( 6272, 18 );
}

void RollBookWnd::iwm_hide()
{
	if ( HasWndStyle( IWS_MODAL) )
		RemoveWndStyle( IWS_MODAL );

	m_dwEmphasizeEffectStartTime = 0;
	m_iTodayRewardIndex = -1;
	m_iOverTooltipIndex = -1;
	RollBookToolTip *pToolTip = dynamic_cast<RollBookToolTip*>( FindChildWnd( ID_TOOL_TIP ) );
	if ( pToolTip && pToolTip->IsShow() )
		pToolTip->HideWnd();
}

void RollBookWnd::SettingRollBookByPacket( DWORD dwRecieveTime, int iState, int iTableIndex, int iCurPosition )
{
	ErrorReport::SetPosition( 6272, 0 );
	ClearList();
	if ( m_pPlayStage )
		m_CurModeType = m_pPlayStage->GetModeType();

	m_TableIndex = iTableIndex;
	//서버에서 주는 Position값은 1~28, 클라는 0~27로 세팅되어 있음.
	m_TablePosition = max( 0, iCurPosition - 1 ) ;
	
	ioINILoader_e kLoader( "config/sp2_roll_book.ini" );
	ErrorReport::SetPosition( 6272, 1 );
	LoadRenewalTimeToINI( dwRecieveTime, kLoader );
	ErrorReport::SetPosition( 6272, 2 );
	LoadTableDataToINI( kLoader, m_TableIndex, m_TablePosition );
	ErrorReport::SetPosition( 6272, 12 );

	if ( iState == ROLLBOOK_ATTEND_OK )
	{
		m_iTodayRewardIndex = m_TablePosition;
		m_fCurTodayRewardScale = m_fTodayRewardStartScale;
		AddWndStyle( IWS_MODAL ); 
		ErrorReport::SetPosition( 6272, 13 );
		if ( !IsShow() )
		{
			ErrorReport::SetPosition( 6272, 14 );
			ShowWnd();
			ErrorReport::SetPosition( 6272, 19 );
		}
	}
	m_bRequestTime = false;
}

void RollBookWnd::LoadRenewalTimeToINI( DWORD dwRecieveTime, ioINILoader& rkLoader )
{
	rkLoader.SetTitle( "common" );
	int iRenewalHour = rkLoader.LoadInt_e( "renewal_hour", 0 );

	CTime RecieveTime = CTime( dwRecieveTime );
	CTime CheckRenewalTime( RecieveTime.GetYear(), RecieveTime.GetMonth(), RecieveTime.GetDay(), iRenewalHour, 0, 0 );
	CTimeSpan Gap = RecieveTime - CheckRenewalTime;
	LONGLONG iSpan = Gap.GetTimeSpan();
	if ( iSpan < 0 )
		m_RequestDateTime = CTime( RecieveTime.GetYear(), RecieveTime.GetMonth(), RecieveTime.GetDay(), iRenewalHour, 1, 0 );
	else
	{
		int iYear = RecieveTime.GetYear();
		int iMonth = RecieveTime.GetMonth();
		int iDay = RecieveTime.GetDay();
		DateHelp::IncreaseDay( iYear, iMonth, iDay );
		m_RequestDateTime = CTime( iYear, iMonth, iDay, iRenewalHour, 1, 0 );
	}
}

void RollBookWnd::LoadTableDataToINI( ioINILoader& rkLoader, const int& iCurTableIndex, const int& iLastTablePosision )
{
	ErrorReport::SetPosition( 6272, 3 );
	rkLoader.SetTitle( "common" );
	int iMaxTable = rkLoader.LoadInt_e( "max_table", 0 );
	char szBuf[MAX_PATH] = "";
	for ( int i=1 ; i<iMaxTable+1 ; ++i )
	{
		sprintf_s( szBuf, "table%d", i );
		rkLoader.SetTitle( szBuf );
		int iTableIndex = rkLoader.LoadInt_e( "table_index", 0 );
		if( iTableIndex == iCurTableIndex )
		{
			ErrorReport::SetPosition( 6272, 4 );
			LoadTableData( rkLoader );
			ErrorReport::SetPosition( 6272, 8 );
			LoadReward( rkLoader, iLastTablePosision );
			break;
		}
	}
	ErrorReport::SetPosition( 6272, 11 );
}

void RollBookWnd::LoadTableData( ioINILoader& rkLoader )
{
	ErrorReport::SetPosition( 6272, 5 );
	m_iTableType = rkLoader.LoadInt_e( "active_type", 0 );
	LoadTitleByType( rkLoader, m_iTableType );
	ErrorReport::SetPosition( 6272, 6 );
	LoadSubTitle( rkLoader );

	RollBookCloseBtn *pBtn = dynamic_cast<RollBookCloseBtn*>( FindChildWnd( ID_CLOSE_BTN ) );
	if ( pBtn )
		pBtn->SetType( m_iTableType );
	ErrorReport::SetPosition( 6272, 7 );
}

void RollBookWnd::LoadTitleByType( ioINILoader& rkLoader, const int& iType )
{
	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "table_title", "", szBuf, MAX_PATH );
	Help::ChangeCharSet( szBuf, '#', ' ' );

	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE ); 

	DWORD iBackColor = 0;
	if ( iType == RBT_DEFAULT )
	{
		if ( m_pNormalUserTitleFrm )
			m_pCurFrm = m_pNormalUserTitleFrm;
		m_Title.SetBkColor( 75, 57, 35 );
	}
	else if ( iType == RBT_RETURN )
	{
		if ( m_pReturnUserTitleFrm )
			m_pCurFrm = m_pReturnUserTitleFrm;
		m_Title.SetBkColor( 33, 90, 162 );
	}
	else if ( iType == RBT_NEWBIE )
	{
		if ( m_pNewUserTitleFrm )
			m_pCurFrm = m_pNewUserTitleFrm;
		m_Title.SetBkColor( 170, 55, 77 );
	}
	m_Title.AddTextPiece( FONT_SIZE_17, szBuf );
}

void RollBookWnd::LoadSubTitle( ioINILoader& rkLoader )
{
	//SubTitle
	enum { MAX_SUBTITLE_LINE = 3, MAX_LINE_COLOR = 5,  };
	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	for(int iLine = 0; iLine < MAX_SUBTITLE_LINE; ++iLine )
	{
		m_SubText[iLine].SetTextStyle( TS_NORMAL );
		m_SubText[iLine].SetBkColor( 0, 0, 0 );
		for(int iSentence = 1; iSentence < MAX_LINE_COLOR+1; iSentence++)
		{
			memset(szColorKeyName, 0, sizeof(szColorKeyName));
			memset(szTextKeyName, 0, sizeof(szTextKeyName));
			memset(szColorValue, 0, sizeof(szColorValue));
			memset(szTextValue, 0, sizeof(szTextValue));

			//color, text
			StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "table_subtitle%d_color%d", iLine+1, iSentence  );
			StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "table_subtitle%d_text%d", iLine+1, iSentence );
			//color, text value
			rkLoader.LoadString(szColorKeyName, "", szColorValue, MAX_PATH);
			if( strcmp( szColorValue, "" ) == 0) 
				break;
			char *pTemp = NULL;	
			DWORD dwColor = strtoul(szColorValue, &pTemp, 16);
			m_SubText[iLine].SetTextColor( dwColor );

			rkLoader.LoadString( szTextKeyName, "", szTextValue, MAX_PATH );	
			if( strcmp( szTextValue, "" ) == 0) 
				break;

			Help::ChangeCharSet( szTextValue, '#', ' ' );

			m_SubText[iLine].AddTextPiece( FONT_SIZE_12, szTextValue );
		}
	}

	if( m_SubText[0].IsEmpty() ) 
	{
		m_SubText[0].SetTextColor( 0 );
		m_SubText[0].AddTextPiece( FONT_SIZE_12, "Untitled" );
	}
}

void RollBookWnd::LoadReward( ioINILoader& rkLoader, const int& iLastTablePosision )
{
	ErrorReport::SetPosition( 6272, 9 );
	for ( int i=0 ; i<MAX_DAY ; ++i )
	{
		CRollBookRewardInfo* pRollbook = new CRollBookRewardInfo;
		if ( pRollbook->Init( rkLoader, i, iLastTablePosision ) )
			m_CurRollBookRewardList.push_back( pRollbook );
		else
			SAFEDELETE( pRollbook );
	}
	ErrorReport::SetPosition( 6272, 10 );
}

void RollBookWnd::ShowRollBookWnd()
{
	//Table에 관련된 패킷을 받지 못했다면 UI Open을 하지 않는다.
	if ( m_TableIndex == 0 )
		return;

	// 요청 시간이라면 패킷을 보낸다.
	if ( m_bRequestTime )
	{
		m_bDirectlyShow = true;
		SP2Packet kPacket( CTPK_ROLLBOOK_RENEWAL );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
	
	ShowWnd();
}

void RollBookWnd::OnProcess( float fTimePerSec )
{
	if ( IsShow() )
	{
		//이펙트 Process
		OnEmphasizeEffectProcess( fTimePerSec );
		OnTodayRewardEffectProcess( fTimePerSec );

		//tooltip Process
		OnToolTipProcess();
	}
	else
	{
		OnRequestProcess();
	}
}

void RollBookWnd::OnToolTipProcess()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCurOverIndex = -1;
	for ( int i=0 ; i<(int)m_CurRollBookRewardList.size() ; ++i )
	{
		const CRollBookRewardInfo* const pRewardInfo = m_CurRollBookRewardList[i];
		if ( !pRewardInfo->IsEmpty() )
		{
			RECT rc;
			SetRect( &rc, iXPos + pRewardInfo->GetRewardIconPosX() - ( ICON_SIZE_X / 2 ), iYPos + pRewardInfo->GetRewardIconPosY() - ( ICON_SIZE_Y / 2 ),
					 iXPos + pRewardInfo->GetRewardIconPosX() + ( ICON_SIZE_X / 2 ) , iYPos + pRewardInfo->GetRewardIconPosY() + ( ICON_SIZE_Y / 2 ) );
			
			if( PtInRect( &rc, pMouse->GetMousePos()) )
			{
				iCurOverIndex = i;
				break;
			}
		}
	}
	
	if ( m_iOverTooltipIndex != iCurOverIndex )
	{
		m_iOverTooltipIndex = iCurOverIndex;
		RollBookToolTip *pToolTip = dynamic_cast<RollBookToolTip*>( FindChildWnd( ID_TOOL_TIP ) );
		if ( pToolTip )
		{
			if ( m_iOverTooltipIndex == -1 )
			{
				pToolTip->SetSize( 0, 0 );
			}
			else
			{
				if( COMPARE( m_iOverTooltipIndex, 0, (int)m_CurRollBookRewardList.size() ) )
				{
					const CRollBookRewardInfo* const pRewardInfo = m_CurRollBookRewardList[m_iOverTooltipIndex];
					if ( pRewardInfo )
						pToolTip->SetInfo( pRewardInfo );
				}
			}
		}
	}
}

void RollBookWnd::OnEmphasizeEffectProcess( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwEmphasizeEffectStartTime + m_dwEmphasizeEffectDuration < dwCurTime )
	{
		m_dwEmphasizeEffectStartTime = dwCurTime;
		m_fCurEmphasizeEffectAlpha = MAX_ALPHA_RATE;
		m_fCurEmphasizeEffectScale = FLOAT1;
	}
	else
	{
		m_fCurEmphasizeEffectAlpha += m_fEmphasizeEffectAlphaSpeed * fTimePerSec;
		m_fCurEmphasizeEffectScale += m_fEmphasizeEffectScaleSpeed * fTimePerSec;
		min( 255, max( 0, m_fCurEmphasizeEffectAlpha ) );
		max( 0, m_fCurEmphasizeEffectScale );
	}
}

void RollBookWnd::OnTodayRewardEffectProcess( float fTimePerSec )
{
	//IWS_EX_SHOW_HIDE_ACTION 세팅인 경우 해당 세팅 액션이 끝난 후 
	//RewardEffect 실행
	if ( m_iTodayRewardIndex == - 1 || m_dwWndAniTime != 0)
		return;

	//TodayRewardEffect End Check
	if ( m_fCurTodayRewardScale < FLOAT1 )
	{
		if( !m_szRewardSnd.IsEmpty() )
			g_SoundMgr.PlaySound( m_szRewardSnd );
		m_iTodayRewardIndex = -1;
	}
	else
		m_fCurTodayRewardScale -= m_fTodayRewardScaleSpeed * fTimePerSec;
}

void RollBookWnd::OnRequestProcess()
{
	enum { PROCESS_TIC = 30000, };
	if ( m_dwProcessCheckTime + PROCESS_TIC <= FRAMEGETTIME() )
	{
		m_dwProcessCheckTime = FRAMEGETTIME();
		if ( m_RequestDateTime.GetTime() != 0 )
		{
			SYSTEMTIME rkServerTime;
			g_MyInfo.GetServerTime( rkServerTime );
			CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
			//다음 갱신 시간 - 현재 시간 <=0 이면 갱신 요청
			//Fail 패킷이 온 경우 재 요청
			CTimeSpan TimeGap = m_RequestDateTime - ServerTime;
			LONGLONG iSpan = TimeGap.GetTimeSpan();
			if ( iSpan <= 0 )
				m_bRequestTime = true;
		}
	}

	// 자동 요청 패킷
	//현재 요청 가능한 시간이고
	if ( m_bRequestTime && m_pPlayStage )
	{
		// PlayStage가 바뀔 때마다 체크
		if ( m_CurModeType != m_pPlayStage->GetModeType() && m_CurModeType != MT_NONE 
			&& m_pPlayStage->GetModeType() != MT_NONE )
		{
			m_CurModeType = m_pPlayStage->GetModeType();
			// 바뀐 모드가 로비일 경우
			if ( m_CurModeType == MT_MYROOM )
			{
				m_bDirectlyShow = false;
				SP2Packet kPacket( CTPK_ROLLBOOK_RENEWAL );
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
		}
		// 현재 로비이고 전투,레더방에서 나온경우 요청
		else if ( m_CurModeType == MT_MYROOM )
		{
			bool bCheckLeaveBattleRoom = false;
			bool bCheckLeaveLadderRoom = false;

			if ( g_BattleRoomMgr.IsBattleRoom() )
				m_bBattleRoomCheck = true;
			else if ( m_bBattleRoomCheck )
			{
				m_bBattleRoomCheck = false;
				bCheckLeaveBattleRoom = true;
			}

			if ( g_LadderTeamMgr.IsLadderTeam() )
				m_bLadderRoomCheck = true;
			else if ( m_bLadderRoomCheck )
			{
				m_bLadderRoomCheck = false;
				bCheckLeaveLadderRoom = true;
			}

			if ( bCheckLeaveBattleRoom || bCheckLeaveLadderRoom )
			{
				m_bDirectlyShow = false;
				SP2Packet kPacket( CTPK_ROLLBOOK_RENEWAL );
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
		}
	}
}

void RollBookWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_CLOSE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

bool RollBookWnd::iwm_spacebar()
{
	iwm_command( FindChildWnd( ID_CLOSE_BTN ), IOBN_BTNUP, 0 );
	return true;
}

void RollBookWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { LINE = 16 };

	//Title
	if ( m_pCurFrm )
		m_pCurFrm->Render( iXPos + 5, iYPos + 6 );

	if ( !m_Title.IsEmpty() )
		m_Title.PrintFullText( iXPos + 314 , iYPos + 11 , TAT_CENTER );

	//SubTitle
	for ( int i=0; i<MAX_SUB_TITLE ; ++i )
	{
		if ( !m_SubText[i].IsEmpty() )
			m_SubText[i].PrintFullText( iXPos + 24 , iYPos + 376 + LINE*i, TAT_LEFT );
	}
	
	OnRewardInfoRender( iXPos, iYPos );
}

void RollBookWnd::OnRewardInfoRender( int iXPos, int iYPos )
{
	enum
	{
		TEXT_POS_X = 31,
		TEXT_POS_Y = 57,
		TEXT_OFFSET_X = 82,
		TEXT_OFFSET_Y = 77,
		MAX_ROW = 7,
	};
	for ( int i=0 ; i<(int)m_CurRollBookRewardList.size() ; ++i )
	{
		const CRollBookRewardInfo* const pRewardInfo= m_CurRollBookRewardList[i];
		if ( !pRewardInfo->IsEmpty() )
		{
			//Print Number
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( 170, 88, 58 );
			g_FontMgr.PrintText( iXPos + TEXT_POS_X + TEXT_OFFSET_X * ( i % MAX_ROW ),
								 iYPos + 4 + TEXT_POS_Y + TEXT_OFFSET_Y * ( i / MAX_ROW ), FONT_SIZE_12, "%d", i+1 );

			//Render Icon
			ioUIRenderImage* pIcon = pRewardInfo->GetPresentIcon();
			ioUIRenderImage* pSubIcon = pRewardInfo->GetPresentSubIcon();
			if ( pIcon )
			{
				int iIconXPos = iXPos + pRewardInfo->GetRewardIconPosX();
				int iIconYPos = iYPos + pRewardInfo->GetRewardIconPosY();

				//Emphasize Icon Render
				if ( pRewardInfo->GetEmphasize() && !pRewardInfo->GetReceive() )
				{
					if ( m_pEmphasizeEffect )
					{
						m_pEmphasizeEffect->SetAlpha( (BYTE)m_fCurEmphasizeEffectAlpha );
						m_pEmphasizeEffect->SetScale( m_fCurEmphasizeEffectScale );
						m_pEmphasizeEffect->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
					}
				}

				if( m_pRewardIconBG )
					m_pRewardIconBG->Render( iIconXPos, iIconYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

				if ( i == m_iOverTooltipIndex )
				{
					if( m_pRewardIconBG )
						m_pRewardIconBG->Render( iIconXPos, iIconYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
				}

				//Aleard Received
				if ( pRewardInfo->GetReceive() )
				{
					if ( m_iTodayRewardIndex == i )
					{
						pIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						if( pSubIcon )
							pSubIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						if ( m_pGainIcon)
						{
							m_pGainIcon->SetScale( m_fCurTodayRewardScale );
							m_pGainIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						}
					}
					else
					{
						pIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						if( pSubIcon )
							pSubIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						if ( m_pGainIcon)
						{
							m_pGainIcon->SetScale( FLOAT1 );
							m_pGainIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						}
					}
						
				}
				else
				{
					// Hidden Item
					if ( pRewardInfo->GetHidden() )
					{
						if ( m_pHidden )
						{
							m_pHidden->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
							if ( i == m_iOverTooltipIndex )
								m_pHidden->Render( iIconXPos, iIconYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
						}
					}
					else
					{
						pIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
						if ( i == m_iOverTooltipIndex )
							pIcon->Render( iIconXPos, iIconYPos, UI_RENDER_SCREEN, TFO_BILINEAR );

						if( pSubIcon )
						{
							pSubIcon->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
							if ( i == m_iOverTooltipIndex )
								pSubIcon->Render( iIconXPos, iIconYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
						}
						if( pRewardInfo->GetPresentType() == PRESENT_COSTUME && m_pCostumeMark )
						{
							m_pCostumeMark->Render( iIconXPos, iIconYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
							if ( i == m_iOverTooltipIndex )
								m_pCostumeMark->Render( iIconXPos, iIconYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RollBookCloseBtn::RollBookCloseBtn() :
m_pCurBackFrm( NULL ),
m_pNormalUserBackFrm( NULL ),
m_pReturnUserBackFrm( NULL ),
m_pNewUserBackFrm( NULL ),
m_pCurDownFrm( NULL ),
m_pNormalUserDownFrm( NULL ),
m_pReturnUserDownFrm( NULL ),
m_pNewUserDownFrm( NULL )
{

}

RollBookCloseBtn::~RollBookCloseBtn()
{
	SAFEDELETE( m_pNormalUserBackFrm );
	SAFEDELETE( m_pReturnUserBackFrm );
	SAFEDELETE( m_pNewUserBackFrm );

	SAFEDELETE( m_pNormalUserDownFrm );
	SAFEDELETE( m_pReturnUserDownFrm );
	SAFEDELETE( m_pNewUserDownFrm );
}

void RollBookCloseBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "NormalBack" )
		{
			SAFEDELETE( m_pNormalUserBackFrm );
			m_pNormalUserBackFrm = pFrame;
		}
		else if( szType == "ReturnBack" )
		{
			SAFEDELETE( m_pReturnUserBackFrm );
			m_pReturnUserBackFrm = pFrame;
		}
		else if( szType == "NewBack" )
		{
			SAFEDELETE( m_pNewUserBackFrm );
			m_pNewUserBackFrm = pFrame;
		}
		else if( szType == "NormalDown" )
		{
			SAFEDELETE( m_pNormalUserDownFrm );
			m_pNormalUserDownFrm = pFrame;
		}
		else if( szType == "ReturnDown" )
		{
			SAFEDELETE( m_pReturnUserDownFrm );
			m_pReturnUserDownFrm = pFrame;
		}
		else if( szType == "NewDown" )
		{
			SAFEDELETE( m_pNewUserDownFrm );
			m_pNewUserDownFrm = pFrame;
		}
		else
			ioButton::AddRenderFrame( szType, pFrame );
	}
}

void RollBookCloseBtn::SetType( const int& iType )
{
	if ( iType == RollBookWnd::RBT_DEFAULT )
	{
		if ( m_pNormalUserBackFrm )
			m_pCurBackFrm = m_pNormalUserBackFrm;

		if ( m_pNormalUserDownFrm )
			m_pCurDownFrm = m_pNormalUserDownFrm;

		SetTitleColor( TCT_DEFAULT_WHITE, D3DCOLOR_XRGB( 75, 57, 35 ) );
	}
	else if ( iType == RollBookWnd::RBT_RETURN)
	{
		if ( m_pReturnUserBackFrm )
			m_pCurBackFrm = m_pReturnUserBackFrm;

		if ( m_pReturnUserDownFrm )
			m_pCurDownFrm = m_pReturnUserDownFrm;

		SetTitleColor( TCT_DEFAULT_WHITE, D3DCOLOR_XRGB( 33, 90, 162 ) );
	}
	else if ( iType == RollBookWnd::RBT_NEWBIE )
	{
		if ( m_pNewUserBackFrm )
			m_pCurBackFrm = m_pNewUserBackFrm;

		if ( m_pNewUserDownFrm )
			m_pCurDownFrm = m_pNewUserDownFrm;

		SetTitleColor( TCT_DEFAULT_WHITE, D3DCOLOR_XRGB( 170, 55, 77 ) );
	}
}

void RollBookCloseBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_pCurBackFrm )
		m_pCurBackFrm->Render( iXPos, iYPos );

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void RollBookCloseBtn::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_pCurBackFrm )
		m_pCurBackFrm->Render( iXPos, iYPos );
	OnDrawOveredAdd( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void RollBookCloseBtn::OnDrawPushed( int iXPos, int iYPos )
{
	if( m_pCurDownFrm )
		m_pCurDownFrm->Render( iXPos, iYPos );
	RenderImageList( iXPos, iYPos, UI_RENDER_SCREEN );
}

/////////////////////////////////////////////////////////////////////////////////////
RollBookToolTip::RollBookToolTip()
{
	m_szPresentName.ClearList();
	m_szRewardInfoOverSnd.Clear();
}

RollBookToolTip::~RollBookToolTip()
{
}

void RollBookToolTip::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );
	m_szRewardInfoOverSnd = xElement.GetStringAttribute_e( "RewardInfoOver" );
}

void RollBookToolTip::OnProcess( float fTimePerSec )
{
	if ( IsShow() )
	{
		if ( GetWidth() != 0 && GetHeight() != 0 )
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
}

void RollBookToolTip::SetInfo( const CRollBookRewardInfo* const pRewardInfo )
{
	if ( pRewardInfo == NULL )
	{
		SetSize( 0, 0 );
		return;
	}
	else if ( pRewardInfo->GetRewardName().IsEmpty() )
	{
		SetSize( 0, 0 );
		return;
	}

	m_szPresentName.ClearList();
	m_szPresentName.SetTextStyle( TS_NORMAL );
	m_szPresentName.SetBkColor( 0, 0, 0 );	
	m_szPresentName.SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색

	ioHashString szText1 = "???";
	if ( !pRewardInfo->GetHidden() || pRewardInfo->GetReceive() )
		szText1 = pRewardInfo->GetRewardName();
	m_szPresentName.AddTextPiece( FONT_SIZE_12, "%s", szText1.c_str() );

	int iMaxWidth  = m_szPresentName.GetFullWidth() + SIDE_TEXT_GAP;
	int iMaxHeight = MIN_HEIGHT;
	SetSize( iMaxWidth, iMaxHeight );
	//ShowWnd();

	if( !m_szRewardInfoOverSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRewardInfoOverSnd );
}

void RollBookToolTip::OnRender()
{
	if ( GetWidth() == 0 && GetHeight() == 0 )
		return;
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;
	if( !m_szPresentName.IsEmpty() )
		m_szPresentName.PrintFullText( iXPos, iYPos, TAT_LEFT );
}