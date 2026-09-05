#include "stdafx.h"

#include "ioEventManager.h"

EventNode::EventNode()
{
	m_bAlive         = false;
	m_eEventType     = EVT_NONE;
	m_dwAnnounceTime = 0;
}

EventNode::~EventNode()
{
	m_ValueVec.clear();
	m_vUseChannelingTypeVec.clear();
	m_kAnnounceInfo.Clear();
}

void EventNode::LoadINI( ioINILoader &a_rkLoader )
{
	m_kAnnounceInfo.Clear();

	m_kAnnounceInfo.m_dwLoopAnnounceDelayTime =  a_rkLoader.LoadInt_e( "loop_delay_time", 0);
	m_kAnnounceInfo.m_bShutDownUser = a_rkLoader.LoadBool_e( "shut_down_user", false );

	enum { MAX_LOOP = 10, };
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char szKeyName[MAX_PATH]="";
		char szBuff[MAX_PATH]="";

		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "start_announce_%d", i+1 );
		a_rkLoader.LoadString( szKeyName, "", szBuff, MAX_PATH );
		if( strcmp( szBuff, "" ) != 0 )
		{
			ioHashString szTempBuff = szBuff;
			m_kAnnounceInfo.m_vStartAnnounceVec.push_back( szTempBuff );
		}

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuff, sizeof( szBuff ) );

		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "start_color_%d", i+1 );
		a_rkLoader.LoadString( szKeyName, "", szBuff, MAX_PATH );
		if( strcmp( szBuff, "" ) != 0 )
		{
			char *pTemp = NULL;
			DWORD dwColor = strtoul(szBuff, &pTemp, 16);
			m_kAnnounceInfo.m_vStartColorVec.push_back( dwColor );
		}

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuff, sizeof( szBuff ) );

		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "loop_announce_%d", i+1 );
		a_rkLoader.LoadString( szKeyName, "", szBuff, MAX_PATH );
		if( strcmp( szBuff, "" ) != 0 )
		{
			ioHashString szTempBuff = szBuff;
			m_kAnnounceInfo.m_vLoopAnnounceVec.push_back( szTempBuff );
		}

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuff, sizeof( szBuff ) );

		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "loop_color_%d", i+1 );
		a_rkLoader.LoadString( szKeyName, "", szBuff, MAX_PATH );
		if( strcmp( szBuff, "" ) != 0 )
		{
			char *pTemp = NULL;
			DWORD dwColor = strtoul(szBuff, &pTemp, 16);
			m_kAnnounceInfo.m_vLoopColorVec.push_back( dwColor );
		}

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuff, sizeof( szBuff ) );

		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "end_announce_%d", i+1 );
		a_rkLoader.LoadString( szKeyName, "", szBuff, MAX_PATH );
		if( strcmp( szBuff, "" ) != 0 )
		{
			ioHashString szTempBuff = szBuff;
			m_kAnnounceInfo.m_vEndAnnounceVec.push_back( szTempBuff );
		}

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuff, sizeof( szBuff ) );

		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "end_color_%d", i+1 );
		a_rkLoader.LoadString( szKeyName, "", szBuff, MAX_PATH );
		if( strcmp( szBuff, "" ) != 0 )
		{
			char *pTemp = NULL;
			DWORD dwColor = strtoul(szBuff, &pTemp, 16);
			m_kAnnounceInfo.m_vEndColorVec.push_back( dwColor );
		}

		AnnounceTimeInfo kInfo;

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "week_on_off_%d", i+1 );
		kInfo.m_dwWeekOnOff = a_rkLoader.LoadInt( szKeyName, 0);
		if( kInfo.m_dwWeekOnOff == 0 )
			continue;

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "start_hour_min_%d", i+1 );
		kInfo.m_wStartHourMin = a_rkLoader.LoadInt( szKeyName, 0);

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "end_hour_min_%d", i+1 );
		kInfo.m_wEndHourMin = a_rkLoader.LoadInt( szKeyName, 0);

		m_kAnnounceInfo.m_vTimeInfoVec.push_back( kInfo );
	}
}

int EventNode::GetValue( int iArray )
{
	if( !COMPARE( iArray, 0, (int) m_ValueVec.size() ) )
		return 0;

	return m_ValueVec[iArray];
}

EventType EventNode::GetType() const
{
	return m_eEventType;
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

void EventNode::SetValue( int iArray, int iValue  )
{
	if( COMPARE( iArray, 0, (int) m_ValueVec.size() ) )
		m_ValueVec[iArray] = iValue;
}

void EventNode::SetValueSize( int iSize )
{
	m_ValueVec.clear();
	for (int i = 0; i < iSize ; i++)
	{
		int iValue = 0;
		m_ValueVec.push_back( iValue );
	}
}

void EventNode::SetType( EventType eEventType )
{
	m_eEventType = eEventType;
}

void EventNode::SetAlive( bool bAlive )
{
	m_bAlive = bAlive;
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

void EventNode::SetUseChannelingType( int iArray, ChannelingType eType )
{
	if( COMPARE( iArray, 0, (int) m_vUseChannelingTypeVec.size() ) )
		m_vUseChannelingTypeVec[iArray] = eType;
}

void EventNode::SetUseChannelingTypeSize( int iSize )
{
	m_vUseChannelingTypeVec.clear();
	for (int i = 0; i < iSize ; i++)
	{
		ChannelingType eType = CNT_NONE;
		m_vUseChannelingTypeVec.push_back( eType );
	}
}

void EventNode::Process( int iWeek , int iHour, int iMin )
{
	if( !m_bAlive ) return;
		
	int iCurrentHourMin = ( iHour*100 ) + iMin;
	int iSize = m_kAnnounceInfo.m_vTimeInfoVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		AnnounceTimeInfo &Info = m_kAnnounceInfo.m_vTimeInfoVec[i];

		if( m_kAnnounceInfo.m_bShutDownUser )
		{
			if( !g_MyInfo.IsShutDownUser() )
				continue;
		}
		
		if( !Info.IsEventWeek( iWeek ) )
			continue;

		if( Info.m_eAnnounceType == AT_START )
		{
			if( !COMPARE( iCurrentHourMin, Info.m_wStartHourMin, Info.m_wEndHourMin ) )
				continue;

			enum { START_ANN_MIN = 5, };
			if( ( Info.m_wStartHourMin-iCurrentHourMin ) < START_ANN_MIN ) // 이벤트 시작 시간부터 4분59초까지는 안내멘트 진행
			{
				Info.m_eAnnounceType = AT_END;
				m_dwAnnounceTime = 0;

				int iAnnounceSize = m_kAnnounceInfo.m_vStartAnnounceVec.size();
				for (int j = 0; j < iAnnounceSize ; j++)
				{
					ioHashString sAnnounce = m_kAnnounceInfo.m_vStartAnnounceVec[j];
					if( sAnnounce.IsEmpty() )
						continue;
					DWORD dwColor = 0xFFE5E5E5; // TCT_DEFAULT_IVORY;
					int iColorSize = m_kAnnounceInfo.m_vStartColorVec.size();
					for (int k = 0; k < iColorSize ; k++)
					{
						if( j == k )
							dwColor = m_kAnnounceInfo.m_vStartColorVec[k];
					}
					g_ChatMgr.SetEventMsg( sAnnounce.c_str(), dwColor );	
				}
			}
			else
			{
				Info.m_eAnnounceType = AT_END;
				m_dwAnnounceTime = 0;
			}
		}
		else if( Info.m_eAnnounceType == AT_END )
		{
			if( !COMPARE( iCurrentHourMin, Info.m_wStartHourMin, Info.m_wEndHourMin ) )
			{
				Info.m_eAnnounceType = AT_START;

				int iAnnounceSize = m_kAnnounceInfo.m_vEndAnnounceVec.size();
				for (int j = 0; j < iAnnounceSize ; j++)
				{
					ioHashString sAnnounce = m_kAnnounceInfo.m_vEndAnnounceVec[j];
					if( sAnnounce.IsEmpty() )
						continue;
					DWORD dwColor = 0xFFE5E5E5; // TCT_DEFAULT_IVORY;
					int iColorSize = m_kAnnounceInfo.m_vEndColorVec.size();
					for (int k = 0; k < iColorSize ; k++)
					{
						if( j == k )
							dwColor = m_kAnnounceInfo.m_vEndColorVec[k];
					}
					g_ChatMgr.SetEventMsg( sAnnounce.c_str(), dwColor );	
				}
			}
			else // LOOP
			{
				if( m_dwAnnounceTime == 0 )
					m_dwAnnounceTime = REALGETTIME();

				if( m_kAnnounceInfo.m_dwLoopAnnounceDelayTime == 0 )
					continue;

				if( REALGETTIME() - m_dwAnnounceTime <= m_kAnnounceInfo.m_dwLoopAnnounceDelayTime )
					continue;
				
				m_dwAnnounceTime = REALGETTIME();
				int iAnnounceSize = m_kAnnounceInfo.m_vLoopAnnounceVec.size();
				for (int j = 0; j < iAnnounceSize ; j++)
				{
					ioHashString sAnnounce = m_kAnnounceInfo.m_vLoopAnnounceVec[j];
					if( sAnnounce.IsEmpty() )
						continue;
					DWORD dwColor = 0xFFE5E5E5; // TCT_DEFAULT_IVORY;
					int iColorSize = m_kAnnounceInfo.m_vLoopColorVec.size();
					for (int k = 0; k < iColorSize ; k++)
					{
						if( j == k )
							dwColor = m_kAnnounceInfo.m_vLoopColorVec[k];
					}
					g_ChatMgr.SetEventMsg( sAnnounce.c_str(), dwColor );	
				}
			}
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------
template<> ioEventManager* Singleton< ioEventManager >::ms_Singleton = 0;

ioEventManager::ioEventManager(void)
{
	m_iMaxDBValue   = 0;
	m_dwProcessTime = 0;
	m_bIsUnablePresentBonus = false;
}

ioEventManager::~ioEventManager( void )
{
	Clear();
}

ioEventManager& ioEventManager::GetSingleton()
{
	return Singleton<ioEventManager>::GetSingleton();
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

void ioEventManager::Clear()
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		SAFEDELETE( *it );
	}
	m_EventNodeVec.clear();

	m_vUnablePresentInfo.clear();
}

bool ioEventManager::IsAlive( EventType eEventType, ChannelingType eChannelingType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->IsAlive( eChannelingType );
	}

	return false;
}

void ioEventManager::SetValue( EventType eEventType, int iArray, int iValue )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->SetValue( iArray, iValue );
	}
}

void ioEventManager::SetSize( int iSize )
{
	for (int i = 0; i < iSize ; i++)
	{
		EventNode *pNode = new EventNode;
		m_EventNodeVec.push_back( pNode );	
	}
}

void ioEventManager::SetValueSize( EventType eEventType, int iSize )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			(*it)->SetValueSize( iSize );
	}
}

void ioEventManager::SetEventType( int iArray , EventType eEventType )
{
	if( COMPARE( iArray, 0, (int) m_EventNodeVec.size() ) )
		m_EventNodeVec[iArray]->SetType( eEventType );
}

void ioEventManager::SetAlive( EventType eEventType, bool bAlive )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			(*it)->SetAlive( bAlive );
	}
}

int ioEventManager::GetValue( EventType eEventType, int iArray )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			return (*it)->GetValue( iArray );
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

void ioEventManager::SetUseChannelingType( EventType eEventType, int iArray, ChannelingType eType )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			(*it)->SetUseChannelingType( iArray, eType );
	}
}

void ioEventManager::SetUseChannelingTypeSize( EventType eEventType, int iSize )
{
	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		if( eEventType == (*it)->GetType() )
			(*it)->SetUseChannelingTypeSize( iSize );
	}
}

void ioEventManager::Process()
{
	enum { DELAY_TIME = 60000, }; // 1분

	if( REALGETTIME() - m_dwProcessTime <= DELAY_TIME )
		return;
	m_dwProcessTime = REALGETTIME();

	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	struct tm *pDate = localtime(&kCurServerTime);
	if( !pDate ) return;

	for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
	{
		(*it)->Process( pDate->tm_wday, pDate->tm_hour, pDate->tm_min );
	}
}

void ioEventManager::LoadINI()
{
	if( m_EventNodeVec.empty() )
		return;

	ioINILoader_e kLoader( "config/sp2_event.ini");
	kLoader.SetTitle_e( "announce_common" );
	int iMax = kLoader.LoadInt( "max", 0);

	for (int i = 0; i < iMax ; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf_e( szTitle, sizeof( szTitle ), "announce_%d", i + 1 );
		kLoader.SetTitle( szTitle );

		int iEventType = kLoader.LoadInt_e( "event_type", 0);
		for(EventNodeVec::iterator it = m_EventNodeVec.begin(); it != m_EventNodeVec.end(); ++it)
		{
			if( iEventType == (*it)->GetType() )
				(*it)->LoadINI( kLoader );
		}
	}
}

void ioEventManager::SetPresentBonus( int iEventType, bool bUnablePresentBonus )
{
	for( int i=0; i<(int)m_vUnablePresentInfo.size(); ++i )
	{
		if( m_vUnablePresentInfo[i].event_type == iEventType )
		{
			m_vUnablePresentInfo[i].is_unable_present_bonus = bUnablePresentBonus;
			return;
		}
	}

	m_vUnablePresentInfo.push_back( UnablePresentInfo(iEventType, bUnablePresentBonus) );
}

bool ioEventManager::IsPresentBonus( int iEventType )
{
	for( int i=0; i<(int)m_vUnablePresentInfo.size(); ++i )
	{
		if( m_vUnablePresentInfo[i].event_type == iEventType )
			return m_vUnablePresentInfo[i].is_unable_present_bonus;
	}

	return false;
}