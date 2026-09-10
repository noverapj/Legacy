#include "stdafx.h"

#include "ioEventUserNode.h"

#include "GUI/ioMemoManager.h"
#include "GUI/EventWnd.h"

EventUserNode::EventUserNode()
{
	m_eEventType = EVT_NONE;
	m_ValueVec.clear();
}

EventUserNode::EventUserNode( EventType eEventType )
{
	m_eEventType = eEventType;
	m_ValueVec.clear();
}
EventUserNode::~EventUserNode()
{
	m_ValueVec.clear();
}

void EventUserNode::SetSize( int iSize )
{
	m_ValueVec.clear();

	for (int i = 0; i < iSize ; i++)
	{
		int iValue = 0;
		m_ValueVec.push_back( iValue );
	}
}

int EventUserNode::GetValue( int iArray )
{
	if( !COMPARE(iArray, 0, (int) m_ValueVec.size()) )
		return 0;

	return m_ValueVec[iArray];
}

void EventUserNode::SetValue( int iArray, int iValue )
{
	if( COMPARE( iArray, 0, (int) m_ValueVec.size() ) )
		m_ValueVec[iArray] = iValue;
}

EventType EventUserNode::GetType() const
{
	return m_eEventType;
}

int EventUserNode::GetAddSize()
{
	return 0;
}

void EventUserNode::Init()
{
}

int EventUserNode::GetSize() const
{
	return m_ValueVec.size();
}

bool EventUserNode::IsEmptyValue()
{
	return false;
}

void EventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
}

void EventUserNode::SetType( EventType eEventType )
{
	m_eEventType = eEventType;
}
//---------------------------------------------------------------------------------------------------------------------------------
ProposalEventUserNode::ProposalEventUserNode()
{
}

ProposalEventUserNode::~ProposalEventUserNode()
{
}

bool ProposalEventUserNode::IsGiveChar()
{
	if( !g_EventMgr.IsAlive( GetType(), g_MyInfo.GetChannelingType() ) )
		return false;

	if( GetValue( VA_PROPOSAL_CNT ) >= g_EventMgr.GetValue( GetType(), EA_MAX_PROPOSAL_CHAR ) )
	{
		if( GetValue( VA_GAVE_CHAR ) < g_EventMgr.GetValue( GetType(), EA_MAX_CHAR_DAY ) )
			return true;
	}

	return false;
}

void ProposalEventUserNode::SetValueGiveChar()
{
	int iCnt = GetValue( VA_PROPOSAL_CNT );
	iCnt -= g_EventMgr.GetValue( GetType(), EA_MAX_PROPOSAL_CHAR );
	if( iCnt < 0 )
		iCnt = 0;
	SetValue( VA_PROPOSAL_CNT , iCnt );

	int iCharCnt = GetValue( VA_GAVE_CHAR );
	iCharCnt++;

	if( iCharCnt >= g_EventMgr.GetValue( GetType(), EA_MAX_CHAR_DAY ) )
	{
		SetValue( VA_PROPOSAL_CNT, 0 );
	}

	SetValue(VA_GAVE_CHAR, iCharCnt );
}

int ProposalEventUserNode::GetAddSize()
{
	return ADD_PROPOSAL_SIZE;
}

void ProposalEventUserNode::Init()
{
	SetValue( VA_ADD_PROPOSAL_CNT, 0 );
	SetValue( VA_ADD_GAVE_CHAR_CNT, 0 );
}
//----------------------------------------------------------------------------------------------------------------------------------
CoinEventUserNode::CoinEventUserNode()
{
}

CoinEventUserNode::~CoinEventUserNode()
{
}

void CoinEventUserNode::Init()
{
	SetValue( VA_ADD_COIN_CNT, 0 );
	SetValue( VA_PLAY_SEC, 0 );
}

int CoinEventUserNode::GetAddSize()
{
	return ADD_COIN_SIZE;
}

void CoinEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iCoinType  = 0;
	int iCoin      = 0;
	int iTotalCoin = 0;
	rkPacket >> iCoinType;
	rkPacket >> iCoin;
	rkPacket >> iTotalCoin;

	enum { MAX_COIN_TYPE = 3, };
	char szCoinType[MAX_COIN_TYPE][MAX_PATH]={"","",""};
	SafeSprintf( szCoinType[0], sizeof( szCoinType[0] ), STR(1) ); // loop를 돌수 있지만 스트링추출을 편하게 하기 위해서 한라인씩 표시함.
	SafeSprintf( szCoinType[1], sizeof( szCoinType[1] ), STR(2) );
	SafeSprintf( szCoinType[2], sizeof( szCoinType[2] ), STR(3) );

	int iArray = 0;
	if( iCoinType == EA_NORMAL_GIVE_CNT )
		iArray = 0;
	else if( iCoinType == EA_BURNING_GIVE_CNT )
		iArray = 1;
	else if( iCoinType == EA_GRADEUP_GIVE_CNT )
		iArray = 2;
	char szHelp[MAX_PATH*2]="";
	SafeSprintf( szHelp, sizeof( szHelp ), STR(4), iCoin, iTotalCoin,szCoinType[iArray]);
	// chat
	g_ChatMgr.SetEventMsg( szHelp );

	// memo
	SYSTEMTIME st;
	GetLocalTime( &st );
	DWORD dwTime = (st.wMonth * 1000000) + (st.wDay * 10000) + (st.wHour * 100) + st.wMinute;
	SP2Packet kPacket( STPK_MEMO_SEND_MSG );
	kPacket << STR(5) << szHelp << dwTime << true; 
	// SP2Packet 의 m_currentPos을 되감기 위해서
	SP2Packet kMovePacket( (char*)kPacket.GetBuffer(), kPacket.GetBufferSize() );
	g_MemoManager.OnlineMemo( kMovePacket );

	// emoticon
	if( pPlayStage )
	{
		ioBaseChar *pOwner = pPlayStage->GetOwnerChar();
		if( pOwner )
			pOwner->SetCoinEventEmoticon();
	}
	LOG.PrintTimeAndLog( 0, "CoinEventUserNode::OnRecievePacket: %d:%d:%d", iCoin, iCoinType, iTotalCoin );
}
//----------------------------------------------------------------------------------------------------------------------------
ExpEventUserNode::ExpEventUserNode()
{
}

ExpEventUserNode::~ExpEventUserNode()
{
}

bool ExpEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//--------------------------------------------------------------------------------------------------------------------------------
PesoEventUserNode::PesoEventUserNode()
{
}

PesoEventUserNode::~PesoEventUserNode()
{
}

bool PesoEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}

//--------------------------------------------------------------------------------------------------------------------------------

FishingEventUserNode::FishingEventUserNode()
{
}

FishingEventUserNode::~FishingEventUserNode()
{
}

bool FishingEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//---------------------------------------------------------------------------------------------------------------------------------
PlayTimeEventUserNode::PlayTimeEventUserNode()
{
}

PlayTimeEventUserNode::~PlayTimeEventUserNode()
{
}

bool PlayTimeEventUserNode::IsGift( GiftType eType )
{
	if( !g_EventMgr.IsAlive( GetType(), g_MyInfo.GetChannelingType() ) )
		return false;

	int iArray = 0;
	if( eType == GT_1_PESO )
	{
		iArray = EA_PASS_PLAYTIME_1;
	}
	else if( eType == GT_2_PESO )
	{
		iArray = EA_PASS_PLAYTIME_2;
	}
	else if( eType == GT_3_SOLDIER )
	{
		iArray = EA_PASS_PLAYTIME_3;
	}
	else if( eType == GT_4_SOLDIER )
	{
		iArray = EA_PASS_PLAYTIME_4;
	}
	else if( eType == GT_5_SOLDIER )
	{
		iArray = EA_PASS_PLAYTIME_5;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "PlayTimeEventUserNode::IsGift : Error Type : %d", (int) eType );
		return false;
	}

	if( !IsGiftData( eType, GS_NOTICE ) )
		return false;

	return true;
}

void PlayTimeEventUserNode::SetGift( GiftType eType )
{
	SetGiftData( eType, GS_USED );	
	LOG.PrintTimeAndLog( 0, "Set Event Value : Type %d : %d : %d",(int) GetType(), GetValue( VA_PLAYTIME_PLAY_SEC), GetValue( VA_PLAYTIME_GET_GIFT ) );
}

bool PlayTimeEventUserNode::IsGiftData( GiftType eType, GiftState eGiftState )
{
	if( eType == GT_1_PESO )
	{
		if( ( GetValue( VA_PLAYTIME_GET_GIFT ) % 10 ) == (int)eGiftState ) // XXXX[X]
			return true;
	}
	else if( eType == GT_2_PESO )
	{
		if( ( ( GetValue( VA_PLAYTIME_GET_GIFT ) % 100 ) / 10 ) == (int)eGiftState ) // XXX[X]X
			return true;
	}
	else if( eType == GT_3_SOLDIER )
	{
		if( ( ( GetValue( VA_PLAYTIME_GET_GIFT ) % 1000 ) / 100 ) == (int)eGiftState ) // XX[X]XX
			return true;
	}
	else if( eType == GT_4_SOLDIER )
	{
		if( ( ( GetValue( VA_PLAYTIME_GET_GIFT ) % 10000 ) / 1000 ) == (int)eGiftState ) // X[X]XXX
			return true;
	}
	else if( eType == GT_5_SOLDIER )
	{
		if( ( GetValue( VA_PLAYTIME_GET_GIFT ) / 10000 ) == (int)eGiftState ) // [X]XXXX
			return true;
	}
	
	return false;
}

void PlayTimeEventUserNode::SetGiftData( GiftType eGiftType , GiftState eGiftState )
{
	if( eGiftType == GT_1_PESO )
	{
		int iValue = GetValue( VA_PLAYTIME_GET_GIFT );
		int iPreValue = iValue % 10;
		iValue -= iPreValue; // 기존값을 삭제한다.
		iValue += (int)eGiftState; // XXXX[X]
		SetValue( VA_PLAYTIME_GET_GIFT, iValue );
	}
	else if( eGiftType == GT_2_PESO )
	{
		int iValue = GetValue( VA_PLAYTIME_GET_GIFT );
		int iPreValue = ( (iValue % 100 ) / 10 );
		iValue -= ( 10 * iPreValue ); // 기존값을 삭제한다.
		iValue += ( 10 * (int)eGiftState ); // XXX[X]X
		SetValue( VA_PLAYTIME_GET_GIFT, iValue );
	}
	else if( eGiftType == GT_3_SOLDIER )
	{
		int iValue = GetValue( VA_PLAYTIME_GET_GIFT );
		int iPreValue = ( ( iValue % 1000 ) / 100 );
		iValue -= ( 100 * iPreValue ); // 기존값을 삭제한다.
		iValue += ( 100 * (int)eGiftState ); // XX[X]XX
		SetValue( VA_PLAYTIME_GET_GIFT, iValue );
	}
	else if( eGiftType == GT_4_SOLDIER )
	{
		int iValue = GetValue( VA_PLAYTIME_GET_GIFT );
		int iPreValue = ( ( iValue % 10000 ) / 1000 );
		iValue -= ( 1000 * iPreValue ); // 기존값을 삭제한다.
		iValue += ( 1000 * (int)eGiftState ); // X[X]XXX
		SetValue( VA_PLAYTIME_GET_GIFT, iValue );
	}
	else if( eGiftType == GT_5_SOLDIER )
	{
		int iValue = GetValue( VA_PLAYTIME_GET_GIFT );
		int iPreValue = ( iValue / 10000 );
		iValue -= ( 10000 * iPreValue ); // 기존값을 삭제한다.
		iValue += ( 10000 * (int)eGiftState ); // [X]XXXX
		SetValue( VA_PLAYTIME_GET_GIFT, iValue );
	}
}

int PlayTimeEventUserNode::GetLimitSecond( GiftType eType )
{
	if( eType == GT_3_SOLDIER )
	{
		return g_EventMgr.GetValue( GetType(), EA_GIFT_3 );
	}
	else if( eType == GT_4_SOLDIER )
	{
		return g_EventMgr.GetValue( GetType(), EA_GIFT_4 );
	}
	else if( eType == GT_5_SOLDIER )
	{
		return g_EventMgr.GetValue( GetType(), EA_GIFT_5 );
	}

	return 0;
}

void PlayTimeEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iSubType = 0;
	rkPacket >> iSubType;
	if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_USED )
	{
		__int64 iMoney     = 0;
		int  iGetGiftValue = 0;
		bool bOffMouseBusy = false;
		int  iGiftType     = 0;
		rkPacket >> iMoney;
		rkPacket >> iGetGiftValue;
		rkPacket >> bOffMouseBusy;
		rkPacket >> iGiftType;

		if( bOffMouseBusy )
			g_App.SetMouseBusy( false );

		g_MyInfo.SetMoney( iMoney );
		SetValue( VA_PLAYTIME_GET_GIFT, iGetGiftValue );

		if( iGiftType == PlayTimeEventUserNode::GT_1_PESO )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), g_EventMgr.GetValue( EVT_PLAYTIME, EA_GIFT_1 ) );
		}
		else if( iGiftType == PlayTimeEventUserNode::GT_2_PESO )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), g_EventMgr.GetValue( EVT_PLAYTIME, EA_GIFT_2 ) );
		}
		else if( iGiftType == PlayTimeEventUserNode::GT_3_SOLDIER )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), g_EventMgr.GetValue( EVT_PLAYTIME, EA_GIFT_3 )/3600 );
		}
		else if( iGiftType == PlayTimeEventUserNode::GT_4_SOLDIER )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), g_EventMgr.GetValue( EVT_PLAYTIME, EA_GIFT_4 )/3600 );
		}
		else if( iGiftType == PlayTimeEventUserNode::GT_5_SOLDIER )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), g_EventMgr.GetValue( EVT_PLAYTIME, EA_GIFT_5 )/3600 );
		}
		else
			LOG.PrintTimeAndLog( 0, "%s Error Gift Type :%d" __FUNCTION__, iGiftType );
	}
	else if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_NOTICE )
	{
		int iPlayTimeSec = 0;
		int iGiftState   = 0;
		rkPacket >> iPlayTimeSec;
		rkPacket >> iGiftState;

		SetValue( VA_PLAYTIME_PLAY_SEC, iPlayTimeSec );
		SetValue( VA_PLAYTIME_GET_GIFT, iGiftState );

		int iPlayHour = iPlayTimeSec/3600;
		if( iPlayHour <= 0 )
			iPlayHour = 0;

		char szHelp[MAX_PATH*2]="";
		SafeSprintf( szHelp, sizeof( szHelp ), STR(6), iPlayHour );
		// chat
		g_ChatMgr.SetEventMsg( szHelp );

		// memo
		SYSTEMTIME st;
		GetLocalTime( &st );
		DWORD dwTime = (st.wMonth * 1000000) + (st.wDay * 10000) + (st.wHour * 100) + st.wMinute;
		SP2Packet kPacket( STPK_MEMO_SEND_MSG );
		kPacket << STR(7) << szHelp << dwTime << true; 
		// SP2Packet 의 m_currentPos을 되감기 위해서
		SP2Packet kMovePacket( (char*)kPacket.GetBuffer(), kPacket.GetBufferSize() );
		g_MemoManager.OnlineMemo( kMovePacket );
	}
	else if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_SECOND )
	{
		int iPlayTimeSec = 0;
		int iGiftState   = 0;
		rkPacket >> iPlayTimeSec;
		rkPacket >> iGiftState;
		SetValue( VA_PLAYTIME_PLAY_SEC, iPlayTimeSec );
		SetValue( VA_PLAYTIME_GET_GIFT, iGiftState );
	}
}
//---------------------------------------------------------------------------------------------------------------------------------
ChildrenDayEventUserNode::ChildrenDayEventUserNode()
{
}

ChildrenDayEventUserNode::~ChildrenDayEventUserNode()
{
}

bool ChildrenDayEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//---------------------------------------------------------------------------------------------------------------------------------
PesoBonusEventUserNode::PesoBonusEventUserNode()
{
}

PesoBonusEventUserNode::~PesoBonusEventUserNode()
{
}

bool PesoBonusEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}

void PesoBonusEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	// 이벤트 팝업
	__int64 iMyPeso = 0;
	rkPacket >> iMyPeso;
	g_MyInfo.SetMoney( iMyPeso );
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BuyCharNoLevelLimitEventUserNode::BuyCharNoLevelLimitEventUserNode()
{
}

BuyCharNoLevelLimitEventUserNode::~BuyCharNoLevelLimitEventUserNode()
{
}

bool BuyCharNoLevelLimitEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}

bool BuyCharNoLevelLimitEventUserNode::IsNoLevelLimit( bool bCharBuyPeso, int iCharLimitSecond, int iCharPeriodType )
{
	if( !g_EventMgr.IsAlive( GetType(), g_MyInfo.GetChannelingType() ) )
		return false;

	if( !bCharBuyPeso )
		return false;

	if( iCharPeriodType != CPT_TIME )
		return false;

	if( iCharLimitSecond != g_EventMgr.GetValue( GetType(), EA_CHAR_LIMIT_SECOND ) )
		return false;

	return true;
}

bool BuyCharNoLevelLimitEventUserNode::IsAliveUserNode()
{
	if( !g_EventMgr.IsAlive( GetType(), g_MyInfo.GetChannelingType() ) )
		return false;

	return true;
}
//--------------------------------------------------------------------------------------------------------------------------------
GradeUpEventUserNode::GradeUpEventUserNode()
{
}

GradeUpEventUserNode::~GradeUpEventUserNode()
{
}

bool GradeUpEventUserNode::IsEmptyValue()
{
	return true;
}

bool GradeUpEventUserNode::IsGift()
{
	if( !g_EventMgr.IsAlive( GetType(), g_MyInfo.GetChannelingType() ) )
		return false;

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_1_GRADE ) )
		return true;

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_2_GRADE ) )
		return true;

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_3_GRADE ) )
		return true;

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_4_GRADE ) )
		return true;

	return false;
}

int GradeUpEventUserNode::GetBounsPeso()
{
	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_1_GRADE ) )
		return g_EventMgr.GetValue( GetType(), EA_GRADEUP_1_PESO );

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_2_GRADE ) )
		return g_EventMgr.GetValue( GetType(), EA_GRADEUP_2_PESO );

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_3_GRADE ) )
		return g_EventMgr.GetValue( GetType(), EA_GRADEUP_3_PESO );

	if( g_MyInfo.GetGradeLevel() == g_EventMgr.GetValue( GetType(), EA_GRADEUP_4_GRADE ) )
		return g_EventMgr.GetValue( GetType(), EA_GRADEUP_4_PESO );

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------
PCRoomEventUserNode::PCRoomEventUserNode()
{
}

PCRoomEventUserNode::~PCRoomEventUserNode()
{
}

bool PCRoomEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//---------------------------------------------------------------------------------------------------------------------------------
ChanceMortmainCharEventUserNode::ChanceMortmainCharEventUserNode()
{
}

ChanceMortmainCharEventUserNode::~ChanceMortmainCharEventUserNode()
{
}

void ChanceMortmainCharEventUserNode::Init()
{
	SetValue( VA_CMC_MAGIC_CODE, 0 );
	SetValue( VA_CMC_MORTMAIN_CNT, 0 );
}

void ChanceMortmainCharEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iSubType = 0;
	rkPacket >> iSubType;
	if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_USED )
	{
		int iValue1, iValue2;
		rkPacket >> iValue1 >> iValue2;
		SetValue( VA_CMC_MAGIC_CODE, iValue1 );
		SetValue( VA_CMC_MORTMAIN_CNT, iValue2 );

		int iBonusType;
		rkPacket >> iBonusType;

		__int64 iMyMoney;
		rkPacket >> iMyMoney;
		g_MyInfo.SetMoney( iMyMoney );
	}
	else if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_SECOND )
	{
		int iValue1, iValue2;
		rkPacket >> iValue1 >> iValue2;
		SetValue( VA_CMC_MAGIC_CODE, iValue1 );
		SetValue( VA_CMC_MORTMAIN_CNT, iValue2 );
	}
}
//---------------------------------------------------------------------------------------------------------------------------------
OneDayGoldItemEvent::OneDayGoldItemEvent()
{
}

OneDayGoldItemEvent::~OneDayGoldItemEvent()
{
}

void OneDayGoldItemEvent::Init()
{
	SetValue( VA_GOLD_ITEM_RECV_DATE, 0 );
}

void OneDayGoldItemEvent::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	TCPNetwork::MouseBusy( false );
	int iSubType = 0;
	rkPacket >> iSubType;
	if( iSubType == EVENT_DATA_UPDATE_ONEDAY_CHANGE )
	{
	}
	else if( iSubType == EVENT_DATA_UPDATE_ONEDAY_USE )
	{
		// 받은 선물
		int iValue1, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
		bool  bAlarm = false;
		short iPresentType = 0;
		rkPacket >> iValue1 >> iPresentType >> iPresentValue1 >> iPresentValue2  >> iPresentValue3 >> iPresentValue4 >> bAlarm;
		SetValue( VA_GOLD_ITEM_RECV_DATE, iValue1 );
	}
	else if( iSubType == EVENT_DATA_UPDATE_ONEDAY_USE_FAILED )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}
//---------------------------------------------------------------------------------------------------------------------------------
DormancyUserEvent::DormancyUserEvent()
{
}

DormancyUserEvent::~DormancyUserEvent()
{
}

bool DormancyUserEvent::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//---------------------------------------------------------------------------------------------------------------------------------
PlayTimePresentEventUserNode::PlayTimePresentEventUserNode()
{
}

PlayTimePresentEventUserNode::~PlayTimePresentEventUserNode()
{
}

void PlayTimePresentEventUserNode::Init()
{
	SetValue( VA_PLAYTIME_PRESENT_TIME_CNT, 0 );
}

void PlayTimePresentEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iSubType = 0;
	rkPacket >> iSubType;
	if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_USED )
	{
		SetValue( VA_PLAYTIME_PRESENT_TIME_CNT, 0 );
	}
	else if( iSubType == EVENT_DATA_UPDATE_PLAYTIME_SECOND )
	{
		int iPlayTimeCnt = 0;
		rkPacket >> iPlayTimeCnt;
		SetValue( VA_PLAYTIME_PRESENT_TIME_CNT, iPlayTimeCnt );
	}
}
//--------------------------------------------------------------------------------------------------------------------------------
ChristmasEventUserNode::ChristmasEventUserNode()
{
	m_dwSendTimer = 0;
}

ChristmasEventUserNode::~ChristmasEventUserNode()
{
}

void ChristmasEventUserNode::Init()
{
	SetValue( VA_CHRISTMAS_GET_GIFT_DATE, 0 );
}

void ChristmasEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iSubType = 0;
	rkPacket >> iSubType;
	if( iSubType == EVENT_DATA_UPDATE_CHRISTMAS_WARNING )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( iSubType == EVENT_DATA_UPDATE_CHRISTMAS_GET_GIFT )
	{
		DWORD dwCurDate = 0;
		rkPacket >> dwCurDate;
		SetValue( VA_CHRISTMAS_GET_GIFT_DATE, dwCurDate );
	}
}

void ChristmasEventUserNode::SendRequestGift()
{
	enum { SEND_DELAY_TIME = 10000, };

	if( REALGETTIME() - m_dwSendTimer < SEND_DELAY_TIME )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	m_dwSendTimer = REALGETTIME();

	if( g_EventMgr.GetValue( GetType(), EA_CHRISTMAS_IS_ONLY_ONE_GIFT ) == 1 && GetValue( VA_CHRISTMAS_GET_GIFT_DATE ) != 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
    
	SP2Packet kPacket( CTPK_EVENT_DATA_UPDATE );
	kPacket << (int)GetType();
	TCPNetwork::SendToServer( kPacket );
}
//---------------------------------------------------------------------------------------------------------------------------------
BuyItemEventUserNode::BuyItemEventUserNode()
{
}

BuyItemEventUserNode::~BuyItemEventUserNode()
{
}

bool BuyItemEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//---------------------------------------------------------------------------------------------------------------------------------
ExerciseSoldierEventUserNode::ExerciseSoldierEventUserNode()
{
}

ExerciseSoldierEventUserNode::~ExerciseSoldierEventUserNode()
{
}

bool ExerciseSoldierEventUserNode::IsEmptyValue()
{
	return true; // 사용하는 value가 없다.
}
//-----------------------------------------------------------------------------------------------------------------------------------
ConnectionTimeEventUserNode::ConnectionTimeEventUserNode()
{
}

ConnectionTimeEventUserNode::~ConnectionTimeEventUserNode()
{
}

void ConnectionTimeEventUserNode::Init()
{
	SetValue( VA_CT_GET_CHANCE_DATE, 0 );
	SetValue( VA_CT_IS_CHANCE, 0 );
}

void ConnectionTimeEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iSubType   = 0;
	int iCheckDate = 0;
	int iIsChance  = 0;
	rkPacket >> iSubType;
	rkPacket >> iCheckDate;
	rkPacket >> iIsChance;

	SetValue( VA_CT_GET_CHANCE_DATE, iCheckDate );
	SetValue( VA_CT_IS_CHANCE, iIsChance );

	if( iSubType == EVENT_DATA_UPDATE_CONNECTION_TIME_NO_CHANCE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}
//----------------------------------------------------------------------------------------------------------------------------------
OneDayGiftEventUserNode::OneDayGiftEventUserNode()
{
}

OneDayGiftEventUserNode::~OneDayGiftEventUserNode()
{
}

void OneDayGiftEventUserNode::Init()
{
	SetValue( VA_OG_GET_GIFT, 0 );
}
//-----------------------------------------------------------------------------------------------------------------------------------
GradePresentEventUserNode::GradePresentEventUserNode()
{
}

GradePresentEventUserNode::~GradePresentEventUserNode()
{
}

void GradePresentEventUserNode::Init()
{
	SetValue( VA_GP_CAN_RECEIVE_GIFT, 0 );
}
//-----------------------------------------------------------------------------------------------------------------------------------
ConnectionTimeSelectGiftEventUserNode::ConnectionTimeSelectGiftEventUserNode()
{
}

ConnectionTimeSelectGiftEventUserNode::~ConnectionTimeSelectGiftEventUserNode()
{
}

void ConnectionTimeSelectGiftEventUserNode::Init()
{
	SetValue( VA_CTSG_GET_CHANCE_DATE, 0 );
	SetValue( VA_CTSG_IS_CHANCE, 0 );
}

void ConnectionTimeSelectGiftEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	int iSubType   = 0;
	int iCheckDate = 0;
	int iIsChance  = 0;
	rkPacket >> iSubType;
	rkPacket >> iCheckDate;
	rkPacket >> iIsChance;

	SetValue( VA_CTSG_GET_CHANCE_DATE, iCheckDate );
	SetValue( VA_CTSG_IS_CHANCE, iIsChance );

	if( iSubType == EVENT_DATA_UPDATE_CONNECTION_TIME_SELECT_GIFT_NO_CHANCE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------
EntryEventUserNode::EntryEventUserNode()
{
}

EntryEventUserNode::~EntryEventUserNode()
{
}

void EntryEventUserNode::Init()
{
	SetValue( VA_E_GET_GIFT, 0 );
	SetValue( VA_E_CAN_RECEIVE_GIFT, 0 ); // 메모리 변수
}

int EntryEventUserNode::GetAddSize()
{
	return ADD_ENTRY_SIZE;
}
//----------------------------------------------------------------------------------------------------------------------------
LadderPointEventUserNode::LadderPointEventUserNode()
{
}

LadderPointEventUserNode::~LadderPointEventUserNode()
{
}
//-----------------------------------------------------------------------------------------------------------------------------------
ConnectAndPlayTimeEventUserNode::ConnectAndPlayTimeEventUserNode()
{
}

ConnectAndPlayTimeEventUserNode::~ConnectAndPlayTimeEventUserNode()
{
}

void ConnectAndPlayTimeEventUserNode::Init()
{
	SetValue( VA_CAP_CONNECT_RESET_DATE, 0 );
	SetValue( VA_CAP_POINT_AND_SEC, 0 ); 
}

void ConnectAndPlayTimeEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	g_ChatMgr.SetSystemMsg( "개발자K의 가위바위보 도전권 1장획득(홈페이지에서 가위바위보를 할 수 있습니다)" );
}
//---------------------------------------------------------------------------------------------------------------------------------
FreeDayEventUserNode::FreeDayEventUserNode()
{
}

FreeDayEventUserNode::~FreeDayEventUserNode()
{
}

void FreeDayEventUserNode::OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
}