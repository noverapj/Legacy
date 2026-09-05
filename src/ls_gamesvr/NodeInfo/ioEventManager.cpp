#include "stdafx.h"

#include "../EtcHelpFunc.h"

#include "UserNodeManager.h"
#include ".\ioeventmanager.h"

#include <strsafe.h>

extern CLog EventLOG;

EventNode::EventNode()
{
	 m_wStartYear  = 0;
	 m_wStartMonth = 0;
	 m_wStartDate  = 0;
	 m_wStartHour  = 0;

	 m_wEndYear    = 0;
	 m_wEndMonth   = 0;
	 m_wEndDate    = 0;
	 m_wEndHour    = 0;

	 m_bAlive      = false;
	 m_eEventType  = EVT_NONE;
	 m_eModeCategory   = MC_DEFAULT; 
}

EventNode::~EventNode()
{
	m_ValueVec.clear();
	m_vUseChannelingTypeVec.clear();
}

void EventNode::LoadINI( ioINILoader &a_rkLoader, bool bCreateLoad )
{
	if( bCreateLoad )
	{
		m_ValueVec.clear();
		m_vUseChannelingTypeVec.clear();
	}

	m_eEventType  = (EventType) a_rkLoader.LoadInt( "type", 0);
	m_eModeCategory	  = (ModeCategory) a_rkLoader.LoadInt( "mode_category", 0 );

	m_wStartYear  = a_rkLoader.LoadInt( "start_year", 0);
	m_wStartMonth = a_rkLoader.LoadInt( "start_month", 0);
	m_wStartDate  = a_rkLoader.LoadInt( "start_date", 0);
	m_wStartHour  = a_rkLoader.LoadInt( "start_hour", 0);

	m_wEndYear    = a_rkLoader.LoadInt( "end_year", 0);
	m_wEndMonth   = a_rkLoader.LoadInt( "end_month", 0);
	m_wEndDate    = a_rkLoader.LoadInt( "end_date", 0);
	m_wEndHour    = a_rkLoader.LoadInt( "end_hour", 0);

	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Type:%d] %d-%d-%d-%d ~ %d-%d-%d-%d", (int)m_eEventType, m_wStartYear, m_wStartMonth, m_wStartDate, m_wStartHour, m_wEndYear, m_wEndMonth, m_wEndDate, m_wEndHour );
	int iMaxValue = a_rkLoader.LoadInt( "max_value", 0);
	for (int i = 0; i < iMaxValue ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof(szKeyName), "value_%d", i+1 );
		int iValue = a_rkLoader.LoadInt( szKeyName, 0 );
		if( bCreateLoad )
			m_ValueVec.push_back( iValue );
		else
			SetValue( i, iValue );
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Type:%d] Value%d : %d",(int)GetType(), i+1, GetValue( i ) );
	}

	int iMaxUseChannelingType = a_rkLoader.LoadInt( "max_use_channeling_type", 0 );
	for (int i = 0; i < iMaxUseChannelingType ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "use_channeling_type%d", i+1 );
		ChannelingType eType = (ChannelingType) a_rkLoader.LoadInt( szKeyName, (int) CNT_NONE );
		if( bCreateLoad )
			m_vUseChannelingTypeVec.push_back( eType );
		else
			SetUseChannelingType( i, eType );

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Type:%d] use channeling type%d : %d", (int)GetType() , i+1, (int)GetUseChannelingType( i ) );
	}
}

void EventNode::CheckAlive( SYSTEMTIME &rSt, bool bSend /*= true*/ )
{
	bool bPreAlive = m_bAlive;
	m_bAlive = IsCheckAlive(rSt);

	if( !bSend ) // 처음셋팅이면
		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Start Setting [Type:%d] Alive : %d", (int)m_eEventType, m_bAlive );


	if( bPreAlive != m_bAlive )
	{
		if( bSend )
		{
			SP2Packet kPacket(STPK_EVENT_ALIVE);
			kPacket << (int) m_eEventType;
			kPacket << m_bAlive;
			g_UserNodeManager.SendMessageAll( kPacket );
		}

		EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Type:%d] Alive : %d", (int)m_eEventType, m_bAlive );
	}
}

void EventNode::Update(int iValues[], int iValueCount)
{
	int iIndex = 2, iMaxValue = 0;
	m_wStartYear	= iValues[iIndex++];
	m_wStartMonth	= iValues[iIndex++];
	m_wStartDate	= iValues[iIndex++];
	m_wStartHour	= iValues[iIndex++];

	m_wEndYear		= iValues[iIndex++];
	m_wEndMonth		= iValues[iIndex++];
	m_wEndDate		= iValues[iIndex++];
	m_wEndHour		= iValues[iIndex++];

	iMaxValue		= iValues[iIndex++];
	m_ValueVec.clear();

	for(int i = 0 ; i < iMaxValue ; i++)
	{
		m_ValueVec.push_back(iValues[iIndex++]);
	}

	m_vUseChannelingTypeVec.clear();
	int iMaxUseChannelingType = iValues[iIndex++];
	for (int i = 0; i < iMaxUseChannelingType ; i++)
	{
		m_vUseChannelingTypeVec.push_back( (ChannelingType)iValues[iIndex++] );
	}
}

bool EventNode::IsCheckAlive( SYSTEMTIME &rSt )
{
	if( COMPARE( Help::ConvertYYMMDDHHMMToDate( rSt.wYear, rSt.wMonth, rSt.wDay, rSt.wHour, rSt.wMinute ), 
			  	 Help::ConvertYYMMDDHHMMToDate( m_wStartYear, m_wStartMonth, m_wStartDate, m_wStartHour, 0 ),
			     Help::ConvertYYMMDDHHMMToDate( m_wEndYear, m_wEndMonth, m_wEndDate, m_wEndHour, 0 ) ) )
	{
		return true;
	}
	return false;
}

EventType EventNode::GetType() const
{
	return m_eEventType;
}

int EventNode::GetValue( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_ValueVec.size() ) )
		return 0;

	return m_ValueVec[iArray];
}



int EventNode::GetValueSize() const
{
	return m_ValueVec.size();
}

int EventNode::GetUseChannelingTypeSize() const
{
	return m_vUseChannelingTypeVec.size();
}

ChannelingType EventNode::GetUseChannelingType( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_vUseChannelingTypeVec.size() ) )
		return CNT_NONE;

	return m_vUseChannelingTypeVec[iArray];
}

bool EventNode::IsAlive( ChannelingType eUserChannelingType ) 
{
	if( !m_bAlive )
		return false;
	
	if( m_vUseChannelingTypeVec.empty() )
		return true;

	for( vChannelingTypeVec::iterator iter = m_vUseChannelingTypeVec.begin(); iter != m_vUseChannelingTypeVec.end(); ++iter )
	{
	    ChannelingType &reType = *iter;
		if( reType == CNT_NONE )
			return true;
		if( reType == eUserChannelingType )
			return true;
	}

	return false;
}

void EventNode::SetValue( int iArray, int iValue )
{
	if( !COMPARE( iArray, 0, (int) m_ValueVec.size() ) )
		return;

	m_ValueVec[iArray] = iValue;
}

void EventNode::SetUseChannelingType( int iArray, ChannelingType eUserChannelingType )
{
	if( !COMPARE( iArray, 0, (int) m_vUseChannelingTypeVec.size() ) )
		return;

	m_vUseChannelingTypeVec[iArray] = eUserChannelingType;
}

int EventNode::GetStartDate()
{
	return ( m_wStartYear*10000 ) + ( m_wStartMonth*100 ) + m_wStartDate;
}

int EventNode::GetEndDate()
{
	return ( m_wEndYear*10000 ) + ( m_wEndMonth*100 ) + m_wEndDate;
}

//--------------------------------------------------------------------------------
ioPlazaMonsterEventNode::ioPlazaMonsterEventNode()
{
	m_vecGroupDay.clear();
}

ioPlazaMonsterEventNode::~ioPlazaMonsterEventNode()
{
	m_vecGroupDay.clear();
}

void ioPlazaMonsterEventNode::LoadINI( ioINILoader &a_rkLoader, bool bCreateLoad )
{
	// 기본형식
	EventNode::LoadINI( a_rkLoader, bCreateLoad );

	if( bCreateLoad )
	{
		m_vecGroupDay.clear();
	}

	// declare
	char szKey[ MAX_PATH ] = { 0, };
	char szBuf[ MAX_PATH ] = { 0, };

	int iMaxGroup = a_rkLoader.LoadInt( "total_group", 0 );
	for( int i = 0 ; i < iMaxGroup ; ++i )
	{
		GROUP_DAY	kStruct;

		wsprintf( szKey, "total_day_%d", i + 1 );
		kStruct.iTotal = a_rkLoader.LoadInt( szKey, 0 );

		if( kStruct.iTotal == 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "(sp2_event.ini) : %s = 0", szKey );
			break;
		}

		wsprintf( szKey, "day_%d", i + 1 );
		a_rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		int tokenCount = 0;
		char* next = NULL;
		char* pos = strtok_s( szBuf, ".", &next );

		int day = atoi( pos );

		kStruct.vecDays.push_back( day );
		
		while( pos )
		{
			tokenCount++;

			pos = strtok_s( NULL, ".", &next );
			if( pos != NULL )
			{
				int day = atoi( pos );
				kStruct.vecDays.push_back( day );
			}
		}

		if( tokenCount != kStruct.iTotal )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "(sp2_event.ini) : %s - tokenCount != kStruct.iTotalDay", szKey );

		wsprintf( szKey, "start_hour_day_%d", i + 1 );
		kStruct.iStartHour = a_rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "end_hour_day_%d", i + 1 );
		kStruct.iEndHour = a_rkLoader.LoadInt( szKey, 0 );

		// push_back
		m_vecGroupDay.push_back( kStruct );
	}
}

bool ioPlazaMonsterEventNode::IsEventTime()
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	int iSize = m_vecGroupDay.size();
	for( int i = 0 ; i < iSize ; ++i )
	{
		vector< int >::iterator iter = std::find( m_vecGroupDay[ i ].vecDays.begin(), m_vecGroupDay[ i ].vecDays.end(), st.wDay );
		if( iter != m_vecGroupDay[ i ].vecDays.end() )
		{
			if( COMPARE( st.wHour, m_vecGroupDay[ i ].iStartHour, m_vecGroupDay[ i ].iEndHour ) )
				return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------
PresentBuyEventNode::PresentBuyEventNode()
{
	m_mExceptionItemInfo.clear();
	m_mAllowedItemInfo.clear();
	m_vAllowedItemType.clear();
}

PresentBuyEventNode::~PresentBuyEventNode()
{
}

void PresentBuyEventNode::LoadINI( ioINILoader &a_rkLoader, bool bCreateLoad )
{
	EventNode::LoadINI( a_rkLoader, bCreateLoad );
	char szKey[ MAX_PATH ] = { 0, };
	char szValues[MAX_PATH] = "";

	if( bCreateLoad )
	{
		m_mExceptionItemInfo.clear();
		m_vAllowedItemType.clear();
	}
	
	/*for( int i	= 0; i < 100; i++ )
	{
		wsprintf( szKey, "allowed_itemType%d", i + 1 );
		int iType = a_rkLoader.LoadInt(szKey, 0);
		if( 0 == iType )
			break;

		m_vAllowedItemType.push_back(iType);
	}*/

	EVENTITEM::iterator it;
	for( int i = 0; i < 1000; i++ )
	{
		wsprintf( szKey, "exception_%d", i + 1 );
		a_rkLoader.LoadString( szKey, "", szValues, MAX_PATH );
		
		//value 파싱
		IntVec vValues;
		Help::TokenizeToINT(szValues, ".", vValues);

		if( vValues.empty() || vValues.size() < 2 )
			break;

		int iType	= vValues[0];
		int iCode	= vValues[1];

		it = m_mExceptionItemInfo.find(iType);
		if( it == m_mExceptionItemInfo.end() )
		{
			ITEMCODE vInfo;
			vInfo.push_back(iCode);

			m_mExceptionItemInfo.insert( std::make_pair(iType, vInfo) );
		}
		else
		{
			ITEMCODE& vInfo = it->second;
			vInfo.push_back(iCode);
		}
	}

	for( int i = 0; i < 1000; i++ )
	{
		wsprintf( szKey, "allowed_%d", i + 1 );
		a_rkLoader.LoadString( szKey, "", szValues, MAX_PATH );
		
		//value 파싱
		IntVec vValues;
		Help::TokenizeToINT(szValues, ".", vValues);

		if( vValues.empty() || vValues.size() < 2 )
			break;

		int iType	= vValues[0];
		int iCode	= vValues[1];

		it = m_mAllowedItemInfo.find(iType);
		if( it == m_mAllowedItemInfo.end() )
		{
			ITEMCODE vInfo;
			vInfo.push_back(iCode);

			m_mAllowedItemInfo.insert( std::make_pair(iType, vInfo) );
		}
		else
		{
			ITEMCODE& vInfo = it->second;
			vInfo.push_back(iCode);
		}
	}
}

bool PresentBuyEventNode::IsEventWeek(WORD wDayOfWeek, int iValueArray)
{
	// wDayOfWeek : ( 0:일, 1:월, 2:화, 3:수, 4:목, 5:금, 6:토 )
	// 1211111    : 일월화수목금토 / 월요일에 이벤트 있음
	enum { EVENT_WEEK_OFF = 1, EVENT_WEEK_ON = 2, };

	if( iValueArray != EA_PB_FIRST_EVENT_WEEK_ON_OFF && 
		iValueArray != EA_PB_SECOND_EVENT_WEEK_ON_OFF )
	{
		return false;
	}

	int iOnOff = 0;
	if( wDayOfWeek == 0 )
	{
		// [1]211111 
		iOnOff = GetValue(iValueArray) / 1000000;
	}
	else if( wDayOfWeek == 1 )
	{
		// 1[2]11111 
		iOnOff = GetValue(iValueArray) / 100000 % 10;
	}
	else if( wDayOfWeek == 2 )
	{
		// 12[1]1111 
		iOnOff = GetValue(iValueArray) / 10000 % 10;
	}
	else if( wDayOfWeek == 3 )
	{
		// 121[1]111 
		iOnOff = GetValue(iValueArray) / 1000 % 10;
	}
	else if( wDayOfWeek == 4 )
	{
		// 1211[1]11 
		iOnOff = GetValue(iValueArray) / 100 % 10;
	}
	else if( wDayOfWeek == 5 )
	{
		// 12111[1]1 
		iOnOff = GetValue(iValueArray) / 10 % 10;
	}
	else if( wDayOfWeek == 6 )
	{
		// 121111[1] 
		iOnOff = GetValue(iValueArray) % 10;
	}
	else
		return false;

	if( EVENT_WEEK_ON == iOnOff )
		return true;

	return false;
}

void PresentBuyEventNode::CheckAlive( SYSTEMTIME &rSt, bool bSend)
{
	bool bPreAlive = m_bAlive;

	m_bAlive	= false;

	if( IsCheckAlive(rSt) )
	{
		if( IsEventTime(rSt) )
			m_bAlive = true;
	}

	if( bPreAlive != m_bAlive )
	{
		if( bSend )
		{
			SP2Packet kPacket(STPK_EVENT_ALIVE);
			kPacket << (int) m_eEventType;
			kPacket << m_bAlive;
			g_UserNodeManager.SendMessageAll( kPacket );
		}
	}
}

bool PresentBuyEventNode::IsEventTime(SYSTEMTIME& st)
{
	//발동 일, 시간 확인.
	enum { MAX_CHECK = 2, MAX_VALUE = 3,};
	int iValueArrayList[MAX_CHECK][MAX_VALUE]= { EA_PB_FIRST_EVENT_WEEK_ON_OFF,  EA_PB_FIRST_START_TIME,  EA_PB_FIRST_END_TIME,  
		                                         EA_PB_SECOND_EVENT_WEEK_ON_OFF, EA_PB_SECOND_START_TIME, EA_PB_SECOND_END_TIME 	};

	for (int i = 0; i < MAX_CHECK ; i++)
	{
		if( IsEventWeek( st.wDayOfWeek, iValueArrayList[i][0] ) )
		{
			if( COMPARE( st.wHour, GetValue(iValueArrayList[i][1]), GetValue(iValueArrayList[i][2]) ) )
				return true;
		}	
	}

	return false;
}

bool PresentBuyEventNode::IsEventItem(const int iType, const int iCode)
{
	//허용된 아이템 타입인가.
	/*BOOL bAllowed			= FALSE;
	int iAllowedTypeSize	= m_vAllowedItemType.size();

	if( 0 == iAllowedTypeSize )
		bAllowed	= TRUE;

	for( int i = 0; i < iAllowedTypeSize; i++ )
	{
		if( iType == m_vAllowedItemType[i] )
		{
			bAllowed	= TRUE;
			break;
		}
	}

	if( !bAllowed )
		return false;*/

	/*EXCEPTIONITEM::iterator it	= m_mExceptionItemInfo.find(iType);
	if( it == m_mExceptionItemInfo.end() )
		return true;

	EXCEPTIONCODE& stInfo	= it->second;

	for( int i = 0; i < (int)stInfo.size(); i++ )
	{
		if( stInfo[i] == iCode )
			return false;
	}

	return true;*/

	if( m_mAllowedItemInfo.size() != 0 )
	{
		EVENTITEM::iterator it	= m_mAllowedItemInfo.find(iType);
		if( it == m_mAllowedItemInfo.end() )
			return false;

		ITEMCODE& stInfo	= it->second;

		for( int i = 0; i < (int)stInfo.size(); i++ )
		{
			if( stInfo[i] == iCode )
				return true;
		}

		return false;
	}
	else
	{
		EVENTITEM::iterator it	= m_mExceptionItemInfo.find(iType);
		if( it == m_mExceptionItemInfo.end() )
			return true;

		ITEMCODE& stInfo	= it->second;

		for( int i = 0; i < (int)stInfo.size(); i++ )
		{
			if( stInfo[i] == iCode )
				return false;
		}

		return true;
	}
}
//-----------------------------------------------------------------------
ioCumulativeSaleEventNode::ioCumulativeSaleEventNode()
{
	m_mCumulativeSaleItemInfo.clear();
}

ioCumulativeSaleEventNode::~ioCumulativeSaleEventNode()
{

}

void ioCumulativeSaleEventNode::LoadINI( ioINILoader &a_rkLoader, bool bCreateLoad )
{
	EventNode::LoadINI( a_rkLoader, bCreateLoad );
	char szKey[ MAX_PATH ] = { 0, };
	char szValues[MAX_PATH] = "";

	if( bCreateLoad )
	{
		m_mCumulativeSaleItemInfo.clear();
	}
	

	enum { SALE_MAX = 10};
	int iMaxDefalutSale = a_rkLoader.LoadInt("max_default_sale_count", 0);
	int arrDefaultSaleRate[SALE_MAX] = {0,};
	for(int i = 0; i < iMaxDefalutSale ; ++i)
	{
		wsprintf( szKey, "default_sale_rate_%d", i + 1 );
		arrDefaultSaleRate[i] = a_rkLoader.LoadInt( szKey, 0);
	}

	SALEEVENTITEM::iterator it;
	int iMax = a_rkLoader.LoadInt("max_possible_item", 0);
	enum { BASEDATA = 2};
	for( int i = 0; i < iMax; i++ )
	{
		wsprintf( szKey, "possible_%d", i + 1 );
		a_rkLoader.LoadString( szKey, "", szValues, MAX_PATH );
		
		//value 파싱
		IntVec vValues;
		Help::TokenizeToINT(szValues, ".", vValues);
		int iVecSize = vValues.size();
		if( vValues.empty() || iVecSize < BASEDATA ) //기본 충족 조건
			break;

		int iType	= vValues[0];
		ITEMDATA newItem;
		newItem.dwItemCode	= vValues[1];
		if( iVecSize > BASEDATA)
		{
			newItem.iMaxDiscountCount = iVecSize - BASEDATA;
			for( int j = 0 ; j < ITEMDATA::MAX_COUNT ; ++j)
			{
				if( j < newItem.iMaxDiscountCount)
					newItem.iDiscountRate[j] = vValues[j+BASEDATA];
				else
					newItem.iDiscountRate[j] = 0;
			}
		}
		else
		{
			int iMaxCnt = 0;
			for( int i = 0 ; i < SALE_MAX ; ++i )
			{
				if( arrDefaultSaleRate[i] == 0 )
					break;

				newItem.iDiscountRate[i] = arrDefaultSaleRate[i];
				++iMaxCnt;
			}
			newItem.iMaxDiscountCount = iMaxCnt;
		}

		it = m_mCumulativeSaleItemInfo.find(iType);
		if( it == m_mCumulativeSaleItemInfo.end() )
		{
 			vITEMDATA vInfo;
 			vInfo.push_back(newItem);
			m_mCumulativeSaleItemInfo.insert( std::make_pair(iType, vInfo) );
		}
		else
		{
			vITEMDATA& vInfo = it->second;
			vInfo.push_back(newItem);
		}
	}
}

void ioCumulativeSaleEventNode::CheckAlive( SYSTEMTIME &rSt, bool bSend /*= true*/ )
{
	bool bPreAlive = m_bAlive;

	m_bAlive	= false;

	if( IsCheckAlive(rSt) )
	{
		if( IsEventTime(rSt) )
			m_bAlive = true;
	}

	if( bPreAlive != m_bAlive )
	{
		if( bSend )
		{
			SP2Packet kPacket(STPK_EVENT_ALIVE);
			kPacket << (int) m_eEventType;
			kPacket << m_bAlive;
			g_UserNodeManager.SendMessageAll( kPacket );
		}
	}
}

bool ioCumulativeSaleEventNode::IsEventWeek( WORD wDayOfWeek, int iValueArray )
{
	// wDayOfWeek : ( 0:일, 1:월, 2:화, 3:수, 4:목, 5:금, 6:토 )
	// 1211111    : 일월화수목금토 / 월요일에 이벤트 있음
	enum { EVENT_WEEK_OFF = 1, EVENT_WEEK_ON = 2, };

	if( iValueArray != EA_PB_FIRST_EVENT_WEEK_ON_OFF && iValueArray != EA_PB_SECOND_EVENT_WEEK_ON_OFF )
	{
		return false;
	}

	int iOnOff = 0;
	if( wDayOfWeek == 0 )
	{
		iOnOff = GetValue(iValueArray) / 1000000;
	}
	else if( wDayOfWeek == 1 )
	{
		iOnOff = GetValue(iValueArray) / 100000 % 10;
	}
	else if( wDayOfWeek == 2 )
	{
		iOnOff = GetValue(iValueArray) / 10000 % 10;
	}
	else if( wDayOfWeek == 3 )
	{
		iOnOff = GetValue(iValueArray) / 1000 % 10;
	}
	else if( wDayOfWeek == 4 )
	{
		iOnOff = GetValue(iValueArray) / 100 % 10;
	}
	else if( wDayOfWeek == 5 )
	{
		iOnOff = GetValue(iValueArray) / 10 % 10;
	}
	else if( wDayOfWeek == 6 )
	{
		iOnOff = GetValue(iValueArray) % 10;
	}
	else
		return false;

	if( EVENT_WEEK_ON == iOnOff )
		return true;

	return false;
}

bool ioCumulativeSaleEventNode::IsEventTime( SYSTEMTIME& st )
{
	//발동 일, 시간 확인.
	enum { MAX_CHECK = 2, MAX_VALUE = 3,};
	int iValueArrayList[MAX_CHECK][MAX_VALUE]= { EA_PB_FIRST_EVENT_WEEK_ON_OFF,  EA_PB_FIRST_START_TIME,  EA_PB_FIRST_END_TIME,  
		EA_PB_SECOND_EVENT_WEEK_ON_OFF, EA_PB_SECOND_START_TIME, EA_PB_SECOND_END_TIME 	};

	for (int i = 0; i < MAX_CHECK ; i++)
	{
		if( IsEventWeek( st.wDayOfWeek, iValueArrayList[i][0] ) )
		{
			if( COMPARE( st.wHour, GetValue(iValueArrayList[i][1]), GetValue(iValueArrayList[i][2]) ) )
				return true;
		}	
	}

	return false;
}

bool ioCumulativeSaleEventNode::IsEventItem( const int iType, const int iCode )
{
	SALEEVENTITEM::iterator it	= m_mCumulativeSaleItemInfo.find(iType);
	if( it == m_mCumulativeSaleItemInfo.end() )
		return false;

	vITEMDATA& stInfo	= it->second;
	for( int i = 0; i < (int)stInfo.size(); i++ )
	{
		if( stInfo[i].dwItemCode == iCode )
			return true;
	}

	return false;
}

bool ioCumulativeSaleEventNode::IsCumulativeSaleItem( const int iType, const DWORD dwItemCode )
{
	SALEEVENTITEM::iterator it = m_mCumulativeSaleItemInfo.find(iType);
	if( it == m_mCumulativeSaleItemInfo.end())
		return false;

	vITEMDATA& stInfo = it->second;
	for( int i = 0; i < (int)stInfo.size(); i++ )
	{
		if( stInfo[i].dwItemCode == dwItemCode )
			return true;
	}
	return false;
}

int ioCumulativeSaleEventNode::GetCumulativeSaleItemByDiscountRate( const int iType, const DWORD dwItemCode, const int iBuyCnt )
{
	SALEEVENTITEM::iterator it = m_mCumulativeSaleItemInfo.find(iType);
	if( it == m_mCumulativeSaleItemInfo.end())
		return 0;

	vITEMDATA& stInfo = it->second;
	for( int i = 0; i < (int)stInfo.size(); i++ )
	{
		if( stInfo[i].dwItemCode == dwItemCode )
		{
			if( iBuyCnt == 0 ) 
				return 0;
			else
			{
				if(stInfo[i].iMaxDiscountCount <= iBuyCnt)
					return stInfo[i].iDiscountRate[stInfo[i].iMaxDiscountCount - 1];

				return stInfo[i].iDiscountRate[iBuyCnt-1];
			}
		}
	}
	
	return 0;
}

//-----------------------------------------------------------------------
PresentBuyEventNode_2::PresentBuyEventNode_2()
{
	m_mExceptionItemInfo.clear();
	m_mAllowedItemInfo.clear();
	m_vAllowedItemType.clear();
}

PresentBuyEventNode_2::~PresentBuyEventNode_2()
{
}

void PresentBuyEventNode_2::LoadINI( ioINILoader &a_rkLoader, bool bCreateLoad )
{
	EventNode::LoadINI( a_rkLoader, bCreateLoad );
	char szKey[ MAX_PATH ] = { 0, };
	char szValues[MAX_PATH] = "";

	if( bCreateLoad )
	{
		m_mExceptionItemInfo.clear();
		m_vAllowedItemType.clear();
	}
	
	EVENTITEM::iterator it;
	for( int i = 0; i < 1000; i++ )
	{
		wsprintf( szKey, "exception_%d", i + 1 );
		a_rkLoader.LoadString( szKey, "", szValues, MAX_PATH );
		
		//value 파싱
		IntVec vValues;
		Help::TokenizeToINT(szValues, ".", vValues);

		if( vValues.empty() || vValues.size() < 2 )
			break;

		int iType	= vValues[0];
		int iCode	= vValues[1];

		it = m_mExceptionItemInfo.find(iType);
		if( it == m_mExceptionItemInfo.end() )
		{
			ITEMCODE vInfo;
			vInfo.push_back(iCode);

			m_mExceptionItemInfo.insert( std::make_pair(iType, vInfo) );
		}
		else
		{
			ITEMCODE& vInfo = it->second;
			vInfo.push_back(iCode);
		}
	}

	for( int i = 0; i < 1000; i++ )
	{
		wsprintf( szKey, "allowed_%d", i + 1 );
		a_rkLoader.LoadString( szKey, "", szValues, MAX_PATH );
		
		//value 파싱
		IntVec vValues;
		Help::TokenizeToINT(szValues, ".", vValues);

		if( vValues.empty() || vValues.size() < 2 )
			break;

		int iType	= vValues[0];
		int iCode	= vValues[1];

		it = m_mAllowedItemInfo.find(iType);
		if( it == m_mAllowedItemInfo.end() )
		{
			ITEMCODE vInfo;
			vInfo.push_back(iCode);

			m_mAllowedItemInfo.insert( std::make_pair(iType, vInfo) );
		}
		else
		{
			ITEMCODE& vInfo = it->second;
			vInfo.push_back(iCode);
		}
	}
}

bool PresentBuyEventNode_2::IsEventWeek(WORD wDayOfWeek, int iValueArray)
{
	// wDayOfWeek : ( 0:일, 1:월, 2:화, 3:수, 4:목, 5:금, 6:토 )
	// 1211111    : 일월화수목금토 / 월요일에 이벤트 있음
	enum { EVENT_WEEK_OFF = 1, EVENT_WEEK_ON = 2, };

	if( iValueArray != EA_PB_FIRST_EVENT_WEEK_ON_OFF && 
		iValueArray != EA_PB_SECOND_EVENT_WEEK_ON_OFF )
	{
		return false;
	}

	int iOnOff = 0;
	if( wDayOfWeek == 0 )
	{
		// [1]211111 
		iOnOff = GetValue(iValueArray) / 1000000;
	}
	else if( wDayOfWeek == 1 )
	{
		// 1[2]11111 
		iOnOff = GetValue(iValueArray) / 100000 % 10;
	}
	else if( wDayOfWeek == 2 )
	{
		// 12[1]1111 
		iOnOff = GetValue(iValueArray) / 10000 % 10;
	}
	else if( wDayOfWeek == 3 )
	{
		// 121[1]111 
		iOnOff = GetValue(iValueArray) / 1000 % 10;
	}
	else if( wDayOfWeek == 4 )
	{
		// 1211[1]11 
		iOnOff = GetValue(iValueArray) / 100 % 10;
	}
	else if( wDayOfWeek == 5 )
	{
		// 12111[1]1 
		iOnOff = GetValue(iValueArray) / 10 % 10;
	}
	else if( wDayOfWeek == 6 )
	{
		// 121111[1] 
		iOnOff = GetValue(iValueArray) % 10;
	}
	else
		return false;

	if( EVENT_WEEK_ON == iOnOff )
		return true;

	return false;
}

void PresentBuyEventNode_2::CheckAlive( SYSTEMTIME &rSt, bool bSend)
{
	bool bPreAlive = m_bAlive;

	m_bAlive	= false;

	if( IsCheckAlive(rSt) )
	{
		if( IsEventTime(rSt) )
			m_bAlive = true;
	}

	if( bPreAlive != m_bAlive )
	{
		if( bSend )
		{
			SP2Packet kPacket(STPK_EVENT_ALIVE);
			kPacket << (int) m_eEventType;
			kPacket << m_bAlive;
			g_UserNodeManager.SendMessageAll( kPacket );
		}
	}
}

bool PresentBuyEventNode_2::IsEventTime(SYSTEMTIME& st)
{
	//발동 일, 시간 확인.
	enum { MAX_CHECK = 2, MAX_VALUE = 3,};
	int iValueArrayList[MAX_CHECK][MAX_VALUE]= { EA_PB_FIRST_EVENT_WEEK_ON_OFF,  EA_PB_FIRST_START_TIME,  EA_PB_FIRST_END_TIME,  
		                                         EA_PB_SECOND_EVENT_WEEK_ON_OFF, EA_PB_SECOND_START_TIME, EA_PB_SECOND_END_TIME 	};

	for (int i = 0; i < MAX_CHECK ; i++)
	{
		if( IsEventWeek( st.wDayOfWeek, iValueArrayList[i][0] ) )
		{
			if( COMPARE( st.wHour, GetValue(iValueArrayList[i][1]), GetValue(iValueArrayList[i][2]) ) )
				return true;
		}	
	}

	return false;
}

bool PresentBuyEventNode_2::IsEventItem(const int iType, const int iCode)
{
	if( m_mAllowedItemInfo.size() != 0 )
	{
		EVENTITEM::iterator it	= m_mAllowedItemInfo.find(iType);
		if( it == m_mAllowedItemInfo.end() )
			return false;

		ITEMCODE& stInfo	= it->second;

		for( int i = 0; i < (int)stInfo.size(); i++ )
		{
			if( stInfo[i] == iCode )
				return true;
		}

		return false;
	}
	else
	{
		EVENTITEM::iterator it	= m_mExceptionItemInfo.find(iType);
		if( it == m_mExceptionItemInfo.end() )
			return true;

		ITEMCODE& stInfo	= it->second;

		for( int i = 0; i < (int)stInfo.size(); i++ )
		{
			if( stInfo[i] == iCode )
				return false;
		}

		return true;
	}
}

//-----------------------------------------------------------------------
template<> ioEventManager* Singleton< ioEventManager >::ms_Singleton = 0;

ioEventManager::ioEventManager(void)
{
	m_iMaxDBValue			= 0;
	m_dwCurrentTime			= 0;
	m_dwCurrentDate			= 0;
	m_iMileageRatio			= 0;
	m_iPcRoomMileageRatio	= 0;
	m_bMileageShopOpen		= false;
	m_bChanneling_CashBack				= false;
	m_iLosaMileageRatio		= 0;
	m_iCashBackRatio	= 0;
	m_iCashBackPeriod	= 0;

	m_vCharacterCashBack.clear();
	m_vDecoCashBack.clear();
	m_vETCCashBack.clear();
	m_vExtraCashBack.clear();
}

ioEventManager::~ioEventManager( void )
{
	Clear();
}

void ioEventManager::LoadINI( bool bCreateLoad /*= true */ )
{
	if( bCreateLoad )
	{
		Clear();
	}
	else
	{
		m_vCharacterCashBack.clear();
		m_vDecoCashBack.clear();
		m_vETCCashBack.clear();
		m_vExtraCashBack.clear();
	}

	ioINILoader kLoader;
	if( bCreateLoad )
		kLoader.LoadFile( "config/sp2_event.ini" );
	else
		kLoader.ReloadFile( "config/sp2_event.ini" );

	kLoader.SetTitle( "common" );



	m_bMileageShopOpen = kLoader.LoadBool("mileage_shop_open", 0);
	m_iMileageRatio = kLoader.LoadInt("mileage_ratio", 10);
	m_iPcRoomMileageRatio = kLoader.LoadInt("pcroom_mileage_ratio", 10);

	WORD wLosaStartYear;
	WORD wLosaStartMonth;
	WORD wLosaStartDay;
	WORD wLosaStartHour;

	WORD wLosaEndYear;
	WORD wLosaEndMonth;
	WORD wLosaEndDay;
	WORD wLosaEndHour;

	wLosaStartYear  = kLoader.LoadInt( "losa_start_year", 0);
	wLosaStartMonth = kLoader.LoadInt( "losa_start_month", 0);
	wLosaStartDay  = kLoader.LoadInt( "losa_start_day", 0);
	wLosaStartHour  = kLoader.LoadInt( "losa_start_hour", 0);

	wLosaEndYear    = kLoader.LoadInt( "losa_end_year", 0);
	wLosaEndMonth   = kLoader.LoadInt( "losa_end_month", 0);
	wLosaEndDay    = kLoader.LoadInt( "losa_end_day", 0);
	wLosaEndHour    = kLoader.LoadInt( "losa_end_hour", 0);
	

	m_iLosaMileageRatio = kLoader.LoadInt("losa_mileage_ratio", 10);

	m_cLosaStartTime = Help::GetSafeValueForCTimeConstructor( wLosaStartYear, wLosaStartMonth, wLosaStartDay, wLosaStartHour, 0, 0 );
	m_cLosaEndTime = Help::GetSafeValueForCTimeConstructor( wLosaEndYear, wLosaEndMonth, wLosaEndDay, wLosaEndHour, 0, 0 );
	
	//채널링 캐시백 이벤트 
	WORD wCashBackStartYear;
	WORD wCashBackStartMonth;
	WORD wCashBackStartDay;
	WORD wCashBackStartHour;

	WORD wCashBackEndYear;
	WORD wCashBackEndMonth;
	WORD wCashBackEndDay;
	WORD wCashBackEndHour;

	m_bChanneling_CashBack = kLoader.LoadBool( "cashback_open", false );
	if( m_bChanneling_CashBack )
	{
		wCashBackStartYear  = kLoader.LoadInt( "cashback_start_year", 0);
		wCashBackStartMonth = kLoader.LoadInt( "cashback_start_month", 0);
		wCashBackStartDay  = kLoader.LoadInt( "cashback_start_day", 0);
		wCashBackStartHour  = kLoader.LoadInt( "cashback_start_hour", 0);

		wCashBackEndYear    = kLoader.LoadInt( "cashback_end_year", 0);
		wCashBackEndMonth   = kLoader.LoadInt( "cashback_end_month", 0);
		wCashBackEndDay    = kLoader.LoadInt( "cashback_end_day", 0);
		wCashBackEndHour    = kLoader.LoadInt( "cashback_end_hour", 0);

		m_cCashBackStartTime = Help::GetSafeValueForCTimeConstructor( wCashBackStartYear, wCashBackStartMonth, wCashBackStartDay, wCashBackStartHour, 0, 0 );
		m_cCashBackEndTime = Help::GetSafeValueForCTimeConstructor( wCashBackEndYear, wCashBackEndMonth, wCashBackEndDay, wCashBackEndHour, 0, 0 );

		m_iCashBackRatio = kLoader.LoadInt("cashback_ratio", 10);
		m_iCashBackPeriod = kLoader.LoadInt( "cashback_expiraion_time", 24);
	}
	////////
	
	//아이템 구매 캐시백
	WORD wItemCashBackStartYear;
	WORD wItemCashBackStartMonth;
	WORD wItemCashBackStartDay;
	WORD wItemCashBackStartHour;

	WORD wItemCashBackEndYear;
	WORD wItemCashBackEndMonth;
	WORD wItemCashBackEndDay;
	WORD wItemCashBackEndHour;

	m_bItemCashBack = kLoader.LoadBool( "item_cashback_open", false );
	if( m_bItemCashBack )
	{
		wItemCashBackStartYear  = kLoader.LoadInt( "item_cashback_start_year", 0);
		wItemCashBackStartMonth = kLoader.LoadInt( "item_cashback_start_month", 0);
		wItemCashBackStartDay  = kLoader.LoadInt( "item_cashback_start_day", 0);
		wItemCashBackStartHour  = kLoader.LoadInt( "item_cashback_start_hour", 0);

		wItemCashBackEndYear    = kLoader.LoadInt( "item_cashback_end_year", 0);
		wItemCashBackEndMonth   = kLoader.LoadInt( "item_cashback_end_month", 0);
		wItemCashBackEndDay    = kLoader.LoadInt( "item_cashback_end_day", 0);
		wItemCashBackEndHour    = kLoader.LoadInt( "item_cashback_end_hour", 0);

		m_cItemCashBackStartTime = Help::GetSafeValueForCTimeConstructor( wItemCashBackStartYear, wItemCashBackStartMonth, wItemCashBackStartDay, wItemCashBackStartHour, 0, 0 );
		m_cItemCashBackEndTime = Help::GetSafeValueForCTimeConstructor( wItemCashBackEndYear, wItemCashBackEndMonth, wItemCashBackEndDay, wItemCashBackEndHour, 0, 0 );

		m_iItemCashBackRatio = kLoader.LoadInt("item_cashback_ratio", 10);
		m_iItemCashBackPeriod = kLoader.LoadInt( "item_cashback_expiraion_time", 24);

		//용병
		int iMaxCount = 0;
		kLoader.SetTitle( "Character" );
		iMaxCount = kLoader.LoadInt( "max_count", 0 );
		
		for(int i = 0; i< iMaxCount; i++ )
		{
			char szValue[MAX_PATH]="";
			StringCbPrintf( szValue, sizeof( szValue ), "value_%d", i+1 );
			DWORD dwValue = kLoader.LoadInt( szValue, 0 );
			
			ZeroMemory( szValue, sizeof(szValue) );

			m_vCharacterCashBack.push_back( dwValue );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]ItemCashBack Character Value : %d", dwValue );	
		}

		//치장
		kLoader.SetTitle( "Deco" );
		iMaxCount = kLoader.LoadInt( "max_count", 0 );

		for(int i = 0; i< iMaxCount; i++ )
		{
			char szValue[MAX_PATH]="";
			StringCbPrintf( szValue, sizeof( szValue ), "value_%d", i+1 );
			DWORD dwValue = kLoader.LoadInt( szValue, 0 );

			ZeroMemory( szValue, sizeof(szValue) );

			m_vDecoCashBack.push_back( dwValue );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]ItemCashBack Deco Value : %d", dwValue );	
		}

		//특별
		kLoader.SetTitle( "ETC" );
		iMaxCount = kLoader.LoadInt( "max_count", 0 );

		for(int i = 0; i< iMaxCount; i++ )
		{
			char szValue[MAX_PATH]="";
			StringCbPrintf( szValue, sizeof( szValue ), "value_%d", i+1 );
			DWORD dwValue = kLoader.LoadInt( szValue, 0 );

			ZeroMemory( szValue, sizeof(szValue) );

			m_vETCCashBack.push_back( dwValue );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]ItemCashBack ETC Value : %d", dwValue );	
		}

		//장비
		kLoader.SetTitle( "Extra" );
		iMaxCount = kLoader.LoadInt( "max_count", 0 );

		for(int i = 0; i< iMaxCount; i++ )
		{
			char szValue[MAX_PATH]="";
			StringCbPrintf( szValue, sizeof( szValue ), "value_%d", i+1 );
			DWORD dwValue = kLoader.LoadInt( szValue, 0 );

			ZeroMemory( szValue, sizeof(szValue) );

			m_vExtraCashBack.push_back( dwValue );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]ItemCashBack Extra Value : %d", dwValue );	
		}

	}
	////////

	if( isChannelingCashBackOpen() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]CashBack Start Date : %d-%d-%d:%d",wCashBackStartYear, wCashBackStartMonth, wCashBackStartDay, wCashBackStartHour );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]CashBack End Date : %d-%d-%d:%d",wLosaEndYear, wLosaEndMonth, wLosaEndDay, wLosaEndHour );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]CashBack mileage_ratio : %d", m_iLosaMileageRatio  );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]CashBack Event Open, cashBack_ratio : %d, cashBack_Period", m_iCashBackRatio, m_iCashBackPeriod  );
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]LosaDay Start Date : %d-%d-%d:%d",wLosaStartYear, wLosaStartMonth, wLosaStartDay, wLosaStartHour );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]LosaDay End Date : %d-%d-%d:%d",wLosaEndYear, wLosaEndMonth, wLosaEndDay, wLosaEndHour );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][event]LosaDay mileage_ratio : %d", m_iLosaMileageRatio  );


	m_iMaxDBValue = kLoader.LoadInt( "max_db_value", 0);
	EventLOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Max DB Value : %d", m_iMaxDBValue );

	int iMaxEvent = kLoader.LoadInt( "max_event", 0);
	for (int i = 0; i < iMaxEvent ; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf( szTitle, sizeof( szTitle ), "event_%d", i + 1 );
		kLoader.SetTitle( szTitle );
		if( kLoader.LoadInt( "active", 0 ) == 1 )
		{
			EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Event%d is active. ", i + 1 );

			int iType = kLoader.LoadInt( "type", 0 );
			/*if( bCreateLoad && IsExist( (EventType) iType ) )
			{
				EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error - Event Duplication type :%d", iType);
				continue;
			}*/ //같은 타입의 이벤트가 들어올 경우 처리x, 몬스터 카드, 모드별 경험치, 페소 지급을 위해 같은 타입이멘트 생성이유로 주석. 유영재
			
			EventNode *pNode = NULL;

			if( bCreateLoad )
			{
				switch( iType )
				{
				case EVT_PLAZA_MONSTER:
					{
						pNode =	new ioPlazaMonsterEventNode;
					}
					break;
				case EVT_PRESENT_BUY:
					{
						pNode = new PresentBuyEventNode;
					}
					break;
				case EVT_CUMULATIVE_SALE:
					{
						pNode = new ioCumulativeSaleEventNode;
					}
					break;
				case EVT_PRESENT_BUY_2:
					{
						pNode = new PresentBuyEventNode_2;
					}
					break;

				default:
					{
						pNode =	new EventNode;
					}
					break;
				}
			}
			else
			{
				int iModeCategory = 0;
				iModeCategory = kLoader.LoadInt( "mode_category", 0 );

				pNode = GetNode( (EventType) iType, iModeCategory );
			}

			if( pNode )
				pNode->LoadINI( kLoader, bCreateLoad );

			if( bCreateLoad )
				m_EventNodeVec.push_back( pNode );
		}
		else
			EventLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Event%d is inactive. ", i + 1 );
	}
}

void ioEventManager::CheckAlive( bool bSend /*= true*/ )
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	m_dwCurrentDate = ( st.wYear*10000 ) + ( st.wMonth*100 ) + st.wDay;
	
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
	    (*it)->CheckAlive( st, bSend );
	}
}

void ioEventManager::Update(int iValues[], int iValueCount)
{
	EventNode *pNode = GetNode((EventType)iValues[1]);
	if(!pNode)
	{
		pNode = new EventNode;
		m_EventNodeVec.push_back( pNode );
	}

	pNode->Update(iValues, iValueCount);
}

ioEventManager& ioEventManager::GetSingleton()
{
	return Singleton<ioEventManager>::GetSingleton();
}

bool ioEventManager::IsExist( EventType eEventType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return true;
	}

	return false;
}

int ioEventManager::GetSize() const
{
	return m_EventNodeVec.size();
}

EventType ioEventManager::GetType( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_EventNodeVec.size() ) )
		return EVT_NONE;

	return m_EventNodeVec[iArray]->GetType();
}


ModeCategory ioEventManager::GetModeCategory( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_EventNodeVec.size() ) )
		return MC_DEFAULT;

	return (ModeCategory)m_EventNodeVec[iArray]->GetModeCategory();
}

void ioEventManager::Clear()
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		SAFEDELETE( *it );
	}
	m_EventNodeVec.clear();

	m_vCharacterCashBack.clear();
	m_vDecoCashBack.clear();
	m_vETCCashBack.clear();
	m_vExtraCashBack.clear();
}

void ioEventManager::ProcessTime()
{
	if(TIMEGETTIME() - m_dwCurrentTime < 60000) return; // 1분 확인

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	CheckAlive();
	
	m_dwCurrentTime = TIMEGETTIME();
}

bool ioEventManager::IsAlive( EventType eEventType, ChannelingType eChannelingType, ModeCategory eModeCategory )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
		{
			if( eModeCategory == (*it)->GetModeCategory() )
				return (*it)->IsAlive( eChannelingType );
		}
	}

	return false;
}

int ioEventManager::GetValue( EventType eEventType, int iArray, ModeCategory eModeCategory )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
		{
			if( eModeCategory == (*it)->GetModeCategory() )
				return (*it)->GetValue( iArray );
		}
	}

	return 0;
}

int ioEventManager::GetValueSize( EventType eEventType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->GetValueSize();
	}

	return 0;
}

ChannelingType ioEventManager::GetUseChannelingType( EventType eEventType, int iArray )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->GetUseChannelingType( iArray );
	}

	return CNT_NONE;
}

int ioEventManager::GetUseChannelingTypeSize( EventType eEventType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->GetUseChannelingTypeSize();
	}

	return 0;
}

EventNode * ioEventManager::GetNode( EventType eEventType, int iModeCategory )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
		{
			if( 0 != iModeCategory )
			{
				if( (*it)->GetModeCategory() == iModeCategory )
					return (*it);
			}
			else
				return (*it);
		}
	}

	return NULL;
}

int ioEventManager::GetStartDate( EventType eEventType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->GetStartDate();
	}

	return 0;
}

int ioEventManager::GetEndDate( EventType eEventType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->GetEndDate();
	}
	return 0;
}

bool ioEventManager::IsEventTime( const EventType eEventType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->IsEventTime();
	}
	return false;
}

void ioEventManager::NotifySpecificEventInfoWhenLogin(User* pUser)
{
	if( !pUser )
		return;
	
	int iSize = m_EventNodeVec.size();

	for( int i = 0; i < iSize; i++ )
	{
		EventNode* pNode = static_cast< EventNode* > (m_EventNodeVec[i]);
		if( !pNode )
			continue;
		if( pNode->GetType() == EVT_PRESENT_BUY || pNode->GetType() ==  EVT_PRESENT_BUY_2 )
		{
			SP2Packet kPacket(STPK_EVENT_ALIVE);
			PACKET_GUARD_VOID( kPacket.Write((int)pNode->GetType()) );
			PACKET_GUARD_VOID( kPacket.Write(pNode->IsAlive()) );
			pUser->SendMessage( kPacket );
		}
		if(m_EventNodeVec[i]->GetType() ==  EVT_CUMULATIVE_SALE)
		{
			ioCumulativeSaleEventNode* pNode = static_cast< ioCumulativeSaleEventNode* > (m_EventNodeVec[i]);
			if( pNode )
			{
				SP2Packet kPacket(STPK_EVENT_ALIVE);
				PACKET_GUARD_VOID( kPacket.Write((int)pNode->GetType()) );
				PACKET_GUARD_VOID( kPacket.Write(pNode->IsAlive()) );
				pUser->SendMessage( kPacket );
				continue;
			}
		}
	}
}

//아이템 구매시 캐시백 가능 여부 파악
bool ioEventManager::GetItemCashBackValue( DWORD dwType, DWORD dwValue )
{
	switch( dwType )
	{

	case PRESENT_SOLDIER:
		{
			for(CharacterCashBack::iterator iter = m_vCharacterCashBack.begin(); iter != m_vCharacterCashBack.end(); ++iter)
			{
				if( (*iter) == dwValue )
				{
					return true;
				}
			}
			return false;
		}
		
	case PRESENT_DECORATION:
		{
			for(DecoCashBack::iterator iter = m_vDecoCashBack.begin(); iter != m_vDecoCashBack.end(); ++iter)
			{
				if( (*iter) == dwValue )
				{
					return true;
				}
			}
			return false;
		}
	case PRESENT_ETC_ITEM:
		{
			for(ETCCashBack::iterator iter = m_vETCCashBack.begin(); iter != m_vETCCashBack.end(); ++iter)
			{
				if( (*iter) == dwValue )
				{
					return true;
				}
			}
			return false;
		}
	case PRESENT_EXTRAITEM:
		{
			for(ExtraCashBack::iterator iter = m_vExtraCashBack.begin(); iter != m_vExtraCashBack.end(); ++iter)
			{
				if( (*iter) == dwValue )
				{
					return true;
				}
			}
			return false;
		}
	default:
		return false;
	}
	return false;
}