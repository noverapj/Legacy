#include "stdafx.h"
#include "ioRecvPacketLogManager.h"

ioRecvInfoList::ioRecvInfoList()
{
	m_RecvList.clear();
}

ioRecvInfoList::~ioRecvInfoList()
{
	DeleteAll();
}

void ioRecvInfoList::AddInfo( ioHashString szName, DWORD Time )
{
	//정보 추가
	RecvPacketInfo* sInfo = new RecvPacketInfo;
	//정보 초기화
	sInfo->Init( szName, Time, Time );
	m_RecvList.push_back( sInfo );			
}

ioRecvInfoList::RecvPacketInfo* ioRecvInfoList::FindInfo( ioHashString szName )
{
	//비었다면 종료
	if ( m_RecvList.empty() )
		return NULL;

	//모두 돌면서
	InfoList::iterator Iter = m_RecvList.begin();
	for ( ; Iter!=m_RecvList.end() ; ++Iter)
	{
		RecvPacketInfo* pRecvInfo = *Iter;
		//같은 이름이면 반환
		if ( pRecvInfo && pRecvInfo->GetName() == szName )
		{
			return pRecvInfo;
		}
	}
	return NULL;
}

void ioRecvInfoList::PrintInfo( DWORD dwColor, ioHashString szCharName, int eType, ioHashString szRecvName, bool bReset )
{
	//비었다면 종료
	if ( m_RecvList.empty() )
		return;

	//모두 돌면서
	InfoList::iterator Iter = m_RecvList.begin();
	for ( ; Iter!=m_RecvList.end() ; ++Iter)
	{
		RecvPacketInfo* pRecvInfo = *Iter;
		//정보가 있고, 이름이 비었거나, 같으면서, 카운트가 0보다 크면 출력
		if ( pRecvInfo && ( szRecvName.IsEmpty() || pRecvInfo->GetName() == szRecvName ) && pRecvInfo->GetCnt() > 0  )
		{			
			char buf[6][MAX_PATH];
			char total[MAX_PATH];

			//캐릭터 이름, 타입, 받은 이름, 횟수, 시작시간, 받은 시간
			wsprintf_e( buf[0], "CharName : %s", szCharName );
			wsprintf_e( buf[1], "Type : %d", eType );
			wsprintf_e( buf[2], "RecvName : %s", pRecvInfo->GetName() );
			wsprintf_e( buf[3], "Cnt : %d", pRecvInfo->GetCnt() );
			wsprintf_e( buf[4], "StartTime : %d", pRecvInfo->GetStartTime() );
			wsprintf_e( buf[5], "RecvTime : %d", pRecvInfo->GetRecvTime() );
			wsprintf_e( total,"%s, %s, %s, %s, %s, %s", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5] );

			ChatMsg( dwColor, g_MyInfo.GetPublicID().c_str(), total );
			
			//정보 리셋
			if ( bReset )
			{
				pRecvInfo->Reset();
			}
		}
	}
}

void ioRecvInfoList::ChatMsg( DWORD dwColor, const char *szID, const char *szChat )
{
	g_ChatMgr.SetRecvChatMsg( dwColor, szID, szChat );
}

void ioRecvInfoList::DeleteAll()
{
	//비었다면 종료
	if ( m_RecvList.empty() )
		return;

	//모두 돌면서 삭제
	InfoList::iterator Iter = m_RecvList.begin();
	for ( ; Iter!=m_RecvList.end() ; ++Iter)
	{
		RecvPacketInfo* pRecvInfo = *Iter;
		if (pRecvInfo)
		{
			SAFEDELETE( pRecvInfo );
		}
	}
	m_RecvList.erase( m_RecvList.begin(), m_RecvList.end() );
	m_RecvList.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////
template<> ioRecvPacketLogManager* Singleton< ioRecvPacketLogManager >::ms_Singleton = 0;

ioRecvPacketLogManager::ioRecvPacketLogManager()
{
}

ioRecvPacketLogManager::~ioRecvPacketLogManager()
{
	CleanRecvInfo();
}

ioRecvPacketLogManager& ioRecvPacketLogManager::GetSingleton()
{
	return Singleton< ioRecvPacketLogManager >::GetSingleton();
}

void ioRecvPacketLogManager::AddCharRecvInfo( ioHashString szCharName, RecvPacketType eType, ioHashString szRecvName )
{
	//비어있다면 종료
	if ( szCharName.IsEmpty() || szRecvName.IsEmpty() || eType >= RPT_MAX )
		return;

	//캐릭터 검색
	TotalInfoList::iterator First_Find_Iter = m_TotalInfoList.find( szCharName );

	//캐릭터 이름을 못 찾았다면 리스트에 추가
	if ( First_Find_Iter == m_TotalInfoList.end() )
	{
		TypeList* pTypeList = new TypeList;
		pTypeList->clear();

		for ( int i = 0; i<RPT_MAX; ++i )
		{
			//타입 별로 리스트 추가
			ioRecvInfoList* pRecvInfoList = new ioRecvInfoList;		
			pTypeList->insert( std::make_pair( (RecvPacketType)i, pRecvInfoList ) );
		}

		//캐릭터 리스트 추가
		m_TotalInfoList.insert( std::make_pair( szCharName, pTypeList) );
		
		//캐릭터 다시 검색
		First_Find_Iter = m_TotalInfoList.find( szCharName );	
	}
	
	//캐릭터가 있다면
	//해당 캐릭터의 패킷 리스트를 얻고
	TypeList* pTypeList = First_Find_Iter->second;
	if ( pTypeList )
	{
		//해당 타입의 리스트를 얻고 (캐릭터 리스트가 있다면 타입별 리스트는 존재함)
		TypeList::iterator Second_Find_Iter = pTypeList->find( eType );
			
		ioRecvInfoList* pRecvInfoList = Second_Find_Iter->second;
		if ( pRecvInfoList )
		{
			DWORD RecvTime = FRAMEGETTIME();
			ioRecvInfoList::RecvPacketInfo* pInfo = pRecvInfoList->FindInfo( szRecvName );
			//찾았다면 증가 시키고 종료
			if ( pInfo )
			{
				if ( pInfo->GetStartTime() == 0)
				{
					pInfo->SetStartTime( RecvTime );
				}
				pInfo->AddCnt( RecvTime );
				return;
			}
			//못찾으면 새로 생성
			pRecvInfoList->AddInfo( szRecvName, RecvTime );			
		}
	}
}

ioRecvInfoList::RecvPacketInfo* ioRecvPacketLogManager::FindCharRecvInfo( ioHashString szCharName, RecvPacketType eType, ioHashString szRecvName )
{
	//비어있다면 종료
	if ( szCharName.IsEmpty() || szRecvName.IsEmpty() || eType >= RPT_MAX || m_TotalInfoList.empty() )
		return NULL;

	//캐릭터 검색
	TotalInfoList::iterator First_Find_Iter = m_TotalInfoList.find( szCharName );

	//찾았다면
	if ( First_Find_Iter != m_TotalInfoList.end() )
	{
		//캐릭터의 리스트를 얻고
		TypeList* pTypeList = First_Find_Iter->second;
		if ( pTypeList )
		{
			//해당 타입의 리스트를 얻고 (캐릭터 리스트가 있다면 타입별 리스트는 존재함)
			TypeList::iterator Second_Find_Iter = pTypeList->find( eType );

			ioRecvInfoList* pRecvInfoList = Second_Find_Iter->second;
			if ( pRecvInfoList )
			{
				return pRecvInfoList->FindInfo( szRecvName );
			}
		}
	}

	return NULL;
}

void ioRecvPacketLogManager::PrintCharRecvInfo( DWORD dwColor, ioHashString szCharName, RecvPacketType eType, ioHashString szRecvName,  bool bReset )
{
	//비어있다면 종료
	if ( szCharName.IsEmpty() || eType >= RPT_MAX || m_TotalInfoList.empty() )
		return;

	//캐릭터 검색
	TotalInfoList::iterator First_Find_Iter = m_TotalInfoList.find( szCharName );

	//찾았다면
	if ( First_Find_Iter != m_TotalInfoList.end() )
	{
		//캐릭터의 리스트를 얻고
		TypeList* pTypeList = First_Find_Iter->second;
		if ( pTypeList )
		{
			//해당 타입의 리스트를 얻고 (캐릭터 리스트가 있다면 타입별 리스트는 존재함)
			TypeList::iterator Second_Find_Iter = pTypeList->find( eType );

			ioRecvInfoList* pRecvInfoList = Second_Find_Iter->second;
			//있다면 출력
			if ( pRecvInfoList )
			{
				pRecvInfoList->PrintInfo( dwColor, szCharName, (int)eType, szRecvName, bReset );
			}
		}
	}
}

void ioRecvPacketLogManager::PrintChatLog( DWORD dwColor, ioHashString szCharName, ioHashString szLog )
{
	g_ChatMgr.SetRecvChatMsg( dwColor, szCharName.c_str(), szLog.c_str() );
}

void ioRecvPacketLogManager::CleanRecvInfo()
{
	//비어있다면 종료
	if ( m_TotalInfoList.empty() )
	{
		return;
	}

	//모든 캐릭터 리스트를 돌면서
	TotalInfoList::iterator First_Iter = m_TotalInfoList.begin();
	for ( ; First_Iter!=m_TotalInfoList.end() ; )
	{
		//캐릭터의 타입 리스트를 얻고
		TypeList* pTypeList = First_Iter->second;
		if ( pTypeList )
		{
			//캐릭터의 모든 타입별 리스트를 돌면서
			TypeList::iterator Second_Iter = pTypeList->begin();
			for ( ; Second_Iter!=pTypeList->end() ; )
			{
				//받은 패킷의 정보 리스트를 돌면서
				ioRecvInfoList* pRecvInfoList = Second_Iter->second;
				if ( pRecvInfoList )
				{
					//지움
					SAFEDELETE( pRecvInfoList );
				}
				//해당 캐릭터의 타입별 리스트에서 비움
				Second_Iter = pTypeList->erase( Second_Iter );
			}
			pTypeList->clear();
			SAFEDELETE( pTypeList );
		}
		//모든 캐릭터 리스트에서 비움
		First_Iter = m_TotalInfoList.erase( First_Iter );
	}

	//모두 비움
	m_TotalInfoList.clear();
}
