
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../EtcHelpFunc.h"
#include "../Util/cSerialize.h"
#include "User.h"

#include "ioClassExpert.h"
#include "ioMyLevelMgr.h"


ioClassExpert::ioClassExpert()
{
	Initialize( NULL );
}

ioClassExpert::~ioClassExpert()
{
	m_vClassList.clear();
	m_vCopyClassList.clear();
}

void ioClassExpert::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_vClassList.clear();
	m_vCopyClassList.clear();
}

void ioClassExpert::InsertDBClass( CLASSDB &kClassDB )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioClassExpert::InsertDBClass() User NULL!!"); 
		return;
	}

	std::vector<int> contents;
	contents.reserve(MAX_SLOT * 3);
	for(int i = 0;i < MAX_SLOT;i++)
	{
		contents.push_back( kClassDB.m_Data[i].m_iType );
		contents.push_back( kClassDB.m_Data[i].m_iLevel );
		contents.push_back( kClassDB.m_Data[i].m_iExpert );
		contents.push_back( kClassDB.m_Data[i].m_byReinforce );
	}

	// DB Insert
	g_DBClient.OnInsertClassExpert( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), contents );
}

bool ioClassExpert::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioClassExpert::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vCLASSDB::iterator iter, iEnd;
		iEnd = m_vClassList.end();
		for(iter = m_vClassList.begin();iter != iEnd;iter++)
		{
			CLASSDB &kClassDB = *iter;
			if( kClassDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioClassExpert::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vCLASSDB::iterator iter, iEnd;
		iEnd = m_vClassList.end();
		for(iter = m_vClassList.begin();iter != iEnd;iter++)
		{
			CLASSDB &kClassDB = *iter;
			if( kClassDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectClassExpertIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioClassExpert::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vCLASSDB::iterator iter, iEnd;
		iEnd = m_vClassList.end();
		for(iter = m_vClassList.begin();iter != iEnd;iter++)
		{
			CLASSDB &kClassDB = *iter;
			if( kClassDB.m_dwIndex == NEW_INDEX )
			{
				kClassDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioClassExpert::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}
	return false;
}

void ioClassExpert::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioClassExpert::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		CLASSDB kClassDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kClassDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_BREAK( query_data->GetValue( kClassDB.m_Data[i].m_iType, sizeof(int) )  );
			PACKET_GUARD_BREAK( query_data->GetValue( kClassDB.m_Data[i].m_iLevel, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kClassDB.m_Data[i].m_iExpert, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kClassDB.m_Data[i].m_byReinforce, sizeof(BYTE) ) );
		} 
		m_vClassList.push_back( kClassDB );
		//m_vCopyClassList.push_back( kClassDB );
	}	
	LOOP_GUARD_CLEAR();
	/*
	HRYOON 3월 29 일 서버 이동 버그 
	m_vClassList.clear();
	// 유저에게 전송하기 전에 같은 데이터는 삭제한다. 그리고 업데이트 한다. 
	vCLASSDB::iterator iter, iEnd;
	vCLASSDB::iterator iCopyter, iCopyEnd;

	iCopyEnd = m_vCopyClassList.end();
	iEnd = m_vCopyClassList.end();

	
	
	for(iter = m_vCopyClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		CLASSDB kClassCopyDB;

		int iRealLevel = 0, iRealExpert = 0;
		BYTE iRealStat = 0;
		
		for(int i = 0;i < MAX_SLOT;i++)
		{
			GetSameClass ( kClassDB.m_Data[i].m_iType, kClassDB.m_Data[i].m_iLevel, kClassDB.m_Data[i].m_byReinforce, kClassDB.m_Data[i].m_iExpert,  iRealLevel, iRealStat, iRealExpert );
			kClassDB.m_Data[i].m_iLevel = iRealLevel;
			kClassDB.m_Data[i].m_byReinforce = iRealStat;
			kClassDB.m_Data[i].m_iExpert = iRealExpert;
			
		}
		kClassDB.m_bChange = true;
		m_vClassList.push_back( kClassDB );


	}



	SaveData();
	*/
	
	// 유저에게 전송
	SP2Packet kPacket( STPK_CLASSEXPERT_SET ); 
	PACKET_GUARD_VOID ( kPacket.Write((int)m_vClassList.size() * MAX_SLOT) );
	{
		vCLASSDB::iterator iter, iEnd;
		iEnd = m_vClassList.end();
		for(iter = m_vClassList.begin();iter != iEnd;iter++)
		{
			CLASSDB &kClassDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				PACKET_GUARD_VOID( kPacket.Write(kClassDB.m_Data[i].m_iType) );
				PACKET_GUARD_VOID( kPacket.Write(kClassDB.m_Data[i].m_iLevel) );
				PACKET_GUARD_VOID( kPacket.Write(kClassDB.m_Data[i].m_iExpert) );
				PACKET_GUARD_VOID( kPacket.Write(kClassDB.m_Data[i].m_byReinforce) );
			}		
		}
	}	
	m_pUser->SendMessage( kPacket );
}

void ioClassExpert::SaveData()
{

	

	if( m_vClassList.empty() ) return;
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioClassExpert::SaveData() User NULL!!"); 
		return;
	}

	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		if( kClassDB.m_bChange )
		{
			cSerialize v_FT;
			v_FT.Write(kClassDB.m_dwIndex);
			for(int i = 0;i < MAX_SLOT;i++)
			{
				v_FT.Write(kClassDB.m_Data[i].m_iType);
				v_FT.Write(kClassDB.m_Data[i].m_iLevel);
				v_FT.Write(kClassDB.m_Data[i].m_iExpert);
				v_FT.Write(kClassDB.m_Data[i].m_byReinforce);
			}

			if( kClassDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveClassInfo(%s:%d) : None Index", m_pUser->GetPublicID().c_str(), kClassDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateClassExpert( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), v_FT );
				kClassDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveClassInfo(%s:%d)", m_pUser->GetPublicID().c_str(), kClassDB.m_dwIndex );
			}
		}		
	}

}

void ioClassExpert::CreateClass( int iClassType )
{
	if( iClassType <= 0 ) return;

	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	// 신규 용병 구매 빈 슬롯 확인.
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == 0 )
			{
				kClassDB.m_Data[i].m_iType	= iClassType;
				kClassDB.m_Data[i].m_iExpert	= 0;
				kClassDB.m_Data[i].m_iLevel	= 0;
				kClassDB.m_Data[i].m_byReinforce = 0;
				kClassDB.m_bChange = true;
				return;
			}
		}		
	}

	// 빈 슬롯 없다. 슬롯 추가
	CLASSDB kClassDB;
	kClassDB.m_dwIndex = NEW_INDEX;
	kClassDB.m_Data[0].m_iType	= iClassType;
	kClassDB.m_Data[0].m_iLevel	= 0;
	kClassDB.m_Data[0].m_iExpert	= 0;
	kClassDB.m_Data[0].m_byReinforce	= 0;
	m_vClassList.push_back( kClassDB );
	InsertDBClass( kClassDB );
}

int  ioClassExpert::GetClassLevel( int iClassType )
{
	vCLASSDB::iterator iter, iEnd; 
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == iClassType )
				return kClassDB.m_Data[i].m_iLevel;
		}		
	}
	return 0;
}
	// 2018-01-15 by bckim, 탐사 확장 //탐사
int  ioClassExpert::GetClassExp( int iClassType )
{
	vCLASSDB::iterator iter, iEnd; 
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == iClassType )
				return kClassDB.m_Data[i].m_iExpert;
		}		
	}
	return 0;
}
	// End. 2018-01-15 by bckim, 탐사 확장 //탐사

int  ioClassExpert::GetSameClass( int iClassType, int iLevel, BYTE iStat, int iExpert, int &iRealLevel, BYTE &iRealStat, int &RealiExpert )
{
	iRealLevel  = iLevel;
	iRealStat = iStat;
	RealiExpert = iExpert;
	

	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vCopyClassList.end();
	for(iter = m_vCopyClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			
			if( kClassDB.m_Data[i].m_iType == iClassType &&  kClassDB.m_Data[i].m_byReinforce == iStat && kClassDB.m_Data[i].m_iLevel == iLevel
				&& kClassDB.m_Data[i].m_iExpert == iExpert )
			{
				continue;
			}


			if( kClassDB.m_Data[i].m_iType == iClassType )
			{
				if ( kClassDB.m_Data[i].m_iLevel > iLevel  )
				{
					iRealLevel = kClassDB.m_Data[i].m_iLevel;
				}
				if ( kClassDB.m_Data[i].m_byReinforce > iStat  )
				{
					iRealStat = kClassDB.m_Data[i].m_byReinforce;
				}
				if ( kClassDB.m_Data[i].m_iExpert > iExpert  )
				{
					RealiExpert = kClassDB.m_Data[i].m_iExpert;
				}


				kClassDB.m_Data[i].m_iExpert = 0;
				kClassDB.m_Data[i].m_byReinforce = 0;
				kClassDB.m_Data[i].m_iLevel = 0;
				kClassDB.m_Data[i].m_iType = 0;

			}
				
		}		
	}
	return 0;
}


bool ioClassExpert::AddClassExp( int iClassType, int iExp, int &rRemainExp )
{
	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == iClassType )
			{
				kClassDB.m_bChange = true;
				int iNextExp = g_LevelMgr.GetNextLevelupExp( kClassDB.m_Data[i].m_iLevel );
				kClassDB.m_Data[i].m_iExpert += iExp;
				if( iNextExp <= kClassDB.m_Data[i].m_iExpert )
				{
					kClassDB.m_Data[i].m_iLevel++;
					rRemainExp = kClassDB.m_Data[i].m_iExpert - iNextExp;
					kClassDB.m_Data[i].m_iExpert = 0;
					return true;
				}				
				return false;
			}
		}		
	}

	//슬롯에 해당 용병이 없으면 Insert
	CreateClass( iClassType );
	return AddClassExp( iClassType, iExp, rRemainExp );
}

int  ioClassExpert::GetTopLevelClassType()
{
	int iClassType   = 1; // default 아이언
	int iClassLevel  = 0;
	int iClassExpert = 0;

	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == 0 ) continue;

			if( kClassDB.m_Data[i].m_iLevel > iClassLevel )
			{
				iClassType	 = kClassDB.m_Data[i].m_iType;
				iClassLevel	 = kClassDB.m_Data[i].m_iLevel;
				iClassExpert = kClassDB.m_Data[i].m_iExpert;
			}
			else if( kClassDB.m_Data[i].m_iLevel == iClassLevel )
			{
				if( kClassDB.m_Data[i].m_iExpert > iClassExpert )
				{
					iClassType	 = kClassDB.m_Data[i].m_iType;
					iClassLevel	 = kClassDB.m_Data[i].m_iLevel;
					iClassExpert = kClassDB.m_Data[i].m_iExpert;
				}
			}
		}		
	}
	return iClassType;
}

void ioClassExpert::FillMoveData( SP2Packet &rkPacket )
{
	rkPacket << (int)m_vClassList.size();

	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;

		PACKET_GUARD_VOID( rkPacket.Write(kClassDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Write(kClassDB.m_bChange) );

		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_VOID( rkPacket.Write(kClassDB.m_Data[i].m_iType) );
			PACKET_GUARD_VOID( rkPacket.Write(kClassDB.m_Data[i].m_iLevel) );
			PACKET_GUARD_VOID( rkPacket.Write(kClassDB.m_Data[i].m_iExpert) );
			PACKET_GUARD_VOID( rkPacket.Write(kClassDB.m_Data[i].m_byReinforce) );
		}
	}
}

void ioClassExpert::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for(int i = 0;i < iSize;i++)
	{
		CLASSDB kClassDB;
		PACKET_GUARD_VOID( rkPacket.Read(kClassDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Read(kClassDB.m_bChange) );

		for(int j = 0;j < MAX_SLOT;j++)
		{
			PACKET_GUARD_VOID( rkPacket.Read(kClassDB.m_Data[j].m_iType) );
			PACKET_GUARD_VOID( rkPacket.Read(kClassDB.m_Data[j].m_iLevel) );
			PACKET_GUARD_VOID( rkPacket.Read(kClassDB.m_Data[j].m_iExpert) );
			PACKET_GUARD_VOID( rkPacket.Read(kClassDB.m_Data[j].m_byReinforce) );
		}
		if( kClassDB.m_dwIndex == NEW_INDEX && !bDummyNode )
			g_DBClient.OnSelectClassExpertIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
		m_vClassList.push_back( kClassDB );
	}	
}

int ioClassExpert::GetClassReinfoce( const int iClassType )
{
	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == 0 ) continue;

			if( kClassDB.m_Data[i].m_iType == iClassType )
				return kClassDB.m_Data[i].m_byReinforce;
		}		
	}

	return 0;
}

bool ioClassExpert::SetClassReinforce( const int iClassType, const int iReinforce )
{
	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == 0 ) continue;

			if( kClassDB.m_Data[i].m_iType == iClassType )
			{
				kClassDB.m_Data[i].m_byReinforce = iReinforce;
				kClassDB.m_bChange = true;
				return true;
			}
		}
	}

	return false;
}

bool ioClassExpert::IsExistExpertInfo( const int iClassType )
{
	vCLASSDB::iterator iter, iEnd;
	iEnd = m_vClassList.end();
	for(iter = m_vClassList.begin();iter != iEnd;iter++)
	{
		CLASSDB &kClassDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassDB.m_Data[i].m_iType == 0 ) continue;

			if( kClassDB.m_Data[i].m_iType == iClassType )
				return true;
		}
	}

	return false;
}

void ioClassExpert::CreateClassExpertInfo( const int iClassType )
{
	CreateClass( iClassType );
}