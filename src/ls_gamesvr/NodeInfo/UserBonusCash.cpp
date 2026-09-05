#include "stdafx.h"
#include "BonusCashInfo.h"
#include "BonusCashManager.h"
#include "User.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../QueryData/QueryResultData.h"
#include "UserBonusCash.h"

UserBonusCash::UserBonusCash()
{
	Init(NULL);
}

UserBonusCash::~UserBonusCash()
{
	Destroy();
}

void UserBonusCash::Init(User* pUser)
{
	m_pUser	= pUser;

	int iSize	= m_vUserBonusCashTable.size();

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
			delete pInfo;
	}

	iSize	= m_vExpiredBonusCashTable.size();

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vExpiredBonusCashTable[i];
		if( pInfo )
			delete pInfo;
	}

	m_vUserBonusCashTable.clear();
	m_vExpiredBonusCashTable. clear();
}

void UserBonusCash::Destroy()
{
	int iSize	= m_vUserBonusCashTable.size();

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
			delete pInfo;
	}

	iSize	= m_vExpiredBonusCashTable.size();

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vExpiredBonusCashTable[i];
		if( pInfo )
			delete pInfo;
	}
}

void UserBonusCash::DBToExpiredData(CQueryResultData *query_data)
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserBonusCash::DBToExpiredData() User NULL!!"); 
		return;
	}

	while( query_data->IsExist() )
	{	
		DWORD dwTableIndex	= 0;
		int iTotal			= 0;
		int iRemaining		= 0;
		DBTIMESTAMP ExpirationDate;

		PACKET_GUARD_BREAK( query_data->GetValue( dwTableIndex, sizeof(dwTableIndex) ) );			// 테이블 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( iTotal, sizeof(iTotal) ) );						// 총 보너스 캐쉬
		PACKET_GUARD_BREAK( query_data->GetValue( iRemaining, sizeof(iRemaining) ) );				// 잔액
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&ExpirationDate, sizeof(DBTIMESTAMP) ) );	// 만료기간

		BonusCashInfo* pInfo = CreateCashData();
		if( pInfo )
		{
			pInfo->Create(dwTableIndex, iTotal, iRemaining, ExpirationDate);
			m_vExpiredBonusCashTable.push_back(pInfo);
		}
	}

	int iSize	= m_vExpiredBonusCashTable.size();

	SP2Packet kPacket(STPK_BONUS_CASH_INFO);
	PACKET_GUARD_VOID( kPacket.Write(BONUS_CASH_EXPIRATION) );
	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo = m_vExpiredBonusCashTable[i];
		if( pInfo )
		{
			PACKET_GUARD_VOID( kPacket.Write(pInfo->GetIndex()) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->GetTotalAmount()) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->GetRemainingAmount()) );
			PACKET_GUARD_VOID( kPacket.Write(pInfo->GetExpirationDate()) );
		}
		else
		{
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
		}
	}

	m_pUser->SendMessage(kPacket);
}

void UserBonusCash::DBToData(CQueryResultData *query_data, int dwSelectType)
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UserBonusCash::DBToData() User NULL!!"); 
		return;
	}
	m_vUserBonusCashTable.clear();

	while( query_data->IsExist() )
	{
	
		DWORD dwTableIndex	= 0;
		int iTotal			= 0;
		int iRemaining		= 0;
		DBTIMESTAMP ExpirationDate;

		PACKET_GUARD_BREAK( query_data->GetValue( dwTableIndex, sizeof(dwTableIndex) ) );			// 테이블 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( iTotal, sizeof(iTotal) ) );						// 총 보너스 캐쉬
		PACKET_GUARD_BREAK( query_data->GetValue( iRemaining, sizeof(iRemaining) ) );				// 잔액
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&ExpirationDate, sizeof(DBTIMESTAMP) ) );	// 만료기간

		BonusCashInfo* pInfo = CreateCashData();
		if( pInfo )
		{
			pInfo->Create(dwTableIndex, iTotal, iRemaining, ExpirationDate);
			m_vUserBonusCashTable.push_back(pInfo);			//보너스 캐쉬 인덱스 별로 저장
		}
	}
	
	int iSize	= m_vUserBonusCashTable.size();

	if( dwSelectType == GET_BONUS_CASH_PERIOD )
	{
		SP2Packet kPacket(STPK_BONUS_CASH_INFO);
		PACKET_GUARD_VOID( kPacket.Write(BONUS_CASH_ALIVE) );
		PACKET_GUARD_VOID( kPacket.Write(iSize) );

		for( int i = 0; i < iSize; i++ )
		{
			BonusCashInfo* pInfo = m_vUserBonusCashTable[i];
			if( pInfo )
			{
				PACKET_GUARD_VOID( kPacket.Write(pInfo->GetIndex()) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->GetTotalAmount()) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->GetRemainingAmount()) );
				PACKET_GUARD_VOID( kPacket.Write(pInfo->GetExpirationDate()) );
			}
			else
			{
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
			}
		}

		m_pUser->SendMessage(kPacket);
	}
}

BonusCashInfo* UserBonusCash::CreateCashData()
{
	BonusCashInfo* pInfo	= new BonusCashInfo;
	return pInfo;
}

void UserBonusCash::FillMoveData(SP2Packet& kPacket)
{
	int iSize = m_vUserBonusCashTable.size();

	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
			pInfo->FillCashInfo(kPacket);
	}

	iSize = m_vExpiredBonusCashTable.size();

	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vExpiredBonusCashTable[i];
		if( pInfo )
			pInfo->FillCashInfo(kPacket);
	}
}

void UserBonusCash::ApplyMoveData(SP2Packet& kPacket)
{
	int iSize	= 0;

	PACKET_GUARD_VOID( kPacket.Read(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo = CreateCashData();
		if( pInfo )
		{
			pInfo->ApplyCashInfo(kPacket);
			m_vUserBonusCashTable.push_back(pInfo);
		}
	}

	PACKET_GUARD_VOID( kPacket.Read(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo = CreateCashData();
		if( pInfo )
		{
			pInfo->ApplyCashInfo(kPacket);
			m_vExpiredBonusCashTable.push_back(pInfo);
		}
	}
}

BOOL UserBonusCash::AddBonusCash(int iVal, int iExpirationDay)
{
	if( !m_pUser )
		return FALSE;

	CTimeSpan cGap(iExpirationDay, 0, 0, 0);
	CTime cEndDate = CTime::GetCurrentTime() - cGap;

	//DB에 Insert 요청.
	g_DBClient.OnInsertBonusCash(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iVal, cEndDate);
	return TRUE;
}

void UserBonusCash::AddCashTable(BonusCashInfo* pInfo)
{
	m_vUserBonusCashTable.push_back(pInfo);

	std::sort(m_vUserBonusCashTable.begin(), m_vUserBonusCashTable.end());
}

BOOL UserBonusCash::ResultAddBonusCash(int iIndex, int iVal, DWORD dwExpirationDate)
{
	//Insert 응답 후 처리. (m_vUserBonusCashTable push_back 시 정렬 실시)
	BonusCashInfo* pInfo = CreateCashData();
	if( pInfo )
	{
		pInfo->Create(iIndex, iVal, iVal, dwExpirationDate);

		AddCashTable(pInfo);
		return TRUE;
	}

	return FALSE;
}

int	UserBonusCash::GetAvailableBonusCash()
{
	int iSize		= m_vUserBonusCashTable.size();
	int iTotalVal	= 0;

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
		{
			if( !pInfo->IsAvailable() )	// flag , 기간만료여부, 남은잔액 > 0 
				continue;

			iTotalVal += pInfo->GetRemainingAmount();		//HRYOON BONUS CASH 사용 가능 총 보너스 캐쉬 
		}
	}

	return iTotalVal;
}

//메모리 상에서 보너스 캐쉬로 구매가 가능한지 여부 체크함, 구매 가능시에 DB에 요청 보냄
BOOL UserBonusCash::SpendBonusCash(const DWORD dwIndex, const int iVal, int iBuyItemType, int iBuyValue1, int iBuyValue2)
{
	if( !m_pUser )
		return FALSE;

	int iSize		= m_vUserBonusCashTable.size();

	if( 0 == iSize )
	{
		
		return FALSE;
	}

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
		{
			if( pInfo->GetIndex() != dwIndex )
				continue;

			//보너스 캐쉬가 적은 경우
			if( pInfo->GetRemainingAmount() < iVal )
			{
				return FALSE;
			}

			BonusCashUpdateType eType	= BUT_CHANGE;

			if( pInfo->GetRemainingAmount() == iVal )
				eType	= BUT_END;
				
			
			/*g_DBClient.OnUpdateBonusCash(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetIndex(), pInfo->GetRemainingAmount() - iVal, eType, 
								iBuyItemType, iBuyValue1, iBuyValue2);
*/
			g_DBClient.OnUpdateBonusCash(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetIndex(), iVal, eType, 
				iBuyItemType, iBuyValue1, iBuyValue2, m_pUser->GetBillingGUID());
			return TRUE;
		}
	}


	return FALSE;
}

BOOL UserBonusCash::GetMoneyForConsume(IntOfTwoVec& vVal, const int iSpendVal)
{
	int iSize		= m_vUserBonusCashTable.size();
	int iRemainVal	= iSpendVal;	// 사용 할 보너스 캐쉬
	IntOfTwo stInfo;

	if( 0 == iSize || iRemainVal <= 0 )
		return FALSE;

	if( GetAvailableBonusCash() < iSpendVal )
		return FALSE;

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
		{
			if( !pInfo->IsAvailable() )
				continue;

			//HRYOON BONUS CASH 한 열의 보너스 캐쉬 잔액으로 구매 가능여부 
			// vVal -> 보너스 캐쉬 인덱스 & 사용금액
			if( pInfo->GetRemainingAmount() >= iRemainVal )
			{
				stInfo.value1 = pInfo->GetIndex();
				stInfo.value2 = iRemainVal;		//사용 금액
				vVal.push_back(stInfo);

				pInfo->SetFlag(FALSE);			//HRYOON BONUS CASH 플래그 세팅, 사용할 보너스 캐쉬로 잡음
				return TRUE;
			}
			else
			{
				//HRYOON BONUS CASH 한 인덱스에서 일부만 소비할 경우(반복)
				stInfo.value1 = pInfo->GetIndex();
				stInfo.value2 = pInfo->GetRemainingAmount();		//사용금액
				vVal.push_back(stInfo);

				iRemainVal -= pInfo->GetRemainingAmount();			//위에 보너스 캐쉬 쓰고 남은 금액
				pInfo->SetFlag(FALSE);	//HRYOON BONUS CASH 플래그 세팅
			}
		}
	}

	return TRUE;
}

BOOL UserBonusCash::ResultSpendBonusCash(BonusCashUpdateType eType, const int iStatus, const int iIndex, const int iVal, const int iUsedAmount, const int iType, const int iValue1, const int iValue2, const ioHashString &szBillingGUID)
{
	//Update 응답 결과 처리.메모리에 있는 값 갱신함
	CASHTABLE_ITER it	= m_vUserBonusCashTable.begin();

	
	while( it != m_vUserBonusCashTable.end() )
	{
		BonusCashInfo* pInfo	= *it;
		if( pInfo )
		{
			if( pInfo->GetIndex() == iIndex )
			{
				pInfo->UpdateRemainingAmount(iVal);
				pInfo->SetFlag(TRUE);
				
				//해당인덱스에 잔액이 부족해서 구매 실패 된 경우 
				if( iStatus > 0)
				{
					g_LogDBClient.OnInsertUsageOfBonusCash( m_pUser, pInfo->GetIndex(), -iUsedAmount, iType, iValue1, iValue2, szBillingGUID, iStatus);
				}
				else
				{
					g_LogDBClient.OnInsertUsageOfBonusCash( m_pUser, pInfo->GetIndex(), iUsedAmount, iType, iValue1, iValue2, szBillingGUID, iStatus);
				}
				//잔액이 0으로 왔다면 그 인덱스는 삭제
				if( iVal == 0 )
				{
					delete pInfo;
					m_vUserBonusCashTable.erase(it);
				}
				/*switch( eType )
				{
				case BUT_END:
					{
						delete pInfo;
						m_vUserBonusCashTable.erase(it);
					}
					break;
				}*/
				return TRUE;
			}
			else
				it++;
			
		}
	}
	return FALSE;
}

void UserBonusCash::CheckExpiredBonusCash()
{
	if( !m_pUser )
		return;

	static CASHTABLE vExpiredInfo;
	vExpiredInfo.clear();
	CASHTABLE_ITER it	= m_vUserBonusCashTable.begin();

	while( it != m_vUserBonusCashTable.end() )
	{
		BonusCashInfo* pInfo	= *it;
		if( pInfo )
		{
			if( pInfo->IsExpired() )
			{
				vExpiredInfo.push_back(pInfo);

				delete pInfo;
				it	= m_vUserBonusCashTable.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it++;
		}

	}

	int iSize = vExpiredInfo.size();

	if( 0 == iSize )
		return;

	SP2Packet kPacket(STPK_EXPIRED_BONUS_CASH);
	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= vExpiredInfo[i];

		if( pInfo)
		{
			PACKET_GUARD_VOID( kPacket.Write(pInfo->GetIndex()) );
		}
		else
		{
			PACKET_GUARD_VOID( kPacket.Write(0) );
		}
	}

	m_pUser->SendMessage(kPacket);
}

void UserBonusCash::RelieveFlag(const DWORD dwIndex)
{
	int iSize		= m_vUserBonusCashTable.size();

	for( int i = 0; i < iSize; i++ )
	{
		BonusCashInfo* pInfo	= m_vUserBonusCashTable[i];
		if( pInfo )
		{
			if( pInfo->GetIndex() != dwIndex )
				continue;

			pInfo->SetFlag(TRUE);
			return;
		}
	}
}